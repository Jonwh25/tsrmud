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
 **************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* command procedures needed */
DECLARE_DO_FUN(do_quit    	);
DECLARE_DO_FUN(do_info_char	);

/* RT code to delete yourself */

void do_delet( CHAR_DATA *ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\n\r",ch);
}

void do_delete( CHAR_DATA *ch, char *argument)
{
   char strsave[MAX_INPUT_LENGTH];

   if (IS_NPC(ch))
	return;
  
   if (ch->pcdata->confirm_delete)
   {
	if (argument[0] != '\0')
	{
	    send_to_char("Delete status removed.\n\r",ch);
	    ch->pcdata->confirm_delete = FALSE;
	    return;
	}
	else
	{
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    wiznet("$N turns $Mself into line noise.",ch,NULL,0,0,0);
	    stop_fighting(ch,TRUE);
	    do_quit(ch,"");
	    unlink(strsave);
	    return;
 	}
    }

    if (argument[0] != '\0')
    {
	send_to_char("Just type delete. No argument.\n\r",ch);
	return;
    }

    send_to_char("Type delete again to confirm this command.\n\r",ch);
    send_to_char("WARNING: this command is irreversible.\n\r",ch);
    send_to_char("Typing delete with an argument will undo delete status.\n\r",
	ch);
    ch->pcdata->confirm_delete = TRUE;
    wiznet("$N is contemplating deletion.",ch,NULL,0,0,get_trust(ch));
}
	    

/* RT code to display channel status */

void do_channels( CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    send_to_char("   `@channel``     `8status``\n\r",ch);
    send_to_char("---------------------\n\r",ch);
 
    send_to_char("`Ggossip``         ",ch);
    if (!IS_SET(ch->comm,COMM_NOGOSSIP))
      send_to_char("`@ON``\n\r",ch);
    else
      send_to_char("`8OFF``\n\r",ch);

    send_to_char("`Yauction``        ",ch);
    if (!IS_SET(ch->comm,COMM_NOAUCTION))
      send_to_char("`@ON``\n\r",ch);
    else
      send_to_char("`8OFF``\n\r",ch);

    send_to_char("`MMusic``          ",ch);
    if (!IS_SET(ch->comm,COMM_NOMUSIC))
      send_to_char("`@ON``\n\r",ch);
    else
      send_to_char("`8OFF``\n\r",ch);

    send_to_char("`Oooc``            ",ch);
    if (!IS_SET(ch->comm,COMM_NOOOC))
      send_to_char("`@ON``\n\r",ch);
    else
      send_to_char("`8OFF``\n\r",ch);

    send_to_char("`QQuote ``         ",ch);
    if (!IS_SET(ch->comm,COMM_NOQUOTE))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("`ggrats ``       ",ch);
    if (!IS_SET(ch->comm,COMM_NOGRATS))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    if (IS_IMMORTAL(ch))
    {
      send_to_char("`Agod channel``    ",ch);
      if(!IS_SET(ch->comm,COMM_NOWIZ))
        send_to_char("`@ON``\n\r",ch);
      else
        send_to_char("`8OFFi``\n\r",ch);
    }

    send_to_char("`Xshouts``         ",ch);
    if (!IS_SET(ch->comm,COMM_SHOUTSOFF))
      send_to_char("`@ON``\n\r",ch);
    else
      send_to_char("`8OFF``\n\r",ch);

    send_to_char("`Ttells``          ",ch);
    if (!IS_SET(ch->comm,COMM_DEAF))
	send_to_char("`@ON``\n\r",ch);
    else
	send_to_char("`8OFF``\n\r",ch);

    send_to_char("`Tbtells``          ",ch);
    if (!IS_SET(ch->comm,COMM_DEAF))
	send_to_char("`@ON``\n\r",ch);
    else
	send_to_char("`8OFF``\n\r",ch);

    send_to_char("quiet mode     ",ch);
    if (IS_SET(ch->comm,COMM_QUIET))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    if (IS_SET(ch->comm,COMM_AFK))
	send_to_char("You are AFK.\n\r",ch);

    if (IS_SET(ch->comm,COMM_SNOOP_PROOF))
	send_to_char("You are immune to snooping.\n\r",ch);
   
    if (ch->lines != PAGELEN)
    {
	if (ch->lines)
	{
	    sprintf(buf,"You display %d lines of scroll.\n\r",ch->lines+2);
	    send_to_char(buf,ch);
 	}
	else
	    send_to_char("Scroll buffering is off.\n\r",ch);
    }

    if (ch->prompt != NULL)
    {
	sprintf(buf,"Your current prompt is: %s\n\r",ch->prompt);
	send_to_char(buf,ch);
    }

    if (IS_SET(ch->comm,COMM_NOSHOUT))
      send_to_char("You cannot shout.\n\r",ch);
  
    if (IS_SET(ch->comm,COMM_NOTELL))
      send_to_char("You cannot use tells or btells.\n\r",ch);
 
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
     send_to_char("You cannot use channels.\n\r",ch);

    if (IS_SET(ch->comm,COMM_NOEMOTE))
      send_to_char("You cannot show emotions.\n\r",ch);

}

/* RT deaf blocks out all shouts */

void do_deaf( CHAR_DATA *ch, char *argument)
{
    
   if (IS_SET(ch->comm,COMM_DEAF))
   {
     send_to_char("You can now hear tells and btells again.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_DEAF);
   }
   else 
   {
     send_to_char("From now on, you won't hear tells or btells.\n\r",ch);
     SET_BIT(ch->comm,COMM_DEAF);
   }
}

/* RT quiet blocks out all communication */

void do_quiet ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_QUIET))
    {
      send_to_char("Quiet mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_QUIET);
    }
   else
   {
     send_to_char("From now on, you will only hear says and emotes.\n\r",ch);
     SET_BIT(ch->comm,COMM_QUIET);
   }
}

/* afk command */

void do_afk ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_AFK))
    {
      send_to_char("AFK mode removed. Type 'replay' to see tells.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_AFK);
    }
   else
   {
     send_to_char("You are now in AFK mode.\n\r",ch);
     SET_BIT(ch->comm,COMM_AFK);
   }
}

void do_replay (CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
	send_to_char("You can't replay.\n\r",ch);
	return;
    }

    if (buf_string(ch->pcdata->buffer)[0] == '\0')
    {
	send_to_char("You have no tells to replay.\n\r",ch);
	return;
    }

    page_to_char(buf_string(ch->pcdata->buffer),ch);
    clear_buf(ch->pcdata->buffer);
}

/* RT auction rewritten in ROM style */
void do_auction( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't auction until level 2.\n\r", ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOAUCTION))
      {
	send_to_char("Auction channel is now ON.\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOAUCTION);
      }
      else
      {
	send_to_char("Auction channel is now OFF.\n\r",ch);
	SET_BIT(ch->comm,COMM_NOAUCTION);
      }
    }
    else  /* auction message sent, turn auction on if it is off */
    {
	if (IS_SET(ch->comm,COMM_QUIET))
	{
	  send_to_char("You must turn off quiet mode first.\n\r",ch);
	  return;
	}

	if (IS_SET(ch->comm,COMM_NOCHANNELS))
	{
	  send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
	  return;

	REMOVE_BIT(ch->comm,COMM_NOAUCTION);
      }

      sprintf( buf, "`YYou auction '%s'\n\r``", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm,COMM_NOAUCTION) &&
	     !IS_SET(victim->comm,COMM_QUIET) )
	{
	    act_new("`Y$n auctions '$t'``",
		    ch,argument,d->character,TO_VICT,POS_DEAD);
 	}
      }
    }
}

/* RT chat replaced with ROM gossip */
void do_gossip( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't gossip until level 2.\n\r",ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGOSSIP))
      {
        send_to_char("Gossip channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOGOSSIP);
      }
      else
      {
        send_to_char("Gossip channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOGOSSIP);
      }
    }
    else  /* gossip message sent, turn gossip on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
 
       	}

      REMOVE_BIT(ch->comm,COMM_NOGOSSIP);
 
      sprintf( buf, "`GYou gossip '%s``'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGOSSIP) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "`G$n gossips '$t'``", 
		   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

void do_grats( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't grats until level 2.\n\r", ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGRATS))
      {
        send_to_char("Grats channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOGRATS);
      }
      else
      {
        send_to_char("Grats channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOGRATS);
      }
    }
    else  /* grats message sent, turn grats on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
 
        }
 
      REMOVE_BIT(ch->comm,COMM_NOGRATS);
 
      sprintf( buf, "`gYou grats '%s``'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGRATS) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "`g$n grats '$t'``",
                   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

void do_quote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't quote until level 2.\n\r", ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUOTE))
      {
        send_to_char("Quote channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUOTE);
      }
      else
      {
        send_to_char("Quote channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUOTE);
      }
    }
    else  /* quote message sent, turn quote on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
 
        }
 
      REMOVE_BIT(ch->comm,COMM_NOQUOTE);
 
      sprintf( buf, "`QYou quote '%s'``\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUOTE) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "`Q$n quotes '$t'``",
                   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

/* RT OOC channel by Beowolf */
void do_ooc( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't ooc until level 2.\n\r", ch);
	return;
      }
    
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOOOC))
      {
        send_to_char("OOC channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOOOC);
      }
      else
      {
        send_to_char("OOC channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOOOC);
      }
    }
    else  /* ooc sent, turn ooc on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOOOC);
 
      
      sprintf( buf, "`OYou [OOC]:'%s'\n\r``", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOOOC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	  act_new("`O$n [OOC]: '$t'``",
	 	  ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* RT music channel */
void do_music( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't music until level 2.\n\r", ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOMUSIC))
      {
        send_to_char("Music channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
      }
      else
      {
        send_to_char("Music channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOMUSIC);
      }
    }
    else  /* music sent, turn music on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
 
      sprintf( buf, "`MYou MUSIC: '%s'\n\r``", argument );
      send_to_char( buf, ch );
      sprintf( buf, "`M$n MUSIC: '%s'``", argument );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOMUSIC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    act_new("`M$n MUSIC: '$t'``",
		    ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* clan channels
 * Replaced with devotiontalk on 5-5-96 when SECTS were IMP'd
 */

void do_devotiontalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

      /* Check added by Beowolf */
      if (ch->level <= 9)
      {
	send_to_char("You can't speak with your sect until level 10.\n\r", ch);
	return;
      }
      
    if (!is_clan(ch) || clan_table[ch->clan].independent)
    {
	send_to_char("You aren't in a sect.\n\r",ch);
	return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Devotion channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Devotion channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);

      sprintf( buf, "`F[DEVOTION]: '%s'\n\r``", argument );
      send_to_char( buf, ch );
      sprintf( buf, "`F[DEVOTION] $n: '%s'``", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
	     is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
	     !IS_SET(d->character->comm,COMM_QUIET) )
        {
            act_new("`F[DEVOTION] $n:'$t'``",ch,argument,d->character,TO_VICT,POS_DEAD);
        }
    }

    return;
}

void do_buildtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
	send_to_char("Builder channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
	send_to_char("Builder channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_NOWIZ);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

    sprintf( buf, "`B[BUILD]: %s``", argument );
    act_new("`B[BUILD]: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     IS_BUILD(d->character) && 
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{
	    act_new("`B[BUILD] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);
	}
    }

    return;
}

/*
void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
	send_to_char("Immortal channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
	send_to_char("Immortal channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_NOWIZ);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

    sprintf( buf, "`A$n: %s``", argument );
    act_new("`A$n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     IS_IMMORTAL(d->character) && 
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{
	    act_new("`A$n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);
	}
    }

    return;
}*/

void do_say( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\n\r", ch );
        return;
    }

#if defined ( FORCE_LANGUAGE )
        if (!IS_NPC(ch))
       
        do_language(ch, argument, ch->pcdata->speaking);
        else
        {
        act( "`S$n says '$T'``", ch, NULL, argument, TO_ROOM );
        act( "`SYou say '$T'``", ch, NULL, argument, TO_CHAR );
        } 
     
#else
        act( "`S$n says '$T'``", ch, NULL, argument, TO_ROOM );
        act( "`SYou say '$T'``", ch, NULL, argument, TO_CHAR );
        return;
#endif

     /* MOBPROGS */
     if ( !IS_NPC(ch) )
     {
 	CHAR_DATA *mob, *mob_next;
 	for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
 	{
 	    mob_next = mob->next_in_room;
 	    if ( IS_NPC(mob) && HAS_TRIGGER( mob, TRIG_SPEECH )
 	    &&   mob->position == mob->pIndexData->default_pos )
 		mp_act_trigger( argument, mob, ch, NULL, NULL, TRIG_SPEECH );
 	}
     /* end mobprogs */
         return;
 }
}

void do_shout( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't shout until level 2.\n\r", ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      	if (IS_SET(ch->comm,COMM_SHOUTSOFF))
      	{
            send_to_char("You can hear shouts again.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	else
      	{
            send_to_char("You will no longer hear shouts.\n\r",ch);
            SET_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't shout.\n\r", ch );
        return;
    }
 
    REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);

    WAIT_STATE( ch, 12 );

    act( "`XYou shout '$T'``", ch, NULL, argument, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
	     !IS_SET(victim->comm, COMM_QUIET) ) 
	{
	    act("`X$n shouts '$t'``",ch,argument,d->character,TO_VICT);
	}
    }

    return;
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( IS_SET(ch->comm, COMM_QUIET) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch);
	return;
    }

    if (IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char("You must turn off deaf mode first.\n\r",ch);
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",
	    ch,NULL,victim,TO_CHAR);
        sprintf(buf,"`T%s tells you '%s'\n\r``",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	return;
    }

    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
  
    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    && !IS_IMMORTAL(ch))
    {
	act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
  	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    act("$E is AFK, and not receiving tells.",ch,NULL,victim,TO_CHAR);
	    return;
	}

	act("$E is AFK, but your tell will go through when $E returns.",
	    ch,NULL,victim,TO_CHAR);
	sprintf(buf,"`T%s tells you '%s'\n\r``",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	return;
    }

    act( "`TYou tell $N '$t'``", ch, argument, victim, TO_CHAR );
    act_new("`T$n tells you '$t'``",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

     /* MOBPROGS */

    if ( !IS_NPC(ch) && IS_NPC(victim) && HAS_TRIGGER(victim,TRIG_SPEECH) )
 	mp_act_trigger( argument, victim, ch, NULL, NULL, TRIG_SPEECH );
   
    /* end mobprog */

    return;
}

void do_btell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( IS_SET(ch->comm, COMM_QUIET) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch);
	return;
    }

    if (IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char("You must turn off deaf mode first.\n\r",ch);
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Btell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",
	    ch,NULL,victim,TO_CHAR);
        sprintf(buf,"`T%s b-tells you '%s'\a\n\r``",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	return;
    }

    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
  
    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    && !IS_IMMORTAL(ch))
    {
	act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
  	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    act("$E is AFK, and not receiving tells.",ch,NULL,victim,TO_CHAR);
	    return;
	}

	act("$E is AFK, but your tell will go through when $E returns.",
	    ch,NULL,victim,TO_CHAR);
	sprintf(buf,"`T%s b-tells you '%s'\a\n\r``",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	return;
    }

    act( "`TYou tell $N '$t'``\a", ch, argument, victim, TO_CHAR );
    act_new("`T$n tells you '$t'``\a",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

     /* MOBPROGS */

    if ( !IS_NPC(ch) && IS_NPC(victim) && HAS_TRIGGER(victim,TRIG_SPEECH) )
 	mp_act_trigger( argument, victim, ch, NULL, NULL, TRIG_SPEECH );
   
    /* end mobprog */

    return;
}

void do_forget( CHAR_DATA *ch, char *argument )
{

    CHAR_DATA *victim;
    char arg[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
       send_to_char("Forget <ALL or Name>\n\r",ch);
       return;
    }

    if ( ( ( victim = get_char_world(ch, arg) ) == NULL) && !str_cmp(arg,"ALL") )
    {    
        send_to_char( " There not here. \n\r",ch );
        return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim) && ! str_cmp(arg,"ALL") )
    {
	act("$N seems to have misplaced $S link...try again later.",
	    ch,NULL,victim,TO_CHAR);
	return;
    }

    if ( !str_cmp(arg,"ALL"))
    {
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch  && victim->reply == ch)
        {
            victim->reply = NULL;
        }
      }
    }
    else
    {
       if(victim!=NULL)
          victim->reply = NULL;
    }
}    
    

void do_reply( CHAR_DATA *ch, char *argument )
{

    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if ( IS_SET(ch->comm, COMM_NOTELL) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
        act("$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        sprintf(buf,"`R%s tells you '%s'\n\r``",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
        return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    &&  !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
    {
        act_new( "$E is not receiving tells.", ch, 0, victim, TO_CHAR,POS_DEAD);
        return;
    }

    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
        if (IS_NPC(victim))
        {
            act_new("$E is AFK, and not receiving tells.",
		ch,NULL,victim,TO_CHAR,POS_DEAD);
            return;
        }
 
        act_new("$E is AFK, but your tell will go through when $E returns.",
            ch,NULL,victim,TO_CHAR,POS_DEAD);
        sprintf(buf,"`R%s tells you '%s'\n\r``",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
        return;
    }

    act_new("`RYou tell $N '$t'``",ch,argument,victim,TO_CHAR,POS_DEAD);
    act_new("`R$n tells you '$t'``",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

    return;
}

void do_yell( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't yell until level 2.\n\r", ch);
	return;
      }
      
    if (argument[0] == '\0' )
    {
      	if (IS_SET(ch->comm,COMM_YELL))
      	{
            send_to_char("You can hear yells again.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_YELL);
      	}
      	else
      	{
            send_to_char("You will no longer hear yells.\n\r",ch);
            SET_BIT(ch->comm,COMM_YELL);
      	}
      	return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't yell.\n\r", ch );
        return;
    }
 
    REMOVE_BIT(ch->comm,COMM_YELL);

    WAIT_STATE( ch, 12 );

    act("`yYou yell '$t'``",ch,argument,NULL,TO_CHAR);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;
	
        if ( d->connected == CON_PLAYING
	&&   d->character != ch
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == ch->in_room->area 
	&&   !IS_SET(victim->comm, COMM_YELL) 
        &&   !IS_SET(d->character->comm,COMM_QUIET) )
	{
	    act("`y$n yells '$t'``",ch,argument,d->character,TO_VICT);
	}
    }
}

void do_emote( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
    
    /* MOBPROGS */ 
   
    MOBtrigger = FALSE;
    act( "$n $T", ch, NULL, argument, TO_ROOM );
    act( "$n $T", ch, NULL, argument, TO_CHAR );
    MOBtrigger = TRUE;
    
    /* end mobprogs */

    return;
}


void do_pmote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
 
    act( "$n $t", ch, argument, NULL, TO_CHAR );

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->desc == NULL || vch == ch)
	    continue;

        /* MOBPROGS */

  	if ((letter = strstr(argument,vch->name)) == NULL)
  	{
 	    MOBtrigger = FALSE;
  	    act("$N $t",vch,argument,ch,TO_CHAR);
 	    MOBtrigger = TRUE;
  	    continue;
  	}
 
	/* end mobprogs */

	strcpy(temp,argument);
	temp[strlen(argument) - strlen(letter)] = '\0';
   	last[0] = '\0';
 	name = vch->name;
	
	for (; *letter != '\0'; letter++)
	{ 
	    if (*letter == '\'' && matches == strlen(vch->name))
	    {
		strcat(temp,"r");
		continue;
	    }

	    if (*letter == 's' && matches == strlen(vch->name))
	    {
		matches = 0;
		continue;
	    }
	    
 	    if (matches == strlen(vch->name))
	    {
		matches = 0;
	    }

	    if (*letter == *name)
	    {
		matches++;
		name++;
		if (matches == strlen(vch->name))
		{
		    strcat(temp,"you");
		    last[0] = '\0';
		    name = vch->name;
		    continue;
		}
		strncat(last,letter,1);
		continue;
	    }

	    matches = 0;
	    strcat(temp,last);
	    strncat(temp,letter,1);
	    last[0] = '\0';
	    name = vch->name;
	}

        /* MOBPROGS */

 	MOBtrigger = FALSE;
  	act("$N $t",vch,temp,ch,TO_CHAR);
 	MOBtrigger = TRUE;

	/* end mobprogs */
    }
	
    return;
}


/*
 * All the posing stuff.
 */
struct	pose_table_type
{
    char *	message[2*MAX_CLASS];
};

const	struct	pose_table_type	pose_table	[]	=
{
    {
	{
	    "You sizzle with energy.",
	    "$n sizzles with energy.",
	    "You feel very holy.",
	    "$n looks very holy.",
	    "You perform a small card trick.",
	    "$n performs a small card trick.",
	    "You show your bulging muscles.",
	    "$n shows $s bulging muscles."
	}
    },

    {
	{
	    "You turn into a butterfly, then return to your normal shape.",
	    "$n turns into a butterfly, then returns to $s normal shape.",
	    "You nonchalantly turn wine into water.",
	    "$n nonchalantly turns wine into water.",
	    "You wiggle your ears alternately.",
	    "$n wiggles $s ears alternately.",
	    "You crack nuts between your fingers.",
	    "$n cracks nuts between $s fingers."
	}
    },

    {
	{
	    "Blue sparks fly from your fingers.",
	    "Blue sparks fly from $n's fingers.",
	    "A halo appears over your head.",
	    "A halo appears over $n's head.",
	    "You nimbly tie yourself into a knot.",
	    "$n nimbly ties $mself into a knot.",
	    "You grizzle your teeth and look mean.",
	    "$n grizzles $s teeth and looks mean."
	}
    },

    {
	{
	    "Little red lights dance in your eyes.",
	    "Little red lights dance in $n's eyes.",
	    "You recite words of wisdom.",
	    "$n recites words of wisdom.",
	    "You juggle with daggers, apples, and eyeballs.",
	    "$n juggles with daggers, apples, and eyeballs.",
	    "You hit your head, and your eyes roll.",
	    "$n hits $s head, and $s eyes roll."
	}
    },

    {
	{
	    "A slimy green monster appears before you and bows.",
	    "A slimy green monster appears before $n and bows.",
	    "Deep in prayer, you levitate.",
	    "Deep in prayer, $n levitates.",
	    "You steal the underwear off every person in the room.",
	    "Your underwear is gone!  $n stole it!",
	    "Crunch, crunch -- you munch a bottle.",
	    "Crunch, crunch -- $n munches a bottle."
	}
    },

    {
	{
	    "You turn everybody into a little pink elephant.",
	    "You are turned into a little pink elephant by $n.",
	    "An angel consults you.",
	    "An angel consults $n.",
	    "The dice roll ... and you win again.",
	    "The dice roll ... and $n wins again.",
	    "... 98, 99, 100 ... you do pushups.",
	    "... 98, 99, 100 ... $n does pushups."
	}
    },

    {
	{
	    "A small ball of light dances on your fingertips.",
	    "A small ball of light dances on $n's fingertips.",
	    "Your body glows with an unearthly light.",
	    "$n's body glows with an unearthly light.",
	    "You count the money in everyone's pockets.",
	    "Check your money, $n is counting it.",
	    "Arnold Schwarzenegger admires your physique.",
	    "Arnold Schwarzenegger admires $n's physique."
	}
    },

    {
	{
	    "Smoke and fumes leak from your nostrils.",
	    "Smoke and fumes leak from $n's nostrils.",
	    "A spot light hits you.",
	    "A spot light hits $n.",
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue.",
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders."
	}
    },

    {
	{
	    "The light flickers as you rap in magical languages.",
	    "The light flickers as $n raps in magical languages.",
	    "Everyone levitates as you pray.",
	    "You levitate as $n prays.",
	    "You produce a coin from everyone's ear.",
	    "$n produces a coin from your ear.",
	    "Oomph!  You squeeze water out of a granite boulder.",
	    "Oomph!  $n squeezes water out of a granite boulder."
	}
    },

    {
	{
	    "Your head disappears.",
	    "$n's head disappears.",
	    "A cool breeze refreshes you.",
	    "A cool breeze refreshes $n.",
	    "You step behind your shadow.",
	    "$n steps behind $s shadow.",
	    "You pick your teeth with a spear.",
	    "$n picks $s teeth with a spear."
	}
    },

    {
	{
	    "A fire elemental singes your hair.",
	    "A fire elemental singes $n's hair.",
	    "The sun pierces through the clouds to illuminate you.",
	    "The sun pierces through the clouds to illuminate $n.",
	    "Your eyes dance with greed.",
	    "$n's eyes dance with greed.",
	    "Everyone is swept off their foot by your hug.",
	    "You are swept off your feet by $n's hug."
	}
    },

    {
	{
	    "The sky changes color to match your eyes.",
	    "The sky changes color to match $n's eyes.",
	    "The ocean parts before you.",
	    "The ocean parts before $n.",
	    "You deftly steal everyone's weapon.",
	    "$n deftly steals your weapon.",
	    "Your karate chop splits a tree.",
	    "$n's karate chop splits a tree."
	}
    },

    {
	{
	    "The stones dance to your command.",
	    "The stones dance to $n's command.",
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n.",
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer.",
	    "A strap of your armor breaks over your mighty thews.",
	    "A strap of $n's armor breaks over $s mighty thews."
	}
    },

    {
	{
	    "The heavens and grass change colour as you smile.",
	    "The heavens and grass change colour as $n smiles.",
	    "The Burning Man speaks to you.",
	    "The Burning Man speaks to $n.",
	    "Everyone's pocket explodes with your fireworks.",
	    "Your pocket explodes with $n's fireworks.",
	    "A boulder cracks at your frown.",
	    "A boulder cracks at $n's frown."
	}
    },

    {
	{
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing.",
	    "An eye in a pyramid winks at you.",
	    "An eye in a pyramid winks at $n.",
	    "Everyone discovers your dagger a centimeter from their eye.",
	    "You discover $n's dagger a centimeter from your eye.",
	    "Mercenaries arrive to do your bidding.",
	    "Mercenaries arrive to do $n's bidding."
	}
    },

    {
	{
	    "A black hole swallows you.",
	    "A black hole swallows $n.",
	    "Valentine Michael Smith offers you a glass of water.",
	    "Valentine Michael Smith offers $n a glass of water.",
	    "Where did you go?",
	    "Where did $n go?",
	    "Four matched Percherons bring in your chariot.",
	    "Four matched Percherons bring in $n's chariot."
	}
    },

    {
	{
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling.",
	    "The great god Ao gives you a staff.",
	    "The great god Ao gives $n a staff.",
	    "Click.",
	    "Click.",
	    "Atlas asks you to relieve him.",
	    "Atlas asks $n to relieve him."
	}
    }
};



void do_pose( CHAR_DATA *ch, char *argument )
{
    int level;
    int pose;

    if ( IS_NPC(ch) )
	return;

    level = UMIN( ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1 );
    pose  = number_range(0, level);

    act( pose_table[pose].message[2*ch->class+0], ch, NULL, NULL, TO_CHAR );
    act( pose_table[pose].message[2*ch->class+1], ch, NULL, NULL, TO_ROOM );

    return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Bug logged.\n\r", ch );
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Typo logged.\n\r", ch );
    return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}


void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];/*NIKKI*/
    DESCRIPTOR_DATA *d,*d_next;
    int id;
    CHAR_DATA *wch;
    int immcount;
    extern int AUTHORIZE_LOGINS;
    immcount=0;
    
    if ( IS_NPC(ch) )
	return;

    
    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }
 
   /* Arena Code by Whiplash */
    
   if(IS_SET(ch->in_room->room_flags,ROOM_ARENA))
    {
       send_to_char("You can't quit while in the arena!\n\r",ch);
       return;
     }

    if ( ch->position  < POS_STUNNED  )
    {
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }
    if ( ch->in_ship )
    {
	send_to_char( "You can't quit in a ship.\n\r", ch );
	return;
    }
    if ( ch->in_wagon )
    {
	send_to_char( "You can't quit in a wagon.\n\r", ch );
	return;
    }


    for(d = descriptor_list; d != NULL; d = d->next)
    {
        if ( d->connected != CON_PLAYING) 
        continue;
        wch = (d->original != NULL) ? d->original : d->character;
        if(wch->level >= LEVEL_IMMORTAL) immcount++;
    }
    
    if( IS_IMMORTAL(ch) && AUTHORIZE_LOGINS == TRUE && immcount == 1) 
    {
        send_to_char( "Turn off the Authorization you dolt!\n\r", ch);
        return;
     }
    

        send_to_char( "The World is a Program, We're the Virus.\n\rCome Back for More Happy Mudding!\n\r\n\r",ch);
        ch->pcdata->last = current_time; /* Set their last logon time */
        act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
        sprintf( log_buf, "%s has quit.\n\r", ch->name );
        log_string( log_buf );
        wiznet("[`4WIZNET``] `8$N`` `@rejoins the real world.``",ch,NULL,WIZ_LOGINS,0,get_trust(ch));

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    id = ch->id;
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
	close_socket( d );
    sprintf(buf, "%s has been taken from this world by Ao.",ch->name);
    if(!IS_IMMORTAL(ch)) 
    do_info_char(ch,buf);
 /* Toast the Cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if (tch && tch->id == id)
	{
	    extract_char(tch,TRUE);
	    close_socket(d);
	} 
    }

    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

      /* Check added by Beowolf */
      if (ch->level <= 1)
      {
	send_to_char("You can't save until level 2.\n\r", ch);
	return;
      }
 
    ch->pcdata->last=current_time; 
    save_char_obj( ch );
    send_to_char("TSR is now saving your character... \n\r", ch);
    WAIT_STATE(ch,4 * PULSE_VIOLENCE);
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower(ch);
	return;
    }

    if (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOFOLLOW) && !IS_IMMORTAL(ch))
    {
	act("$N doesn't seem to want any followers.\n\r",
             ch,NULL,victim, TO_CHAR);
        return;
    }

    REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    
    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}


void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) && ch->in_room != NULL)
    {
	act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
    	act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
    }
    if (ch->master->pet == ch)
	ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets( CHAR_DATA *ch )
{    
    CHAR_DATA *pet;

    if ((pet = ch->pet) != NULL)
    {
    	stop_follower(pet);
    	if (pet->in_room != NULL)
    	    act("$N slowly fades away.",ch,NULL,pet,TO_NOTVICT);
    	extract_char(pet,TRUE);
    }
    ch->pet = NULL;

    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
    {
    	if (ch->master->pet == ch)
    	    ch->master->pet = NULL;
	stop_follower( ch );
    }

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument( argument, arg );
    one_argument(argument,arg2);

    /*
     * MOBPROGS
     * if (!str_cmp(arg2,"delete"))
     */

    if (!str_cmp(arg2,"delete") || !str_cmp(arg2,"mob"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"kill"))
    {
        send_to_char("I'm NOT attacking first! You do it! I'm scared!\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"push"))
    {
        send_to_char("ME??? I'm just your pet! You push them!\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"bash"))
    {
        send_to_char("I'm NOT attacking first! You do it! I'm scared!\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"help"))
    {
        send_to_char("Since when can mobs read help files?\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"motd"))
    {
        send_to_char("Since when can mobs read the motd?\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"command"))
    {
        send_to_char("Since when can mobs read the command files?\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"commands"))
    {
        send_to_char("Since when can mobs read the command files?\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"social"))
    {
        send_to_char("Since when can mobs read the social files?\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"backstab"))
    {
        send_to_char("I'm NOT attacking first! You do it! I'm scared!\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"steal"))
    {
        send_to_char("Trying to cheat? BUG LOGGED! You have lost a level!\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"circle"))
    {
        send_to_char("I'm NOT attacking first! You do it! I'm scared!\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"who") || !str_cmp(arg2,"whois"))
    {
        send_to_char("You expect me to tell you whos on. Do it
yourself.\n\r",ch);
        return;
    }/* Nikki to fix who crashing the mud */

    /* end mobprogs */

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch 
	||  (IS_IMMORTAL(victim) && victim->trust >= ch->trust))
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	    sprintf( buf, "$n orders you to '%s'.", argument );
	    act( buf, ch, NULL, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
	WAIT_STATE(ch,PULSE_VIOLENCE);
	send_to_char( "Ok.\n\r", ch );
    }
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS(leader, ch) );
	send_to_char( buf, ch );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
		    gch->level,
		    IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act_new("$N isn't following you.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	return;
    }
    
    if (IS_AFFECTED(victim,AFF_CHARM))
    {
        send_to_char("You can't remove charmed mobs from your group.\n\r",ch);
        return;
    }
    
    if (IS_AFFECTED(ch,AFF_CHARM))
    {
    	act_new("You like your master too much to leave $m!",
	    ch,NULL,victim,TO_VICT,POS_SLEEPING);
    	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act_new("$n removes $N from $s group.",
	    ch,NULL,victim,TO_NOTVICT,POS_RESTING);
	act_new("$n removes you from $s group.",
	    ch,NULL,victim,TO_VICT,POS_SLEEPING);
	act_new("You remove $N from your group.",
	    ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	return;
    }

    victim->leader = ch;
    act_new("$N joins $n's group.",ch,NULL,victim,TO_NOTVICT,POS_RESTING);
    act_new("You join $n's group.",ch,NULL,victim,TO_VICT,POS_SLEEPING);
    act_new("$N joins your group.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_gold = 0, amount_silver = 0;
    int share_gold, share_silver;
    int extra_gold, extra_silver;

    argument = one_argument( argument, arg1 );
	       one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }
    
    amount_silver = atoi( arg1 );

    if (arg2[0] != '\0')
	amount_gold = atoi(arg2);

    if ( amount_gold < 0 || amount_silver < 0)
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount_gold == 0 && amount_silver == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold <  amount_gold || ch->silver < amount_silver)
    {
	send_to_char( "You don't have that much to split.\n\r", ch );
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM))
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }
	    
    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;

    share_gold   = amount_gold / members;
    extra_gold   = amount_gold % members;

    if ( share_gold == 0 && share_silver == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->silver	-= amount_silver;
    ch->silver	+= share_silver + extra_silver;
    ch->gold 	-= amount_gold;
    ch->gold 	+= share_gold + extra_gold;

    if (share_silver > 0)
    {
	sprintf(buf,
	    "You split %d silver coins. Your share is %d silver.\n\r",
 	    amount_silver,share_silver + extra_silver);
	send_to_char(buf,ch);
    }

    if (share_gold > 0)
    {
	sprintf(buf,
	    "You split %d gold coins. Your share is %d gold.\n\r",
	     amount_gold,share_gold + extra_gold);
	send_to_char(buf,ch);
    }

    if (share_gold == 0)
    {
	sprintf(buf,"$n splits %d silver coins. Your share is %d silver.",
		amount_silver,share_silver);
    }
    else if (share_silver == 0)
    {
	sprintf(buf,"$n splits %d gold coins. Your share is %d gold.",
		amount_gold,share_gold);
    }
    else
    {
	sprintf(buf,
"$n splits %d silver and %d gold coins, giving you %d silver and %d gold.\n\r",
	 amount_silver,amount_gold,share_silver,share_gold);
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group(gch,ch) && !IS_AFFECTED(gch,AFF_CHARM))
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share_gold;
	    gch->silver += share_silver;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
/*    CHAR_DATA *victim; */

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	    act_new("`T$n tells the group '$t'``",
		ch,argument,gch,TO_VICT,POS_SLEEPING);
    }
    
    act( "`TYou tell the group '$t'``", ch, argument, NULL, TO_CHAR);
    return;
}

/* === Sign Language ======================== */
 void do_sign( CHAR_DATA *ch, char *argument)
  {
  CHAR_DATA *och;
  int    chance;
  int    chance2;
 
  if((chance = get_skill(ch,gsn_sign)) == 0) 
  {
  send_to_char("You don't know how to speak with your hands.\n\r",ch);
  return; 
  }

  if(argument[0] == '\0')
  {
  send_to_char("Sign WHAT?\n\r",ch);
  return; 
  }

  if(number_percent( ) < chance )
   {
    act("`SYou sign, '$t'``",ch,argument,NULL,TO_CHAR);
     
     for(och = ch->in_room->people; och != NULL; och = och->next_in_room )

       if((chance2 = get_skill(och,gsn_sign)) == 0)
       {
       act("`S$n makes funny movements with his hands.``",ch,NULL,och,TO_VICT);
       }

       else
       if(number_percent( ) < chance2)
       {
       act("`S$n signs, '$t'``",ch,argument,och,TO_VICT);
       check_improve(ch,gsn_sign,TRUE,4);
       }
      }
 check_improve(ch,gsn_sign,FALSE,4);
  }
/* === End of Sign Language ===== */

/*
 * === Godspeak Language ======================== 
 *                         -Beowolf
 */

 void do_godspeak( CHAR_DATA *ch, char *argument)
  {
  CHAR_DATA *och;
  int    chance;
  int    chance2;
 
  if((chance = get_skill(ch,gsn_godspeak)) == 0) 
  {
  send_to_char("You don't know how to speak in the tongue of the gods.\n\r",ch);
  return; 
  }

  if(argument[0] == '\0')
  {
  send_to_char("Say WHAT in Godspeak?\n\r",ch);
  return; 
  }

    if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\n\r", ch );
        return;
    }
  
  if(number_percent( ) < chance )
   {
    act("`SYour voice booms  '$t'``",ch,argument,NULL,TO_CHAR);
     
     for(och = ch->in_room->people; och != NULL; och = och->next_in_room )

       if((chance2 = get_skill(och,gsn_godspeak)) == 0)
       {
act("`S$n speaks something in an ancient tongue. A chill runs down your spine.``",ch,NULL,och,TO_VICT);
       }

       else
       if(number_percent( ) < chance2)
       {
       act("`SIn Godspeak, $n says '$t'``",ch,argument,och,TO_VICT);
       check_improve(ch,gsn_godspeak,TRUE,4);
       }
      }
 check_improve(ch,gsn_godspeak,FALSE,4);
  }
/* === End of Godspeak Language ===== */

void do_pray( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_PRAY))
      {
	send_to_char("pray channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_PRAY);
      }
      else
      {
	send_to_char("Pray channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_PRAY);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_PRAY);

    sprintf( buf, "`A$n: %s``", argument );
    act_new("`AYou pray: $t``",NULL,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     IS_IMMORTAL(d->character) && 
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{
	    act_new("`A$n prays: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);
	}
    }

    return;
}
  
void do_admintalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
     if (ch->level < (MAX_LEVEL - 1))
     {
 	send_to_char("Sorry, you are not an Admin!",ch);
 	return;
     }
 
     if ( argument[0] == '\0' )
     {
       if (IS_SET(ch->comm,COMM_NOADMIN))
       {
 	send_to_char("Admin channel is now ON\n\r",ch);
 	REMOVE_BIT(ch->comm,COMM_NOADMIN);
       }
       else
       {
 	send_to_char("Admin channel is now OFF\n\r",ch);
 	SET_BIT(ch->comm,COMM_NOADMIN);
       } 
       return;
     }
 
     REMOVE_BIT(ch->comm,COMM_NOADMIN);
 
     sprintf( buf, "[Admin] $n: %s", argument );
     act_new("`Z[Admin] $n: '$t``'",ch,argument,NULL,TO_CHAR,POS_DEAD);
     for ( d = descriptor_list; d != NULL; d = d->next )
     {
 	if ( d->connected == CON_PLAYING && 
 	     IS_ADMIN(d->character) && 
              !IS_SET(d->character->comm,COMM_NOADMIN) )
 	{
 	    act_new("`Z[Admin] $n: '$t``'",ch,argument,d->character,TO_VICT,POS_DEAD);
 	}
     }
 
     return;
}
 
void do_newbie( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
  
    if (!IS_NEWBIE(ch))
    {
	send_to_char("You're not a Newbie!\n\r", ch);
 	return;
     }
 
     if (argument[0] == '\0' )
     {
       if (IS_SET(ch->comm,COMM_NEWBIE))
       {
         send_to_char("Newbie channel is now ON.\n\r",ch);
         REMOVE_BIT(ch->comm,COMM_NEWBIE);
       }
       else
       {
         send_to_char("Newbie channel is now OFF.\n\r",ch);
         SET_BIT(ch->comm,COMM_NEWBIE);
       }
     }
     else
     {
        if (IS_SET(ch->comm,COMM_QUIET))
         {
           send_to_char("You must turn off quiet mode first.\n\r",ch);
           return;
         }
  
         if (IS_SET(ch->comm,COMM_NOCHANNELS))
         {
           send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
           return;
  
         }
  
       REMOVE_BIT(ch->comm,COMM_NEWBIE);
  
       sprintf( buf, "`n[NEWBIE] %s: '%s``'\n\r", ch->name, argument );
       send_to_char( buf, ch );
       for ( d = descriptor_list; d != NULL; d = d->next )
       {
         CHAR_DATA *victim;
  
         victim = d->original ? d->original : d->character;
  
         if ( d->connected == CON_PLAYING &&
              d->character != ch &&
              !IS_SET(victim->comm,COMM_NEWBIE) &&
              !IS_SET(victim->comm,COMM_QUIET) )
         {
           act_new( "`n[NEWBIE] $n: '$t``'",
                    ch,argument, d->character, TO_VICT,POS_DEAD );
         }
       }
     }
 }
 
void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
	send_to_char("Immortal channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
	send_to_char("Immortal channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_NOWIZ);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

	if (ch->level == MAX_LEVEL)
	{
    	   sprintf( buf, "[Adm] $n: %s", argument );
           act_new("`A[Adm] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 1))
	{
    	   sprintf( buf, "[Imp] $n: %s", argument );
           act_new("`A[Imp] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 2))
	{
    	   sprintf( buf, "[Cre] $n: %s", argument );
           act_new("`A[Cre] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 3))
	{
    	   sprintf( buf, "[Dei] $n: %s", argument );
           act_new("`A[Dei] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 4))
	{
    	   sprintf( buf, "[God] $n: %s", argument );
           act_new("`A[God] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 5))
	{
    	   sprintf( buf, "[Imm] $n: %s", argument );
           act_new("`A[Imm] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 6))
	{
    	   sprintf( buf, "[Dem] $n: %s", argument );
           act_new("`A[Dem] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 7))
	{
    	   sprintf( buf, "[Ang] $n: %s", argument );
           act_new("`A[Ang] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}
	else if (ch->level == (MAX_LEVEL - 8))
	{
    	   sprintf( buf, "[Ava] $n: %s", argument );
           act_new("`A[Ava] $n: $t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
	}

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     IS_IMMORTAL(d->character) && 
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{

	if (ch->level == MAX_LEVEL)
        act_new("`A[Adm] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 1))
        act_new("`A[Imp] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 2))
        act_new("`A[Cre] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 3))
        act_new("`A[Dei] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 4))
        act_new("`A[God] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 5))
        act_new("`A[Imm] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 6))
        act_new("`A[Dem] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	else if (ch->level == (MAX_LEVEL - 7))
        act_new("`A[Ang] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);
	
	else if (ch->level == (MAX_LEVEL - 8))
        act_new("`A[Ava] $n: $t``",ch,argument,d->character,TO_VICT,POS_DEAD);

	}
    }

    return;
}

/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach == NULL || bch == NULL)
	return FALSE;

    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}

/*
 * Lookup a language by name.
 */
int lang_lookup( const char *name )
{
    int ln;

    for ( ln = 0; ln < MAX_LANGUAGE; ln++ )
    {
        if ( !lang_table[ln].name )
            break;
        if ( LOWER( name[0] ) == LOWER( lang_table[ln].name[0] )
            && !str_prefix( name, lang_table[ln].name ) )
            return ln;
    }

    return -1;
}

void do_speak( CHAR_DATA *ch, char *argument )
{
	char	arg [ MAX_INPUT_LENGTH ];
	char	buf [ MAX_STRING_LENGTH];
	int	speaking;
	int	canspeak;

	argument = one_argument(argument, arg);

	buf[0] = '\0';

	if (IS_NPC(ch))
	{
		send_to_char ("Mobiles can't speak !\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		sprintf (buf, "You currently speak %s.\n\r", lang_table[ch->pcdata->speaking].name);
		send_to_char (buf, ch);
	}
	else
	{
		if ((speaking = lang_lookup(arg)) != -1)
		{
			if ((canspeak = ch->pcdata->language[speaking]) == 0)
			{
				sprintf (buf, "But you don't know how to speak %s.\n\r", lang_table[speaking].name);
				send_to_char(buf, ch);
			}
			else
			{
				ch->pcdata->speaking = speaking;
				sprintf (buf, "You will speak %s from now on.\n\r", lang_table[ch->pcdata->speaking].name);
				send_to_char(buf, ch);
			}
		}
		else
		{
			sprintf (buf, "%s is not a valid language!\n\r", arg);
			send_to_char( buf, ch);
		}
	}
}
		
void do_lset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        ln;
    bool       fAll;

    rch = get_char_world( ch, arg1 );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char( "Syntax: lset <victim> <lang> <value>\n\r",	ch );
        send_to_char( "or:     lset <victim> all    <value>\n\r",	ch );
        send_to_char( "Lang being any language.\n\r",			ch );
        return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ch->level <= victim->level && ch != victim )
    {
        send_to_char( "You may not lset your peer nor your superior.\n\r", ch );
        return;
    }

    fAll = !str_cmp( arg2, "all" );
    ln   = 0;
    if ( !fAll && ( ln = lang_lookup( arg2 ) ) < 0 )
    {
        send_to_char( "No such language.\n\r", ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
        send_to_char( "Value must be numeric.\n\r", ch );
        return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
        send_to_char( "Value range is 0 to 100.\n\r", ch );
        return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < 160 )
        {
            send_to_char( "Only Imps may lset all.\n\r", ch );
            return;
        }
        for ( ln = 0; ln < MAX_LANGUAGE; ln++ )
            if ( lang_table[ln].name )
                victim->pcdata->language[ln] = value;
    }
    else
    {
        victim->pcdata->language[ln] = value;
    }
    return;
}

void do_lstat( CHAR_DATA *ch, char *argument )
/* lstat by Maniac && Canth */
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int ln;
    int col;

    one_argument( argument, arg );
    col = 0;

    if ( arg[0] == '\0' )
    {
        send_to_char("lstat whom?\n\r", ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char("That person isn't logged on.\n\r", ch);
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char("Not on NPC's.\n\r", ch);
        return;
    }

    buf2[0] = '\0';

    for ( ln = 0; ln < MAX_LANGUAGE ; ln++ )
    {
        if ( lang_table[ln].name == NULL )
            break;
        sprintf( buf1, "%18s %3d %% ", lang_table[ln].name,
                victim->pcdata->language[ln] );
        strcat( buf2, buf1 );
        if ( ++col %3 == 0 )
            strcat( buf2, "\n\r" );
    }
    if ( col % 3 != 0 )
         strcat( buf2, "\n\r" );
    sprintf( buf1, "%s has %d learning sessions left.\n\r", victim->name,
                victim->pcdata->learn );
    strcat( buf2, buf1 );

    send_to_char( buf2, ch );
    return;

}

void do_learn( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH   ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  ln;
    int  money = ch->level * 150;

    if ( IS_NPC( ch ) )
        return;

    buf1[0] = '\0';

    if ( argument[0] == '\0' )
    {
        CHAR_DATA *mob;
        int        col;

        for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
        {
            if ( IS_NPC( mob ) && IS_SET( mob->act2, ACT2_TEACHER ) )
                break;
        }

        col    = 0;
        for ( ln = 0; ln < MAX_LANGUAGE; ln++ )
        {
            if ( !lang_table[ln].name )
                break;

            if ( ( mob ) || ( ch->pcdata->language[ln] > 0 ) )
            {
                sprintf( buf, "%18s %3d%%  ",
                        lang_table[ln].name, ch->pcdata->language[ln] );
                strcat( buf1, buf );
                if ( ++col % 3 == 0 )
                    strcat( buf1, "\n\r" );
            }
        }

        if ( col % 3 != 0 )
            strcat( buf1, "\n\r" );

        sprintf( buf, "You have %d learning sessions left.\n\r",
                ch->pcdata->learn);
        strcat( buf1, buf );
        sprintf( buf, "Cost of lessons is %d silver coins.\n\r", money );
        strcat( buf1, buf );
        send_to_char( buf1, ch );
    }
    else
    {
        CHAR_DATA *mob;
        int        adept;

        if ( !IS_AWAKE( ch ) )
        {
            send_to_char( "In your dreams, or what?\n\r", ch );
            return;
        }

        for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
        {
            if ( IS_NPC( mob ) && IS_SET( mob->act2, ACT2_TEACHER ) )
                break;
        }

        if ( !mob )
        {
            send_to_char( "You can't do that here.\n\r", ch );
            return;
        }

        if ( ch->pcdata->learn <= 0 )
        {
            send_to_char( "You have no lessons left.\n\r", ch );
            return;
        }
        else if ( money > ch->silver )
        {
            send_to_char( "You don't have enough money to take lessons.\n\r", ch );
            return;
        }

        if (( ln = lang_lookup( argument ) ) < 0)
        {
            send_to_char( "You can't practice that.\n\r", ch );
            return;
        }


        adept = (get_curr_stat(ch, STAT_INT) * 5); /* Max learned = int*5 */

        if ( ch->pcdata->language[ln] >= adept )
        {
            sprintf( buf, "You are already fluent in %s.\n\r",
                lang_table[ln].name );
            send_to_char( buf, ch );
        }
        else
        {
            ch->pcdata->learn--;
            ch->silver                -= money;
            ch->pcdata->language[ln] += int_app[get_curr_stat(ch, STAT_INT )].learn;
            if ( ch->pcdata->language[ln] < adept )
            {
                act( "You take lessons in $T.",
                    ch, NULL, lang_table[ln].name, TO_CHAR );
                act( "$n practices $T.",
                    ch, NULL, lang_table[ln].name, TO_ROOM );
            }
            else
            {
                ch->pcdata->language[ln] = adept;
                act( "You are now fluent in $T.",
                    ch, NULL, lang_table[ln].name, TO_CHAR );
                act( "$n is now fluent in $T.",
                    ch, NULL, lang_table[ln].name, TO_ROOM );
            }
        }
    }
    return;
}

void do_common( CHAR_DATA *ch, char *argument)
{
	do_language(ch, argument, COMMON);
}

void do_dwarvish( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, DWARVISH);
}

void do_elven( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, ELVEN);
}

void do_giant( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, GIANT);
}

void do_demon( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, DEMON);
}

void do_pixie( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, PIXIE);
}

void do_undead( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, UNDEAD);
}

void do_bodak( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, BODAK);
}

void do_sithid( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, SITHID);
}

/* ========== Language ======================== */
void do_language( CHAR_DATA *ch, char *argument, int language)
{
	CHAR_DATA	*och;
	int		chance;
	int		chance2;
	char		*lan_str;
	char		buf	[ 256 ];

	lan_str = lang_table[language].name;

	buf[0] = '\0';
	/* Now find out if we can speak it... */
	if((chance = ch->pcdata->language[language]) == 0)
	{
		sprintf(buf, "You don't know how to speak %s.\n\r", lan_str);
		send_to_char(buf ,ch);
		return;
	}

	if(argument[0] == '\0')
	{
		buf[0] = '\0';
		sprintf(buf, "Say WHAT in %s ??\n\r", lan_str);
		send_to_char(buf ,ch);
		return;
	}
	
	if(number_percent( ) <= chance )
	{
	   buf[0] = '\0';
	   sprintf (buf,"`SYou say '%s'``\n\r", argument);
	   send_to_char(buf, ch);
	   for(och = ch->in_room->people; och != NULL; och = och->next_in_room )
	   {
            if(!IS_NPC(och) && (och != ch))
	    {
	    if((chance2 = och->pcdata->language[language]) == 0)
	      act("$n says something in a strange tongue.",ch,NULL,och,TO_VICT);
	    else
	      if(number_percent( ) <= chance2)
               {
               act( "`S$n says '$t'``", ch, argument, och, TO_VICT );
               }
	      else
               {
               act( "`S$n says something you can't understand``", ch, NULL, och, TO_VICT );
               }  
            }
	   }
	}
	else
	{
	   buf[0] = '\0';
	   sprintf (buf, "`SIn %s, you try to say '%s', but it doesn't sound correct.``\n\r", lan_str, argument);
	   send_to_char(buf, ch);
           for(och = ch->in_room->people; och != NULL; och = och->next_in_room )
           {
            if(!IS_NPC(och) && (och != ch))
            {
            if((chance2 = och->pcdata->language[language]) == 0)
              act("$n says something in a strange tongue.",ch,NULL,och,TO_VICT);
	    else
                if(number_percent( ) <= chance2)
                 {
                 act( "`S$n says something uncomprehensible.``", ch, NULL, och, TO_VICT );
                 }
                else
                 {
                 act( "`S$n says something you can't understand``", ch, NULL, och, TO_VICT );
                 }
           }
       }
   }
}
