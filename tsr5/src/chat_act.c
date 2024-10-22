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
 * Chat_act.c  - Merc-based confrence system.                              *
 *                                                                         *
 * Full chat-mode setup for Merc based MUDS, or could be used separately   *
 * if you cut and pasted the needed parts from Merc code.                  *
 * Created as an addon for Eye of the Storm MUD (network.sos.on.ca 1234)   *
 * -- Altrag Dalosein, Lord of the Dragons..                               *
 ***************************************************************************/

#define unix 1
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "chatmode.h"

bool chat_command(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];

    if (*argument != '/' && *argument != '>')
	return FALSE;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_room_stuff(ch);
	return TRUE;
    }
    if (arg[0] == '>') {
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	if (!(victim = get_char_chat(ch, arg + 1))
	    || victim->in_room != ch->in_room) {
	    send_to_char("They aren't here.\n\r", ch);
	    return TRUE;
	}
	act(buf, ch, argument, victim, TO_NOTVICT);
	act("$N (to you): $t", victim, argument, ch,
	    TO_CHAR);
	act("-- Message directed to &$t$N --", ch, victim,
	    victim, TO_CHAR);
	return TRUE;
    }
    if (!str_prefix(arg + 1, "quit"))
	stop_chat_mode(ch);
    else if (!str_prefix(arg + 1, "page"))
	chat_page(ch, argument);
    else if (!str_prefix(arg + 1, "rooms"))
	chat_show(ch, argument);
    else if (!str_prefix(arg + 1, "who"))
	chat_who(ch, argument);
    else if (!str_prefix(arg + 1, "topic"))
	chat_topic(ch, argument);
    else if (!str_prefix(arg + 1, "help"))
	chat_help(ch, argument);
    else if (!str_prefix(arg + 1, "join"))
	chat_join(ch, argument);
    else if (!str_prefix(arg + 1, "social"))
	do_socials(ch, argument);
    else if (!str_prefix(arg + 1, "invite"))
	chat_invite(ch, argument);
    else if (!str_prefix(arg + 1, "uninvite"))
	chat_uninvite(ch, argument);
    else {
	CHAR_DATA *victim;

	if (!(victim = get_char_chat(ch, arg + 1))
	    || victim->in_room != ch->in_room) {
	    send_to_char("They aren't here.\n\r", ch);
	    return TRUE;
	}
	act("$N (Priv): $t", victim, argument, ch,
	    TO_CHAR);
	act("-- Message sent only to &$t$N --", ch, victim,
	    victim, TO_CHAR);
    }
    return TRUE;
}

void chat_page(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg);

    if (!(victim = get_char_chat(ch, arg))) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    act("$N (PAGE): $t", victim, argument, ch,
	TO_CHAR);
    act("-- Page sent to &$t$N --", ch, victim,
	victim, TO_CHAR);
    return;
}

void chat_join(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAT_ROOM *room;

    argument = one_argument(argument, arg);
    if (arg[0] == '\0') {
	bool ToMain = FALSE;

	if (!str_prefix(ch->name, ch->in_room->name) ||
	    is_name(NULL, ch->name, ch->in_room->name))
	    ToMain = TRUE;
	act("$n has left the room.", ch, NULL, NULL, TO_ROOM);
	char_from_room(ch);
	if (ToMain) {
	    char_to_room(ch, chat_rooms->pRoom);
	    send_room_stuff(ch);
	    act("$n has entered the room.", ch, NULL, NULL,
		TO_ROOM);
	    return;
	}
	for (room = chat_rooms; room; room = room->next) {
	    if (is_name(NULL, ch->name, room->pRoom->name)) {
		char_to_room(ch, room->pRoom);
		send_room_stuff(ch);
		act("$n has entered the room.", ch, NULL, NULL,
		    TO_ROOM);
		return;
	    }
	}
	room = alloc_mem(sizeof(*room));
	room->invited = str_dup("");
	room->pRoom = alloc_mem(sizeof(*room->pRoom));
	room->pRoom->name = str_dup(ch->name);
	sprintf(buf, "%s'%s room.", ch->name,
		(ch->name[strlen(ch->name) - 1] == 's' ? "" : "s"));
	room->pRoom->description = str_dup(buf);
	room->pRoom->people = NULL;
	last_chat_room->next = room;
	last_chat_room = room;
	char_to_room(ch, room->pRoom);
	send_to_char("!! Room Created !!\n\r", ch);
	send_room_stuff(ch);
	return;
    }
    if (!str_prefix(arg, ch->name) || is_name(NULL, arg, ch->name)
	|| !str_cmp(arg, "self")) {
	chat_join(ch, "");
	return;
    }
    for (room = chat_rooms; room; room = room->next)
	if (!str_prefix(arg, room->pRoom->name) ||
	    is_name(NULL, arg, room->pRoom->name)) {
	    if (!is_name(NULL, ch->name, room->invited) &&
		str_cmp(room->pRoom->name, "Main") &&
		!is_name(NULL, ch->name, room->pRoom->name) &&
		!is_name(NULL, "All", room->invited)) {
		send_to_char("You are not invited to that room.\n\r", ch);
		return;
	    }
	    act("$n has left the room.", ch, NULL, NULL, TO_ROOM);
	    char_from_room(ch);
	    char_to_room(ch, room->pRoom);
	    send_room_stuff(ch);
	    act("$n has entered the room.", ch, NULL, NULL, TO_ROOM);
	    return;
	}
    send_to_char("!! Room does not exist !!\n\r", ch);
    return;
}

void chat_show(CHAR_DATA * ch, char *argument)
{
    CHAT_ROOM *room;
    char buf[MAX_STRING_LENGTH];
    int rcount = 0;

    buf[0] = '\0';

    for (room = chat_rooms; room; room = room->next) {
	sprintf(buf + strlen(buf), "%-16s%s\n\r", room->pRoom->name,
		room->pRoom->description);
	rcount++;
    }
    sprintf(buf + strlen(buf), "There %s %d active room%s.\n\r",
	    (rcount == 1 ? "is" : "are"),
	    rcount,
	    (rcount == 1 ? "" : "s"));
    send_to_char(buf, ch);
    return;
}

void chat_topic(CHAR_DATA * ch, char *argument)
{
    CHAT_ROOM *room;

    for (room = chat_rooms; room; room = room->next)
	if (room->pRoom == ch->in_room)
	    break;
    if (!room)
	return;
    if (!is_name(NULL, ch->name, room->pRoom->name)) {
	send_to_char("You are not in your room.\n\r", ch);
	return;
    }
    free_string(room->pRoom->description);
    room->pRoom->description = str_dup(argument);
    act("$n has changed the topic to '$t'.", ch,
	argument, NULL, TO_ROOM);
    return;
}

void chat_who(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int rcount = 0;

    argument = one_argument(argument, arg);
    if (!str_cmp(arg, "game")) {
	do_who(ch, argument);
	return;
    }
    buf[0] = '\0';

    for (victim = chat_list; victim; victim = victim->next) {
	sprintf(buf + strlen(buf), "%-16s%-13s%s\n\r",
		victim->name, victim->in_room->name,
		victim->in_room->description);
	rcount++;
    }

    sprintf(buf + strlen(buf), "There %s %d %s in the conference.\n\r",
	    (rcount == 1 ? "is" : "are"),
	    rcount,
	    (rcount == 1 ? "person" : "people"));
    send_to_char(buf, ch);
    return;
}

void chat_help(CHAR_DATA * ch, char *argument)
{
    send_to_char(
		    "/help                  This help screen\n\r", ch);
    send_to_char(
		    "/join <channel>        Join a channel\n\r", ch);
    send_to_char(
		"/page <who> <msg>      Global private message\n\r", ch);
    send_to_char(
	       "/<who> <msg>           Channel private message\n\r", ch);
    send_to_char(
	      "><who> <msg>           Channel directed message\n\r", ch);
    send_to_char(
		    "/quit                  Quit the conference\n\r", ch);
    send_to_char(
		    "/rooms                 List active rooms\n\r", ch);
    send_to_char(
		    "/topic <topic>         Set room topic\n\r", ch);
    send_to_char(
	     "/who ['game' [name]]   List people in conference\n\r", ch);
    send_to_char(
	   "/invite [who]          Invite someone to your room\n\r", ch);
    send_to_char(
	    "/uninvite <who>        Uninvite someone from your room\n\r",
		    ch);
    send_to_char(
		  "/social                List all the socials\n\r", ch);
    send_to_char(
		    ".<social>              Perform a social\n\r", ch);
    send_to_char(
		    ",<msg>                 Emote something\n\r", ch);
    return;
}

void chat_invite(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAT_ROOM *room;
    CHAR_DATA *victim;
    char name[MAX_STRING_LENGTH];

    for (room = chat_rooms; room; room = room->next)
	if (is_name(NULL, room->pRoom->name, ch->name))
	    break;
    if (!room) {
	send_to_char("You do not have a room!\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	char *pInv;
	char iArg[MAX_STRING_LENGTH];
	int width = 0;
	int ncount = 0;

	if (is_name(NULL, room->invited, "all")) {
	    send_to_char("All\n\r\n\r", ch);
	    return;
	}
	pInv = room->invited;
	for (pInv = one_argument(pInv, iArg); iArg[0] != '\0';
	     pInv = one_argument(pInv, iArg)) {
	    width += strlen(iArg) + 1;
	    ncount++;
	    if (width >= 79) {
		send_to_char("\n\r", ch);
		width = 0;
	    }
	    send_to_char(iArg, ch);
	}
	if (ncount == 0)
	    send_to_char("No one is invited to your room.", ch);
	send_to_char("\n\r\n\r", ch);
	return;
    }
    if (!(victim = get_char_chat(ch, arg)) && str_cmp(arg, "all")) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (!victim)
	strcpy(name, "All");
    else {
	if (victim == ch) {
	    send_to_char("You invite yourself.\n\r", ch);
	    return;
	}
	strcpy(name, victim->name);
	act("You have been invited to &$t$N's room.", victim,
	    ch, ch, TO_CHAR);
    }
    if (is_name(NULL, name, room->invited)) {
	send_to_char("They are already invited.\n\r", ch);
	return;
    }
    strcat(name, " ");
    send_to_char(name, ch);
    send_to_char("invited.\n\r", ch);
    strcat(name, room->invited);
    free_string(room->invited);
    room->invited = str_dup(name);
}

void chat_uninvite(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char name[MAX_STRING_LENGTH];
    CHAT_ROOM *room;

    for (room = chat_rooms; room; room = room->next)
	if (is_name(NULL, room->pRoom->name, ch->name))
	    break;
    if (!room) {
	send_to_char("You do not have a room!\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Uninvite whom?\n\r", ch);
	return;
    }
    if (!(victim = get_char_chat(ch, arg)) && str_cmp(arg, "all")) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (!victim)
	strcpy(name, "All ");
    else {
	if (victim == ch) {
	    send_to_char("You can't kick yourself out!\n\r", ch);
	    return;
	}
	sprintf(name, "%s ", victim->name);
	if (is_name(NULL, name, room->invited)) {
	    act("You have been uninvited from &$t$N's room.", victim,
		ch, ch, TO_CHAR);
	    if (victim->in_room == room->pRoom)
		chat_command(victim, "/join Main");
	}
    }
    if (!is_name(NULL, name, room->invited)) {
	send_to_char("They are not invited!\n\r", ch);
	return;
    }
    room->invited = string_replace(room->invited, name, "");
    send_to_char(name, ch);
    send_to_char("uninvited.\n\r", ch);
}
