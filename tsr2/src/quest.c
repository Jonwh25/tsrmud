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

/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.01. Please do not remove this notice from this file. *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mud.h"

DECLARE_DO_FUN( do_say );

/* Object vnums for Quest Rewards */

#define QUEST_ITEM1 1214
#define QUEST_ITEM2 1211
#define QUEST_ITEM3 1213
#define QUEST_ITEM4 1212
#define QUEST_ITEM5 1210

/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 1215
#define QUEST_OBJQUEST2 1216
#define QUEST_OBJQUEST3 1217
#define QUEST_OBJQUEST4 1218
#define QUEST_OBJQUEST5 1219

/* These are ranges for rewards to make it simpler to change them in the future
   These changes were made by Wakko of Tunfaire (Peter McGraw), if you use em
   keep this line of comments, otherwise, I don't care :)  */

#define MIN_GOLD 100
#define MAX_GOLD 250
#define MIN_QPOINTS 25
#define MAX_QPOINTS 60

/* Local functions */

void generate_quest	args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update	args(( void ));
bool quest_level_diff   args(( int clevel, int mlevel));
bool quest_chance	args(( int num ));
ROOM_INDEX_DATA 	*find_location( CHAR_DATA *ch, char *arg );

/* QUEST CHANCE function. I use this everywhere in my code, very handy :> */

bool quest_chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

/* The main quest function */

void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.\n\r",ch);
        send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
	return;
    }
    if (!strcmp(arg1, "info"))
    {
	if (IS_SET(ch->pcdata->flags, PCFLAG_QUEST))
	{
	    if (ch->questmob == -1 && ch->questgiver->short_descr != NULL)
	    {
		sprintf(buf, "Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->questgiver->short_descr);
		send_to_char(buf, ch);
	    }
	    else if (ch->questobj > 0)
	    {
                questinfoobj = get_obj_index(ch->questobj);
		if (questinfoobj != NULL)
		{
		    sprintf(buf, "You are on a quest to recover the fabled %s!\n\r",questinfoobj->name);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	    else if (ch->questmob > 0)
	    {
                questinfo = get_mob_index(ch->questmob);
		if (questinfo != NULL)
		{
	            sprintf(buf, "You are on a quest to slay the dreaded %s!\n\r",questinfo->short_descr);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	}
	else
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	return;
    }
    if (!strcmp(arg1, "points"))
    {
	sprintf(buf, "You have %d quest points.\n\r",ch->questpoints);
	send_to_char(buf, ch);
	return;
    }
    else if (!strcmp(arg1, "time"))
    {
	if (!IS_SET(ch->pcdata->flags, PCFLAG_QUEST))
	{
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	    if (ch->nextquest > 1)
	    {
		sprintf(buf, "There are %d minutes remaining until you can go on another quest.\n\r",ch->nextquest);
		send_to_char(buf, ch);
	    }
	    else if (ch->nextquest == 1)
	    {
		sprintf(buf, "There is less than a minute remaining until you can go on another quest.\n\r");
		send_to_char(buf, ch);
	    }
	}
        else if (ch->countdown > 0)
        {
	    sprintf(buf, "Time left for current quest: %d\n\r",ch->countdown);
	    send_to_char(buf, ch);
	}
	return;
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an
   ACT_QUESTMASTER flag instead of a special procedure. */

    for ( questman = ch->in_room->first_person; questman != NULL; questman = questman->next_in_room )
    {
	if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }

    if ( questman->fighting != NULL)
    {
	send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }

    ch->questgiver = questman;

/* And, of course, you will need to change the following lines for YOUR
   quest item information. Quest items on Moongate are unbalanced, very
   very nice items, and no one has one yet, because it takes awhile to
   build up quest points :> Make the item worth their while. */

    if (!strcmp(arg1, "list"))
    {
        act( AT_BLUE,"$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM);
	act ( AT_BLUE, "You ask $N for a list of quest items.",ch, NULL, questman, TO_CHAR);
	sprintf(buf, "Current Quest Items available for Purchase:\n\r\
3000qp.........Dragon Tooth Sword.\n\r\
2000qp.........Shield of Legends.\n\r\
1500qp.........Flaming Bracers.\n\r\
1000qp.........Dragon Scale Pants.\n\r\
 500qp.........Diamond Earrings.\n\r\
 250qp.........3,000 gold pieces.\n\r\
To buy an item, type 'QUEST BUY <item>'.\n\r");
	send_to_char(buf, ch);
	return;
    }

    else if (!strcmp(arg1, "buy"))
    {
	if (arg2[0] == '\0')
	{
	    send_to_char("To buy an item, type 'QUEST BUY <item>'.\n\r",ch);
	    return;
	}
	if (is_name(arg2, "Dragon tooth sword"))
	{
	    if (ch->questpoints >= 3000)
	    {
		ch->questpoints -= 3000;
	        obj = create_object(get_obj_index(QUEST_ITEM1),ch->level);
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, "shield legends"))
	{
	    if (ch->questpoints >= 2000)
	    {
		ch->questpoints -= 2000;
	        obj = create_object(get_obj_index(QUEST_ITEM2),ch->level);
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, "flaming bracers"))
	{
	    if (ch->questpoints >= 1500)
	    {
		ch->questpoints -= 1500;
	        obj = create_object(get_obj_index(QUEST_ITEM3),ch->level);
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, "scale pants"))
	{
	    if (ch->questpoints >= 1000)
	    {
		ch->questpoints -= 1000;
	        obj = create_object(get_obj_index(QUEST_ITEM4),ch->level);
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questman,buf);
		return;
	    }
	}
	else if (is_name(arg2, "diamond earrings"))
	{
	    if (ch->questpoints >= 500)
	    {
		ch->questpoints -= 500;
	        obj = create_object(get_obj_index(QUEST_ITEM5),ch->level);
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questman,buf);
		return;
	    }
	}

	else if (is_name(arg2, "gold"))
	{
	    if (ch->questpoints >= 250)
	    {
		ch->questpoints -= 250;
	        ch->gold += 3000;
    	        act( AT_BLUE, "$N gives 3,000 gold pieces to $n.", ch, NULL, questman, TO_ROOM );
    	        act( AT_BLUE, "$N has 3,000 in gold transfered from $s Swiss account to your balance.",   ch, NULL, questman, TO_CHAR );
	        return;
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questman,buf);
		return;
	    }
	}
	else
	{
	    sprintf(buf, "I don't have that item, %s.",ch->name);
	    do_say(questman, buf);
	}
	if (obj != NULL)
	{
    	    act( AT_BLUE, "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
    	    act( AT_BLUE, "$N gives you $p.",   ch, obj, questman, TO_CHAR );
	    obj_to_char(obj, ch);
	}
	return;
    }
    else if (!strcmp(arg1, "request"))
    {
        act( AT_BLUE, "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
	act ( AT_BLUE, "You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
	if (IS_IMMORTAL(ch)) {
    	    act( AT_BLUE, "$n kneels before $N.", questman, NULL, ch, TO_ROOM);
	    act( AT_BLUE, "$n kneels before you.", questman, NULL, ch, TO_CHAR);
	    if (ch->sex == 2)
	        sprintf(buf, "There are no quests worthy of your attention, my Lady.");
	    else if (ch->sex == 1)
	 	sprintf(buf, "There are no quests worthy of your attention, my Lord.");
	    else sprintf(buf, "There are no quests worthy of your divine intervention.");
	    do_say(questman, buf);
	    return;
	}

	if (IS_SET(ch->pcdata->flags, PCFLAG_QUEST))
	{
	    sprintf(buf, "But you're already on a quest!");
	    do_say(questman, buf);
	    return;
	}
	if (ch->nextquest > 0)
	{
	    sprintf(buf, "You're very brave, %s, but let someone else have a chance.",ch->name);
	    do_say(questman, buf);
	    sprintf(buf, "Come back later.");
	    do_say(questman, buf);
	    return;
	}

	sprintf(buf, "Thank you, brave %s!",ch->name);
	do_say(questman, buf);
        ch->questmob = 0;
	ch->questobj = 0;

	generate_quest(ch, questman);

        if (ch->questmob > 0 || ch->questobj > 0)
	{
            ch->countdown = number_range(10,30);
	    SET_BIT(ch->pcdata->flags, PCFLAG_QUEST);
	    sprintf(buf, "You have %d minutes to complete this quest.",ch->countdown);
	    do_say(questman, buf);
	    sprintf(buf, "May the gods go with you!");
	    do_say(questman, buf);
	}
	return;
    }
    else if (!strcmp(arg1, "complete"))
    {
        act( AT_BLUE, "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
	act ( AT_BLUE, "You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
	if (ch->questgiver != questman)
	{
	    sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
	    do_say(questman,buf);
	    return;
	}

	if (IS_SET(ch->pcdata->flags, PCFLAG_QUEST))
	{
	    if (ch->questmob == -1 && ch->countdown > 0)
	    {
		int reward, pointreward;

	    	reward = number_range(MIN_GOLD,MAX_GOLD);
	    	pointreward = number_range(MIN_QPOINTS,MAX_QPOINTS);

		sprintf(buf, "Congratulations on completing your quest!");
		do_say(questman,buf);
		sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.",pointreward,reward);
		do_say(questman,buf);

	        REMOVE_BIT(ch->pcdata->flags, PCFLAG_QUEST);
	        ch->questgiver = NULL;
	        ch->countdown = 0;
	        ch->questmob = 0;
		ch->questobj = 0;
	        ch->nextquest = 10;
		ch->gold += reward;
		ch->questpoints += pointreward;

	        return;
	    }
	    else if (ch->questobj > 0 && ch->countdown > 0)
	    {
		bool obj_found = FALSE;

    		for (obj = ch->first_carrying; obj != NULL; obj= obj_next)
    		{
        	    obj_next = obj->next_content;

		    if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
		    {
			obj_found = TRUE;
            	        break;
		    }
        	}
		if (obj_found == TRUE)
		{
		    int reward, pointreward;

		    reward = number_range(MIN_GOLD,MAX_GOLD);
		    pointreward = number_range(MIN_QPOINTS,MAX_QPOINTS);

		    act( AT_BLUE, "You hand $p to $N.",ch, obj, questman, TO_CHAR);
		    act( AT_BLUE, "$n hands $p to $N.",ch, obj, questman, TO_ROOM);

	    	    sprintf(buf, "Congratulations on completing your quest!");
		    do_say(questman,buf);
		    sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.",pointreward,reward);
		    do_say(questman,buf);

	            REMOVE_BIT(ch->pcdata->flags, PCFLAG_QUEST);
	            ch->questgiver = NULL;
	            ch->countdown = 0;
	            ch->questmob = 0;
		    ch->questobj = 0;
	            ch->nextquest = 10;
		    ch->gold += reward;
		    ch->questpoints += pointreward;
		    extract_obj(obj);
		    return;
		}
		else
		{
		    sprintf(buf, "You haven't completed the quest yet, but there is still time!");
		    do_say(questman, buf);
		    return;
		}
		return;
	    }
	    else if ((ch->questmob > 0 || ch->questobj > 0) && ch->countdown > 0)
	    {
		sprintf(buf, "You haven't completed the quest yet, but there is still time!");
		do_say(questman, buf);
		return;
	    }
	}
	if (ch->nextquest > 0)
	    sprintf(buf,"But you didn't complete your quest in time!");
	else sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);
	do_say(questman, buf);
	return;
    }

    send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY.\n\r",ch);
    send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *vsearch;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];
    long mcounter;
    int mob_vnum;

    /*  Randomly selects a mob from the world mob list. If you don't
	want a mob to be selected, make sure it is immune to summon.
	Or, you could add a new mob flag called ACT_NOQUEST. The mob
	is selected for both mob and obj quests, even tho in the obj
	quest the mob is not used. This is done to assure the level
	of difficulty for the area isn't too great for the player. */

    for (mcounter = 0; mcounter < 99999; mcounter ++)
    {
	mob_vnum = number_range(50, 32600);

	if ( (vsearch = get_mob_index(mob_vnum) ) != NULL )
	{
            if (quest_level_diff(ch->level, vsearch->level) == TRUE
                && vsearch->pShop == NULL
    		&& !IS_SET(vsearch->act, ACT_TRAIN)
    		&& !IS_SET(vsearch->act, ACT_PRACTICE)
		&& !IS_SET(vsearch->act, ACT_PET)
		&& !IS_SET(vsearch->act, ACT_IMMORTAL)
		&& !IS_SET(vsearch->act, ACT_SCHOLAR)
		&& !IS_SET(vsearch->act, ACT_MOUNTED)
		&& !IS_SET(vsearch->act, ACT_PROTOTYPE)
		&& !IS_SET(vsearch->affected_by, AFF_CHARM)
		&& !IS_SET(vsearch->affected_by, AFF_INVISIBLE)
		&& quest_chance(40)) break;
		else vsearch = NULL;
	}
    }

    if ( vsearch == NULL || ( victim = get_char_world( ch, vsearch->player_name ) ) == NULL )
    {
	sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
	do_say(questman, buf);
	sprintf(buf, "Try again later.");
	do_say(questman, buf);
	ch->nextquest = 2;
        return;
    }

    if ( ( room = find_location( ch, victim->name ) ) == NULL )
    {
	sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
	do_say(questman, buf);
	sprintf(buf, "Try again later.");
	do_say(questman, buf);
	ch->nextquest = 2;
        return;
    }

    /*  40% chance it will send the player on a 'recover item' quest. */

    if (quest_chance(40))
    {
	int objvnum = 0;

	switch(number_range(0,4))
	{
	    case 0:
	    objvnum = QUEST_OBJQUEST1;
	    break;

	    case 1:
	    objvnum = QUEST_OBJQUEST2;
	    break;

	    case 2:
	    objvnum = QUEST_OBJQUEST3;
	    break;

	    case 3:
	    objvnum = QUEST_OBJQUEST4;
	    break;

	    case 4:
	    objvnum = QUEST_OBJQUEST5;
	    break;
	}

        questitem = create_object( get_obj_index(objvnum), ch->level );
	obj_to_room(questitem, room);
	ch->questobj = questitem->pIndexData->vnum;

	sprintf(buf, "Vile pilferers have stolen %s from the royal treasury!",questitem->short_descr);
	do_say(questman, buf);
	do_say(questman, "My court wizardess, with her magic mirror, has pinpointed its location.");

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

	sprintf(buf, "Look in the general area of %s for %s!",room->area->name, room->name);
	do_say(questman, buf);
	return;
    }

    /* Quest to kill a mob */

    else
    {
    switch(number_range(0,1))
    {
	case 0:
        sprintf(buf, "An enemy of mine, %s, is making vile threats against the crown.",victim->short_descr);
        do_say(questman, buf);
        sprintf(buf, "This threat must be eliminated!");
        do_say(questman, buf);
	break;

	case 1:
	sprintf(buf, "The City of Shadows most heinous criminal, %s, has escaped from the dungeon!",victim->short_descr);
	do_say(questman, buf);
	sprintf(buf, "Since the escape, %s has murdered %d civillians!",victim->short_descr, number_range(2,20));
	do_say(questman, buf);
	do_say(questman,"The penalty for this crime is death, and you are to deliver the sentence!");
	break;
    }

    if (room->name != NULL)
    {
        sprintf(buf, "Seek %s out somewhere in the vicinity of %s!",victim->short_descr,room->name);
        do_say(questman, buf);

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

	sprintf(buf, "That location is in the general area of %s.",room->area->name);
	do_say(questman, buf);
    }
    ch->questmob = victim->pIndexData->vnum;
    }
    return;
}

/* 
 *  Level differences to search for. Moongate has 350
 *  levels, so you will want to tweak these greater or
 *  less than statements for yourself. - Vassago 
 *  clevel = player's level  
 *  mlevel = level of mob   
 */

bool quest_level_diff(int clevel, int mlevel)
{
    if ( ( clevel < 16 )
    && ( ( mlevel >= ( clevel - 7  ) )
    &&   ( mlevel <= ( clevel + 5  ) ) ) ) return TRUE;

    else if ( ( clevel > 15  && clevel < 31 )
	 &&   ( ( mlevel >= ( clevel - 3  ) )
	 &&     ( mlevel <= ( clevel + 7  ) ) ) ) return TRUE;

    else if ( ( clevel > 30  && clevel < 46 )
	 &&   ( ( mlevel >= ( clevel - 1  ) )
	 &&     ( mlevel <= ( clevel + 10 ) ) ) ) return TRUE;

    else if ( ( clevel > 45  && clevel < 60 )
	 &&   ( ( mlevel >= ( clevel - 0  ) )
	 &&     ( mlevel <= ( clevel + 15 ) ) ) ) return TRUE;

    else if ( ( clevel > 60 )
	 &&   ( mlevel >= clevel ) ) return TRUE;

    else return FALSE;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;

    for ( d = first_descriptor; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {

        ch = d->character;

        if (ch->nextquest > 0)
        {
            ch->nextquest--;
            if (ch->nextquest == 0)
            {
                send_to_char("You may now quest again.\n\r",ch);
                return;
            }
        }

        else if (IS_SET(ch->pcdata->flags, PCFLAG_QUEST))
        {
            if (--ch->countdown <= 0)
            {
                char buf [MAX_STRING_LENGTH];

                ch->nextquest = 10;
                sprintf(buf, "You have run out of time for your quest!\n\rYou m
ay quest again in %d minutes.\n\r",ch->nextquest);
                send_to_char(buf, ch);
                REMOVE_BIT(ch->pcdata->flags, PCFLAG_QUEST);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
            }
            if (ch->countdown > 0 && ch->countdown < 6)
            {
                send_to_char("Better hurry, you're almost out of time for your
quest!\n\r",ch);
                return;
            }
        }
        }
    }
    return;
}

