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
 *  File: bit.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was written by Jason Dinkel and inspired by Russ Taylor,     *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
   The code below uses a table lookup system that is based on suggestions
   from Russ Taylor.  There are many routines in handler.c that would benefit
   with the use of tables.  You may consider simplifying your code base by
   implementing a system like below with such functions. -Jason Dinkel
 */


#define unix 1
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
#include "interp.h"



struct flag_stat_type {
    const struct flag_type *structure;
    bool stat;
};



/*****************************************************************************
 Name:		flag_stat_table
 Purpose:	This table catagorizes the tables following the lookup
 		functions below into stats and flags.  Flags can be toggled
 		but stats can only be assigned.  Update this table when a
 		new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] =
{
/*  {   structure               stat    }, */
    {area_flags, FALSE},
    {sex_flags, TRUE},
    {exit_flags, FALSE},
    {door_resets, TRUE},
    {room_flags, FALSE},
    {room2_flags, FALSE},
    {sector_flags, TRUE},
    {type_flags, TRUE},
    {extra_flags, FALSE},
    {anti_race_flags, FALSE},
    {anti_class_flags, FALSE},
    {wear_flags, FALSE},
    {act_flags, FALSE},
    {act2_flags, FALSE},
    {affect_flags, FALSE},
    {affect2_flags, FALSE},
    {apply_flags, TRUE},
    {wear_loc_flags, TRUE},
    {wear_loc_strings, TRUE},
    {weapon_flags, TRUE},
    {container_flags, FALSE},
    {liquid_flags, TRUE},
    {immune_flags, FALSE},

    {0, 0}
};



/*****************************************************************************
 Name:		is_stat( table )
 Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in bit.c.
 ****************************************************************************/
bool is_stat(const struct flag_type *flag_table)
{
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++) {
	if (flag_stat_table[flag].structure == flag_table
	    && flag_stat_table[flag].stat)
	    return TRUE;
    }
    return FALSE;
}



/*
 * This function is Russ Taylor's creation.  Thanks Russ!
 * All code copyright (C) Russ Taylor, permission to use and/or distribute
 * has NOT been granted.  Use only in this OLC package has been granted.
 */
/*****************************************************************************
 Name:		flag_lookup( flag, table )
 Purpose:	Returns the value of a single, settable flag from the table.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in bit.c.
 ****************************************************************************/
int flag_lookup(const char *name, const struct flag_type *flag_table)
{
    int flag;

    for (flag = 0; *flag_table[flag].name; flag++) {	/* OLC 1.1b */
	if (!str_cmp(name, flag_table[flag].name)
	    && flag_table[flag].settable)
	    return flag_table[flag].bit;
    }

    return NO_FLAG;
}



/*****************************************************************************
 Name:		flag_value( table, flag )
 Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
 Called by:	olc.c and olc_act.c.
 ****************************************************************************/
int flag_value(const struct flag_type *flag_table, char *argument)
{
    char word[MAX_INPUT_LENGTH];
    int bit;
    int marked = 0;
    bool found = FALSE;

    if (is_stat(flag_table)) {
	one_argument(argument, word);

	if ((bit = flag_lookup(word, flag_table)) != NO_FLAG)
	    return bit;
	else
	    return NO_FLAG;
    }
    /*
     * Accept multiple flags.
     */
    for (;;) {
	argument = one_argument(argument, word);

	if (word[0] == '\0')
	    break;

	if ((bit = flag_lookup(word, flag_table)) != NO_FLAG) {
	    SET_BIT(marked, bit);
	    found = TRUE;
	}
    }

    if (found)
	return marked;
    else
	return NO_FLAG;
}



/*****************************************************************************
 Name:		flag_string( table, flags/stat )
 Purpose:	Returns string with name(s) of the flags or stat entered.
 Called by:	act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *flag_string(const struct flag_type *flag_table, int bits)
{
    static char buf[512];
    int flag;

    buf[0] = '\0';

    for (flag = 0; *flag_table[flag].name; flag++) {	/* OLC 1.1b */
	if (!is_stat(flag_table) && IS_SET(bits, flag_table[flag].bit)) {
	    strcat(buf, " ");
	    strcat(buf, flag_table[flag].name);
	} else if (flag_table[flag].bit == bits) {
	    strcat(buf, " ");
	    strcat(buf, flag_table[flag].name);
	    break;
	}
    }
    return (buf[0] != '\0') ? buf + 1 : "none";
}
