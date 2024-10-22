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
 **************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "mud.h"

void do_remor( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REMORT, you must spell it out.\n\r", ch );
    return;
}

void do_remort( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    char strsave[MAX_INPUT_LENGTH], player_name[MAX_INPUT_LENGTH];
    char player_pwd[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    int player_incarnations, ch_move, ch_hit, ch_mana, ch_class, ch_race;

    if ( IS_NPC(ch) || ( d = ch->desc ) == NULL )
	return;

     /* 
      * chack to see if the PC has remort 10 times, if so dont let
      * PC remort again.
      */
     if( ch->pcdata->incarnations > 3 )
     {
     	send_to_char( "Sorry you can only remort 3 times.\n\r", ch );
     	return;
     }
     
     /*
      * Check to make sure PC is level 50+ and that they have the remort flag
      * if they are not lvl 50+ and no flag set dont remort.
      */ 
    if ( ch->level < LEVEL_HERO && !IS_SET( ch->act, PLR_REMORT ) )
    {
	sprintf( buf,
	    "You must be level %d or already have remorted to remort.\n\r",
	    LEVEL_HERO );
	send_to_char( buf, ch );
	return;
    }
    
     /*
      * Make sure PC is in ROM_VNUM_REMORT if not dont remort.
    if ( ch->in_room->vnum != ROOM_VNUM_REMORT )
    {
    	send_to_char( "You must be in the remort lounge to remort!\n\r", ch );
    	return;
    }
      * If the PC has type remort twice lets remort the S.O.B
      */
    if ( ch->pcdata->confirm_remort )
    { 
	if ( argument[0] != '\0' )
	{
	    send_to_char( "Remort status removed.\n\r", ch );
	    ch->pcdata->confirm_remort = FALSE;
	    REMOVE_BIT( ch->act, PLR_REMORT );
	    return;
	}
	else
	{
             /* Announce that player has remorted. */
             sprintf(buf2, "%s entered into REMORT!", ch->name);
             do_info(ch, buf2);

	    /*
	     * Get ready to delete the pfile, send a nice informational message.
	     */
	    sprintf( strsave, "%s%c/%s", PLAYER_DIR, tolower( ch->name[0] ), capitalize( ch->name ) );
	    stop_fighting( ch, TRUE );
            send_to_char( "\n\r\n\r", ch);
            send_to_char( "CONGRATULATIONS! You have chosen to remort and will be allowed\n\r", ch );
            send_to_char( "to choose from a new Race OR Class. In the unlikely event that you\n\r", ch );
           send_to_char( "are disconnected or the MUD crashes while creating your character,\n\r", ch );
           send_to_char( "create a new character as normal and write a note to 'immortal'.\n\r", ch );
	    send_to_char( "\n\r[Hit Enter to Continue]", ch );

	    /*
	     * I quote:
	     * "After extract_char the ch is no longer valid!"
	     */
	    sprintf( player_name, "%s", capitalize( ch->name ) );
	    sprintf( player_pwd, "%s", ch->pcdata->pwd );
	    player_incarnations = ++ch->pcdata->incarnations;
	    ch_class		= ch->class;
	    ch_race 		= ch->race;
	    ch_hit		= ch->max_hit  / 3;
	    ch_move		= ch->move     / 3;

            if ( IS_VAMPIRE(ch))
	       ch_mana		= ch->max_mana + 250;
            else
	       ch_mana		= ch->max_mana / 3;

	    SET_BIT( ch->act, PLR_REMORT );
            
            /*
             * BIT needed by Beowolf for more nifty stuff.
             * Next bit is set for display in who list purposes and any
             * Remort check that is needed. - Beowolf.
             */
	    SET_BIT( ch->pcdata->flags, PCFLAG_REMORT );

	    if ( ch->desc && ch->desc->original && IS_SET(ch->act, ACT_POLYMORPHED))
	        do_revert( ch, "" );

	    if ( ch->desc && ch->desc->original )
		do_return( ch, "" );

	    for ( wch = first_char; wch; wch = wch->next )
		if ( wch->reply == ch )
		    wch->reply = NULL;
	     
	    char_from_room( ch );
	
	    UNLINK( ch, first_char, last_char, next, prev );
	    
	    /*
	     * Delete the pfile, but don't boot the character.
	     * Instead, do a load_char_obj to get a new ch,
	     * saving the password, and the incarnations.  Then,
	     * set the PLR_REMORT bit and drop the player in at
	     * CON_BEGIN_REMORT.
	     */
	    unlink( strsave );
	    /*
	     * inside save.c
	     */
	    load_char_obj( d, player_name, FALSE );
	    /*
	     * return from save.c
	     */
	    d->character->pcdata->pwd 		= str_dup( player_pwd );
	    d->character->pcdata->incarnations  = player_incarnations;
	    d->character->max_hit		= ch_hit;
	    d->character->max_move		= ch_move;
	    d->character->max_mana		= ch_mana;
	    d->character->race			= ch_race;
	    d->character->class			= ch_class;
		    SET_BIT( ch->act, PLR_REMORT );
		    d->connected = CON_BEGIN_REMORT;
		    return;
	}
    }

    if ( argument[0] != '\0' )
    {
	send_to_char( "Just type remort.  No argument.\n\r", ch );
	return;
    }

    send_to_char("Type remort again to confirm this command.\n\r", ch );
    send_to_char("WARNING: This command is irreversible.\n\r", ch );
    send_to_char("Typing remort with an argument will undo remort status.\n\r",
	ch );
    ch->pcdata->confirm_remort = TRUE;
}
