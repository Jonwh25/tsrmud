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
/*
 * Scan.c has been added by Akasha for use on TSR.
 * Date: 03-11-2001
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


int scan_room(CHAR_DATA * ch, const ROOM_INDEX_DATA * room, char *buf)
{
    CHAR_DATA *target = room->people;
    int number_found = 0;

    while (target != NULL) {	/* Will repeat as long there are more people in the room */
	if (can_see(target, ch)) {	/* If the character can see the target */
	    strcat(buf, " - ");
	    strcat(buf, IS_NPC(target) ? target->short_descr : target->name);
	    strcat(buf, "\n\r");
	    number_found++;
	}
	target = target->next_in_room;
    }

    return number_found;
}

void do_scan(CHAR_DATA * ch, char *argument)
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *room;
    extern char *const dir_name[];
    char buf[MAX_STRING_LENGTH];
    int dir;
    int distance;

    sprintf(buf, "Right here you see:\n\r");
    if (scan_room(ch, ch->in_room, buf) == 0) {
	strcat(buf, "Noone\n\r");
    }
    send_to_char(buf, ch);

    for (dir = 0; dir < 6; dir++) {	/* To look in all 10 directions */
	room = ch->in_room;	/* Sets the starting point, feeds the for loop */

	for (distance = 1; distance < 4; distance++) {
	    pexit = room->exit[dir];	/* Will find the door leading to the next room */
	    if ((pexit == NULL) || (pexit->to_room == NULL) ||
		(IS_SET(pexit->exit_info, EX_CLOSED))) {
		break;		/* If exit is not there OR if exit points to nothing OR if the exit is closed */
	    }
	    /* If the character can see the room */
	    sprintf(buf, "%d %s from here you see:\n\r", distance, dir_name[dir]);
	    if (scan_room(ch, pexit->to_room, buf)) {	/* If something is in the room */
		send_to_char(buf, ch);
	    }
	    room = pexit->to_room;	/* Goes to the next room, continues the for loop */
	}			/* end for distance */
    }				/* end for dir */
}

/*
 * Code for Scan from current TSR interp.c file

 char *const distance[4]=
 {
 "right here.", "nearby to the %s.", "not far %s.", "off in the distance %s."
 };

 void scan_list           args((ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch,
 sh_int depth, sh_int door));
 void scan_char           args((CHAR_DATA *victim, CHAR_DATA *ch,
 sh_int depth, sh_int door));
 void do_scan(CHAR_DATA *ch, char *argument)
 {
 extern char *const dir_name[];
 char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
 ROOM_INDEX_DATA *scan_room;
 EXIT_DATA *pExit;
 sh_int door, depth;

 argument = one_argument(argument, arg1);

 if (arg1[0] == '\0')
 {
 act("$n looks all around.", ch, NULL, NULL, TO_ROOM);
 send_to_char("Looking around you see:\n\r", ch);
 scan_list(ch->in_room, ch, 0, -1);

 for (door=0;door<10;door++)
 {
 if ((pExit = ch ->in_room->exit[door]) != NULL)
 scan_list(pExit->u1.to_room, ch, 1, door);
 }
 return;
 }
 else if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))     door = 0;
 else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))      door = 1;
 else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))     door = 2;
 else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))      door = 3;
 else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up" ))       door = 4;
 else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))      door = 5;
 else if (!str_cmp(arg1,"ne") || !str_cmp(arg1,"northeast"))  door = 6;
 else if (!str_cmp(arg1,"nw") || !str_cmp(arg1,"northwest"))  door = 7;
 else if (!str_cmp(arg1,"se") || !str_cmp(arg1,"southeast"))  door = 8;
 else if (!str_cmp(arg1,"sw") || !str_cmp(arg1,"southwest"))  door = 9;
 else { send_to_char("Which way do you want to scan?\n\r", ch); return; }

 act("You peer intently $T.", ch, NULL, dir_name[door], TO_CHAR);
 act("$n peers intently $T.", ch, NULL, dir_name[door], TO_ROOM);
 sprintf(buf, "Looking %s you see:\n\r", dir_name[door]);

 scan_room = ch->in_room;

 for (depth = 1; depth < 4; depth++)
 {
 if ((pExit = scan_room->exit[door]) != NULL)
 {
 scan_room = pExit->u1.to_room;
 scan_list(pExit->u1.to_room, ch, depth, door);
 }
 }
 return;
 }

 void scan_list(ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch, sh_int depth,
 sh_int door)
 {
 CHAR_DATA *rch;

 if (scan_room == NULL) return;
 for (rch=scan_room->people; rch != NULL; rch=rch->next_in_room)
 {
 if (rch == ch) continue;
 if (!IS_NPC(rch) && rch->invis_level > get_trust(ch)) continue;
 if (can_see(ch, rch)) scan_char(rch, ch, depth, door);
 }
 return;
 }

 void scan_char(CHAR_DATA *victim, CHAR_DATA *ch, sh_int depth, sh_int door)
 {
 extern char *const dir_name[];
 extern char *const distance[];
 char buf[MAX_INPUT_LENGTH], buf2[MAX_INPUT_LENGTH];

 buf[0] = '\0';

 strcat(buf, PERS(victim, ch));
 strcat(buf, ", ");
 sprintf(buf2, distance[depth], dir_name[door]);
 strcat(buf, buf2);
 strcat(buf, "\n\r");

 send_to_char(buf, ch);
 return;
 }
 * End Current TSR code for Scan
 */
