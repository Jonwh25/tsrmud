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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/***************************************************************************
 *  As the Wheel Weaves based on ROM 2.4. Original code by Dalsor, Caxandra,
 *	and Zy of AWW. See changes.log for a list of changes from the original
 *	ROM code. Credits for code created by other authors have been left
 *	intact at the head of each function.
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"

/* Thanks to Kyndig and Sandi Fallon for assisting with arguments - Dalsor.
 * At the time I started on furnishings, I was about 12 hours straight into
 * the code and very brain dead. Thanks to them for pointing out my mistakes.
 */

/* Homes uses aspects of OLC, and it should be installed, or the necessary
 * componants installed in order for this to work
 */

/* Damn, this is a lot of defines :(
 * Each one of these vnums will need to be created. If this many items
 * aren't desired, removed the uneccesary ones and comment out the code
 */
#define HOME_ITEM_1   2208	/* fish */

RESET_DATA *new_reset_data args((void));
void add_reset args((ROOM_INDEX_DATA * room, RESET_DATA * loc_reset, int index));
void free_reset_data args((RESET_DATA * pReset));
void home_buy(CHAR_DATA * ch);
void home_sell(CHAR_DATA * ch, char *argument);
void home_describe(CHAR_DATA * ch);

void do_home(CHAR_DATA * ch, char *argument)
{

    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    RESET_DATA *loc_reset;
    OBJ_DATA *furn;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MSL];

    loc = ch->in_room;
    loc_area = ch->in_room->area;
    buf[0] = '\0';

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (IS_NPC(ch)
	|| ch == NULL)
	return;

    if (arg1[0] == '\0' || !strcmp(arg1, "list")) {
	stc("What about a home?\n\r", ch);
	stc("\n\rSyntax: Home (buy, sell, furnish, describe)\n\r", ch);
	stc("\n\rExtended:\n\rHome (buy, sell, furnish, describe)\n\r", ch);
	stc("Buy      (purchase a home in the room you are standing in)\n\r", ch);
	stc("Sell     (confirm - does not prompt for confirmation!)\n\r", ch);
	stc("Furnish  (allows purchases of items)\n\r", ch);
	stc("Describe (describe the room - uses the OLC editor)\n\r", ch);
	return;
    }
    if (!is_name(ch, ch->in_room->area->filename, "hog.are")) {
	stc("You aren't in Caemlyn.\n\r", ch);
	return;
    }
    if (!IS_SET(loc->room2_flags, ROOM_HOME)) {
	stc("You cannot buy a home in this room.\n\r", ch);
	return;
    }
    /* Find out what the argument is, if any */
    if (!strcmp(arg1, "buy"))
	home_buy(ch);
    else if (!strcmp(arg1, "sell"))
	home_sell(ch, arg2);
    else if (!strcmp(arg1, "describe"))
	home_describe(ch);
    else if (!strcmp(arg1, "furnish"))
	/* Home furnish was left in here because I didn't feel like
	 * redoing all the arguments - Dalsor
	 */
    {
	if (!is_room_owner(ch, loc)) {
	    stc("But you do not own this room!\n\r", ch);
	    return;
	}
	if (arg2[0] == '\0') {
	    stc("This command allows you to furnish your home.\n\r", ch);
	    stc("You must be carrying gold to purchase furnishings,\n\r", ch);
	    stc("and be standing in your home. You cannot have more\n\r", ch);
	    stc("than five items in your home.\n\r", ch);
	    stc("\n\rSyntax: Home (furnish) (item name)\n\r", ch);
	    stc("   Window      5000 gold\n\r", ch);
	    stc("   Aquarium    7500 gold (fish, turtle, snake, spider)\n\r", ch);
	    stc("   Chair       5000 gold (sturdy, highback, carved, hard)\n\r", ch);
	    stc("   Desk        7500 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Sofa        7500 gold (plush, padded, comfortable, fluffed)\n\r", ch);
	    stc("   Endtable    1000 gold (oak, pine, carved, teak)\n\r", ch);
	    stc("   Table       7500 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Recliner    7500 gold\n\r", ch);
	    stc("   Lamp        2000 gold\n\r", ch);
	    stc("   Mirror      2500 gold\n\r", ch);
	    stc("   Lantern     1500 gold\n\r", ch);
	    stc("   Torch       1000 gold\n\r", ch);
	    stc("   Dresser     5000 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Footchest   5000 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Armoire     7500 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Wardrobe    7500 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Bookcase    7500 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Chifferobe  7500 gold (oak, pine, carved, polished, teak)\n\r", ch);
	    stc("   Cot         1000 gold\n\r", ch);
	    stc("   Featherbed  2500 gold\n\r", ch);
	    stc("   Twinbed     5000 gold\n\r", ch);
	    stc("   Postedbed   7500 gold\n\r", ch);
	    stc("   Canopybed   8500 gold\n\r", ch);
	    stc("   Royalbed    9900 gold\n\r", ch);
	    return;
	} else if (!strcmp(arg2, "window")) {
	    stc("Windows aren't available yet.\n\r", ch);
	    stc("Windows will allow you to look into or out of a home.\n\r", ch);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "aquarium")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		SET_BIT(loc_area->area_flags, AREA_CHANGED);
		return;
	    }
	    if (arg3[0] != '\0') {
		if (!strcmp(arg3, "fish")) {
		    furn = create_object(get_obj_index(HOME_ITEM_1), 0);
		}
		/*else if ( !strcmp ( arg3, "turtle" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_2),0);}
		   else if ( !strcmp ( arg3, "snake" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_3),0);}
		   else if ( !strcmp ( arg3, "spider" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_4),0);} */
		else {
		    stc("Invalid aquarium type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid aquarium type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %ld for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "chair")) {
	    if (ch->money.gold < 5000) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "sturdy" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_8),0);}
		   else if ( !strcmp ( arg3, "highback" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_6),0);}
		   else if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_5),0);}
		   else if ( !strcmp ( arg3, "hard" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_7),0);}
		   else */
		{
		    stc("Invalid chair type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid chair type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 5000;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "desk")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_9),0);}
		   else if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_10),0);}
		   else if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_11),0);}
		   else if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_12),0);}
		   else if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_13),0);}
		   else */
		{
		    stc("Invalid desk type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid desk type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "sofa")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "plush" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_15),0);}
		   else if ( !strcmp ( arg3, "padded" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_16),0);}
		   else if ( !strcmp ( arg3, "comfortable" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_17),0);}
		   else if ( !strcmp ( arg3, "fluffed" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_18),0);}
		   else */
		{
		    stc("Invalid sofa type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid sofa type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "endtable")) {
	    if (ch->money.gold < 1000) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_20),0);}
		   else if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_21),0);}
		   else if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_22),0);}
		   else if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_23),0);}
		   else */
		{
		    stc("Invalid endtable type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid endtable type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 1000;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "table")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_25),0);}
		   else if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_26),0);}
		   else if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_27),0);}
		   else if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_28),0);}
		   else if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_29),0);}
		   else */
		{
		    stc("Invalid table type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid table type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "recliner")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    /*furn = create_object(get_obj_index(HOME_ITEM_30),0); */
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "lamp")) {
	    if (ch->money.gold < 2000) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    stc("Your lamp will provide light for your home for a time.\n\r", ch);
	    ch->money.gold -= 2000;
	    /*furn = create_object(get_obj_index(HOME_ITEM_31),0); */
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "mirror")) {
	    stc("Mirrors aren't available yet.\n\r", ch);
	    stc("Mirrors will allow you to see your reflection... Pretty useless, eh?\n\r", ch);
	    return;
	} else if (!strcmp(arg2, "lantern")) {
	    if (ch->money.gold < 1500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    stc("Your lantern will provide light for your home for a time.\n\r", ch);
	    ch->money.gold -= 1500;
	    /*furn = create_object(get_obj_index(HOME_ITEM_32),0); */
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "torch")) {
	    if (ch->money.gold < 1000) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    stc("Your torch will provide light for your home for a time.\n\r", ch);
	    ch->money.gold -= 1000;
	    /*furn = create_object(get_obj_index(HOME_ITEM_33),0); */
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "dresser")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_34),0);}
		   else if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_35),0);}
		   else if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_36),0);}
		   else if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_37),0);}
		   else if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_38),0);}
		   else */
		{
		    stc("Invalid dresser type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid dresser type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "footchest")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_40),0);}
		   else if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_41),0);}
		   else if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_42),0);}
		   else if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_43),0);}
		   else if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_44),0);}
		   else */
		{
		    stc("Invalid footchest type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid footchest type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "armoire")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_46),0);}
		   if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_47),0);}
		   if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_48),0);}
		   if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_49),0);}
		   if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_50),0);}
		   else */
		{
		    stc("Invalid armoire type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid armoire type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "wardrobe")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_52),0);}
		   if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_53),0);}
		   if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_54),0);}
		   if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_55),0);}
		   if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_56),0);}
		   else */
		{
		    stc("Invalid wardrobe type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid wardrobe type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "bookcase")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_58),0);}
		   if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_59),0);}
		   if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_60),0);}
		   if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_61),0);}
		   if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_62),0);}
		   else */
		{
		    stc("Invalid bookcase type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid bookcase type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "chifferobe")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    if (arg3[0] != '\0') {
		/*if ( !strcmp ( arg3, "oak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_64),0);}
		   if ( !strcmp ( arg3, "pine" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_65),0);}
		   if ( !strcmp ( arg3, "carved" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_66),0);}
		   if ( !strcmp ( arg3, "polished" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_67),0);}
		   if ( !strcmp ( arg3, "teak" ) )
		   {furn = create_object(get_obj_index(HOME_ITEM_68),0);}
		   else */
		{
		    stc("Invalid chifferobe type.\n\r", ch);
		    return;
		}
	    } else {
		stc("Invalid chifferobe type.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "cot")) {
	    if (ch->money.gold < 1000) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 1000;
	    /*furn = create_object(get_obj_index(HOME_ITEM_71),0); */
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "featherbed")) {
	    if (ch->money.gold < 2500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 2500;
	    /*furn = create_object(get_obj_index(HOME_ITEM_72),0); */
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "twinbed")) {
	    if (ch->money.gold < 5000) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 5000;
	    /*furn = create_object(get_obj_index(HOME_ITEM_75),0); */
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "postedbed")) {
	    if (ch->money.gold < 7500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 7500;
	    /*furn = create_object(get_obj_index(HOME_ITEM_74),0); */
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "canopybed")) {
	    if (ch->money.gold < 8500) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 8500;
	    /*furn = create_object(get_obj_index(HOME_ITEM_73),0); */
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else if (!strcmp(arg2, "royalbed")) {
	    if (ch->money.gold < 9900) {
		stc("You do not have enough gold for this purchase.\n\r", ch);
		return;
	    }
	    ch->money.gold -= 9900;
	    /*furn = create_object(get_obj_index(HOME_ITEM_70),0); */
	    loc_reset = new_reset_data();
	    loc_reset->command = 'O';
	    loc_reset->arg1 = furn->pIndexData->vnum;
	    loc_reset->arg2 = 0;
	    loc_reset->arg3 = loc->vnum;
	    add_reset(loc, loc_reset, 0);
	    printf_to_char(ch, "You have been deducted %d for your purchase.", furn->cost);
	    obj_to_room(furn, ch->in_room);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	} else {
	    stc("You have not provided the name of the item to purchase.\n\r", ch);
	    return;
	}
    } else {
	stc("No such Home Command. See HELP HOME for more informatio.\n\r", ch);
	return;
    }
    return;
}

void home_buy(CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    char buf[MSL];

    loc = ch->in_room;
    loc_area = ch->in_room->area;
    if (loc->owner[0] == '\0') {
	if (ch->money.gold < 25000) {
	    stc("This command allows you to buy a home.\n\r", ch);
	    stc("You must be standing in the room to buy.\n\r", ch);
	    stc("You start with a blank, untitled room that is void of\n\r", ch);
	    stc("furnishings and light. A single door allows entrance.\n\r", ch);
	    stc("A home costs 25,000 GOLD. You must be carrying the coins.\n\r", ch);
	    stc("\n\rSyntax: Home buy\n\r", ch);
	    return;
	} else {
	    free_string(loc->owner);
	    loc->owner = str_dup(ch->name);
	    ch->money.gold -= 25000;
	    free_string(loc->name);
	    sprintf(buf, "%s's Home", ch->name);
	    loc->name = str_dup(buf);
	    stc("Congratulations on purchasing your new home!\n\r", ch);
	    stc("You are now 25,000 gold coins lighter and the owner\n\r", ch);
	    stc("of the room in which you now stand! Be sure to describe\n\r", ch);
	    stc("your home and purchase furnishings for it.\n\r", ch);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
	}
    } else {
	stc("This home is already owned.\n\r", ch);
	return;
    }
    return;
}

void home_sell(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    RESET_DATA *current_reset;
    RESET_DATA *next_reset;
    OBJ_DATA *furn;
    OBJ_DATA *furn_next;

    loc = ch->in_room;
    loc_area = ch->in_room->area;

    if (!is_room_owner(ch, loc)) {
	stc("But you do not own this room!\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	stc("This command allows you to sell your home.\n\r", ch);
	stc("You will no longer own your home once it is sold,\n\r", ch);
	stc("and you will be compensated half the cost of the home,\n\r", ch);
	stc("not including items you have purchased.\n\r", ch);
	stc("You must be standing in the room which you own.\n\r", ch);
	stc("\n\rSyntax: Home (sell) (confirm)\n\r", ch);
	return;
    } else if (!strcmp(argument, "confirm")) {
	free_string(loc->owner);
	loc->owner = str_dup("");
	ch->money.gold += 12500;
	free_string(loc->name);
	loc->name = str_dup("An Abandoned Home");
	stc("Your home has been sold and you are now 12,500 gold coins richer!\n\r", ch);

	if (!ch->in_room->reset_first) {
	    return;
	}
	/* Thanks to Edwin and Kender for the help with killing
	 * resets. Pointed out some real problems with my handling
	 * of it. Thanks a bunch guys.
	 * This is the exact way Kender put it, and it works great!
	 */
	for (current_reset = ch->in_room->reset_first; current_reset;
	     current_reset = next_reset) {
	    next_reset = current_reset->next;
	    free_reset_data(current_reset);
	}

	ch->in_room->reset_first = NULL;

	for (furn = ch->in_room->contents; furn != NULL; furn = furn_next) {
	    furn_next = furn->next_content;
	    extract_obj(furn);
	}
	SET_BIT(loc_area->area_flags, AREA_CHANGED);
	return;
    } else {
	stc("This command allows you to sell your home.\n\r", ch);
	stc("You will no longer own your home once it is sold,\n\r", ch);
	stc("and you will be compensated half the cost of the home,\n\r", ch);
	stc("not including items you have purchased.\n\r", ch);
	stc("You must be standing in the room which you own.\n\r", ch);
	stc("\n\rSyntax: Home (sell) (confirm)\n\r", ch);
	return;
    }
}

void home_describe(CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;

    loc = ch->in_room;
    loc_area = ch->in_room->area;

    if (!is_room_owner(ch, loc)) {
	stc("But you do not own this room!\n\r", ch);
	return;
    } else {
	stc("This command allows you to describe your home.\n\r", ch);
	stc("You should not describe items that are in the room,\n\r", ch);
	stc("rather allowing the furnishing of the home to do that.\n\r", ch);
	stc("If you currently own this room, you will be placed into.\n\r", ch);
	stc("the room editor. Be warned that while in the room editor,\n\r", ch);
	stc("you are only allowed to type the description. If you are\n\r", ch);
	stc("unsure or hesitant about this, please note the Immortals,\n\r", ch);
	stc("or better, discuss the how-to's with a Builder.\n\r", ch);
	stc("Syntax: Home (describe)\n\r", ch);
	string_append(ch, &loc->description);
	SET_BIT(loc_area->area_flags, AREA_CHANGED);
	return;
    }

}
