
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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

/*=======================================================================*
 * function: do_challenge                                                *
 * purpose: sends initial arena match query                              *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "merc.h"
#include "recycle.h"

DECLARE_DO_FUN(do_info_char);

/* Local Functions That were not included with arena code added by Beowolf */
bool    fight_in_progress; /* for pk arena scheduling */

void do_challenge(CHAR_DATA *ch, char *argument)
 {
  CHAR_DATA *victim; 
  char buf[MAX_STRING_LENGTH]; 

/* == First make all invalid checks == */
 if(IS_NPC(ch))
 return; 

 if( fight_in_progress )
  {
send_to_char("Sorry, there is a fight in progress, please wait a few moments.\n\r",ch);
return; 
  }

 if(IS_SET(ch->act2,ACT2_CHALLENGED))
  {
send_to_char("You have already been challenged, either ACCEPT or DECLINE first.\n\r",ch);
return; 
  }

 if(ch->hit < ch->max_hit)
  {
  send_to_char("You must be fully healed to fight in the arena.\n\r",ch);
  return;
  }

 if(IS_AFFECTED(ch,AFF_INVISIBLE))
 {
 send_to_char("Not while invisible.\n\r",ch);
 return;
 }

 if((victim = get_char_world(ch,argument)) == NULL)
  {
send_to_char("They are not playing.\n\r",ch);
return;
  }

 if(IS_NPC(victim) || IS_IMMORTAL(victim) || victim == ch )
  {
send_to_char("You cannot challenge NPC's, Immortals, or yourself.\n\r",ch);
return;
  }

 if(IS_AFFECTED(victim,AFF_BLIND))
 {
 send_to_char("That person is blind right now.\n\r",ch);
 return;
 }

 if(IS_SET(victim->act2,ACT2_CHALLENGER))
  {
send_to_char("They have already challenged someone else.\n\r",ch);
return;
  }

 if(victim->fighting != NULL )
  {
send_to_char("That person is engaged in battle right now.\n\r",ch); 
return; 
  }

 if(victim->hit < victim->max_hit)
  {
send_to_char("That player is not healthy enough to fight right now.\n\r",ch);
return;
  }

 if(victim->level <=5 )
  {
send_to_char("That player is just a newbie!\n\r",ch);
return; 
  }

 /* Check added by Beowolf */
 if ( ch->level - victim->level >= 20 )
 {
   send_to_char("You can't challenge someone 20 levels lower than you!\n\r",ch);
   return;
 }

/* == Now for the challenge == */
 SET_BIT(ch->act2,ACT2_CHALLENGER);
 send_to_char("Challenge has been sent\n\r",ch);
 act("$n has challenged you to a death match.",ch,NULL,victim,TO_VICT);
 sprintf(buf,"type: ACCEPT %s to meet the challenge.\n\r",ch->name);
 sprintf(buf,"%s has just challenged %s.\n\r",ch->name, victim->name);
 do_info_char(ch,buf);
 send_to_char(buf,victim);
 sprintf(buf,"type: DECLINE %s to chicken out.\n\r",ch->name); 
 send_to_char(buf,victim);
 return;
}

/*=======================================================================*
 * function: do_accept                                                   *
 * purpose: to accept the arena match, and move the players to the arena *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
void do_accept(CHAR_DATA *ch, char *argument)
 {
 CHAR_DATA *victim; 
 char buf[MAX_STRING_LENGTH]; 
 int char_room; 
 int vict_room; 
 extern bool fight_in_progress; 

 /*== Default to NO fight in progress ==*/
 fight_in_progress = !fight_in_progress;

 /*== the room VNUM's for our arena.are ==*/
 /* we use 1051 thru 1066 for a 4x4 arena */
 char_room = number_range(5901,5910);
 vict_room = number_range(5901,5910);

/* == first make all invalid checks == */
 if(IS_NPC(ch))
 return;

 if((victim = get_char_world(ch,argument)) == NULL)
  {
send_to_char("They aren't logged in!\n\r",ch);
return;
  }

 if(victim == ch)
  {
send_to_char("You haven't challenged yourself!\n\r",ch);
return;
  }

 if(!IS_SET(victim->act2,ACT2_CHALLENGER))
  {
send_to_char("That player hasn't challenged you!\n\r",ch); 
return; 
  }

/* == now get to business == */

SET_BIT(ch->act2,ACT2_CHALLENGED); 
send_to_char("You have accepted the challenge!\n\r",ch);
act("$n accepts your challenge!",ch,NULL,victim,TO_VICT);

/* == announce the upcoming event == */

sprintf(buf,
"%s has accepted %s's challenge to honorable battle in the Arena of Death!",
ch->name,victim->name);
do_info_char(ch,buf);
sprintf(buf,"%s has %d wins and %d losses\n\r",
victim->name,victim->pcdata->awins,victim->pcdata->alosses);
do_info_char(ch,buf);
sprintf(buf,"%s has %d wins and %d losses\n\r",ch->name,ch->pcdata->awins,ch->pcdata->alosses); 
do_info_char(ch,buf); 

/* == now move them both to an arena for the fun == */
send_to_char("You are teleported to the arena!\n\r",ch);
char_from_room(ch); 
char_to_room(ch,get_room_index(char_room));

send_to_char("You are teleported to the arena!\n\r",victim); 
char_from_room(victim); 
char_to_room(victim,get_room_index(vict_room)); 

fight_in_progress = TRUE; 
return; 
}

/*=======================================================================*
 * function: do_decline                                                  *
 * purpose: to chicken out from a sent arena challenge                   *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
void do_decline(CHAR_DATA *ch, char *argument )
 {
  CHAR_DATA *victim; 
  char buf[MAX_STRING_LENGTH]; 

/*== make all invalid checks == */
  if(IS_NPC(ch))
  return;
  
 if((victim = get_char_world(ch,argument)) == NULL)
  {
send_to_char("They aren't logged in!\n\r",ch);
return;
  }

 if(!IS_SET(victim->act2,ACT2_CHALLENGER))
  {
send_to_char("That player hasn't challenged you.\n\r",ch);
return;
  }

 if(victim == ch)
 return; 

/*== now actually decline == */
REMOVE_BIT(victim->act2,ACT2_CHALLENGER); 
send_to_char("Challenge declined!\n\r",ch);
act("$n has declined your challenge.",ch,NULL,victim,TO_VICT);
sprintf(buf,"%s has chickened out to %s's challege!", ch->name,victim->name);
do_info_char(ch,buf);
return;
}

/*======================================================================*
 * function: do_bet                                                     *
 * purpose: to allow players to wager on the outcome of arena battles   *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96               *
 *======================================================================*/
void do_bet(CHAR_DATA *ch, char *argument)
 {
 char arg[MAX_INPUT_LENGTH]; 
 char buf[MAX_STRING_LENGTH]; 
 CHAR_DATA *fighter; 
 int wager; 

 argument = one_argument(argument, arg); 

 if(argument[0] == '\0' || !is_number(arg))
  {
    send_to_char("Syntax: BET [amount] [player]\n\r",ch); 
    return;
  }

/*== disable the actual fighters from betting ==*/
 if(IS_SET(ch->act2,ACT2_CHALLENGER) || IS_SET(ch->act2,ACT2_CHALLENGED))
  {
    send_to_char("You can't bet on this battle.\n\r",ch); 
    return; 
  }

/*== make sure the choice is valid ==*/
 if((fighter = get_char_world(ch,argument)) == NULL)
  {
     send_to_char("That player is not in the arena.\n\r",ch); 
     return; 
  }

/*== do away with the negative number trickery ==*/
 if(!str_prefix("-",arg))
  {
send_to_char("Error: Invalid argument!\n\r",ch); 
return; 
  }

 wager   = atoi(arg);

 if(wager > 1001 || wager < 1)
  {
  send_to_char("Wager range is between 1 and 1000\n\r",ch);
  return; 
  }

/*== make sure they have the cash ==*/
 if(wager > ch->gold)
  {
  send_to_char("You don't have that much gold to wager!\n\r",ch); 
  return; 
  }

/*== now set the info ==*/
ch->gladiator = fighter;
ch->pcdata->plr_wager = wager;
sprintf(buf,"You have placed a %d gold wager on %s\n\r",wager,fighter->name);
send_to_char(buf,ch);
return; 
}



