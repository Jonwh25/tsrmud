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

#define unix 1
#if defined( macintosh )
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


/* Auction variables */
OBJ_DATA *auc_obj;
CHAR_DATA *auc_bid;

/* Due to new money format gold/silver/copper */
#ifdef NEW_MONEY
  MONEY_DATA auc_cost;
#else
  int auc_cost;
#endif

int auc_count = -1;
CHAR_DATA *auc_held;

/* Auction semi-local */
void auc_channel    args( ( char *auction ) );

/*
 * Local functions.
 */
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );
void    newbie_help	args( ( CHAR_DATA *ch, char *argument ) );
bool    is_ignoring     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/*
 *  playerlist   -- Decklarean
 */
void delete_playerlist  args( ( char * name ) );



/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    int              position;

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "{w%s what?{x\n\r", verb );
	buf[0] = UPPER( buf[0] );
	return;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
	sprintf( buf, "{wYou can't %s.{x\n\r", verb );
	send_to_char( buf, ch );
	return;
    }
    
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENT ) 
         && (get_trust(ch)<L_DIR) )
    {
        send_to_char( "{wYou can't do that here.{x\n\r", ch );
        return;
    }

    REMOVE_BIT( ch->deaf, channel );
/*
    if (    IS_QUESTOR( ch ) 
         && channel != CHANNEL_GOSSIP 
         && channel != CHANNEL_YELL  )
    {
        send_to_char( "{wYou can't do that while you are questing.{x\n\r", ch);
        return;
    }
*/

    switch ( channel )
    {
    default:
	sprintf( buf, "{wYou %s '%s'{x\n\r", verb, argument );
	send_to_char( buf, ch );
	sprintf( buf, "{w$n %ss '$t'{x",     verb );
	break;
    /* 
     * Player Channels 
     */
    case CHANNEL_OOC:
	sprintf( buf, "{B$n [OOC]: '$t'{x" );
	position        = ch->position;
	ch->position    = POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position    = position;
	break;
    case CHANNEL_GOSSIP:
	sprintf( buf, "{RYou %s '%s'{x\n\r", verb, argument );
	send_to_char( buf, ch );
	sprintf( buf, "{R$n %ss '$t'{x",     verb );
	break;
    case CHANNEL_QUOTE:
        sprintf( buf, "{cYou %s '%s'{x\n\r", verb, argument );
        send_to_char( buf, ch);
        sprintf( buf, "{c$n %ss '$t'{x", verb);
        break;

    case CHANNEL_GRATZ:
        sprintf( buf, "{mYou %s '%s'{x\n\r", verb, argument );
        send_to_char( buf, ch);
        sprintf( buf, "{m$n %ss '$t'{x", verb);
        break;
        	
    case CHANNEL_GUILD:
        sprintf( buf, "{R[{g%s{R]{g $n: '$t'{x", (ch->guild != NULL)
         ? ch->guild->name : "NONE");
        position        = ch->position;
        ch->position   = POS_STANDING;
        act( buf, ch, argument, NULL, TO_CHAR );
        ch->position    = position;
        break;
    case CHANNEL_CLAN:
        sprintf( buf, "{B<{g%s{B>{g $n: '$t'{x",
	       ( get_clan_index(ch->clan) && (get_clan_index(ch->clan))->name ?
		(get_clan_index(ch->clan))->name : "Unclanned") );
        position        = ch->position;
        ch->position   = POS_STANDING;
        act( buf, ch, argument, NULL, TO_CHAR );
        ch->position    = position;
        break;
    case CHANNEL_CLASS:
	sprintf( buf, "{w{%s} $n: $t{x", 
		 class_table[prime_class(ch)].who_long );
        position        = ch->position;
        ch->position   = POS_STANDING;
        act( buf, ch, argument, NULL, TO_CHAR );
        ch->position    = position;
	break;
    case CHANNEL_VENT:
        sprintf( buf, "{Y<{gVENT{Y>{g $n: '$t'{x" );
        position        = ch->position;
        ch->position    = POS_STANDING;
        act( buf, ch, argument, NULL, TO_CHAR );
        ch->position    = position;
        break; 
    /*
     * Immortal Channels and HERO Channel 
     */
    case CHANNEL_IMMTALK:
	sprintf( buf, "{Y$n: $t{x");
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    case CHANNEL_HERO:
	sprintf( buf, "{R(HERO){G $n: '$t'{x" );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }
    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	    && vch != ch
	    && !IS_SET( och->deaf, channel ) 
	    && !IS_SET( och->in_room->room_flags, ROOM_SILENT ) )
	{
            if (is_ignoring(och, ch) && !IS_IMMORTAL(ch))
		continue;

	    if (    IS_QUESTOR( och ) 
                 && channel != CHANNEL_GOSSIP 
		 && channel != CHANNEL_YELL  )
                continue;

	    if ( channel == CHANNEL_VENT && och->level < 3 ) 
                continue;

	    if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL( och ) )
		continue;

	    if ( channel == CHANNEL_HERO && !IS_HERO( och ) )
	        continue;

            if ( ( channel == CHANNEL_CLASS )
                && ( prime_class( vch ) != prime_class( ch ) ) )
                {
                 if ( IS_SET( och->deaf, CHANNEL_CLASS_MASTER ) ||
                      get_trust( och ) < L_IMP )
                  continue;
                }

            if ( ( channel == CHANNEL_GUILD )
                && ( vch->guild != ch->guild ) )
                {
                  if ( IS_SET( och->deaf, CHANNEL_GUILD_MASTER ) ||
                      get_trust( och ) < L_IMP )
                   continue;
                }
            if ( ( channel == CHANNEL_CLAN )
                && ( vch->clan != ch->clan ) )
                {
                  if ( IS_SET( och->deaf, CHANNEL_CLAN_MASTER ) ||
                      get_trust( och ) < L_IMP )
                    continue;
                }
	    if ( channel == CHANNEL_YELL
		&& vch->in_room->area != ch->in_room->area )
	        continue;


/* If ch is not in Mudschool, don't send to chars in mudschool unless you are Immortal  
	    if ( ( !IS_SET( ch->in_room->area->area_flags, AREA_MUDSCHOOL ) )
		 && ( ch->level < LEVEL_IMMORTAL ) )
		if ( ( IS_SET( vch->in_room->area->area_flags, AREA_MUDSCHOOL ) ) 
		   && ( vch->level < LEVEL_IMMORTAL ) )
		   continue;

   If ch IS in Mudschool, send only to chars in mudschool and Immortals 
	    if ( ( IS_SET( ch->in_room->area->area_flags, AREA_MUDSCHOOL ) ) 
		 && ( ch->level < LEVEL_IMMORTAL ) )
		if ( ( !IS_SET( vch->in_room->area->area_flags, AREA_MUDSCHOOL ) )
		   && ( vch->level < LEVEL_IMMORTAL ) )
		   continue;
*/

	    position		= vch->position;
	    if ( channel != CHANNEL_GOSSIP && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
     
            switch ( channel )
            {
             default:
	             act( buf, ch, argument, vch, TO_VICT ); break;
             case CHANNEL_IMMTALK:
	             act( buf, ch, argument, vch, TO_VICT ); break;
	     case CHANNEL_HERO:
	             act( buf, ch, argument, vch, TO_VICT ); break;
	     case CHANNEL_GUILD:
                     act( buf, ch, argument, vch, TO_VICT ); break;
             case CHANNEL_CLAN:
                     act( buf, ch, argument, vch, TO_VICT ); break;
             case CHANNEL_CLASS:
                     act( buf, ch, argument, vch, TO_VICT ); break;
	     case CHANNEL_OOC:
		     act( buf, ch, argument, vch, TO_VICT );
		     break;
             case CHANNEL_VENT:

/* so only trust 112+ can see person's name */
/*                     if ( vch->trust > 111 )
		         sprintf( buf, "{Y<{gVENT{Y>{g $n: '$t'{x" ); 
		     else
  			 sprintf( buf, "{Y<{gVENT{Y>{g: '$t'{x" );   */

                     act( buf, ch, argument, vch, TO_VICT );
                     break; 
            }
	    vch->position	= position;
	}
    }

    return;
}

void auc_channel ( char *auction )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  sprintf( buf, "AUCTION: %s\n\r", auction );

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( d->connected != CON_PLAYING )
      continue;

    if ( !IS_SET( (d->original ? d->original : d->character)->deaf, CHANNEL_AUCTION ) )
      write_to_buffer( d, buf, 0 );
  }

  return;
}

void do_gdt( CHAR_DATA *ch, char *argument )
{
  if(ch->guild == NULL)
  {
    send_to_char( "{wYou are not guilded.{x\n\r", ch);
    return;
  }
  talk_channel( ch, argument, CHANNEL_GUILD, "guildtalk" );
  return;
}

void do_clan( CHAR_DATA *ch, char *argument )
{
  if(ch->clan == 0)
  {
    send_to_char( "{wYou are not clanned.{x\n\r", ch);
    return;
  }
    talk_channel( ch, argument, CHANNEL_CLAN, "clantalk" );
    return;
}

void do_class( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_CLASS, "classtalk" );
    return;
}

void do_auction( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
#ifdef NEW_MONEY
    MONEY_DATA aucamt;
    char arg3 [ MAX_STRING_LENGTH ];
#else
    int bid = 0;
#endif
    
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );
#ifdef NEW_MONEY
/* Synax: auction boots 5 copper,gold, or silver */
  
    aucamt.gold = aucamt.silver = aucamt.copper = 0;
    argument = one_argument( argument, arg3 );
    if ( !str_cmp( arg3, "gold" ) )
      aucamt.gold = is_number( arg1 ) ? atoi( arg1 ) : 0;
    else if ( !str_cmp( arg3, "silver" ) )
      aucamt.silver = is_number( arg1 ) ? atoi( arg1 ) : 0;
    else if ( !str_cmp( arg3, "copper" ) )
      aucamt.copper = is_number( arg1 ) ? atoi( arg1 ) : 0;
    else
    {
      send_to_char(  "{wSyntax: auction <item> <amount> <currency type>{x\n\r", ch );
      return;
    }
#else
    bid = is_number( arg1 ) ? atoi( arg1 ) : 0;
#endif    

    if ( IS_NPC( ch ) )
    {
      send_to_char(  "{wYou can't auction items.{x\n\r", ch );
      return;
    }
    
    if ( arg[0] == '\0' )
    {
      send_to_char(  "{wAuction which item?{x\n\r", ch );
      return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
      if ( !auc_obj || !auc_held || auc_held != ch )
      {
	send_to_char( "{wYou do not have an item being auctioned.{x\n\r",ch);
	return;
      }
      if ( auc_bid )
      {
	send_to_char( "{wYou may not remove your item after a bid has been made.{x\n\r", ch );
	return;
      }
      REMOVE_BIT(ch->deaf, CHANNEL_AUCTION);
      sprintf(log_buf, "{w%s has been removed from the auction.{x\n\r", auc_obj->short_descr );
      auc_channel( log_buf );
      act(  "{w$p appears suddenly in your hands.{x", ch, auc_obj, NULL, TO_CHAR );
      act(  "{w$p appears suddenly in the hands of $n.{x", ch, auc_obj, NULL,
        TO_ROOM );
      obj_to_char( auc_obj, ch );
      auc_obj = NULL;
      auc_held = NULL;

#ifdef NEW_MONEY
      auc_cost.gold = auc_cost.silver = auc_cost.copper = 0;
#else
      auc_cost = 0;
#endif

      auc_count = -1;
      return;
    }

#ifdef NEW_MONEY
      if ( ( aucamt.gold <= 0 ) && ( aucamt.silver <= 0 ) &&
	   ( aucamt.copper <= 0 ) )
    {
      send_to_char( "{wAuction it for how much?{x\n\r",ch );
      return;
    }    
/* Lowest bidding price to start an auction */
    if ( ( aucamt.gold < 100 ) && ( aucamt.silver < 100 )
       && ( aucamt.copper < 50 ) )
    {
      send_to_char(  "{wThat is too low of a starting bidding price.{x\n\r",
		   ch );
      return;
    }

#else

    if ( bid <= 0 )
    {
      send_to_char( "{wAuction it for how much?{x\n\r",ch );
      return;
    }
    
    if ( bid < 100 )
    {
      send_to_char(  "{wThat is too low of a starting bidding price.{x\n\r", ch );
      return;
    }
#endif    

    if ( auc_obj )
    {
      send_to_char(  "{wThere is already an item being auctioned.{x\n\r", ch );
      return;
    }
   else
    {
      if ( ( auc_obj = get_obj_carry( ch, arg ) ) )
      {
        if ( (auc_obj->pIndexData->vnum > 1 && auc_obj->pIndexData->vnum < 23 ) 
            || ( auc_obj->item_type == ITEM_CONTAINER && auc_obj->contains ) )
        { 
          send_to_char(  "{wYou can't auction that.{x\n\r", ch );
          auc_obj = NULL;
          return;
        }
        
	REMOVE_BIT( ch->deaf, CHANNEL_AUCTION );
	act(  "{w$p disappears from your inventory.{x", ch, auc_obj, NULL,
	 TO_CHAR );
	act(  "{w$p disappears from the inventory of $n.{x", ch, auc_obj,
	 NULL, TO_ROOM );
	obj_from_char( auc_obj );
	auc_held = ch;
	auc_bid = NULL;

#ifdef NEW_MONEY
/* Only one currency will be above zero */
	auc_cost.gold   = aucamt.gold;
	auc_cost.silver = aucamt.silver;
	auc_cost.copper = aucamt.copper;
	auc_count = 0;
	sprintf( log_buf, "%s a level %d object for %s",
	   auc_obj->short_descr, auc_obj->level, money_string( &auc_cost ));

#else

	auc_cost = bid;
	auc_count = 0;
        sprintf( log_buf, "%s a level %d object for %d gold coins.", auc_obj->short_descr, auc_obj->level, bid );
#endif
        auc_channel( log_buf );
        sprintf( log_buf, "%s auctioning %s.", auc_held->name, auc_obj->name );
        log_string( log_buf, CHANNEL_GOD, -1 );
	return;
      }
     else
      {
        send_to_char(  "{wYou are not carrying that item.{x\n\r", ch );
        return;
      }
    }
    
/*    talk_channel( ch, argument, CHANNEL_AUCTION, "auction" );*/
    return;
}


void do_bid( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
#ifdef NEW_MONEY
  MONEY_DATA amt;
  char arg2 [ MAX_STRING_LENGTH ];
  int min_bid = 0;
  bool bid_amt = TRUE;
#else
  int bid = 0;
#endif

  if ( !auc_obj )
  {
    send_to_char(  "{wThere is no auction at the moment.{x\n\r", ch );
    return;
  }

  if ( !auc_held )
  {
    bug( "Do_bid: auc_obj found without auc_held.\n\r",0);
    return;
  }

  if ( ch == auc_held )
  {
    send_to_char(  "{wIf you want your item back, you should 'auction remove' it.{x\n\r", ch );
    return;
  }

  if ( auc_bid && ch == auc_bid )
  {
    send_to_char(  "{wYou already hold the highest bid.{x\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );

#ifdef NEW_MONEY 
  amt.gold = amt.silver = amt.copper = 0;

/* New syntax: bid 1000 copper */

  argument = one_argument( argument, arg2 );
  if ( is_number( arg ) )
  {
    if ( !str_cmp( arg2, "gold" ) )
      amt.gold = atoi( arg );
    else if ( !str_cmp( arg2, "silver" ) )
      amt.silver = atoi( arg );
    else if ( !str_cmp( arg2, "copper" ) )
      amt.copper = atoi( arg );
    else
    {
      send_to_char(  "{wSyntax: bid <amount> <currency type>{x\n\r",
     		   ch );
      return;
    }
  }
  else
  {
    send_to_char(  "{wSyntax: bid <amount> <currency type>{x\n\r", ch );
    return;
  }

  if ( auc_cost.gold > 0 )        /* has to be 100 plus */
  {
     min_bid = ( auc_cost.gold + 100 );
     if ( ( (amt.gold*C_PER_G) + (amt.silver*S_PER_G) +
 	    (amt.copper) ) < min_bid*C_PER_G )
	bid_amt = FALSE;
  }
  else if ( auc_cost.silver > 0 ) /* has to be 100 plus */
  {
     min_bid = ( auc_cost.silver + 100 );
     if ( ( (amt.gold*C_PER_G) + (amt.silver*S_PER_G) +
            (amt.copper) ) < min_bid*S_PER_G )
	bid_amt = FALSE;
  }
  else if ( auc_cost.copper > 0 ) /* has to be 50 plus */
  {
     min_bid = ( auc_cost.copper + 50 );
     if ( ( (amt.gold*C_PER_G) + (amt.silver*S_PER_G) +
     	    (amt.copper) ) < min_bid )
	bid_amt = FALSE;
   }
  /* else bug, auc_cost has to have a cost */

  if ( !bid_amt )
  {  
   send_to_char(  "{wBid is not high enough.{x\n\r", ch );
   return;
  }     

  if ( ( (ch->money.gold*C_PER_G) + (ch->money.silver*S_PER_G) +
         (ch->money.copper) ) < ( (amt.gold*C_PER_G) + (amt.silver*S_PER_G) +
         (amt.copper) ) )
  {
    send_to_char(  "{wYou are not carrying that much money.{x\n\r", ch );
    return;
  }
  
  REMOVE_BIT(ch->deaf, CHANNEL_AUCTION);
  sprintf( buf, "{wAmount bid on %s: %s{x", auc_obj->short_descr, money_string( &amt ) );
  auc_channel( buf );
  auc_cost.gold   = amt.gold;
  auc_cost.silver = amt.silver;
  auc_cost.copper = amt.copper;
  auc_count = 0;
  auc_bid = ch;
  return;
}
#else
  bid = is_number( arg ) ? atoi( arg ) : 0;

  if ( auc_cost > bid - 100 )
  {
    sprintf( buf, "{wYou must bid at least %d gold coins in this auction.{x\n\r", auc_cost + 100 );
    send_to_char(  buf, ch );
    return;
  }

  if ( ch->gold < bid )
  {
    send_to_char(  "{wYou are not carrying that much gold.{x\n\r", ch );
    return;
  }

  REMOVE_BIT(ch->deaf, CHANNEL_AUCTION);
  sprintf( buf, "{w%d gold coins bid on %s.{x", bid, auc_obj->short_descr );
  auc_channel( buf );

  auc_cost = bid;
  auc_count = 0;
  auc_bid = ch;
  return;
}
#endif

/*  Chat channel commented out because it is annoying.

void do_chat( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_CHAT, "chat" );
    return;
}   
*/

/* OOC added by Hannibal */
void do_ooc( CHAR_DATA *ch, char *argument )
{    
    talk_channel( ch, argument, CHANNEL_OOC, "OOC" );
    return;
}

/* VENT added by Angi */
void do_vent( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_VENT, "VENT" );
    return;
}

/*
 * Alander's new channels.
 */
void do_music( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
    return;
}



void do_question( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "question" );
    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "answer" );
    return;
}



void do_gossip( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_GOSSIP, "gossip" );
    WAIT_STATE( ch, 12 );
    return;
}

void do_quote( CHAR_DATA *ch, char* argument)
{
    talk_channel( ch, argument, CHANNEL_QUOTE, "quote");
    WAIT_STATE( ch, 12);
    return;
}

void do_gratz( CHAR_DATA *ch, char* argument)
{
    talk_channel( ch, argument, CHANNEL_GRATZ, "congratulate");
    WAIT_STATE( ch, 12);
    return;
}

void do_yell( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_YELL, "yell" );
    return;
}


void do_immtalk( CHAR_DATA *ch, char *argument )
{
    if(!IS_NPC( ch ) && !IS_IMMORTAL(ch))
    {
        send_to_char("{wYou are still but a lowly mortal.{x\n\r", ch );
        return;
    }

    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}

void do_hero( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC( ch ) && ch->level < LEVEL_HERO )
    {
        send_to_char("{wYou are not yet at max mortal level, you cannot use this channel.{x\n\r", ch );
        return;
    }

    talk_channel( ch, argument, CHANNEL_HERO, "hero" );
    return;
}


void do_say( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "{wSay what?{x\n\r", ch );
	return;
    }

    MOBtrigger = FALSE;
    act( "{G$n says '$T'{x", ch, NULL, argument, TO_ROOM );
    act( "{GYou say '$T'{x", ch, NULL, argument, TO_CHAR );

/* Check if ch is asking newbie helper for help */

    if ( !IS_NPC( ch ) && ( ch->level < 4 ) && 
       ( IS_SET( ch->in_room->area->area_flags, AREA_MUDSCHOOL ) ) )
      newbie_help( ch, argument );

    return;
}

void do_osay( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "{wSay what?{x\n\r", ch );
	return;
    }

    MOBtrigger = FALSE;
    act( "{M$n osays '$T'{x", ch, NULL, argument, TO_ROOM );
    act( "{MYou osay '$T'{x", ch, NULL, argument, TO_CHAR );

    return;
}

void do_tell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        position;
    char buf [ MAX_STRING_LENGTH ];
   
    if ( !IS_NPC( ch ) && (   IS_SET( ch->act, PLR_SILENCE )
			   || IS_SET( ch->act, PLR_NO_TELL ) ) )
    {
	send_to_char( "{wYour message didn't get through.{x\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENT ) )
    {
        send_to_char( "{wYou can't do that here.{x\n\r", ch );
        return;
    }
    
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "{wTell whom what?{x\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
/*   if ( !( victim = get_char_world( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) ) */
    if (    (    !( victim = get_pc_world( ch, arg )   )
              && !( victim = get_char_world( ch, arg ) )  )
         || (    IS_NPC( victim ) 
              && victim->in_room != ch->in_room           ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SILENT ) )
    {
        act(  "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
        return;
    }
    
    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
	return;
    }
    
    if (is_ignoring(victim, ch) && !IS_IMMORTAL(ch))
    {
	send_to_char( "They are ignoring you right now.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( !( victim->desc ) ) )
    {
        act( "{w$E is link-dead.{x", ch, 0, victim, TO_CHAR );
        return;
    }
/*    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK ) )
	{
	act(  "{w$E is {m<{RA{WF{BK{m>{w at the moment.{x", ch, 0, victim, 
	TO_CHAR );
	return;
	} */
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK ) )
    {
        sprintf( buf, "{w%s %s.{x", victim->name, 
           ( victim->pcdata && victim->pcdata->afkchar[0] != '\0' )
           ? victim->pcdata->afkchar : "{wis {m<{RA{WF{BK{m>{w at the moment{x" );
        act(  buf, ch, NULL, victim, TO_CHAR );
        return;
    } 
        
    if ( victim->desc && victim->desc->pString )
    {
      act(  "{w$E is in a writing buffer.  Try again in a few minutes.{x", ch, 0, victim, TO_CHAR );
      return;
    }
    act( "{MYou tell $N '$t'{x", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "{M$n tells you '$t'{x", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    return;
}

void do_itell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        position;
    char buf [ MAX_STRING_LENGTH ];
   
    if ( !IS_NPC( ch ) && (   IS_SET( ch->act, PLR_SILENCE )
			   || IS_SET( ch->act, PLR_NO_TELL ) ) )
    {
	send_to_char( "{wYour message didn't get through.{x\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENT ) )
    {
        send_to_char( "{wYou can't do that here.{x\n\r", ch );
        return;
    }
    
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "{wTell whom what?{x\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
/*   if ( !( victim = get_char_world( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) ) */
    if (    (    !( victim = get_pc_world( ch, arg )   )
              && !( victim = get_char_world( ch, arg ) )  )
         || (    IS_NPC( victim ) 
              && victim->in_room != ch->in_room           ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SILENT ) )
    {
        act(  "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
        return;
    }
    
    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
	return;
    }
    
    if (is_ignoring(victim, ch) && !IS_IMMORTAL(ch))
    {
	send_to_char( "They are ignoring you right now.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( !( victim->desc ) ) )
    {
        act( "{w$E is link-dead.{x", ch, 0, victim, TO_CHAR );
        return;
    }
/*    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK ) )
	{
	act(  "{w$E is {m<{RA{WF{BK{m>{w at the moment.{x", ch, 0, victim, 
	TO_CHAR );
	return;
	} */
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK ) )
    {
        sprintf( buf, "{w%s %s.{x", victim->name, 
           ( victim->pcdata && victim->pcdata->afkchar[0] != '\0' )
           ? victim->pcdata->afkchar : "{wis {m<{RA{WF{BK{m>{w at the moment{x" );
        act(  buf, ch, NULL, victim, TO_CHAR );
        return;
    } 
        
    if ( victim->desc && victim->desc->pString )
    {
      act(  "{w$E is in a writing buffer.  Try again in a few minutes.{x", ch, 0, victim, TO_CHAR );
      return;
    }
    act( "{WYou itell $N '$t'{x", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "{W$n itells you '$t'{x", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    return;
}


void do_remote( CHAR_DATA *ch, char *argument )
/*** Remote, added by ShayDn 27/6/96 ***/

{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        position;

    if ( !IS_NPC( ch ) && (   IS_SET( ch->act, PLR_SILENCE )
			   || IS_SET( ch->act, PLR_NO_TELL ) ) )
    {
	send_to_char( "{wYour message didn't get through.{x\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENT ) )
    {
        send_to_char( "{wYou can't do that here.{x\n\r", ch );
        return;
    }
    
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "{wRemote what to whom?{x\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( !( victim = get_char_world( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SILENT ) )
    {
        act(  "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
        return;
    }
    
    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
	return;
    }
    
    if ( !IS_NPC( victim ) && ( !( victim->desc ) ) )
    {
        act( "{w$E is link-dead.{x", ch, 0, victim, TO_CHAR );
        return;
    }
    if ( victim->desc && victim->desc->pString )
    {
      act(  "{w$E is in a writing buffer.{x", ch, 0, victim, TO_CHAR );
      return;
    }

    act( "{wYou emote: '{M$n $t{w' to $N.{x", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "{w+ $n $t.{x", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int        position;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
	send_to_char( "{wYour message didn't get through.{x\n\r", ch );
	return;
    }

    if ( !( victim = ch->reply ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "{wReply what?{x\n\r", ch );
        return;
    }

    if (is_ignoring(victim, ch) && !IS_IMMORTAL(ch))
    {
	send_to_char( "They are ignoring you right now.\n\r", ch );
	return;
    }

    if ( ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) ) 
           || ( IS_SET( victim->in_room->room_flags, ROOM_SILENT ) 
           && (get_trust(ch) < L_APP ) ) )
    {
	act( "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
	return;
    }
    if ( victim->desc && victim->desc->pString )
    {
      act(  "{w$E is in a writing buffer.{x", ch, 0, victim, TO_CHAR );
      return;
    }

    act( "{MYou tell $N '$t'{x",  ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "{M$n tells you '$t'{x", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    return;
}


void do_ireply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int        position;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
	send_to_char( "{wYour message didn't get through.{x\n\r", ch );
	return;
    }

    if ( !( victim = ch->reply ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "{wReply what?{x\n\r", ch );
        return;
    }

    if (is_ignoring(victim, ch) && !IS_IMMORTAL(ch))
    {
	send_to_char( "They are ignoring you right now.\n\r", ch );
	return;
    }

    if ( ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) ) 
           || ( IS_SET( victim->in_room->room_flags, ROOM_SILENT ) 
           && (get_trust(ch) < L_APP ) ) )
    {
	act( "{w$E can't hear you.{x", ch, 0, victim, TO_CHAR );
	return;
    }
    if ( victim->desc && victim->desc->pString )
    {
      act(  "{w$E is in a writing buffer.{x", ch, 0, victim, TO_CHAR );
      return;
    }

    act( "{WYou itell $N '$t'{x",  ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "{W$n itells you '$t'{x", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char  buf [ MAX_STRING_LENGTH ];
    char *plast;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "{wYou are an emotionless blob.  You must have done {RSOMETHING{w to deserve this...{x\n\r",ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "{wEmote what?{x\n\r", ch );
	return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    sprintf( buf, "%s%s{x", (*argument == '\'') ? "" : " ", argument );
    if ( isalpha( plast[-1] ) )
	strcat( buf, "." );

    act( "{M$n$T{x", ch, NULL, buf, TO_ROOM );
    act( "{M$n$T{x", ch, NULL, buf, TO_CHAR );
    return;
}



/*
 * All the posing stuff.
 */
#if 0
struct	pose_table_type
{
    char * message[ 2*MAX_CLASS ];
};

const	struct	pose_table_type	pose_table	[]	=
{
    {
	{
	    "{MYou sizzle with energy.{x",
	    "{M$n sizzles with energy.{x",
	    "{MYou feel very holy.{x",
	    "{M$n looks very holy.{x",
	    "{MYou perform a small card trick.{x",
	    "{M$n performs a small card trick.{x",
	    "{MYou show your bulging muscles.{x",
	    "{M$n shows $s bulging muscles.{x",
            "{MStop it with the Ouija board, will ya?{x",
            "{MGreat, $n is playing with $s Ouija board again.{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x"
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "{MYou turn into a butterfly, then return to your normal shape.{x",
	    "{M$n turns into a butterfly, then returns to $s normal shape.{x",
	    "{MYou nonchalantly turn wine into water.{x",
	    "{M$n nonchalantly turns wine into water.{x",
	    "{MYou wiggle your ears alternately.{x",
	    "{M$n wiggles $s ears alternately.{x",
	    "{MYou crack nuts between your fingers.{x",
	    "{M$n cracks nuts between $s fingers.{x",
            "{MYou read everyone's mind....and shudder with disgust.{x",
            "{M$n reads your mind...eww, you pervert!{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x",
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "{MBlue sparks fly from your fingers.{x",
	    "{MBlue sparks fly from $n's fingers.{x",
	    "{MA halo appears over your head.{x",
	    "{MA halo appears over $n's head.{x",
	    "{MYou nimbly tie yourself into a knot.{x",
	    "{M$n nimbly ties $mself into a knot.{x",
	    "{MYou grizzle your teeth and look mean.{x",
	    "{M$n grizzles $s teeth and looks mean.{x",
            "{MYou show everyone your awards for perfect school attendance.{x",
            "{MYou aren't impressed by $n's school attendance awards.  Geek.{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x",
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "{MLittle red lights dance in your eyes.{x",
	    "{MLittle red lights dance in $n's eyes.{x",
	    "{MYou recite words of wisdom.{x",
	    "{M$n recites words of wisdom.{x",
	    "{MYou juggle with daggers, apples, and eyeballs.{x",
	    "{M$n juggles with daggers, apples, and eyeballs.{x",
	    "{MYou hit your head, and your eyes roll.{x",
	    "{M$n hits $s head, and $s eyes roll.{x",
            "{MA will-o-the-wisp arrives with your slippers.{x",
            "{MA will-o-the-wisp affives with $n's slippers.{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x",
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "{MA slimy green monster appears before you and bows.{x",
	    "{MA slimy green monster appears before $n and bows.{x",
	    "{MDeep in prayer, you levitate.{x",
	    "{MDeep in prayer, $n levitates.{x",
	    "{MYou steal the underwear off every person in the room.{x",
	    "{MYour underwear is gone!  $n stole it!{x",
	    "{MCrunch, crunch -- you munch a bottle.{x",
	    "{MCrunch, crunch -- $n munches a bottle.{x",
            "{MWhat's with the extra leg?{x",
            "{MWhy did $n sprout an extra leg just now?{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x",
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "{MYou turn everybody into a little pink elephant.{x",
	    "{MYou are turned into a little pink elephant by $n.{x",
	    "{MAn angel consults you.{x",
	    "{MAn angel consults $n.{x",
	    "{MThe dice roll ... and you win again.{x",
	    "{MThe dice roll ... and $n wins again.{x",
	    "{M... 98, 99, 100 ... you do pushups.{x",
	    "{M... 98, 99, 100 ... $n does pushups.{x",
            "{MThe spoons flee as you begin to concentrate.{x",
            "{MThe spoons flee as $n begins to concentrate.{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x",
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "{MA small ball of light dances on your fingertips.{x",
	    "{MA small ball of light dances on $n's fingertips.{x",
	    "{MYour body glows with an unearthly light.{x",
	    "{M$n's body glows with an unearthly light.{x",
	    "{MYou count the money in everyone's pockets.{x",
	    "{MCheck your money, $n is counting it.{x",
	    "{MArnold Schwarzenegger admires your physique.{x",
	    "{MArnold Schwarzenegger admires $n's physique.{x",
            "{MStop wiggling your brain at people.{x",
            "{MMake $n stop wiggling $s brain at you!{x",
            "{MYou talk to some nearby bugs.{x",
            "{M$n converses with nature.{x",
            "{MYou show off your bulging muscles.{x",
            "{M$n shows $s bulging muscles.{x",
            "{MYou preach about the wonders of the church.{x",
            "{M$n gives you a sermon.{x",
            "{MYou play a short rift on your lute.{x",
            "{M$n plays a short rift on $s lute.{x",
            "{MYou proclaim '{GI vant to suck your blood{M!'{x",
            "{M$n proclaims '{GI vant to suck your blood{M!'.{x"
	}
    },

    {
	{
	    "Smoke and fumes leak from your nostrils.",         /* Xaviar working..... this is so i can find my place */
	    "Smoke and fumes leak from $n's nostrils.",          
	    "A spotlight hits you.",
	    "A spotlight hits $n.",
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue.",
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders.",
            "MENSA called...they want your opinion on something.",
            "MENSA just called $n for consultation.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "Oomph!  $n squeezes water out of a granite boulder.",
            "Chairs fly around the room at your slightest whim.",
            "Chairs fly around the room at $n's slightest whim.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "$n picks $s teeth with a spear.",
            "Oof...maybe you shouldn't summon any more hippopotamuses.",
            "Oof!  Guess $n won't be summoning any more hippos for a while.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "Everyone is swept off their bart by your hug.",
	    "You are swept off your feet by $n's hug.",
            "Oops...your hair is sizzling from thinking too hard.",
            "Oops...$n's hair is sizzling from thinking too hard.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "$n's karate chop splits a tree.",
            "What?  You were too busy concentrating.",
            "What?  Oh, $n was lost in thought...again.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "A strap of $n's armor breaks over $s mighty thews.",
            "Will you get down here before you get hurt?",
            "Quick, get a stick, $n is doing $s pinata impression again.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "A boulder cracks at $n's frown.",
            "Careful...don't want to disintegrate anyone!",
            "LOOK OUT!  $n is trying to disintegrate something!",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "Mercenaries arrive to do $n's bidding.",
            "You run off at the mouth about 'mind over matter'.",
            "Yeah, yeah, mind over matter.  Shut up, $n.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
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
	    "Four matched Percherons bring in $n's chariot.",
            "Thud.",
            "Thud.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You proclaim 'I vant to suck your blood!'",
            "$n proclaims 'I vant to suck your blood!'."
	}
    },

    {
	{
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling.",
	    "The great god Mota gives you a staff.",
	    "The great god Mota gives $n a staff.",
	    "Click.",
	    "Click.",
	    "Atlas asks you to relieve him.",
	    "Atlas asks $n to relieve him.",
            "You charm the pants off everyone...and refuse to give them back.",
            "Your pants are charmed off by $n, and $e won't give them back.",
            "You talk to some nearby bugs.",
            "$n converses with nature.",
            "You show off your bulging muscles.",
            "$n shows $s bulging muscles.",
            "You preach about the wonders of the church.",
            "$n gives you a sermon.",
            "You play a short rift on your lute.",
            "$n plays a short rift on $s lute.",
            "You kill Strahd in a few rounds and take over Ravenloft.",
            "After quickly dispatching Strahd, $n takes over Ravenloft."
	}
    }
};

void do_pose( CHAR_DATA *ch, char *argument )
{
    int level;
    int pose;

    if ( IS_NPC( ch ) )
	return;

    level = UMIN( ch->level,
		 sizeof( pose_table ) / sizeof( pose_table[0] ) - 1 );
    pose  = number_range( 0, level );

    act( pose_table[pose].message[2*ch->class+0], ch, NULL, NULL, TO_CHAR );
    act( pose_table[pose].message[2*ch->class+1], ch, NULL, NULL, TO_ROOM );

    return;
}
#endif



void do_info( CHAR_DATA *ch, char *argument )
{
    info( argument, 0, 0 );
    return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE,  argument );
    send_to_char( "{wOk.  Thanks.{x\n\r", ch );
    return;
}



void do_idea( CHAR_DATA *ch, char *argument )
{
    append_file( ch, IDEA_FILE, argument );
    send_to_char( "{wOk.  Thanks.{x\n\r", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "{wOk.  Thanks.  Please do not report the same typo multiple times.{x\n\r", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "{wRent?! Ther's no stinkin rent here!  Just save and quit.{x\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "{wIf you want to {RQUIT{w, you have to spell it out.{x\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *PeT;
    CHAR_DATA *gch;
    #ifdef NEW_MONEY
     MONEY_DATA tax;     
    #endif
    CHAR_DATA *wch;
    int immcount;
    extern int AUTHORIZE_LOGINS;
    immcount=0;


    if ( IS_NPC( ch ) )
       return;

    if ( ch->position == POS_FIGHTING )
    {
       send_to_char( "{wNo way! You are fighting.{x\n\r", ch );
       return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
       send_to_char( "{wYou're not {RDEAD{w yet.\{xn\r", ch );
       return;
    }

    if ( ch->combat_timer )
    {
       send_to_char( "{wYour adreneline is pumping too hard.{x\n\r",ch);         /* Xaviar working */
       return;
    }

    if ( ch->in_room->vnum == ROOM_VNUM_SMITHY )
    {
       send_to_char(  "{wYou cannot quit in this room.{x\n\r", ch );
       return;
    }

    if ( ch->questobj )
    if ( ch->questobj->carried_by == ch )
       extract_obj( ch->questobj ); /* if on char extract */
    else
       ch->questobj->timer = 1; /* Let obj_update extract it - Hann */

    raffect_remall( ch );

    /* For Auth code - Beowolf */
    for(d = descriptor_list; d != NULL; d = d->next)
    {
        if ( d->connected != CON_PLAYING) 
        continue;
        wch = (d->original != NULL) ? d->original : d->character;
        if(wch->level >= LEVEL_IMMORTAL) immcount++;
    }
    
    if( IS_IMMORTAL(ch) && AUTHORIZE_LOGINS == TRUE && immcount == 1) 
    {
        send_to_char( "{wTurn off the Authorization you moron!{x\n\r", ch);
        return;
    }

    #ifdef NEW_MONEY

    if ( (ch->money.gold > 500000) ||
	 (ch->money.silver/SILVER_PER_GOLD > 500000) ||
         (ch->money.copper/COPPER_PER_GOLD > 500000) )
    {

       tax.gold   = (ch->money.gold > 500000) ? ch->money.gold * 0.15 : 0;
       tax.silver = (ch->money.silver/SILVER_PER_GOLD > 500000) ?
		     ch->money.silver * 0.15 : 0;
       tax.copper = (ch->money.copper/COPPER_PER_GOLD > 500000) ?
		     ch->money.copper * 0.15 : 0;

       sprintf( log_buf, 
               "{wYou have been charged an out of the bank protection fee of %s{x\n\r",
	       money_string( &tax ) );
       send_to_char(  log_buf, ch );
       sub_money( &ch->money, &tax );
    }

    #else

    if ( ch->gold > 500000 )
    {
       int tax;
      
       tax = ch->gold * 0.15 ;
      
       sprintf( log_buf, "{wYou have been charged %d coin%s as a fee for the out of bank protection of your gold while you are
away.{x\n\r",
               tax, tax > 1 ? "s" : "" );
       send_to_char(  log_buf, ch );
       ch->gold -= tax;
    }
    #endif
   
    if ( IS_SET( ch->act2, PLR2_SOUND ))
    {
       stc("!!SOUND(Tsr/Evoice19 V=100)\n\r", ch); 
       stc("{wThe {RADDICTION{w will call you back!{x\n\r", ch); 
    }
    else
       stc("{wThe {RADDICTION{w will call you back!{x\n\r", ch); 


    if ( !IS_SET( ch->act, PLR_WIZINVIS ) && !IS_AFFECTED2(ch, AFF_PLOADED) )
    {
       act("{w$n has left the game.{x", ch, NULL, NULL, TO_ROOM );

       if ( !IS_IMMORTAL(ch))
          info( "%s has left the realms.{x", (int)(ch->name), 0 );
    }
    else
    {
       for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
       {
          if ( ch != gch )
          if ( get_trust(gch) >= ch->wizinvis )
          {
             act( "{w$N slightly phased has left the storm.{x",gch,NULL,ch,TO_CHAR);
          }
       }
    }

    if ( IS_AFFECTED2( ch, AFF_PLOADED ) )
       REMOVE_BIT( ch->affected_by2, AFF_PLOADED );

    if (ch->level != L_IMP)
    { 
       sprintf(log_buf,"{w%s has quit in room vnum {Y%d{w.{x", ch->name, ch->in_room->vnum);
       log_string( log_buf, -1, -1 );
       wiznet( log_buf, ch, NULL, WIZ_LOGINS, 0, get_trust( ch ) );
    }

    recent_add( ch );
     
    /*
     * After extract_char the ch is no longer valid!
     */
    if (!IS_NPC( ch ) )
    if ( IS_SET( ch->act, PLR_QUEST ) )
       REMOVE_BIT( ch->act, PLR_QUEST );

    if ( IS_SET( ch->act, PLR_QUESTOR ) );
       REMOVE_BIT( ch->act, PLR_QUESTOR );

    if ( auc_held && ch == auc_held && auc_obj )
    {
       if ( auc_bid )
       {
          #ifdef NEW_MONEY
	  if ( ( (auc_bid->money.gold*C_PER_G) + (auc_bid->money.silver*S_PER_G) +
	          auc_bid->money.copper ) < ( (auc_cost.gold*C_PER_G) +
	         (auc_cost.silver*S_PER_G) + auc_cost.copper ) )
          {
             sprintf(log_buf, "{wHolder of %s has left; bidder cannot pay for item; returning to owner.{x",
		   auc_obj->short_descr );
             obj_to_char( auc_obj, ch );
          }
          else
          {
             sprintf(log_buf, "{wHolder of %s has left; selling item to last bidder.{x", 
                auc_obj->short_descr );
             obj_to_char( auc_obj, auc_bid );
	     add_money( &ch->money, &auc_cost );
             spend_money( &auc_bid->money, &auc_cost );
          }
       }
       else
       {
          sprintf(log_buf, "{wHolder of %s has left; removing item from auction.{x",
                auc_obj->short_descr );
          auc_channel( log_buf );
          obj_to_char( auc_obj, ch );
       }
       auc_obj = NULL;
       auc_bid = NULL;
       auc_held = NULL;
       auc_cost.gold = auc_cost.silver = auc_cost.copper = 0;
       auc_count = -1;
    }
    #else

    if ( auc_bid->gold < auc_cost )
    {
       sprintf(log_buf, "{wHolder of %s has left; bidder cannot pay for item; returning to owner.{x", auc_obj->short_descr);
       obj_to_char( auc_obj, ch );
    }
    else
    {
       sprintf(log_buf, "{wHolder of %s has left; selling item to last bidder.{x",
	  auc_obj->short_descr );
       obj_to_char( auc_obj, auc_bid );
       auc_bid->gold -= auc_cost;
       ch->gold += auc_cost;
    }
    else
    {
       sprintf(log_buf, "{wHolder of %s has left; removing item from auction.{x",
	auc_obj->short_descr );
       auc_channel( log_buf );
       obj_to_char( auc_obj, ch );
    }
    auc_obj = NULL;
    auc_bid = NULL;
    auc_held = NULL;
    auc_cost = 0;
    auc_count = -1;

    #endif

    if ( auc_bid && auc_bid == ch && auc_obj )
    {
       sprintf(log_buf, "{wHighest bidder for %s has left; returning item to owner.{x", auc_obj->short_descr );

       if ( auc_held )
	obj_to_char( auc_obj, auc_held );

       auc_channel( log_buf);
       auc_obj = NULL;
       auc_bid = NULL;
       auc_held = NULL;

       #ifdef NEW_MONEY
        auc_cost.gold = auc_cost.silver = auc_cost.copper = 0;
       #else
        auc_cost = 0;
       #endif
        auc_count = -1;
    }
	
    save_char_obj( ch, TRUE );
    save_finger( ch );

    for ( PeT = ch->in_room->people; PeT; PeT = PeT->next_in_room )
    {
       if ( IS_NPC( PeT ) )
       if ( IS_SET( PeT->act, ACT_PET ) && ( PeT->master == ch ) )
       {
          extract_char( PeT, TRUE );
	  break;
       }
    }       
    /* Free note that might be there somehow */        
    if (ch->pcdata->in_progress)
       free_note (ch->pcdata->in_progress);

    d = ch->desc;

    if (ch->desc != NULL) 
       d->host = str_dup(ch->pcdata->host);

    extract_char( ch, TRUE );

    if ( d )
       close_socket( d );

    return;
}



void do_delet( CHAR_DATA *ch, char *argument )
{
  send_to_char( 
      "{wIf you want to {RDELETE{w yourself, spell it out!{x\n\r", ch );
  return;
}

void do_delete( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  
  if ( !ch->desc )
    return;
  if ( str_cmp(ch->desc->incomm, "delete yes") )
  {
    send_to_char( 
        "{wIf you want to {RDELETE{w yourself, type '{Ydelete yes{w'{x\n\r", ch );
    return;
  }
  if ( ch->desc->original || IS_NPC(ch) )
  {
    send_to_char(  "{wYou may not delete a switched character.{x\n\r",
        ch );
    return;
  }
  stop_fighting(ch, TRUE);
  send_to_char(  "{wYou are no more.{x\n\r", ch );
  act( "$n is no more.", ch, NULL, NULL, TO_ROOM );
  info( "%s is no more.", (int)(ch->name), 0 );
  sprintf(log_buf, "%s has DELETED in room vnum %d.", ch->name,
      ch->in_room->vnum);
  log_string(log_buf, -1, -1);
  wiznet(log_buf, ch, NULL, WIZ_LOGINS, 0, 0);
  if ( auc_held && ch == auc_held && auc_obj )
  {
    if ( auc_bid )
    {
#ifdef NEW_MONEY
     if ( ( (auc_bid->money.gold*C_PER_G) + (auc_bid->money.silver*S_PER_G) +
             auc_bid->money.copper ) < ( (auc_cost.gold*C_PER_G) +
            (auc_cost.silver*S_PER_G) + auc_cost.copper ) )
      {
        sprintf(log_buf, "{wHolder of %s has left; bidder cannot pay for item{x; "
            "{wreturning to owner.{x", auc_obj->short_descr);
        obj_to_char( auc_obj, ch );
      }
      else
      {
        sprintf(log_buf, "{wHolder of %s has left; selling item to last bidder.{x",
            auc_obj->short_descr );
        obj_to_char( auc_obj, auc_bid );
        add_money( &ch->money, &auc_cost );
	spend_money( &auc_bid->money, &auc_cost );
      }
    }
    else
    {
      sprintf(log_buf, "{wHolder of %s has left; removing item from auction.{x",
        auc_obj->short_descr );
      auc_channel( log_buf );
      obj_to_char( auc_obj, ch );
    }
    auc_obj = NULL;
    auc_bid = NULL;
    auc_held = NULL;
    auc_cost.gold = auc_cost.silver = auc_cost.copper = 0;
    auc_count = -1;
  }

  if ( auc_bid && auc_bid == ch && auc_obj )
  {
    sprintf(log_buf, "{wHighest bidder for %s has left; returning item to owner.{x",
	    auc_obj->short_descr );
    if ( auc_held )
      obj_to_char( auc_obj, auc_held );
    auc_channel( log_buf);
    auc_obj = NULL;
    auc_bid = NULL;
    auc_held = NULL;
    auc_cost.gold = auc_cost.silver = auc_cost.copper = 0;
    auc_count = -1;
  }

#else

      if ( auc_bid->gold < auc_cost )
      {
        sprintf(log_buf, "{wHolder of %s has left; bidder cannot pay for item{x; "
            "{wreturning to owner.{x", auc_obj->short_descr);
        obj_to_char( auc_obj, ch );
      }
      else
      {
        sprintf(log_buf, "{wHolder of %s has left; selling item to last bidder.{x",
            auc_obj->short_descr );
        obj_to_char( auc_obj, auc_bid );
        auc_bid->gold -= auc_cost;
        ch->gold += auc_cost;
      }
    }
    else
    {
      sprintf(log_buf, "{wHolder of %s has left; removing item from auction.{x",     
        auc_obj->short_descr );
      auc_channel( log_buf );
      obj_to_char( auc_obj, ch );
    }
    auc_obj = NULL;
    auc_bid = NULL;
    auc_held = NULL;
    auc_cost = 0;
    auc_count = -1;
  }

  if ( auc_bid && auc_bid == ch && auc_obj )
  {
    sprintf(log_buf, "{wHighest bidder for %s has left; returning item to owner.{x", auc_obj->short_descr );
    if ( auc_held )
      obj_to_char( auc_obj, auc_held );
    auc_channel( log_buf);
    auc_obj = NULL;
    auc_bid = NULL;
    auc_held = NULL;
    auc_cost = 0;
    auc_count = -1;
  }

#endif


  sprintf(log_buf, "rm -f %s%c/%s", PLAYER_DIR, LOWER(ch->name[0]),
      capitalize(ch->name));
  system( log_buf );
/*  remove(log_buf); */ /* Don't think was working right */
  strcat(log_buf, ".fng");
  system( log_buf );
/*  remove(log_buf); */
  sprintf(log_buf, "rm %s%c/%s.cps", PLAYER_DIR, LOWER(ch->name[0]),
      capitalize(ch->name));
/*  remove(log_buf);*/
  system( log_buf );

  delete_playerlist( ch->name );

  d = ch->desc;
  extract_char(ch, TRUE);
  if ( d )
    close_socket(d);
  return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( ch->level < 2 )
    {
	send_to_char( "{wYou must be at least second level to save.{x\n\r", ch );
	return;
    }

    save_char_obj( ch, FALSE );
    save_finger( ch );
    send_to_char( "{wThe gods smile upon you and save your soul.{x\n\r", ch );
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "{wFollow whom?{x\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master )
    {
	act( "{wBut you'd rather follow $N!{x", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "{wSilly...you already follow yourself.{x\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if ( ( ch->level - victim->level < -8
	  || ch->level - victim->level >  8 )
	&& !IS_HERO( ch ) )
    {
	send_to_char( "{wYou feel unworthy to follow.{x\n\r", ch );
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
  
    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "{w$n now follows you.{x", ch, NULL, master, TO_VICT );

    act( "{wYou now follow $N.{x",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{

    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
	affect_strip( ch, gsn_domination   );
    }

    if ( can_see( ch->master, ch ) )
	act( "{w$n stops following you.{x",
	    ch, NULL, ch->master, TO_VICT );
    act( "{wYou stop following $N.{x",
	ch, NULL, ch->master, TO_CHAR );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch, char *name )
{
    CHAR_DATA *fch;

    if ( ch->master )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = char_list; fch; fch = fch->next )
    {
        if ( fch->deleted )
	    continue;
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = NULL;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char       arg [ MAX_INPUT_LENGTH ];
    bool       found;
    bool       fAll;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "{wOrder whom to do what?{x\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "{wYou feel like taking, not giving, orders.{x\n\r", ch );
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
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "{wThey aren't here.{x\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "{wAye aye, right away!{x\n\r", ch );
	    return;
	}

	if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch )
	{
	    send_to_char( "{wDo it yourself!{x\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;

        if ( och->deleted )
	    continue;

/* You cannot order a newbie helper around period. Angi */
	if ( IS_NPC( och ) )
	  if ( IS_SET( och->act, ACT_NEWBIE ) )
	    continue;

	if ( IS_AFFECTED( och, AFF_CHARM )
	    && och->master == ch 
	    && ( fAll || och == victim ) )
	{
	    found = TRUE;
	    act( "{w$n orders you to '$t'.{x", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
	send_to_char( "{wOk.{x\n\r", ch );
    else
	send_to_char( "{wYou have no followers here.{x\n\r", ch );
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ( ch->leader ) ? ch->leader : ch;
	sprintf( buf, "{w%s's group:\n\r", PERS( leader, ch ) );
	send_to_char( buf, ch );

	for ( gch = char_list; gch; gch = gch->next )
	{
	    if ( gch->deleted )
	        continue;
	    if ( is_same_group( gch, ch ) )
	    {
		  sprintf( buf,
		  "[%2d %s] %-12s %4d/%4d hp %4d/%4d %s %4d/%4d mv %5d xp\n\r",
			gch->level,
			IS_NPC( gch ) ? "Mob"
			              : class_table[prime_class(gch)].who_name,
			capitalize( PERS( gch, ch ) ),
			gch->hit,   MAX_HIT( gch ),
			MT( gch ),  MT_MAX(gch),
			is_class( gch, CLASS_VAMPIRE ) ? "blood" : "mana",
			gch->move,  MAX_MOVE(gch),
			gch->exp );
/*
		if ( !is_class( gch, CLASS_VAMPIRE ) )
		  sprintf( buf,
		  "[%2d %s] %-12s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
			gch->level,
			IS_NPC( gch ) ? "Mob"
			              : class_table[prime_class(gch)].who_name,
			capitalize( PERS( gch, ch ) ),
			gch->hit,   MAX_HIT( gch ),
			gch->mana,  MAX_MANA(gch),
			gch->move,  MAX_MOVE(gch),
			gch->exp );
		else 
		  sprintf( buf,
		  "[%2d %s] %-12s %4d/%4d hp %4d/%4d bp %4d/%4d mv %5d xp\n\r",
			gch->level,
			IS_NPC( gch ) ? "Mob"
			              : class_table[prime_class(gch)].who_name,
			capitalize( PERS( gch, ch ) ),
			gch->hit,   MAX_HIT( gch ),
			gch->bp,    MAX_BP( gch ),
			gch->move,  MAX_MOVE(gch),
			gch->exp );
*/
                if ( gch->gspell && gch->gspell->timer > 0 )
		 send_to_char( buf, ch );
		else
 		 send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "{wThey aren't here.{x\n\r", ch );
	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "{wBut you are following someone else!{x\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act( "{w$N isn't following you.{x", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act( "{wYou remove $N from your group.{x", ch, NULL, victim, TO_CHAR    );
	act( "{w$n removes you from $s group.{x",  ch, NULL, victim, TO_VICT    );
	act( "{w$n removes $N from $s group.{x",   ch, NULL, victim, TO_NOTVICT );
	return;
    }

    if (   ch->level - victim->level < -8
	|| ch->level - victim->level >  8 )
    {
	act( "{w$N cannot join your group.{x",  ch, NULL, victim, TO_CHAR       );
	act( "{wYou cannot join $n's group.{x", ch, NULL, victim, TO_VICT       );
	act( "{w$N cannot join $n's group.{x",  ch, NULL, victim, TO_NOTVICT    );
	return;
    }

    victim->leader = ch;
    act( "{w$N joins your group.{x", ch, NULL, victim, TO_CHAR    );
    act( "{wYou join $n's group.{x", ch, NULL, victim, TO_VICT    );
    act( "{w$N joins $n's group.{x", ch, NULL, victim, TO_NOTVICT );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];
    int        members;
    int        amount = 0;
    int        share  = 0;
    int        extra  = 0;
#ifdef NEW_MONEY
    MONEY_DATA amt;
    char arg2 [ MAX_STRING_LENGTH ];
#endif

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    if ( arg[0] == '\0' )
    {
	send_to_char( "{wSplit how much?{x\n\r", ch );
	return;
    }
#ifdef NEW_MONEY
/* split 5 copper,gold,silver */

    amount = is_number( arg ) ? atoi( arg ) : 0;
    amt.gold = amt.silver = amt.copper = 0;

    if ( !str_cmp( arg2, "gold" ) )
      amt.gold = amount;
    else if ( !str_cmp( arg2, "silver" ) )
      amt.silver = amount;
    else if ( !str_cmp( arg2, "copper" ) )
      amt.copper = amount;
    else
    {
      send_to_char(  "{wSyntax: split <amount> <currency type>{x\n\r", 
		    ch );
      return;
    }

    if ( ( amt.gold < 0 ) || ( amt.silver < 0 )
	|| ( amt.copper < 0 ) )
    {
        send_to_char( "{wYour group wouldn't like that.{x\n\r", ch );
        return;
    }

    if ( ( amt.gold == 0 ) && ( amt.silver == 0 )
	&& ( amt.copper == 0 ) )
    {
        send_to_char( "{wYou hand out zero coins, but no one notices.{x\n\r", 
		     ch );
        return;
    }
    if ( ( ch->money.gold < amt.gold ) || ( ch->money.silver < amt.silver )
	|| ( ch->money.copper < amt.copper ) )
    {
	sprintf( buf, "{wYou don't have that many %s coins.{x\n\r", arg2 );
        send_to_char( buf, ch );
        return;
    }
#else    

    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "{wYour group wouldn't like that.{x\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "{wYou hand out zero coins, but no one notices.{x\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "{wYou don't have that much gold.{x\n\r", ch );
	return;
    }

#endif  

    members = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
	return;
    }
	    
    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "{wDon't even bother, cheapskate.{x\n\r", ch );
	return;
    }
#ifdef NEW_MONEY
    
    if ( !str_cmp( arg2, "gold" ) )
    {
      ch->money.gold -= amount;
      ch->money.gold += share + extra;
    }
    else if ( !str_cmp( arg2, "silver" ) )
    {
      ch->money.silver -= amount;
      ch->money.silver += share + extra;
    }
    else if ( !str_cmp( arg2, "copper" ) )
    {
      ch->money.copper -= amount;
      ch->money.copper += share + extra;
    }
   
    sprintf( buf,
        "{wYou split %s  Your share is %d %s coins.{x\n\r",
        money_string( &amt ), share + extra, arg2 );
    send_to_char( buf, ch );

    sprintf( buf, "{w$n splits %s.  Your share is %d %s coins.{x",
        money_string( &amt ), share, arg2 );

#else

    ch->gold -= amount;
    ch->gold += share + extra;

    sprintf( buf,
	"{wYou split %d gold coins.  Your share is %d gold coins.{x\n\r",
	amount, share + extra );
    send_to_char( buf, ch );

    sprintf( buf, "{w$n splits %d gold coins.  Your share is %d gold coins.{x",
	amount, share );
#endif
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( buf, ch, NULL, gch, TO_VICT );
#ifdef NEW_MONEY
	    gch->money.gold   += ( !str_cmp( arg2, "gold" ) ) ?
			         share : 0;
	    gch->money.silver += ( !str_cmp( arg2, "silver" ) ) ?
				 share : 0;
	    gch->money.copper += ( !str_cmp( arg2, "copper" ) ) ?
				 share : 0;
#else
	    gch->gold += share;
#endif
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];

    if ( argument[0] == '\0' )
    {
	send_to_char( "{wTell your group what?{x\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "{wYour message didn't get through!{x\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf( buf, "{C%s tells the group '%s'.{x\n\r", ch->name, argument );
    for ( gch = char_list; gch; gch = gch->next )
    {
        if ( gch->deleted )
	    continue;
	if ( is_same_group( gch, ch ) )
	    send_to_char( buf, gch );
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
    if ( ach->deleted || bch->deleted )
      return FALSE;
    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader;
    if ( ach->deleted || bch->deleted )
      return FALSE;
    return ach == bch;
}

void do_pray( CHAR_DATA *ch, char *argument )
{
  if ( !IS_GOOD( ch ) )
  {
    send_to_char( "{wThalador forgives some of your sins.{x\n\r", ch);
    ch->alignment += 10;
  }
  else if (!IS_NPC(ch) && ch->pcdata->learned[gsn_prayer] < number_percent( ))
  {
    send_to_char( "{wThalador smiles upon you, and sends you a gold coin.{x\n\r", ch );
#ifdef NEW_MONEY
    ch->money.gold += 1;
#else
    ch->gold++;
#endif
  }
  else if ( !is_affected( ch, gsn_prayer ) )
  {
    AFFECT_DATA af;

    af.duration = ch->level / 3;
    af.level = ch->level;
    af.type = gsn_prayer;
    af.bitvector = 0;

    af.location = APPLY_HIT;
    af.modifier = number_range( ch->level, ch->level * 2 );
    affect_to_char(ch, &af);

    af.location = APPLY_MANA;
    af.modifier = number_range( ch->level / 2, ch->level );
    if ( is_class( ch, CLASS_VAMPIRE ) )
    {
      af.location = APPLY_BP;
      af.modifier /= 4;
    }
    affect_to_char( ch, &af );

    af.location = APPLY_INT;
    af.modifier = ((ch->level - 1) / 50) + 1;
    affect_to_char( ch, &af );

    af.location = APPLY_DEX;
    affect_to_char( ch, &af );

    send_to_char(  "{wThalador places his blessing upon you.{x\n\r", ch);
    update_skpell( ch, gsn_prayer );
  }
  else
  {
    send_to_char(  "{wThalador frowns at your greed.{x\n\r", ch );
    ch->alignment -= 10;
  }

  send_to_char(  "{wYou fall to the ground, unconscious.{x\n\r", ch );
  act(  "{w$n falls to the ground, unconscious.{x", ch, NULL, NULL, TO_ROOM );
  STUN_CHAR( ch, 4, STUN_COMMAND );
  return;
}

void newbie_help( CHAR_DATA *ch, char *argument )
{
    NEWBIE_DATA *pNewbie;
    CHAR_DATA   *helper;
    bool 	found = FALSE;
    char	buf [ MAX_STRING_LENGTH ];

/* Check for newbie helper in the same room as ch  */
    for ( helper = ch->in_room->people; helper; helper = helper->next_in_room )
    {
      if ( IS_NPC( helper )  &&
	   ( IS_SET( helper->act, ACT_NEWBIE ) ) )
 	 break;
    }

    if ( !helper )
      return;

    for ( pNewbie = newbie_first; pNewbie; pNewbie = pNewbie->next )
    {
      if ( argument[0] == pNewbie->keyword[0] &&
           !str_cmp( argument, pNewbie->keyword ) )
      {
	 found = TRUE;
	 break;
      }
    }

/* if keyword not found, have 30% chance that helper says something
   -- don't want mob responding to every say not found as a keyword */
    if ( !found )
    {
      if ( chance( 30 ) )
      {
        act( 
 	  "{M$n tells you 'I'm not sure if I can help you. Try being more specific.'{x", 
     	   helper, buf, ch, TO_VICT );  
        return;
      }
      else
        return;
    }
    else
    {
/* Mob has two answers. Can respond with both, or give a percentage chance
   that it will respond one or the other for variation. */

      do_say( helper, pNewbie->answer1 );
      if ( chance( 50 ) )
        do_say( helper, pNewbie->answer2 );
    }
  
    return;
   
}


/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
void do_colour( CHAR_DATA *ch, char *argument )
{
    char 	arg[ MAX_STRING_LENGTH ];

    argument = one_argument( argument, arg );

    if( !*arg )
    {
	if( !IS_SET( ch->act, PLR_COLOUR ) )
	{
	    SET_BIT( ch->act, PLR_COLOUR );
	    send_to_char( "{bC{ro{yl{co{mu{gr{x is now {rON{w, Way Cool!{x\n\r", ch );
	}
	else
	{
	    send_to_char_bw( "{wColour is now OFF, <sigh>{x\n\r", ch );
	    REMOVE_BIT( ch->act, PLR_COLOUR );
	}
	return;
    }
    else
    {
	send_to_char_bw( "{wColour Configuration is unavailable in this\n\r", ch );
	send_to_char_bw( "version of colour, sorry{x\n\r", ch );
    }

    return;
}


/* 
 * Page command originally written by Beowolf for TSR.
 * Added 3-6-2001 by Beowolf
 */
void do_beep(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("{wBeep whom?{x\n\r", ch);
	return;
    }

    /* Can beep PC's anywhere just not NPC's in same room. */

    if ((victim = get_char_world(ch, arg)) == NULL
	|| (IS_NPC(victim) && victim->in_room != ch->in_room)) {
	send_to_char("{wThey aren't here you moron! Type who...{x\n\r", ch);
	return;
    }
    if (is_ignoring(victim, ch) && !IS_IMMORTAL(ch))
    {
	send_to_char( "{wThey are ignoring you right now!!{x\n\r", ch );
	return;
    }

    if (victim->desc == NULL && !IS_NPC(victim)) {
	act("{w$N seems to have misplaced $S link...try later.{x",
	    ch, NULL, victim, TO_CHAR);
	sprintf(buf, "\a{wYou have been beeped by %s!!{x\n\r\a", PERS(ch, victim));
	return;
    }
    act("{wYou have paged $N{x \n\r", ch, argument, victim, TO_CHAR);
    act("\a{wYou have been paged by $n! WAKE UP! {x\a\n\r", ch, argument, victim, TO_VICT);
    victim->reply = ch;
    return;
}

 
/* 
 * Bad names bad names, whatcha gonna do, whatcha gonna do
 * when an imm goes off on you... written by Jubelo for TSR.
 * Added by Beowolf on 3/22/01 
 */
void addbadname(CHAR_DATA * ch, char *argument)
{
    FILE *fp;

    if (IS_NPC(ch) || argument[0] == '\0')
	return;

    fclose(fpReserve);

    if ((fp = fopen(BADWORDS_FILE, "a")) == NULL) {
	perror(BADWORDS_FILE);
	send_to_char("{wError, Could not open file!{x\n\r", ch);
    } else {
	fprintf(fp, "{w%s{x\n\r", argument);
	fclose(fp);
    }
    fpReserve = fopen(NULL_FILE, "r");
    send_to_char("{wBad name added.{x\n\r", ch);
    return;
}


/*
 * Ignore players!!! 
 * Added by Beowolf 3/23/01
 */
void do_ignore(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
	return;

    if (argument[0] == '\0') {
	send_to_char("{wYou are currently ignoring the following: ", ch);
	send_to_char(ch->pcdata->ignore, ch);
	send_to_char("\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    buf[0] = '\0';
    strcpy(buf, ch->pcdata->ignore);

    if (!str_cmp(arg1, "+")) {
	if (!is_name(ch, arg2, buf)) {

	   CHAR_DATA *victim;

           victim = arg2;

           if ( ( victim = get_char_world( ch, arg2 ) ) == NULL
              || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
           {
               send_to_char("{wThey aren't here you moron! Type who...{x\n\r",ch);
               return;
           }

           if (IS_IMMORTAL(victim))
           {
               send_to_char("{wYou can't ignore immortals you fool.{x\n\r",ch);
               return;
           }
	    strcat(buf, arg2);
	    strcat(buf, " ");
	}
	free_string(ch->pcdata->ignore);
	ch->pcdata->ignore = str_dup(buf);
	sprintf(buf2, "{wYou are now ignoring %s.{x\n\r", arg2);
	send_to_char(buf2, ch);
	return;
    } else if (!str_cmp(arg1, "-")) {
	if (is_name(ch, arg2, buf)) {
	    buf2[0] = '\0';
	    for (;;) {
		strcpy(buf, one_argument(buf, buf1));
		if (buf1[0] == '\0')
		    break;
		if (strcmp(arg2, buf1)) {
		    strcat(buf2, buf1);
		    strcat(buf2, " ");
		}
	    }
	    free_string(ch->pcdata->ignore);
	    ch->pcdata->ignore = str_dup(buf2);
	    sprintf(buf1, "{wYou are no longer ignoring %s.{x\n\r", arg2);
	    send_to_char(buf1, ch);
	    return;
	}
    } else {
	send_to_char("{wSyntax: ignore <+/-> <player>.{x\n\r", ch);
    }

    return;

}

/*
 * Check for ignoring player
 */
bool is_ignoring( CHAR_DATA * ch, CHAR_DATA * victim)
{
     if (IS_NPC(ch) || IS_NPC(victim))
	return(FALSE);

     if (IS_IMMORTAL(victim))
	return(FALSE);

     if (is_name(ch, victim->name, ch->pcdata->ignore))
	return(TRUE);

     return(FALSE);
}

