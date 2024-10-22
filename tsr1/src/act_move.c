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

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_recall	);
DECLARE_DO_FUN(do_stand		);
DECLARE_DO_FUN(push_obj		);
DECLARE_DO_FUN(do_prompt	);
DECLARE_DO_FUN(do_say   	);


char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down"
};

char *	const	dir_rev_name	[]		=
{
    "the north", "the east", "the south", "the west", "above", "below"
};

const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );
void    move_ship       args( ( CHAR_DATA *ch, int door, bool follow ) );
void    move_wagon	args( ( CHAR_DATA *ch, int door, bool follow ) );
bool    chance          args( ( int num ) );

bool    chance          ( int num );



void move_char( CHAR_DATA *ch, int door, bool follow )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int chance;

    if ( door < 0 || door > 5 )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

   /* following lines for mob_is_guard. Froth */

    if ( checkmobguard ( ch, door ) )
	return;

   /* end of trap and mob guard section here */
    
   /*
    * MOBPROGS
    * Exit trigger, if activated, bail out. Only PCs are triggered.
    */
    if ( !IS_NPC(ch) && mp_exit_trigger( ch, door ) )
 	return;

    /* end mobprogs */

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room))
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED)
    &&  (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))
    &&   !IS_TRUSTED(ch,ANGEL))
    {
	act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
	send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }
    
    /* is the character in a ship */
    if ( ch->in_ship )
    {
	move_ship( ch, door, FALSE );
	return;
    }
    
    /* is the character in a wagon */
    if ( ch->in_wagon )
    {
	move_wagon( ch, door, FALSE );
	return;
    }


    /* Cave In Code */	

    if ( to_room->status >= 1 ) 
    {	
	send_to_char( "You are unable to enter that room at this time.\n\r", ch );
	return;
    }
    
    if ( !is_room_owner(ch,to_room) && room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) )
    {
	int iClass, iGuild;
	int move;

	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++)	
	    {
	    	if ( iClass != ch->class
	    	&&   to_room->vnum == class_table[iClass].guild[iGuild] )
	    	{
		    send_to_char( "You aren't allowed in there.\n\r", ch );
		    return;
		}
	    }
	}

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
	    if ( !IS_AFFECTED(ch, AFF_FLYING) && !IS_IMMORTAL(ch))
	    {
		send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}
        
	/* Underwater Code added on 6-12-96. Code donated by 
	 * Shelly Lahman a.k.a Honey.
	 */

	if  ( in_room->sector_type == SECT_UNDERWATER
	||    to_room->sector_type == SECT_UNDERWATER )
   	{
	     if ( !IS_AFFECTED(ch, AFF_WATER_BREATH) && !IS_IMMORTAL(ch)) 
	     {
		send_to_char( "You can't breathe underwater!\n\r", ch);
		return;
	     }
	}

    /* Cave in Code */

    if ( to_room->sector_type == SECT_UNDERGROUND 
    ||   to_room->room_flags == ROOM_UNDERGROUND )
    {
	chance = number_range( ch->level / 2, ch->level * 3 / 5 );
	
	    if ( number_percent( ) < chance )
	    {
		to_room->status += 1;
		send_to_char( "Darkness surrounds you as the walls cavein.\n\r", ch );
		char_from_room( ch );
		char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
		do_look( ch, "" );
		ch->hit -= ( ch->hit -1 );
		ch->exp -= 100;
		return;
	    }
     }

	if (( in_room->sector_type == SECT_WATER_NOSWIM
	||    to_room->sector_type == SECT_WATER_NOSWIM )
  	&&    !IS_AFFECTED(ch,AFF_FLYING))
	{
	    OBJ_DATA *obj;
	    bool found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;

	    if (IS_IMMORTAL(ch))
		found = TRUE;

	    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	    {
		if ( obj->item_type == ITEM_BOAT )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
	    {
		send_to_char( "You need a boat to go there.\n\r", ch );
		return;
	    }
	}

	move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

        move /= 2;  /* i.e. the average */


	/* conditional effects */
	if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE))
	    move /= 2;

	if (IS_AFFECTED(ch,AFF_SLOW))
	    move *= 2;

	if ( ch->move < move )
	{
	    send_to_char( "You are too exhausted.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, 1 );
	ch->move -= move;
    }


    if ( !IS_AFFECTED(ch, AFF_SNEAK)
    &&   ch->invis_level < LEVEL_HERO)
	act( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, to_room );
    if ( !IS_AFFECTED(ch, AFF_SNEAK)
    &&   ch->invis_level < LEVEL_HERO)
	act( "$n has arrived.", ch, NULL, NULL, TO_ROOM );

    do_look( ch, "auto" );

    if (in_room == to_room) /* no circular follows */
	return;

/*
	in_room->track[2] = in_room->track[1];
	in_room->track[1] = in_room->track[0];
	WRITE_STR(in_room->track[0], GET_PROPER_NAME(ch));    
    
	in_room->trackdir[2] = in_room->trackdir[1];
	in_room->trackdir[1] = in_room->trackdir[0];
	WRITE_STR(in_room->trackdir[0], dir_name[door]);
*/
    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next_in_room;

	if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM) 
	&&   fch->position < POS_STANDING)
	    do_stand(fch,"");

	if ( fch->master == ch && fch->position == POS_STANDING 
	&&   can_see_room(fch,to_room))
	{

	    if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
	    &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
	    {
		act("You can't bring $N into the city.",
		    ch,NULL,fch,TO_CHAR);
		act("You aren't allowed in the city.",
		    fch,NULL,NULL,TO_CHAR);
		continue;
	    }

	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door, TRUE );
	}
    }

     /*
      * MOBPROGS
      * If someone is following the char, these triggers get activated
      * for the followers before the char, but it's safer this way...
      */

     if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
 	mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_ENTRY );
     if ( !IS_NPC( ch ) )
     	mp_greet_trigger( ch );
     
     /* end mobprogs */
    return;
}

void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH, FALSE );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST, FALSE );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH, FALSE );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST, FALSE );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP, FALSE );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN, FALSE );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
 	/* open portal */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1], EX_ISDOOR))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1], EX_CLOSED))
	    {
		send_to_char("It's already open.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1], EX_LOCKED))
	    {
		send_to_char("It's locked.\n\r",ch);
		return;
	    }

	    REMOVE_BIT(obj->value[1], EX_CLOSED);
	    act("You open $p.",ch,obj,NULL,TO_CHAR);
	    act("$n opens $p.",ch,obj,NULL,TO_ROOM);
	    return;
 	}

	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER)
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act("You open $p.",ch,obj,NULL,TO_CHAR);
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{

	    if (!IS_SET(obj->value[1],EX_ISDOOR)
	    ||   IS_SET(obj->value[1],EX_NOCLOSE))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's already closed.\n\r",ch);
		return;
	    }

	    SET_BIT(obj->value[1],EX_CLOSED);
	    act("You close $p.",ch,obj,NULL,TO_CHAR);
	    act("$n closes $p.",ch,obj,NULL,TO_ROOM);
	    return;
	}

	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	act("You close $p.",ch,obj,NULL,TO_CHAR);
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1],EX_ISDOOR)
	    ||  IS_SET(obj->value[1],EX_NOCLOSE))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }
	    if (!IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's not closed.\n\r",ch);
	 	return;
	    }

	    if (obj->value[4] < 0 || IS_SET(obj->value[1],EX_NOLOCK))
	    {
		send_to_char("It can't be locked.\n\r",ch);
		return;
	    }

	    if (!has_key(ch,obj->value[4]))
	    {
		send_to_char("You lack the key.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1],EX_LOCKED))
	    {
		send_to_char("It's already locked.\n\r",ch);
		return;
	    }

	    SET_BIT(obj->value[1],EX_LOCKED);
	    act("You lock $p.",ch,obj,NULL,TO_CHAR);
	    act("$n locks $p.",ch,obj,NULL,TO_ROOM);
	    return;
	}

	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	act("You lock $p.",ch,obj,NULL,TO_CHAR);
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
 	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1],EX_ISDOOR))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's not closed.\n\r",ch);
		return;
	    }

	    if (obj->value[4] < 0)
	    {
		send_to_char("It can't be unlocked.\n\r",ch);
		return;
	    }

	    if (!has_key(ch,obj->value[4]))
	    {
		send_to_char("You lack the key.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1],EX_LOCKED))
	    {
		send_to_char("It's already unlocked.\n\r",ch);
		return;
	    }

	    REMOVE_BIT(obj->value[1],EX_LOCKED);
	    act("You unlock $p.",ch,obj,NULL,TO_CHAR);
	    act("$n unlocks $p.",ch,obj,NULL,TO_ROOM);
	    return;
	}

	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	act("You unlock $p.",ch,obj,NULL,TO_CHAR);
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch,gsn_pick_lock))
    {
	send_to_char( "You failed.\n\r", ch);
	check_improve(ch,gsn_pick_lock,FALSE,2);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1],EX_ISDOOR))
	    {	
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's not closed.\n\r",ch);
		return;
	    }

	    if (obj->value[4] < 0)
	    {
		send_to_char("It can't be unlocked.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1],EX_PICKPROOF))
	    {
		send_to_char("You failed.\n\r",ch);
		return;
	    }

	    REMOVE_BIT(obj->value[1],EX_LOCKED);
	    act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
	    check_improve(ch,gsn_pick_lock,TRUE,2);
	    return;
	}

	    


	
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
        act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
        act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
	check_improve(ch,gsn_pick_lock,TRUE,2);
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 && !IS_IMMORTAL(ch))
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	check_improve(ch,gsn_pick_lock,TRUE,2);

	/* pick the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (argument[0] != '\0')
    {
	if (ch->position == POS_FIGHTING)
	{
	    send_to_char("Maybe you should finish fighting first?\n\r",ch);
	    return;
	}
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
	if (obj->item_type != ITEM_FURNITURE
	||  (!IS_SET(obj->value[2],STAND_AT)
	&&   !IS_SET(obj->value[2],STAND_ON)
	&&   !IS_SET(obj->value[2],STAND_IN)))
	{
	    send_to_char("You can't seem to find a place to stand.\n\r",ch);
	    return;
	}
	if (ch->on != obj && count_users(obj) >= obj->value[0])
	{
	    act_new("There's no room to stand on $p.",
		ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
	}
 	ch->on = obj;
    }
    
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }
	
	if (obj == NULL)
	{
	    send_to_char( "You wake and stand up.\n\r", ch );
	    act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	    ch->on = NULL;
	}
	else if (IS_SET(obj->value[2],STAND_AT))
	{
	   act_new("You wake and stand at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	   act("$n wakes and stands at $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],STAND_ON))
	{
	    act_new("You wake and stand on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    act("$n wakes and stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	else 
	{
	    act_new("You wake and stand in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    act("$n wakes and stands in $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_STANDING;
	do_look(ch,"auto");
	break;

    case POS_RESTING: case POS_SITTING:
	if (obj == NULL)
	{
	    send_to_char( "You stand up.\n\r", ch );
	    act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	    ch->on = NULL;
	}
	else if (IS_SET(obj->value[2],STAND_AT))
	{
	    act("You stand at $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands at $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],STAND_ON))
	{
	    act("You stand on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	else
	{
	    act("You stand in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (ch->position == POS_FIGHTING)
    {
	send_to_char("You are already fighting!\n\r",ch);
	return;
    }

    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0')
    {
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else obj = ch->on;

    if (obj != NULL)
    {
        if (!IS_SET(obj->item_type,ITEM_FURNITURE) 
    	||  (!IS_SET(obj->value[2],REST_ON)
    	&&   !IS_SET(obj->value[2],REST_IN)
    	&&   !IS_SET(obj->value[2],REST_AT)))
    	{
	    send_to_char("You can't rest on that.\n\r",ch);
	    return;
    	}

        if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
        {
	    act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
    	}
	
	ch->on = obj;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	if (IS_AFFECTED(ch,AFF_SLEEP))
	{
	    send_to_char("You can't wake up!\n\r",ch);
	    return;
	}

	if (obj == NULL)
	{
	    send_to_char( "You wake up and start resting.\n\r", ch );
	    act ("$n wakes up and starts resting.",ch,NULL,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],REST_AT))
	{
	    act_new("You wake up and rest at $p.",
		    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
	    act("$n wakes up and rests at $p.",ch,obj,NULL,TO_ROOM);
	}
        else if (IS_SET(obj->value[2],REST_ON))
        {
            act_new("You wake up and rest on $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
            act("$n wakes up and rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
            act_new("You wake up and rest in $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
            act("$n wakes up and rests in $p.",ch,obj,NULL,TO_ROOM);
        }
	ch->position = POS_RESTING;
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	if (obj == NULL)
	{
	    send_to_char( "You rest.\n\r", ch );
	    act( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
	}
        else if (IS_SET(obj->value[2],REST_AT))
        {
	    act("You sit down at $p and rest.",ch,obj,NULL,TO_CHAR);
	    act("$n sits down at $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
	    act("You sit on $p and rest.",ch,obj,NULL,TO_CHAR);
	    act("$n sits on $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
	    act("You rest in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
        }
	ch->position = POS_RESTING;
	break;

    case POS_SITTING:
	if (obj == NULL)
	{
	    send_to_char("You rest.\n\r",ch);
	    act("$n rests.",ch,NULL,NULL,TO_ROOM);
	}
        else if (IS_SET(obj->value[2],REST_AT))
        {
	    act("You rest at $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests at $p.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
	    act("You rest on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
	    act("You rest in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_RESTING;
	break;
    }


    return;
}


void do_sit (CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (ch->position == POS_FIGHTING)
    {
	send_to_char("Maybe you should finish this fight first?\n\r",ch);
	return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0')
    {
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else obj = ch->on;

    if (obj != NULL)                                                              
    {
	if (!IS_SET(obj->item_type,ITEM_FURNITURE)
	||  (!IS_SET(obj->value[2],SIT_ON)
	&&   !IS_SET(obj->value[2],SIT_IN)
	&&   !IS_SET(obj->value[2],SIT_AT)))
	{
	    send_to_char("You can't sit on that.\n\r",ch);
	    return;
	}

	if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
	{
	    act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
	}

	ch->on = obj;
    }
    switch (ch->position)
    {
	case POS_SLEEPING:
	    if (IS_AFFECTED(ch,AFF_SLEEP))
	    {
		send_to_char("You can't wake up!\n\r",ch);
		return;
	    }

            if (obj == NULL)
            {
            	send_to_char( "You wake and sit up.\n\r", ch );
            	act( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
            }
            else if (IS_SET(obj->value[2],SIT_AT))
            {
            	act_new("You wake and sit at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else if (IS_SET(obj->value[2],SIT_ON))
            {
            	act_new("You wake and sit on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else
            {
            	act_new("You wake and sit in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits in $p.",ch,obj,NULL,TO_ROOM);
            }

	    ch->position = POS_SITTING;
	    break;
	case POS_RESTING:
	    if (obj == NULL)
		send_to_char("You stop resting.\n\r",ch);
	    else if (IS_SET(obj->value[2],SIT_AT))
	    {
		act("You sit at $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits at $p.",ch,obj,NULL,TO_ROOM);
	    }

	    else if (IS_SET(obj->value[2],SIT_ON))
	    {
		act("You sit on $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    ch->position = POS_SITTING;
	    break;
	case POS_SITTING:
	    send_to_char("You are already sitting down.\n\r",ch);
	    break;
	case POS_STANDING:
	    if (obj == NULL)
    	    {
		send_to_char("You sit down.\n\r",ch);
    	        act("$n sits down on the ground.",ch,NULL,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SIT_AT))
	    {
		act("You sit down at $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits down at $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SIT_ON))
	    {
		act("You sit on $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else
	    {
		act("You sit down in $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits down in $p.",ch,obj,NULL,TO_ROOM);
	    }
    	    ch->position = POS_SITTING;
    	    break;
    }
    return;
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING: 
	if (argument[0] == '\0' && ch->on == NULL)
	{
	    send_to_char( "You go to sleep.\n\r", ch );
	    act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
	    ch->position = POS_SLEEPING;
	}
	else  /* find an object and sleep on it */
	{
	    if (argument[0] == '\0')
		obj = ch->on;
	    else
	    	obj = get_obj_list( ch, argument,  ch->in_room->contents );

	    if (obj == NULL)
	    {
		send_to_char("You don't see that here.\n\r",ch);
		return;
	    }
	    if (obj->item_type != ITEM_FURNITURE
	    ||  (!IS_SET(obj->value[2],SLEEP_ON) 
	    &&   !IS_SET(obj->value[2],SLEEP_IN)
	    &&	 !IS_SET(obj->value[2],SLEEP_AT)))
	    {
		send_to_char("You can't sleep on that!\n\r",ch);
		return;
	    }

	    if (ch->on != obj && count_users(obj) >= obj->value[0])
	    {
		act_new("There is no room on $p for you.",
		    ch,obj,NULL,TO_CHAR,POS_DEAD);
		return;
	    }

	    ch->on = obj;
	    if (IS_SET(obj->value[2],SLEEP_AT))
	    {
		act("You go to sleep at $p.",ch,obj,NULL,TO_CHAR);
		act("$n goes to sleep at $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SLEEP_ON))
	    {
	        act("You go to sleep on $p.",ch,obj,NULL,TO_CHAR);
	        act("$n goes to sleep on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else
	    {
		act("You go to sleep in $p.",ch,obj,NULL,TO_CHAR);
		act("$n goes to sleep in $p.",ch,obj,NULL,TO_ROOM);
	    }
	    ch->position = POS_SLEEPING;
	}
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    act_new( "$n wakes you.", ch, NULL, victim, TO_VICT,POS_SLEEPING );
    do_stand(victim,"");
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if (IS_AFFECTED(ch,AFF_SNEAK))
	return;

    if ( number_percent( ) < get_skill(ch,gsn_sneak))
    {
	check_improve(ch,gsn_sneak,TRUE,3);
	af.where     = TO_AFFECTS;
	af.type      = gsn_sneak;
	af.level     = ch->level; 
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }
    else
	check_improve(ch,gsn_sneak,FALSE,3);

    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( number_percent( ) < get_skill(ch,gsn_hide))
    {
	SET_BIT(ch->affected_by, AFF_HIDE);
	check_improve(ch,gsn_hide,TRUE,3);
    }
    else
	check_improve(ch,gsn_hide,FALSE,3);

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;

    if (IS_NPC(ch) && !IS_SET(ch->act,ACT_PET))
    {
	send_to_char("Only players can recall.\n\r",ch);
	return;
    }
  
    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM );

    if ( ( location = get_room_index( class_table[ch->class].hometown ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE))
    {
	send_to_char( "Ao has forsaken you.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
	int lose,skill;

	skill = get_skill(ch,gsn_recall);

	if ( number_percent() < 80 * skill / 100 )
	{
	    check_improve(ch,gsn_recall,FALSE,6);
	    WAIT_STATE( ch, 4 );
	    sprintf( buf, "You failed!.\n\r");
	    send_to_char( buf, ch );
	    return;
	}

	lose = (ch->desc != NULL) ? 25 : 50;
	gain_exp( ch, 0 - lose );
	check_improve(ch,gsn_recall,TRUE,4);
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
	
    }

    ch->move /= 2;
    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    
    if (ch->pet != NULL)
	do_recall(ch->pet,"");

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mob;
    sh_int stat = - 1;
    char *pOutput = NULL;
    int cost;

    if ( IS_NPC(ch) )
	return;

    /*
     * Check for trainer.
     */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
	if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
	    break;
    }

    if ( mob == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "You have %d training sessions.\n\r", ch->train );
	send_to_char( buf, ch );
	argument = "foo";
    }

    cost = 1;

    if ( !str_cmp( argument, "str" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_STR )
	    cost    = 1;
	stat        = STAT_STR;
	pOutput     = "strength";
    }

    else if ( !str_cmp( argument, "int" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_INT )
	    cost    = 1;
	stat	    = STAT_INT;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( argument, "wis" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_WIS )
	    cost    = 1;
	stat	    = STAT_WIS;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( argument, "dex" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_DEX )
	    cost    = 1;
	stat  	    = STAT_DEX;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( argument, "con" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_CON )
	    cost    = 1;
	stat	    = STAT_CON;
	pOutput     = "constitution";
    }

    else if ( !str_cmp(argument, "hp" ) )
	cost = 1;

    else if ( !str_cmp(argument, "mana" ) )
	cost = 1;

    else
    {
	strcpy( buf, "You can train:" );
	if ( ch->perm_stat[STAT_STR] < get_max_train(ch,STAT_STR)) 
	    strcat( buf, " str" );
	if ( ch->perm_stat[STAT_INT] < get_max_train(ch,STAT_INT))  
	    strcat( buf, " int" );
	if ( ch->perm_stat[STAT_WIS] < get_max_train(ch,STAT_WIS)) 
	    strcat( buf, " wis" );
	if ( ch->perm_stat[STAT_DEX] < get_max_train(ch,STAT_DEX))  
	    strcat( buf, " dex" );
	if ( ch->perm_stat[STAT_CON] < get_max_train(ch,STAT_CON))  
	    strcat( buf, " con" );
	strcat( buf, " hp mana");

	if ( buf[strlen(buf)-1] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char( buf, ch );
	}
	else
	{
	    /*
	     * This message dedicated to Jordan ... you big stud!
	     */
	    act( "You have nothing left to train, you $T!",
		ch, NULL,
		ch->sex == SEX_MALE   ? "big stud" :
		ch->sex == SEX_FEMALE ? "hot babe" :
					"wild thing",
		TO_CHAR );
	}

	return;
    }

    if (!str_cmp("hp",argument))
    {
    	if ( cost > ch->train )
    	{
       	    send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }
 
	ch->train -= cost;
        ch->pcdata->perm_hit += 10;
        ch->max_hit += 10;
        ch->hit +=10;
        act( "Your durability increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's durability increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }
 
    if (!str_cmp("mana",argument))
    {
        if ( cost > ch->train )
        {
            send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }

	ch->train -= cost;
        ch->pcdata->perm_mana += 10;
        ch->max_mana += 10;
        ch->mana += 10;
        act( "Your power increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's power increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }

    if ( ch->perm_stat[stat]  >= get_max_train(ch,stat) )
    {
	act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }

    if ( cost > ch->train )
    {
	send_to_char( "You don't have enough training sessions.\n\r", ch );
	return;
    }

    ch->train		-= cost;
  
    ch->perm_stat[stat]		+= 1;
    act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
    return;
}

/*
 * Mounts.
 * Gravestone
 * Times of Chaos
 * toc.pcix.com 9000
 */

void do_mount( CHAR_DATA *ch, char *argument )
{
    int chance;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    bool found;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    if( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char("What are you trying to do?\n\r",ch);
	return;
    }

    if( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char("blah\n\r",ch);
	return;
    }

    if( !IS_NPC(victim)
    || victim->fighting != NULL
    || !IS_SET(victim->act2, ACT2_MOUNTABLE) )
    {
	act("You can't ride $N",ch,NULL,victim,TO_CHAR);
	return;
    }

    if( !IS_NPC(ch) && ch->pcdata->mounted )
    {
	send_to_char("You must dismount first\n\r",ch);
	return;
    }

    if(IS_NPC(victim) && victim->ridden)
    {
	act("$N already has a rider",ch,NULL,victim,TO_CHAR);
	return;
    }

    found = FALSE;
    
    /* check to see if the rider has a saddle */

    for( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if( obj->item_type == ITEM_SADDLE )
	{
	    found = TRUE;
	    break;
	}
    }

    if(!found)
    {
	found = FALSE;
	for( obj = victim->carrying; obj; obj = obj->next_content )
	{
	    if( obj->item_type == ITEM_SADDLE && obj->wear_loc == WEAR_BODY)
	    {
		found = TRUE;
		break;
	    }
	}
     	if(!found)
	{
	    send_to_char("You need a saddle...\n\r",ch);
	    return;
	}
    }

    chance = get_skill(ch,gsn_ride);

    if( chance < 2 )
    {
	send_to_char( "Best go learn riding a little better before you try it\n\r",ch);
	return;
    }

    if( chance > 90 )
	chance = 90;

    if( IS_IMMORTAL(ch) )
	chance = 100;

    /* lets see if the mount bucks the rider */

    if( number_percent () > chance)
    {
        act("You are thrown off $N !!",ch,NULL,NULL,TO_CHAR);
        act("$n is thrown off $N !",ch,NULL,NULL,TO_ROOM);
	if(ch->hit < 25)
	   ch->hit = 1;
	else
	   ch->hit -= 25;

	check_improve(ch,gsn_ride, FALSE,6);
	return;
    }

    if(get_eq_char( victim, WEAR_BODY) == NULL)
    {
	obj_from_char(obj);
	obj_to_char(obj,victim);
	sprintf(buf,"%s saddles %s and mounts $m.", ch->name,
		victim->short_descr ? victim->short_descr: victim->name );
	obj->wear_loc = WEAR_BODY;
    }
    else
    	sprintf(buf,"%s mounts %s",ch->name,
		victim->short_descr ? victim->short_descr : victim->name );
        if ( !IS_AFFECTED(ch, AFF_SNEAK)
        &&   ch->invis_level < LEVEL_HERO)
		act( buf,ch,NULL,NULL,TO_ROOM );
	send_to_char("You mount your steed.\n\r",ch);

	check_improve(ch,gsn_ride,FALSE,6);

	SET_BIT(victim->act, ACT_PET);
	victim->master = ch;
	ch->pet = victim;

	ch->pet->ridden = TRUE;
	ch->pcdata->mounted = TRUE;

	WAIT_STATE( ch, skill_table[gsn_ride].beats );
	return;
}

void do_dismount( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mount;

    if(IS_NPC(ch))
	return;
 
    if(!ch->pcdata->mounted) {
	send_to_char("But...you aren't riding anything!\n\r",ch);
	return;
    }

        if ( !IS_AFFECTED(ch, AFF_SNEAK)
        &&   ch->invis_level < LEVEL_HERO)
	sprintf(buf,"%s dismounts from %s", ch->name,
	ch->pet->short_descr ? ch->pet->short_descr : ch->pet->name );
	act( buf, ch, NULL, NULL, TO_ROOM );
	send_to_char("You dismount.\n\r",ch);

    if(ch->pet != NULL)
	mount = ch->pet;
    else {
	ch->pcdata->mounted = FALSE;
	return;
    }

    mount->ridden = FALSE;

    ch->pcdata->mounted = FALSE;

    mount->master = NULL;
    REMOVE_BIT(mount->act, ACT_PET);

    return;
}
 
void do_sail( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sail what?\n\r", ch ); 
	return; 
    }

    if ( IS_NPC( ch ) ) return;  /* not for npc */
    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	send_to_char( "You cannot sail against your will.\n\r", ch );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( !obj )
    {
	send_to_char( "You don't see that here.\n\r", ch );
	return;
    }

    /* ok, there is an object -- is it a ship?*/
    if ( obj->item_type != ITEM_BOAT )
    {
	send_to_char( "That is not a boat!\n\r", ch );
	return;
    }

    if ( obj->ship_captin != NULL )
    {
	if ( obj->ship_captin != ch->leader )
	{
	    send_to_char( "You are not the captin of this ship.\n\r", ch );
	    return;
	}
    }

    /* are we already inside */
    if ( ch->in_ship )
    {
	send_to_char( "You are already aboard a ship.\n\r", ch );
	return;
    }

    /* ok, we got a ship */
    act( "You board $p.", ch, obj, NULL, TO_CHAR );
    act( "$n boards $p and vanishes inside.", ch, obj, NULL, TO_ROOM );
    ch->in_ship 	= TRUE;
    ch->ship_name 	= obj;
    ch->ship_fuel	= obj->value[4];
    do_prompt( ch, "<%hhp %mm %vmv %Sship> ");

    if ( obj->ship_captin != NULL )
	    return;

    obj->ship_captin = ch;
    return;
}

void do_port( CHAR_DATA *ch, char *argument )
{
    /*char buf[MAX_STRING_LENGTH];*/
    OBJ_DATA *obj;

    /* are we already inside */
    if ( !ch->in_ship )
    {
	send_to_char( "You are not aboard any vessel.\n\r", ch );
	return;
    }

    obj = ch->ship_name;
    /* ok, we got a ship */
    act( "You step out from $p.", ch, ch->ship_name, NULL, TO_CHAR );
    act( "$n steps out from $p.", ch, ch->ship_name, NULL, TO_ROOM );

    ch->in_ship 			= FALSE;
    ch->ship_name 			= NULL;
    ch->ship_fuel			= 0;
    obj->ship_captin      		= NULL;
    do_prompt( ch, "<%hhp %mm %vmv> ");
    return;
}
 
void do_ride( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Ride what?\n\r", ch ); 
	return; 
    }

    if ( IS_NPC( ch ) ) return;  /* not for npc */
    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	send_to_char( "You cannot ride against your will.\n\r", ch );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( !obj )
    {
	send_to_char( "You don't see that here.\n\r", ch );
	return;
    }

    /* ok, there is an object -- is it a wagon?*/
    if ( obj->item_type != ITEM_CARRIAGE )
    {
	send_to_char( "That is not a wagon!\n\r", ch );
	return;
    }

    if ( obj->wagon_driver != NULL )
    {
	if ( obj->wagon_driver != ch->leader )
	{
	    send_to_char( "You are not the driver of this wagon.\n\r", ch );
	    return;
	}
    }

    /* are we already inside */
    if ( ch->in_wagon )
    {
	send_to_char( "You are already aboard a wagon.\n\r", ch );
	return;
    }

    /* ok, we got a vehicle */
    act( "You hop up on $p.", ch, obj, NULL, TO_CHAR );
    act( "$n hops up on $p and grabs the reins.", ch, obj, NULL, TO_ROOM );
    ch->in_wagon 	= TRUE;
    ch->wagon_name 	= obj;
    ch->wagon_fuel	= obj->value[4];
    do_prompt( ch, "<%hhp %mm %vmv %Wwagon> ");

    if ( obj->wagon_driver != NULL )
	    return;

    obj->wagon_driver = ch;
    return;
}

void do_stop( CHAR_DATA *ch, char *argument )
{
   /* char buf[MAX_STRING_LENGTH];*/
    OBJ_DATA *obj;

    /* are we already inside */
    if ( !ch->in_wagon )
    {
	send_to_char( "You are not riding a wagon.\n\r", ch );
	return;
    }

    obj = ch->wagon_name;
    /* ok, we got a wagon */
    act( "You step down from $p.", ch, ch->wagon_name, NULL, TO_CHAR );
    act( "$n steps down from $p.", ch, ch->wagon_name, NULL, TO_ROOM );

    ch->in_wagon 			= FALSE;
    ch->wagon_name 			= NULL;
    ch->wagon_fuel			= 0;
    obj->wagon_driver    		= NULL;
    do_prompt( ch, "<%hhp %mm %vmv> ");
    return;
}

void move_ship( CHAR_DATA *ch, int door, bool follow )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int move;

    if ( door < 0 || door > ( MAX_DIR - 1 ) )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    if ( ch->ship_name->ship_captin != NULL )
    {
	if ( ch->ship_name->ship_captin != ch )
	{
	     send_to_char( "You're not the pilot of the ship.\n\r", ch );
	     return;
	}
    }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room))
    {
	send_to_char( "You can't move that way.\n\r", ch );
	return;
    }
	
	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_INSIDE
	||   to_room->sector_type == SECT_INSIDE )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_CITY
	||   to_room->sector_type == SECT_CITY )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_FIELD
	||   to_room->sector_type == SECT_FIELD )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_HILLS
	||   to_room->sector_type == SECT_HILLS )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_FOREST
	||   to_room->sector_type == SECT_FOREST )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_MOUNTAIN
	||   to_room->sector_type == SECT_MOUNTAIN )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_DESERT
	||   to_room->sector_type == SECT_DESERT )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_UNDERWATER
	||   to_room->sector_type == SECT_UNDERWATER )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        
	if ( in_room->sector_type == SECT_UNDERGROUND
	||   to_room->sector_type == SECT_UNDERGROUND )
	{
 	  send_to_char( "Are you crazy? Your boat can't go there!\n\r", ch );
 	  return;
	}
        

    if (IS_SET(pexit->exit_info, EX_CLOSED)
    &&  (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))
    &&   !IS_TRUSTED(ch,ANGEL))
    {
	act( "A closed door prevents you from moving that way.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if ( !is_room_owner(ch,to_room) && room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

    move /= 2;  /* i.e. the average */

    if ( ch->ship_fuel < move )
    {
   send_to_char( "The winds have died. Your ship has stopped moving.\n\r", ch );
   return;
    }

    ch->ship_fuel 		-= move;
    ch->ship_name->value[4]  -= move;

    if ( ch->invis_level < LEVEL_HERO )
   	act( "$p sails $T.", ch, ch->ship_name, dir_name[door], TO_ROOM );

    /* move player */
    char_from_room( ch );
    char_to_room( ch, to_room );

    do_look( ch, "auto" );

    obj_from_room( ch->ship_name );
    obj_to_room( ch->ship_name, to_room );

    if (in_room == to_room) /* no circular follows */
	return;

    return;
}

void move_wagon( CHAR_DATA *ch, int door, bool follow )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int move;

    if ( door < 0 || door > ( MAX_DIR - 1 ) )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    if ( ch->wagon_name->wagon_driver != NULL )
    {
	if ( ch->wagon_name->wagon_driver != ch )
	{
	     send_to_char( "You're not the driver of this wagon.\n\r", ch );
	     return;
	}
    }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room))
    {
	send_to_char( "You can't move that way.\n\r", ch );
	return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED)
    &&  (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))
    &&   !IS_TRUSTED(ch,ANGEL))
    {
	act( "A closed door prevents you from moving that way.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if ( !is_room_owner(ch,to_room) && room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

    move /= 2;  /* i.e. the average */

    if ( ch->wagon_fuel < move )
    {
   send_to_char( "The horses cannot go any further. They are tired.\n\r", ch );
   return;
    }

    ch->wagon_fuel 		-= move;
    ch->wagon_name->value[4]  -= move;

    if ( ch->invis_level < LEVEL_HERO )
   	act( "$p leaves $T.", ch, ch->wagon_name, dir_name[door], TO_ROOM );

    /* move player */
    char_from_room( ch );
    char_to_room( ch, to_room );

    do_look( ch, "auto" );

    obj_from_room( ch->wagon_name );
    obj_to_room( ch->wagon_name, to_room );

    if (in_room == to_room) /* no circular follows */
	return;

    return;
}

/* push/drag */
void push_char( CHAR_DATA *ch, CHAR_DATA *victim, int door, bool follow )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int move;
	
    if ( door < 0 || door > (MAX_DIR -1) )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room))
    {
       act( "$n slams you against the wall!",  ch, NULL, victim, TO_VICT    );
       act( "You slam $N againts the wall!",ch, NULL, victim, TO_CHAR);
       return;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    &&   !IS_AFFECTED(ch, AFF_PASS_DOOR) )
    {
        act( "$n slams you against a closed door.",  ch, NULL, victim, TO_VICT    );
        send_to_char("The door is closed.\n\r",ch);
        return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)] + 
           movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)];

    move /= 2;  /* i.e. the average */

    /* conditional effects */
    if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE))
	move /= 2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	move *= 2;

    if ( ch->move < move )
    {
	    send_to_char( "You are too exhausted to push.\n\r", ch );
	    return;
    }

    WAIT_STATE( ch, 1 );
    ch->move -= move;

    act( "$n slams into you, pushing you $t.",ch,dir_name[door],victim,TO_VICT);
    act( "You slam into $N, pushing $M $t.",ch,dir_name[door],victim,TO_CHAR);
    act("$n slams into $N, pushing $M $t.",ch,dir_name[door],victim,TO_NOTVICT);
    char_from_room( victim );
    char_to_room( victim, to_room );
    do_look( victim, "auto" );

    act( "$n flies into the room.", victim, NULL, NULL, TO_ROOM );

    if (in_room == to_room) /* no circular follows */
	return;

    return;
}

void do_push( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int door, mob_angry;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Push who or what?\n\r", ch ); 
	return; 
    }

    /* is it an object? */
    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj )
    {
        push_obj( ch, argument );
        return;
    }

    argument=one_argument( argument, arg );

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    { 
	send_to_char( "They aren't here.\n\r", ch ); 
	return; 
    }

    if (victim == ch)
    {
	send_to_char("You push yourself around the room... wheeee\n\r", ch);
	return;
    }

    if (!IS_NPC(victim))
    {
	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
	    send_to_char("Not in this room.\n\r", ch);
	    return;
	}
    } 
 
    if ( IS_NPC(victim) && IS_SET(victim->act, ACT_IS_HEALER) )
    {
       send_to_char( "You can't push the healer!\n\r", ch);
       return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
  	send_to_char("The shopkeeper wouldn't like that.\n\r",ch);
        return;
    }
    
    if ( victim->position == POS_RESTING )
    { 
	act( "$N is resting, try dragging $M instead.", ch, NULL, victim, TO_CHAR); 
	return; 
    }

    if ( !IS_AWAKE(ch) )
    { 
	send_to_char( "In your dreams or what?\n\r", ch ); 
	return;
    }

    if ( !IS_AWAKE(victim) )
    { 
	act( "$N is asleep.  Wake $M first!", ch, NULL, victim, TO_CHAR ); 
	return; 
    }

    if ( ch->position == POS_FIGHTING )
    { 
	send_to_char( "You can't do that in the middle of a fight!\n\r", ch); 
	return;
    }

    if ( victim->position == POS_FIGHTING )
    { 
	send_to_char( "That person is fighting!\n\r", ch); 
	return;
    }

    argument=one_argument( argument, arg );
    if     (!str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = DIR_NORTH;
    else if(!str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = DIR_EAST;
    else if(!str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = DIR_SOUTH;
    else if(!str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = DIR_WEST;
    else if(!str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = DIR_UP;
    else if(!str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = DIR_DOWN;
    else
    {
	act( "Push $N where?", ch, NULL, victim, TO_CHAR ); 
	return;
    }

    if ( get_curr_stat(ch, STAT_STR) > get_curr_stat(victim, STAT_STR) )
    {
     	push_char( ch, victim, door, FALSE );
    }
    else
    {
       /* 
        * To make it fair, lets give them a 40% chance of pushing 
	*  if they fail the str? 
	*/
        if ( chance( 40 ) )
        {
     	    push_char( ch, victim, door, FALSE );
        }
        else
        {
	    /* 20% chance of the mob attacking the victim */
	    if ( chance( 20 ) )
	    {
		if ( IS_NPC( victim ) )
		{
    		    if ( get_curr_stat(ch, STAT_STR) > get_curr_stat(victim, STAT_STR) )
		    {
		    	mob_angry = number_range( 1, 5 );
			switch( mob_angry )
			{
			    case 1:
				do_say( victim, "Leave me alone!" ); break;
			    case 2:
				do_say( victim, "Do not push me!" ); break;
			    case 3:
				do_say( victim, "Get away from me!" ); break;
			    case 4:
				do_say( victim, "I am NOT enjoying this." ); break;
			    case 5:
				do_say( victim, "Stop picking on me!" ); break;
			}
		    }
		    else
		    {
		    	mob_angry = number_range( 1, 5 );
		    	switch( mob_angry )
	 	    	{
			    case 1:
		    		sprintf( buf, "I don't have to deal with this abuse!  Prepare to die, %s!", ch->name );
		    		do_say( victim, buf ); break;
			    case 2:
				do_say( victim, "Enough!  You shall die now!" );
				break;
			    case 3:
				do_say( victim, "You like pushing me, eh?  I don't like it!!!" );
				break;
			    case 4:
				sprintf( buf, "Stop pushing me %s!!!", ch->name );
				do_say( victim, buf ); break;
			    case 5:
				do_say( victim, "FOOL!  Now you shall die!" ); break;
		    	}

		        /* attack */
	    	        multi_hit( ch, victim, TYPE_UNDEFINED );
		    }
	    	}
	    }
	    else
	    {
		act("You throw your weight against $N, but $E won't budge.",
		ch,NULL,victim,TO_CHAR);

		act("$n throws $s weight against $N, but $E won't budge.",
		ch, NULL, victim, TO_NOTVICT);

		act("$n throws $s weight against you, but you won't budge.",
		ch, NULL, victim, TO_VICT);
	    }
        }
   }
   return;
}

void drag_char( CHAR_DATA *ch, CHAR_DATA *victim, int door, bool follow )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int move;
	
    if ( door < 0 || door > ( MAX_DIR -1 ) )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room)) {
        if (IS_AWAKE(victim) )  
         send_to_char( "You get dragged around the room, how humiliating!\n\r",victim ); 
        return; }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    &&   !IS_AFFECTED(ch, AFF_PASS_DOOR) )
    {
	act( "You are dragged against a close door.", victim, NULL, NULL, TO_CHAR );
        send_to_char("The door is closed!\n\r",ch);
	return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r",ch );
	return;
    }


    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	   + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)];

    move /= 2;  /* i.e. the average */

    /* conditional effects */
    if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE))
	move /= 2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	move *= 2;

    if ( ch->move < move )
    {
	    send_to_char( "You are too exhausted to drag.\n\r", ch );
	    return;
    }

    WAIT_STATE( ch, 1 );
    ch->move -= move;

    act( "$n grabs a hold of you and drags you $t!",ch,dir_name[door],victim,TO_VICT);
    act( "You grab a hold of $N, and drag $M $t!",ch,dir_name[door],victim,TO_CHAR);
    act("$n grabs a hold of $N, and drags $M $t.",ch,dir_name[door],victim,TO_NOTVICT);
    char_from_room( ch );
    char_from_room( victim );
    char_to_room( ch, to_room );
    char_to_room( victim, to_room );
    do_look( ch, "auto" );
    do_look( victim, "auto" );
    act("$n arrives, dragging $N with $m.",ch,NULL,victim,TO_NOTVICT);

    if (in_room == to_room) /* no circular follows */
	return;

    return;
}

void do_drag( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int door;

    argument=one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Drag who?\n\r", ch);
	return; 
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    { 
	send_to_char( "They aren't here.\n\r", ch ); 
	return; 
    }

    if (victim == ch)
    {
	send_to_char("Huh? How are you going to do that?\n\r", ch);
	return;
    }

    if (!IS_NPC(victim))
    {
	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
	    send_to_char("Not in this room.\n\r", ch);
	    return;
	}
    } 

    if ( IS_NPC(victim) && IS_SET(victim->act, ACT_IS_HEALER) )
    {
       send_to_char( "You can't drag the healer!\n\r", ch);
       return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
	send_to_char("The shopkeeper wouldn't like that.\n\r",ch); 
        return;
    }	

    if ( ch->position == POS_RESTING )
    { 
	send_to_char( "Maybe you should stand first?\n\r", ch);
	return;
    }

    if ( victim->position == POS_STANDING )
    { 
	act( "$N is standing.", ch, NULL, victim, TO_CHAR);
	return; 
    }

    if ( !IS_AWAKE(ch) )
    {
	send_to_char( "In your dreams or what?\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    { 
	send_to_char( "Not while you are fighting!\n\r", ch); 
	return;
    }

    argument=one_argument( argument, arg );
    if      (!str_cmp(arg, "n" ) || !str_cmp( arg, "north" ) ) door = DIR_NORTH;
    else if (!str_cmp(arg, "e" ) || !str_cmp( arg, "east"  ) ) door = DIR_EAST;
    else if (!str_cmp(arg, "s" ) || !str_cmp( arg, "south" ) ) door = DIR_SOUTH;
    else if (!str_cmp(arg, "w" ) || !str_cmp( arg, "west"  ) ) door = DIR_WEST;
    else if (!str_cmp(arg, "u" ) || !str_cmp( arg, "up"    ) ) door = DIR_UP;
    else if (!str_cmp(arg, "d" ) || !str_cmp( arg, "down"  ) ) door = DIR_DOWN;
    else
    {
	act( "Drag $N where?", ch, NULL, victim, TO_CHAR ); 
	return;
    }

    if ( get_curr_stat(ch, STAT_STR) > get_curr_stat(victim, STAT_STR) )
    {
     	drag_char( ch, victim, door, FALSE );
    }
    else
    {
        if ( chance( 40 ) )
        {
     	    drag_char( ch, victim, door, FALSE );
        }
        else
	{
	    act("You try to drag $N out of the room, but $E won't budge.",
		ch,NULL,victim,TO_CHAR);

 	    act("$n tries to drag $N out of the room, but $E won't budge.",
		ch, NULL, victim, TO_NOTVICT);

 	    act("$n tries to drag you out of the room, but you won't budge.", 
		ch, NULL, victim, TO_VICT);
	}
    }
   return;
}

void push_obj( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    int door;

    argument=one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Push what?\n\r", ch ); 
	return; 
    }

    /* search for an item in the room */
    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( !obj )
    {
       	send_to_char ( "I don't see that here.\n\r", ch);
        return;
    }

    argument=one_argument( argument, arg1 );
     if (!str_cmp(arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = DIR_NORTH;
else if (!str_cmp(arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = DIR_EAST;
else if (!str_cmp(arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = DIR_SOUTH;
else if (!str_cmp(arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = DIR_WEST;
else if (!str_cmp(arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = DIR_UP;
else if (!str_cmp(arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = DIR_DOWN;
    else
    {
	act( "Push $p where?", ch, obj, NULL, TO_CHAR ); 
	return;
    }

   /* is the item pushable? */
   if ( (!IS_SET( obj->wear_flags, ITEM_TAKE ) &&
        (!IS_SET( obj->extra_flags, ITEM_PUSHABLE ) ) ) )
   {
      	act( "You try to push $p, but it won't budge.", ch, 
		obj, NULL, TO_CHAR);
      	act( "$n tries to push $p, but it won't budge.", 
		ch, obj, NULL, TO_ROOM);
        return;
   }

    /* can we push the item? */
    if ( obj->weight > (2 * can_carry_w(ch)) )
    {
      	act( "You try to push $p, but it won't budge.", ch, 
		obj, NULL, TO_CHAR);
      	act( "$n tries to push $p, but it won't budge.", 
		ch, obj, NULL, TO_ROOM);
        return;
    }

    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   can_see_room(ch,pexit->u1.to_room) )
    {
        sprintf( buf, "You push $p %s.", dir_name[door] );
	act( buf, ch, obj, NULL, TO_CHAR );

        sprintf( buf, "$n pushes $p %s.", dir_name[door] );
	act( buf, ch, obj, NULL, TO_ROOM );

	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

   	    obj_from_room( obj );
   	    obj_to_room( obj, to_room );
            sprintf( buf, "$p flies into the room from %s.", dir_rev_name[door] );

	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( buf, rch, obj, NULL, TO_CHAR );
	}
    }
    else
    {
	send_to_char( "You can't do that.\n\r", ch );
    }
    return;
}
