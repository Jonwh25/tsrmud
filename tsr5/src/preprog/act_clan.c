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
 * Clan routines for clan funtions.. I hope to move the ones created prior *
 * to this file in here eventually as well.                                *
 * -- Altrag Dalosein, Lord of the Dragons                                 *
 ***************************************************************************/

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"


/*
 * Illuminati bestow command, for deity only.
 */
void do_bestow(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (ch->clan != 1 || ch->clev < 5) {
	send_to_char("{wHuh?{x\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg);

    if (!(victim = get_char_room(ch, arg)) || IS_NPC(victim)) {
	send_to_char("{wThey aren't here.{x\n\r", ch);
	return;
    }
    if (victim->clan != ch->clan) {
	send_to_char("{wThey aren't in your clan!{x\n\r", ch);
	return;
    }
    if (!IS_SET(victim->act, PLR_CSKILL)) {
	SET_BIT(victim->act, PLR_CSKILL);
	act("{w$N bestowed with the Transmute skill.{x", ch, NULL, victim,
	    TO_CHAR);
	send_to_char("{wYou have been given the Transmute skill.{x\n\r",
		     victim);
    } else {
	REMOVE_BIT(victim->act, PLR_CSKILL);
	act("{wTransmute denied from $N.{x", ch, NULL, victim, TO_CHAR);
	send_to_char("{wYou have been denied the Transmute skill.{x\n\r",
		     victim);
    }
}


/*
 * Illuminati transmute skill, must be given to a member by the deity
 */
void do_transmute(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    int chance;

    if (ch->clan != 1 || !IS_SET(ch->act, PLR_CSKILL)
	|| IS_NPC(ch)) {
	send_to_char("{wHuh?{x\n\r", ch);
	return;
    }
    if (!(obj = get_obj_carry(ch, argument))) {
	send_to_char("{wYou do not have that item.{x\n\r", ch);
	return;
    }
    if (obj->level > 0)
	chance = (ch->level * 75) / obj->level;
    else
	chance = 85;

    if (number_percent() < chance) {
	obj->extra_flags |= ITEM_NO_DAMAGE;
	act("{w$p transmuted.{x", ch, obj, NULL, TO_CHAR);
	return;
    }
    act("{wYou failed!  $p exploded in your face!{x", ch, obj, NULL,
	TO_CHAR);
    extract_obj(obj);
    return;
}


/*
 * ??? Doomshield skill
 */
void do_doomshield(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA af;

    if (ch->clan != -1) {
	send_to_char("{wHuh?{x\n\r", ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_DOOMSHIELD))
	return;

    af.type = gsn_doomshield;
    af.level = ch->level;
    af.duration = 5;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_DOOMSHIELD;
    affect_to_char2(ch, &af);
    act("{w$n is surrounded with an unholy aura.{x", ch, NULL,
	NULL, TO_ROOM);
    send_to_char("{wYou are surrounded with an unholy aura.{x\n\r",
		 ch);
    return;
}

void do_unholystrength(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA af;

    if (ch->clan != 8) {
	send_to_char("{wHuh?{x\n\r", ch);
	return;
    }
    if (ch->fighting || ch->ctimer || IS_AFFECTED2(ch, AFF_UNHOLYSTRENGTH)) {
	send_to_char("{wYou failed.{x\n\r", ch);
	return;
    }
    af.type = gsn_unholystrength;
    af.level = ch->level;
    af.duration = -1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_UNHOLYSTRENGTH;
    affect_to_char2(ch, &af);
/*  ch->ctimer = 17; */

    send_to_char("{wYou feel the strength of the Unholy Plague run through you.{x\n\r", ch);
    return;
}

void do_image(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA af;

    if (ch->clan != 3) {
	send_to_char("{wHuh?{x\n\r", ch);
	return;
    }
    if (ch->ctimer || ch->combat_timer) {
	send_to_char("{wYou can't right now.{x\n\r", ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_IMAGE))
	return;

    af.location = APPLY_NONE;
    af.modifier = number_range(1, 5);
    af.duration = number_fuzzy(3);
    af.bitvector = AFF_IMAGE;
    af.type = gsn_image;
    af.level = ch->level;
    ch->ctimer = af.duration + 10;
    affect_to_char2(ch, &af);
    send_to_char("{wYou are surrounded by images of the Talisman.{x\n\r",
		 ch);
    act("{w$n suddenly splits into many shimmering images.{x", ch, NULL, NULL, TO_ROOM);
    return;
}
/* END */
