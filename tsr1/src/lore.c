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
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
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
#include "magic.h"
#include "recycle.h"

void do_lore(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument,arg);
    if ( arg[0] == '\0' )
    {
	send_to_char("What do you want to lore?\n\r", ch);
	return;
    }

    obj = get_obj_carry(ch,arg,ch);
    if ( obj == NULL )
    {
	send_to_char("You aren't carrying that.\n\r", ch);
	return;
    }

    WAIT_STATE(ch, skill_table[gsn_lore].beats);

    if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch,gsn_lore) )
    {
	act("You look at $p, but you can't find out any additional information.",
	    ch,obj,NULL,TO_CHAR);
	act("$n looks at $p but cannot find out anything.", ch, obj, NULL, TO_ROOM);
	return;
    }
    else
    {	
	act("$n studies $p, discovering all of it's hidden powers.",ch,obj,NULL,TO_ROOM);
	if ( ( number_percent( ) * number_percent( ) ) < 40 )
	{
	    obj->cost += ( get_skill(ch,gsn_lore) * 0.001 * obj->cost);
	    send_to_char( "Your understanding of the lore behind it increases its worth!\n\r",  ch );
	}

	spell_identify(gsn_lore,( 4 * obj->level )/3,ch,obj,TARGET_OBJ); 
	check_improve(ch,gsn_lore,TRUE,4);
    }
}	



