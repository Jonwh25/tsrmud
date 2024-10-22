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
 *  File: olc_help.c                                                       *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This work is a derivative of Talen's post to the Merc Mailing List.    *
 *  It has been modified by Jason Dinkel to work with the new OLC.         *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
*    TSR versions I, II, III, IV, & IV copyright 1995-2000 Jon Whitehouse  *
*	TSR has been brought to you by Jonathan Whitehouse AKA: 	   *
*	          Beowolf & Darkoth - jonwh@lords.com			   *
*	By using this code, you have agreed to follow the terms of the	   *
*	TSR license, in the file realms/doc/tsr.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "olc.h"


#define HEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define EDIT_HELP(Ch, Help)	( Help = (HELP_DATA *)Ch->desc->pEdit )

/*
 * Help Editor Prototypes
 */
DECLARE_OLC_FUN(hedit_create);
DECLARE_OLC_FUN(hedit_delete);
DECLARE_OLC_FUN(hedit_desc);
DECLARE_OLC_FUN(hedit_level);
DECLARE_OLC_FUN(hedit_keywords);
DECLARE_OLC_FUN(hedit_show);


void free_help args((HELP_DATA * pHelp));

const struct olc_cmd_type hedit_table[] =
{
/*  {   command         function        }, */

    {"commands", show_commands},
    {"create", hedit_create},
    {"delete", hedit_delete},
    {"desc", hedit_desc},
    {"level", hedit_level},
    {"keywords", hedit_keywords},
    {"show", hedit_show},

    {"?", show_help},

    {"", 0,}
};



void hedit(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (ch->pcdata->security == 0)
	send_to_char("HEdit: Insufficient security to modify area.\n\r", ch);

    if (command[0] == '\0') {
	hedit_show(ch, argument);
	return;
    }
    if (!str_cmp(command, "done")) {
	edit_done(ch);
	return;
    }
    if (ch->pcdata->security == 0) {
	interpret(ch, arg);
	return;
    }
    /* Search Table and Dispatch Command. */
    for (cmd = 0; hedit_table[cmd].name[0] != '\0'; cmd++) {
	if (!str_cmp(command, hedit_table[cmd].name)) {
	    (*hedit_table[cmd].olc_fun) (ch, argument);
	    return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
}



/* Entry point for editing help_data. */
void do_hedit(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    HELP_DATA *iHelp;

    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Syntax:  edit help <keywords>\n\r", ch);
	return;
    } else {
	for (iHelp = help_first; iHelp; iHelp = iHelp->next) {
	    /*
	     * This help better not exist already!
	     */
	    if (is_name(ch, arg, iHelp->keyword)) {
		ch->desc->pEdit = (void *) iHelp;
		ch->desc->editor = ED_HELP;
		break;
	    }
	}

	if (!iHelp) {
	    iHelp = new_help();
	    iHelp->keyword = str_dup(arg);

	    if (!help_first)
		help_first = iHelp;
	    if (help_last)
		help_last->next = iHelp;

	    help_last = iHelp;
	    iHelp->next = NULL;
	    ch->desc->pEdit = (void *) iHelp;
	    ch->desc->editor = ED_HELP;
	}
    }
    return;
}



HEDIT(hedit_show)
{
    HELP_DATA *pHelp;
    char buf[MAX_STRING_LENGTH];

    if (!EDIT_HELP(ch, pHelp)) {
	send_to_char("Null help file.\n\r", ch);
	return FALSE;
    }
    sprintf(buf,
	    "Seen at level: [%d]\n\r"
	    "Keywords:      [%s]\n\r"
	    "Text:\n\r%s\n\r",
	    pHelp->level, pHelp->keyword, pHelp->text);
    send_to_char(buf, ch);

    return FALSE;
}



HEDIT(hedit_create)
{
    HELP_DATA *iHelp;
    HELP_DATA *NewHelp;
    char buf[MAX_STRING_LENGTH];

    if (!EDIT_HELP(ch, iHelp)) {
	send_to_char("Null help file.\n\r", ch);
	return FALSE;
    }
    if (argument[0] == '\0') {
	send_to_char("Syntax: create <keywords>\n\r", ch);
	return FALSE;
    }
    /*
     * This help better not exist already!
     */
    for (iHelp = help_first; iHelp; iHelp = iHelp->next) {
	if (is_name(ch, argument, iHelp->keyword)) {
	    send_to_char("That help file already exists.\n\r", ch);
	    return FALSE;
	}
    }

    NewHelp = new_help();
    NewHelp->keyword = str_dup(argument);

    if (!help_first)		/* If it is we have a leak */
	help_first = NewHelp;
    if (help_last)
	help_last->next = NewHelp;

    help_last = NewHelp;
    NewHelp->next = NULL;
    ch->desc->pEdit = (void *) NewHelp;
    ch->desc->editor = ED_HELP;

    sprintf(buf, "Created help with the keyword(s): %s\n\r",
	    NewHelp->keyword);
    send_to_char(buf, ch);

    return TRUE;
}



bool hedit_delete(CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    HELP_DATA *pMark;
    EDIT_HELP(ch, pHelp);

    if (argument[0] != '\0') {
	send_to_char("Type delete by itself.\n\r", ch);
	return FALSE;
    }
    if (pHelp == help_first) {
	help_first = pHelp->next;
	free_string(pHelp->keyword);
	free_string(pHelp->text);
	free_mem(pHelp, sizeof(*pHelp));
	top_help--;
	ch->desc->pEdit = NULL;
	ch->desc->editor = 0;
	send_to_char("Deleted.\n\r", ch);
	return TRUE;
    }
    for (pMark = help_first; pMark; pMark = pMark->next) {
	if (pHelp == pMark->next) {
	    pMark->next = pHelp->next;
/*      pHelp->next = help_free;
   help_free = pHelp; */
	    free_string(pHelp->keyword);
	    free_string(pHelp->text);
	    free_mem(pHelp, sizeof(*pHelp));
	    top_help--;
	    ch->desc->pEdit = NULL;
	    ch->desc->editor = 0;
	    send_to_char("Deleted.\n\r", ch);
	    return TRUE;
	}
    }
    return FALSE;
}



HEDIT(hedit_desc)
{
    HELP_DATA *pHelp;

    if (!EDIT_HELP(ch, pHelp)) {
	send_to_char("Null help file.\n\r", ch);
	return FALSE;
    }
    if (argument[0] != '\0') {
	send_to_char("Syntax:  desc\n\r", ch);
	return FALSE;
    }
    string_append(ch, &pHelp->text);
    return TRUE;
}



HEDIT(hedit_level)
{
    HELP_DATA *pHelp;
    int value;

    if (!EDIT_HELP(ch, pHelp)) {
	send_to_char("Null help file.\n\r", ch);
	return FALSE;
    }
    value = atoi(argument);

    if (argument[0] == '\0' || value < -1) {
	send_to_char("Syntax:  level [level >= -1]\n\r", ch);
	return FALSE;
    }
    pHelp->level = value;
    send_to_char("Help level set.\n\r", ch);

    return TRUE;
}



HEDIT(hedit_keywords)
{
    HELP_DATA *pHelp;
    int i;
    int length;

    if (!EDIT_HELP(ch, pHelp)) {
	send_to_char("Null help file.\n\r", ch);
	return FALSE;
    }
    if (argument[0] == '\0') {
	send_to_char("Syntax:  keywords <keywords>\n\r", ch);
	return FALSE;
    }
    length = strlen(argument);
    for (i = 0; i < length; i++)
	argument[i] = toupper(argument[i]);

    pHelp->keyword = str_dup(argument);
    send_to_char("Help keywords set.\n\r", ch);
    return TRUE;
}
