/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/***************************************************************************
 *									   *
 *       TSR MUD skills recoded and redone by Beowolf. The Code was        *
 *	 taken from Envy and Silly MUD code & special thanks goes          *
 *	 out to those very cool code bases.				   *
 *									   *
 **************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_chameleon	);
DECLARE_DO_FUN(do_heighten	);
DECLARE_DO_FUN(do_shadow	);
DECLARE_DO_FUN(do_snare 	);
DECLARE_DO_FUN(do_untangle	);



void do_chameleon ( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_chameleon].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    send_to_char( "You attempt to blend in with your surroundings.\n\r", ch);

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
        REMOVE_BIT( ch->affected_by, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_chameleon] )
        SET_BIT( ch->affected_by, AFF_HIDE );

    return;
}

void do_heighten ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_heighten].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_heighten ) )
        return;

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_heighten] )
    {
	af.type      = gsn_heighten;
	af.duration  = 24;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = AFF_DETECT_INVIS;
	affect_to_char( ch, &af );

	af.bitvector = AFF_DETECT_HIDDEN;
	affect_to_char( ch, &af );
	
	af.bitvector = AFF_INFRARED;
	affect_to_char( ch, &af );
	
	send_to_char( "Your senses are heightened.\n\r", ch );
    }
    return;

}

void do_shadow ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_shadow].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    send_to_char( "You attempt to move in the shadows.\n\r", ch );
    affect_strip( ch, gsn_shadow );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_shadow] )
    {
	af.type      = gsn_shadow;
	af.duration  = ch->level;
	af.modifier  = APPLY_NONE;
	af.location  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }
    return;

}

/* Snare skill by Binky for EnvyMud */
void do_snare( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    /*
     *  First, this checks for case of no second argument (valid only
     *  while fighting already).  Later, if an argument is given, it 
     *  checks validity of argument.  Unsuccessful snares flow through
     *  and receive messages at the end of the function.
     */

    if ( arg[0] == '\0' )
    {
	if ( !( victim = ch->fighting ) )
	{
	    send_to_char( "Ensnare whom?\n\r", ch );
	    return;
	}
	/* No argument, but already fighting: valid use of snare */
	WAIT_STATE( ch, skill_table[gsn_snare].beats );

	if ( IS_NPC( ch )
	    || number_percent( ) < ch->pcdata->learned[gsn_snare] )
	{    
	    affect_strip( victim, gsn_snare );  

	    af.type      = gsn_snare;
	    af.duration  = 1 + ( ( ch->level ) / 8 );
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_HOLD;

	    affect_to_char( victim, &af );

	    act( "You have ensnared $M!", ch, NULL, victim, TO_CHAR    );
	    act( "$n has ensnared you!",  ch, NULL, victim, TO_VICT    );
	    act( "$n has ensnared $N.",   ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
	    act( "You failed to ensnare $M.  Uh oh!",
		ch, NULL, victim, TO_CHAR    );
	    act( "$n tried to ensnare you!  Get $m!",
		ch, NULL, victim, TO_VICT    );
	    act( "$n attempted to ensnare $N, but failed!",
		ch, NULL, victim, TO_NOTVICT );
	}
    }
    else				/* argument supplied */
    {
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( !IS_NPC( ch ) && !IS_NPC( victim ) )
	{
	    send_to_char( "You can't ensnare another player.\n\r", ch );
	    return;
	}

	if ( victim != ch->fighting ) /* TRUE if not fighting, or fighting  */
	{                             /* if person other than victim        */
	    if ( ch->fighting )       /* TRUE if fighting other than vict.  */ 
	    {		
		send_to_char(
		    "Take care of the person you are fighting first!\n\r",
			     ch );
		return;
	    }                             
	    WAIT_STATE( ch, skill_table[gsn_snare].beats );

	    if ( IS_NPC( ch )       /* here, arg supplied, ch not fighting  */
		|| number_percent( ) < ch->pcdata->learned[gsn_snare] )
	    {
		affect_strip( victim, gsn_snare );  

		af.type      = gsn_snare;
		af.duration  = 3 + ( (ch->level ) / 8 );
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_HOLD;

		affect_to_char( victim, &af );

		act( "You have ensnared $M!", ch, NULL, victim, TO_CHAR    );
		act( "$n has ensnared you!",  ch, NULL, victim, TO_VICT    );
		act( "$n has ensnared $N.",   ch, NULL, victim, TO_NOTVICT );
	    }
	    else
	    {
		act( "You failed to ensnare $M.  Uh oh!",
		    ch, NULL, victim, TO_CHAR    );
		act( "$n tried to ensnare you!  Get $m!",
		    ch, NULL, victim, TO_VICT    );
		act( "$n attempted to ensnare $N, but failed!",
		    ch, NULL, victim, TO_NOTVICT );
	    }
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	else
	{
	    WAIT_STATE( ch, skill_table[gsn_snare].beats );

	    if ( IS_NPC( ch )
		|| number_percent( ) < ch->pcdata->learned[gsn_snare] )
	    {
		affect_strip( victim, gsn_snare );  

		af.type      = gsn_snare;
		af.duration  = 1 + ( ( ch->level ) / 8 );
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_HOLD;

		affect_to_char( victim, &af );

		act( "You have ensnared $M!", ch, NULL, victim, TO_CHAR    );
		act( "$n has ensnared you!",  ch, NULL, victim, TO_VICT    );
		act( "$n has ensnared $N.",   ch, NULL, victim, TO_NOTVICT );
	    }
	    else
	    {
		act( "You failed to ensnare $M.  Uh oh!",
		    ch, NULL, victim, TO_CHAR    );
		act( "$n tried to ensnare you!  Get $m!",
		    ch, NULL, victim, TO_VICT    );
		act( "$n attempted to ensnare $N, but failed!",
		    ch, NULL, victim, TO_NOTVICT );
	    }
	}
    }

    return;
}



/* Untangle by Thelonius for EnvyMud */
void do_untangle( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    char         arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_untangle].skill_level[ch->class] )
    {
	send_to_char( "You aren't nimble enough.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	victim = ch;
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    }

    if ( !is_affected( victim, gsn_snare ) )
	return;

    if ( ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch ) 
	    && number_percent( ) < ch->pcdata->learned[gsn_untangle] ) )
    {
	affect_strip( victim, gsn_snare );

	if ( victim != ch )
        {
	    act( "You untangle $N.",  ch, NULL, victim, TO_CHAR    );
	    act( "$n untangles you.", ch, NULL, victim, TO_VICT    );
	    act( "$n untangles $n.",  ch, NULL, victim, TO_NOTVICT );
        }
	else
        {
	    send_to_char( "You untangle yourself.\n\r", ch );
	    act( "$n untangles $mself.", ch, NULL, NULL, TO_ROOM );
        }

	return;
    }
}

void do_track (CHAR_DATA *ch, char *argument)
{
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *in_room;
    
    in_room = ch->in_room;
    
    if (in_room->sector_type == SECT_UNDERWATER)
    {
	send_to_char("You cannot follow any trails underwater.\n\r", ch);
	return;
    }
    
    if (in_room->sector_type == SECT_AIR)
    {
	send_to_char("You are flying in the air, there are no trails to follow.\n\r", ch);
	return;
    }
    
    if (in_room->sector_type == SECT_WATER_NOSWIM)
    {
	send_to_char("The water is obscuring all possible trails.\n\r", ch);
	return;
    }
    
    if (in_room->sector_type == SECT_WATER_SWIM
	&& ch->generation < 10)
    {
	send_to_char("The water has obscured all possible tracks.\n\r", ch);
	return;
    }

    if (in_room->track[0] != NULL
	&& in_room->trackdir[0] != NULL
	&& in_room->track[0] != ch->pcdata->switchname)
    {
	sprintf(buf, "You detect the trail of %s leading %s.\n\r",
	    in_room->track[0], in_room->trackdir[0]);
	send_to_char(buf, ch);
	found = TRUE;
    }       
    
    if (in_room->track[1] != NULL
	&& in_room->trackdir[1] != NULL
	&& ch->generation > 5
	&& in_room->track[1] != ch->pcdata->switchname)
    {
	sprintf(buf, "You detect the feint trail of %s leading %s.\n\r",
	    in_room->track[1], in_room->trackdir[1]);
	send_to_char(buf, ch);
	found = TRUE;
    }
    
    if (in_room->track[2] != NULL
	&& in_room->trackdir[2] != NULL
	&& ch->generation > 8
	&& in_room->track[2] != ch->pcdata->switchname)
    {
	sprintf(buf, "You detect the very feint trail of %s leading %s.\n\r",
	    in_room->track[2], in_room->trackdir[2]);
	send_to_char(buf, ch);
	found = TRUE;
    }

    if (!found)
	send_to_char("You detect no trails from this place.\n\r", ch);
} 
