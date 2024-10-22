/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 * player.c was created on 3-6-2001 by Beowolf. This file should be used   *
 * for player commands that are added to the game. This will help keep     *
 * the code mess down. Please do not delete these headers. Thanks!         * 
 *                                                                         *
 ***************************************************************************/

#define unix 1
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "merc.h"
#include "interp.h"


/*
 * Local functions.
 */
bool get_obj args((CHAR_DATA * ch, OBJ_DATA * obj,
		   OBJ_DATA * container));




/* 
 * Joe:Lockers        Check if theres a locker in the room 
 * and if so return the obj data
 * Locker Code written by Joe. Added 3/14/01 by Beowolf 
 */
OBJ_DATA *get_obj_locker_room(CHAR_DATA * ch, int vnum)
{
    OBJ_DATA *obj;

    for (obj = ch->in_room->contents; obj; obj = obj->next_content)
	if (obj->pIndexData->vnum == vnum)
	    return obj;
    return NULL;
}


/*
 * Find an obj in player's inventory without checking 
 * for invis blind etc.... Used for lockers
 */
OBJ_DATA *get_locker(CHAR_DATA * ch)
{
    OBJ_DATA *obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
	if (obj->pIndexData->vnum == LOCKER_VNUM)
	    return obj;
    }

    return NULL;
}


void do_chest(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *container;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (ch->in_room->vnum != LOCKER_ROOM) {
	send_to_char("You are not in the Chest Room.\n\r", ch);
	return;
    }
    if ((container = get_locker(ch)) == NULL) {
	send_to_char("Please contact an immortal and tell them chests don't work.", ch);
	return;
    }
    if (!str_cmp(arg1, "get")) {
	/* 'get obj container' */
	obj = get_obj_list(ch, arg2, container->contains);

	if (obj == NULL) {
	    act("Your chest doesn't contain that.", ch, NULL, arg2, TO_CHAR);
	    return;
	}
	act("$n gets $p from $s chest.", ch, obj, container, TO_ROOM);
	act("You get $p from your chest.", ch, obj, container, TO_CHAR);
	get_obj(ch, obj, container);
    } else if (!str_cmp(arg1, "put")) {
	/* First get the object from inventory */
	if ((obj = get_obj_carry(ch, arg2)) == NULL) {
	    send_to_char("You do not have that item.\n\r", ch);
	    return;
	}
	/* Now check to see if meets conditions */
	if (obj->item_type == ITEM_CONTAINER) {
	    send_to_char("Containers are not allowed in chests.\n\r", ch);
	    return;
	}
	if (!can_drop_obj(ch, obj)) {
	    send_to_char("You can't let go of it.\n\r", ch);
	    return;
	}
	/* Put obj in locker */
	obj_from_char(obj);
	obj_to_obj(obj, container);

	act("$n puts $p in $s chest.", ch, obj, container, TO_ROOM);
	act("You put $p in your chest.", ch, obj, container, TO_CHAR);
    } else if (!str_cmp(arg1, "list")) {
	do_look(ch, "in locker");
	return;
    } else {
	send_to_char("Syntax :\n\r", ch);
	send_to_char("chest put *object*\n\r", ch);
	send_to_char("chest get *object*\n\r", ch);
	send_to_char("chest list.\n\r", ch);
    }
    return;
}

/* END OF LOCKERS */

/*
 * This command checks for hidden objects
 * Originally written for ROM by whiplash@tft.nacs.net 
 * Added 3/16/01 - By Beowolf
 */
void do_search(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;


    if (!IS_NPC(ch)
	&& !can_use_skpell(ch, gsn_search)) {
	send_to_char("You search around clumsily.\n\r", ch);
	return;
    }
    if (number_percent() < ch->pcdata->learned[gsn_search]) {
	send_to_char("You search the room..\n\r", ch);
	update_skpell(ch, gsn_search);

	for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content) {
	    if (IS_SET(obj->extra_flags, ITEM_HIDDEN)) {
		printf_to_char(ch, "You reveal %s\n\r", obj->short_descr);
		REMOVE_BIT(obj->extra_flags, ITEM_HIDDEN);
	    }
	}
	send_to_char("You have searched everywhere.\n\r", ch);
	WAIT_STATE(ch, 24);
    } else
	send_to_char("You didn't uncover anything unusual.\n\r", ch);
    update_skpell(ch, gsn_search);
    WAIT_STATE(ch, 24);
    return;
}


/* 
 * Rent code by Ryouga (ryouga@jessi.indstate.edu)
 * Added by Beowolf on 3/16/01
 */
void do_rental(CHAR_DATA * ch, char *argument)
{

    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->in_room->room2_flags, ROOM_RENTAL)) {
	if (IS_SET(ch->act2, PLR2_TENNANT)) {
	    send_to_char("You haven't checked out from last time, yet!\n\r", ch);
	    return;
	}
	send_to_char("You rent the room.\n\r", ch);
	REMOVE_BIT(ch->in_room->room2_flags, ROOM_RENTAL);
	SET_BIT(ch->in_room->room2_flags, ROOM_PRIVATE);
	SET_BIT(ch->in_room->room2_flags, ROOM_RENTED);
	SET_BIT(ch->act2, PLR2_TENNANT);
	ch->money.silver -= 50;
	return;
    } else {
	send_to_char("You can't rent this room.\n\r", ch);
	return;
    }
}


void do_checkout(CHAR_DATA * ch, char *argument)
{

    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->in_room->room2_flags, ROOM_RENTED)) {
	if (IS_SET(ch->act2, PLR2_TENNANT)) {
	    send_to_char("You check out of the room.\n\r", ch);
	    REMOVE_BIT(ch->act2, PLR2_TENNANT);
	    REMOVE_BIT(ch->in_room->room2_flags, ROOM_RENTED);
	    SET_BIT(ch->in_room->room2_flags, ROOM_RENTAL);
	    ch->pcdata->rentroom = 0;
	    return;
	} else {
	    send_to_char("This is not your room.\n\r", ch);
	    return;
	}
    }
}
