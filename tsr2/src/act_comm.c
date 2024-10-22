/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider             |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops  |~'~.VxvxV.~'~*
 * ------------------------------------------------------------------------ *
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
 * Chastain, Michael Quan, and Mitchell Tse.                                *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
 * ------------------------------------------------------------------------ *
 *			   Player communication module			    *
 ****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/*
 *  Externals
 */
void send_obj_page_to_char(CHAR_DATA * ch, OBJ_INDEX_DATA * idx, char page);
void send_room_page_to_char(CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page);
void send_page_to_char(CHAR_DATA * ch, MOB_INDEX_DATA * idx, char page);
void send_control_page_to_char(CHAR_DATA * ch, char page);

/*
 * Local functions.
 */
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );

char *  scramble        args( ( const char *argument, int modifier ) );			    
char *  drunk_speech    args( ( const char *argument, CHAR_DATA *ch ) ); 

/* Text scrambler -- Altrag */
char *scramble( const char *argument, int modifier )
{
    static char arg[MAX_INPUT_LENGTH];
    sh_int position;
    sh_int conversion = 0;
    
	modifier %= number_range( 80, 300 ); /* Bitvectors get way too large #s */
    for ( position = 0; position < MAX_INPUT_LENGTH; position++ )
    {
    	if ( argument[position] == '\0' )
    	{
    		arg[position] = '\0';
    		return arg;
    	}
    	else if ( argument[position] >= 'A' && argument[position] <= 'Z' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - 'A';
	    	conversion = number_range( conversion - 5, conversion + 5 );
	    	while ( conversion > 25 )
	    		conversion -= 26;
	    	while ( conversion < 0 )
	    		conversion += 26;
	    	arg[position] = conversion + 'A';
	    }
	    else if ( argument[position] >= 'a' && argument[position] <= 'z' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - 'a';
	    	conversion = number_range( conversion - 5, conversion + 5 );
	    	while ( conversion > 25 )
	    		conversion -= 26;
	    	while ( conversion < 0 )
	    		conversion += 26;
	    	arg[position] = conversion + 'a';
	    }
	    else if ( argument[position] >= '0' && argument[position] <= '9' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - '0';
	    	conversion = number_range( conversion - 2, conversion + 2 );
	    	while ( conversion > 9 )
	    		conversion -= 10;
	    	while ( conversion < 0 )
	    		conversion += 10;
	    	arg[position] = conversion + '0';
	    }
	    else
	    	arg[position] = argument[position];
	}
	arg[position] = '\0';
	return arg;	     
}

/* I'll rewrite this later if its still needed.. -- Altrag */
char *translate( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument )
{
	return "";
}

char *drunk_speech( const char *argument, CHAR_DATA *ch )
{
  const char *arg = argument;
  static char buf[MAX_INPUT_LENGTH*2];
  char buf1[MAX_INPUT_LENGTH*2];
  sh_int drunk;
  char *txt;
  char *txt1;  

  if ( IS_NPC( ch ) || !ch->pcdata ) return (char *) argument;

  drunk = ch->pcdata->condition[COND_DRUNK];

  if ( drunk <= 0 )
    return (char *) argument;

  buf[0] = '\0';
  buf1[0] = '\0';

  if ( !argument )
  {
     bug( "Drunk_speech: NULL argument", 0 );
     return "";
  }

  /*
  if ( *arg == '\0' )
    return (char *) argument;
  */

  txt = buf;
  txt1 = buf1;

  while ( *arg != '\0' )
  {
    if ( toupper(*arg) == 'S' )
    {
	if ( number_percent() < ( drunk * 2 ) )		/* add 'h' after an 's' */
	{
	   *txt++ = *arg;
	   *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( toupper(*arg) == 'X' )
    {
	if ( number_percent() < ( drunk * 2 / 2 ) )
	{
	  *txt++ = 'c', *txt++ = 's', *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( number_percent() < ( drunk * 2 / 5 ) )  /* slurred letters */
    {
      sh_int slurn = number_range( 1, 2 );
      sh_int currslur = 0;	

      while ( currslur < slurn )
	*txt++ = *arg, currslur++;
    }
   else
    *txt++ = *arg;

    arg++;
  };

  *txt = '\0';

  txt = buf;

  while ( *txt != '\0' )   /* Let's mess with the string's caps */
  {
    if ( number_percent() < ( 2 * drunk / 2.5 ) )
    {
      if ( isupper(*txt) )
        *txt1 = tolower( *txt );
      else
      if ( islower(*txt) )
        *txt1 = toupper( *txt );
      else
        *txt1 = *txt;
    }
    else
      *txt1 = *txt;

    txt1++, txt++;
  };

  *txt1 = '\0';
  txt1 = buf1;
  txt = buf;

  while ( *txt1 != '\0' )   /* Let's make them stutter */
  {
    if ( *txt1 == ' ' )  /* If there's a space, then there's gotta be a */
    {			 /* along there somewhere soon */

      while ( *txt1 == ' ' )  /* Don't stutter on spaces */
        *txt++ = *txt1++;

      if ( ( number_percent() < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
      {
	sh_int offset = number_range( 0, 2 );
	sh_int pos = 0;

	while ( *txt1 != '\0' && pos < offset )
	  *txt++ = *txt1++, pos++;

	if ( *txt1 == ' ' )  /* Make sure not to stutter a space after */
	{		     /* the initial offset into the word */
	  *txt++ = *txt1++;
	  continue;
	}

	pos = 0;
	offset = number_range( 2, 4 );	
	while (	*txt1 != '\0' && pos < offset )
	{
	  *txt++ = *txt1;
	  pos++;
	  if ( *txt1 == ' ' || pos == offset )  /* Make sure we don't stick */ 
	  {		               /* A hyphen right before a space	*/
	    txt1--;
	    break;
	  }
	  *txt++ = '-';
	}
	if ( *txt1 != '\0' )
	  txt1++;
      }     
    }
   else
    *txt++ = *txt1++;
  }

  *txt = '\0';

  return buf;
}

/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int position;

    if ( IS_NPC( ch ) && channel == CHANNEL_CLAN )
    {
	send_to_char( "Mobs can't be in clans.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && channel == CHANNEL_ORDER )
    {
	send_to_char( "Mobs can't be in orders.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && channel == CHANNEL_COUNCIL )
    {
	send_to_char( "Mobs can't be in councils.\n\r", ch);
	return;
    }

    if ( IS_NPC( ch ) && channel == CHANNEL_GUILD )
    {
        send_to_char( "Mobs can't be in guilds.\n\r", ch );
	return;
    }
                                              
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }
    
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
      if ( ch->master )
	send_to_char( "I don't think so...\n\r", ch->master );
      return;
    }

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\n\r", verb );
	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );	/* where'd this line go? */
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	ch_printf( ch, "You can't %s.\n\r", verb );
	return;
    }

    REMOVE_BIT(ch->deaf, channel);

    switch ( channel )
    {
    default:
	set_char_color( AT_GOSSIP, ch );
	ch_printf( ch, "You %s '%s'\n\r", verb, argument );
	sprintf( buf, "$n %ss '$t'",     verb );
	break;
    case CHANNEL_CLAN:
        set_char_color( AT_DEV, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %s '$t'", verb );
        break;
    case CHANNEL_OOC:
        set_char_color( AT_OOC, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %s '$t'", verb );
        break;
    case CHANNEL_SCHAT:
        set_char_color( AT_OOC, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %s '$t'", verb );
        break;
    case CHANNEL_WARTALK:
        set_char_color( AT_WARTALK, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_GOSSIP:
        set_char_color( AT_GOSSIP, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_NEWBIE:
        set_char_color( AT_NEWBIE, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_MUSIC:
        set_char_color( AT_MUSIC, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_GRATS:
        set_char_color( AT_GRATS, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_ASK:
        set_char_color( AT_ASK, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_SHOUT:
        set_char_color( AT_SHOUT, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_YELL:
        set_char_color( AT_YELL, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_HIGHGOD:
        set_char_color( AT_MUSE, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_HIGH:
        set_char_color( AT_THINK, ch );
        ch_printf( ch, "You %s '%s'\n\r", verb, argument );
        sprintf( buf, "$n %ss '$t'", verb );
        break;
    case CHANNEL_AVTALK:
        set_char_color( AT_BUILD, ch );
	sprintf( buf, "$n-> $t" );
	position	= ch->position;
	ch->position	= POS_STANDING;
        act( AT_BUILD, buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    case CHANNEL_AVSPEAK:
        set_char_color( AT_PURPLE, ch );
	sprintf( buf, "[Avatar] $n '$t'" );
	position	= ch->position;
	ch->position	= POS_STANDING;
        act( AT_PURPLE, buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument, verb );
	append_to_file( LOG_FILE, buf2 );
    }

    for ( d = first_descriptor; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	&&   vch != ch
	&&  !IS_SET(och->deaf, channel) )
	{
		char *sbuf = argument;
  	    
	    if ( channel != CHANNEL_NEWBIE && NOT_AUTHED(och) )
		continue;		
	    if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL(och) )
		continue;
	    if ( channel == CHANNEL_AVTALK && !IS_BUILDER(och) )
		continue;
	    if ( channel == CHANNEL_AVSPEAK && !IS_HERO(och) )
		continue;
            if ( channel == CHANNEL_WARTALK && NOT_AUTHED( och ) )
                continue;
	    if ( channel == CHANNEL_HIGHGOD && get_trust( och ) < sysdata.muse_level )
		continue;
	    if ( channel == CHANNEL_HIGH    && get_trust( och ) < sysdata.think_level )
		continue;

            /* Fix by Narn to let newbie council members see the newbie channel. */
	    if ( channel == CHANNEL_NEWBIE  && 
                  ( !IS_HERO(och) && !NOT_AUTHED(och) 
                  && !( och->pcdata->council && 
                     !str_cmp( och->pcdata->council->name, "Newbie Council" ) ) ) )
		continue;
	    if ( IS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
	    	continue;
	    if ( channel == CHANNEL_YELL
	    &&   vch->in_room->area != ch->in_room->area )
		continue;

	    if ( channel == CHANNEL_CLAN || channel == CHANNEL_ORDER
	    ||   channel == CHANNEL_GUILD )
	    {
		if ( IS_NPC( vch ) )
		  continue;
		if ( vch->pcdata->clan != ch->pcdata->clan )
	    	  continue;
	    }

	    if ( channel == CHANNEL_COUNCIL )
	    {
		if ( IS_NPC( vch ) )
		  continue;
		if ( vch->pcdata->council != ch->pcdata->council )
	    	  continue;
	    }

	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
		if ( !knows_language( vch, ch->speaking, ch ) &&
			 (!IS_NPC(ch) || ch->speaking != 0) )
			sbuf = scramble(argument, ch->speaking);
	    MOBtrigger = FALSE;
	    if ( channel == CHANNEL_IMMTALK || channel == CHANNEL_AVTALK )
	      act( AT_IMMORT, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_AVSPEAK)
              act( AT_PURPLE, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_WARTALK)
              act( AT_WARTALK, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_GOSSIP)
              act( AT_GOSSIP, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_CLAN)
              act( AT_DEV, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_NEWBIE)
              act( AT_NEWBIE, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_MUSIC)
              act( AT_MUSIC, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_GRATS)
              act( AT_GRATS, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_ASK)
              act( AT_ASK, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_SHOUT)
              act( AT_SHOUT, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_YELL)
              act( AT_YELL, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_HIGHGOD)
              act( AT_MUSE, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_HIGH)
              act( AT_THINK, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_OOC)
              act( AT_OOC, buf, ch, sbuf, vch, TO_VICT );
	    else
	      act( AT_GOSSIP, buf, ch, sbuf, vch, TO_VICT );
	    vch->position	= position;
	}
    }

    return;
}

void do_gossip( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_GOSSIP, "gossip" );
    return;
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->clan 
    ||   ch->pcdata->clan->clan_type == CLAN_ORDER
    ||   ch->pcdata->clan->clan_type == CLAN_GUILD )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_CLAN, "[DEVOTION]" );
    return;
}

void do_newbiechat( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch )
       || ( !NOT_AUTHED( ch ) && !IS_HERO(ch) 
       && !( ch->pcdata->council && 
          !str_cmp( ch->pcdata->council->name, "Newbie Council" ) ) ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    talk_channel( ch, argument, CHANNEL_NEWBIE, "newbiechat" );
    return;
}

void do_ot( CHAR_DATA *ch, char *argument )
{
  do_ordertalk( ch, argument );
}

void do_ordertalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->clan 
         || ch->pcdata->clan->clan_type != CLAN_ORDER )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_ORDER, "[ORDER]" );
    return;
}

void do_counciltalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->council )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_COUNCIL, "counciltalk" );
    return;
}

void do_guildtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->clan || ch->pcdata->clan->clan_type != CLAN_GUILD )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_GUILD, "guildtalk" );
    return;
}

void do_music( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
    return;
}


void do_grats( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_GRATS, "grat" );
    return;
}

void do_ask( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ASK, "ask" );
    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ASK, "answer" );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHOUT, "shout" );
  WAIT_STATE( ch, 12 );
  return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_YELL, "yell" );
  return;
}


void do_muse( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_HIGHGOD, "muse" );
    return;
}


void do_think( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_HIGH, "think" );
    return;
}


void do_avtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_AVTALK, "avtalk" );
    return;
}


void do_say( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *vch;
    int actflags;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	{
		char *sbuf = argument;

		if ( vch == ch )
			continue;
		if ( !knows_language(vch, ch->speaking, ch) &&
			 (!IS_NPC(ch) || ch->speaking != 0) )
			sbuf = scramble(argument, ch->speaking);
	      sbuf = drunk_speech( sbuf, ch );

		MOBtrigger = FALSE;
		act( AT_SAY, "$n says '$t'", ch, sbuf, vch, TO_VICT );
	}
/*    MOBtrigger = FALSE;
    act( AT_SAY, "$n says '$T'", ch, NULL, argument, TO_ROOM );*/
    ch->act = actflags;
    MOBtrigger = FALSE;
    act( AT_SAY, "You say '$T'", ch, NULL, drunk_speech( argument, ch ), TO_CHAR ); 
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    if ( char_died(ch) )
      return;
    oprog_speech_trigger( argument, ch ); 
    if ( char_died(ch) )
      return;
    rprog_speech_trigger( argument, ch ); 
    return;
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int position;
    CHAR_DATA *switched_victim;
    switched_victim = NULL;

    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch)
    && ( IS_SET(ch->act, PLR_SILENCE)
    ||   IS_SET(ch->act, PLR_NO_TELL) ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL 
       || ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
       || (!NOT_AUTHED(ch) && NOT_AUTHED(victim) && !IS_IMMORTAL(ch) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You have a nice little chat with yourself.\n\r", ch );
	return;
    }

    if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
    {
	send_to_char( "They can't hear you because you are not authorized.\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& ( get_trust( ch ) > LEVEL_AVATAR ) 
        && !IS_SET(victim->switched->act, ACT_POLYMORPHED)
	&& !IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }

   else if ( !IS_NPC( victim ) && ( victim->switched ) 
        && (IS_SET(victim->switched->act, ACT_POLYMORPHED) 
 	||  IS_AFFECTED(victim->switched, AFF_POSSESS) ) )
     switched_victim = victim->switched;

   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
      {
      send_to_char( "That player is afk.\n\r", ch );
      return;
      }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_TELL, "$E has $S tells turned off.", ch, NULL, victim,
		TO_CHAR );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_SILENCE ) ) )
      {
      send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );
      }   

    if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || (!IS_NPC(victim)&&IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_TELL, "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_TELL, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

 
    if(switched_victim)
      victim = switched_victim;
   
    act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    if ( knows_language( victim, ch->speaking, ch )
    ||  (IS_NPC(ch) && !ch->speaking) )
	act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
    else
	act( AT_TELL, "$n tells you '$t'", ch, scramble(argument, ch->speaking), victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (tell to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;


    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched )
	&& can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
      {
      send_to_char( "That player is afk.\n\r", ch );
      return;
      }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
	TO_CHAR );
      return;
    }

    if ( ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || ( !IS_NPC(victim) && IS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    act( AT_TELL, "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    if ( knows_language( victim, ch->speaking, ch ) ||
    	 (IS_NPC(ch) && !ch->speaking) )
	    act( AT_TELL, "$n tells you '$t'", ch, argument, victim, TO_VICT );
	else
		act( AT_TELL, "$n tells you '$t'", ch, scramble(argument, ch->speaking), victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (reply to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    CHAR_DATA *vch;
    int actflags;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	{
		char *sbuf = buf;
		
		if ( !knows_language( vch, ch->speaking, ch ) &&
			 (!IS_NPC(ch) && ch->speaking != 0) )
			sbuf = scramble(buf, ch->speaking);
		MOBtrigger = FALSE;
		act( AT_ACTION, "$n $t", ch, sbuf, vch, (vch == ch ? TO_CHAR : TO_VICT) );
	}
/*    MOBtrigger = FALSE;
    act( AT_ACTION, "$n $T", ch, NULL, buf, TO_ROOM );
    MOBtrigger = FALSE;
    act( AT_ACTION, "$n $T", ch, NULL, buf, TO_CHAR );*/
    ch->act = actflags;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s %s (emote)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    return;
}


void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}


void do_ide( CHAR_DATA *ch, char *argument )
{
    send_to_char("If you want to send an idea, type 'idea <message>'.\n\r", ch);
    send_to_char("If you want to identify an object and have the identify spell,\n\r", ch);
    send_to_char("Type 'cast identify <object>'.\n\r", ch);
    return;
}

void do_idea( CHAR_DATA *ch, char *argument )
{
    append_file( ch, IDEA_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_WHITE, ch );
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}

void do_quit( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];  /* Beowolf for do_info */
/*  OBJ_DATA *obj; */ /* Unused */
    int x, y;
    int level;

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_POLYMORPHED))
    { 
      send_to_char("You can't quit while polymorphed.\n\r", ch);
      return;
    }

    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	set_char_color( AT_RED, ch );
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	set_char_color( AT_BLOOD, ch );
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }

    /* Mounted check */
    if (ch->mount )
    {
 	send_to_char( "What??? Your mount wouldn't like that!!\n\r", ch);
	return;
    }

    if ( get_timer(ch, TIMER_RECENTFIGHT) > 0
    &&  !IS_IMMORTAL(ch) )
    {
	set_char_color( AT_RED, ch );
	send_to_char( "Your adrenaline is pumping too hard to quit now!\n\r", ch );
	return;
    }

    if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller) ) )
    {
	send_to_char("Wait until you have bought/sold the item on auction.\n\r", ch);
	return;
    }

    if (IS_SET(ch->pcdata->flags, PCFLAG_SOUND))
     act( AT_SAY, "!!SOUND(Tsr/Evoice19 V=100)", ch, NULL, NULL, TO_CHAR);

    set_char_color( AT_WHITE, ch );
    send_to_char( "Your surroundings begin to fade as a mystical swirling vortex of colors\n\renvelops your body... When you come to, things are not as they were.\n\r\n\r", ch );
    act( AT_SAY, "A strange voice says, 'We await your return, $n...'", ch, NULL, NULL, TO_CHAR );
    act( AT_BYE, "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf( buf,"%s returns to the dust from whence they came.", ch->name);
    if(!IS_IMMORTAL(ch) && ch->level > 2 ) 
    do_info(ch, buf);
    set_char_color( AT_GREY, ch);

    /* don't show who's logging off to leaving player */
    sprintf( log_buf, "%s has quit.", ch->name );
    wiznet( log_buf, ch, NULL, WIZ_LOGINS, 0, 0);
    log_string(log_buf);
    quitting_char = ch;
    save_char_obj( ch );
    saving_char = NULL;

    level = get_trust(ch);
    /*
     * After extract_char the ch is no longer valid!
     */
    extract_char( ch, TRUE );
    for ( x = 0; x < MAX_WEAR; x++ )
	for ( y = 0; y < MAX_LAYERS; y++ )
	    save_equipment[x][y] = NULL;
    return;
}


void send_rip_screen( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(RIPSCREEN_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_rip_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(RIPTITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_ansi_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(ANSITITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_ascii_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH];

    if ((rpfile = fopen(ASCTITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void do_omenu( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
	 
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: omenu <object> [page]  \n\r",     ch );
        send_to_char( "      Where:    <object> is a prototype object  \n\r",     ch );
        send_to_char( "            and  <page>  is an optional letter to select menu-pages\n\r",     ch );
        return;
    }
	 
    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    /* can redit or something */

    ch->inter_type = OBJ_TYPE;
    ch->inter_substate = SUB_NORTH;
    if( ch->inter_editing != NULL) DISPOSE(ch->inter_editing);
    ch->inter_editing      =  str_dup(obj->pIndexData->name);
    sscanf(ch->inter_editing,"%s",ch->inter_editing);  /*one-arg*/
    ch->inter_editing_vnum =  obj->pIndexData->vnum;
    send_obj_page_to_char(ch, obj->pIndexData, arg2[0]);   
}


void do_rmenu( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *idx;
    char arg1[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
	 
    idx = ch->in_room;
    /* can redit or something */

    ch->inter_type = ROOM_TYPE;
    ch->inter_substate = SUB_NORTH;
    if( ch->inter_editing != NULL) DISPOSE(ch->inter_editing);
    ch->inter_editing      =  str_dup(idx->name);  
    sscanf(ch->inter_editing,"%s",ch->inter_editing);  /*one-arg*/
    ch->inter_editing_vnum =  idx->vnum;
    send_room_page_to_char(ch, idx, arg1[0]);   
}

void do_cmenu( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
	 
    ch->inter_type = CONTROL_TYPE;
    if( ch->inter_editing != NULL) DISPOSE(ch->inter_editing);
    ch->inter_editing      =  str_dup("Control Panel");  
    sscanf(ch->inter_editing,"%s",ch->inter_editing);  /*one-arg*/
    send_control_page_to_char(ch, arg1[0]);   
}


void do_mmenu( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
	 
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: mmenu <victim> [page]  \n\r",     ch );
        send_to_char( "      Where:    <victim> is a prototype mob  \n\r",     ch );
        send_to_char( "            and  <page>  is an optional letter to select menu-pages\n\r",     ch );
        return;
    }
      

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    if ( get_trust( ch ) < victim->level )
    {
	set_char_color( AT_IMMORT, ch );
	send_to_char( "Their godly glow prevents you from getting a good look .\n\r", ch );
	return;
    }
    ch->inter_type = MOB_TYPE;
    if( ch->inter_editing != NULL) DISPOSE(ch->inter_editing);
    ch->inter_editing      =  str_dup(arg1);
    sscanf(ch->inter_editing,"%s",ch->inter_editing);  /*one-arg*/
    ch->inter_editing_vnum =  victim->pIndexData->vnum;
    send_page_to_char(ch, victim->pIndexData, arg2[0]);   
}


void do_rip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rip ON or OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	send_rip_screen(ch);
	SET_BIT(ch->act,PLR_RIP);
	SET_BIT(ch->act,PLR_ANSI);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->act,PLR_RIP);
	send_to_char( "!|*\n\rRIP now off...\n\r", ch );
	return;
    }
}

void do_ansi( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "ANSI ON or OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	SET_BIT(ch->act,PLR_ANSI);
	set_char_color( AT_WHITE + AT_BLINK, ch);
	send_to_char( "ANSI ON!!!\n\r", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->act,PLR_ANSI);
	send_to_char( "Okay... ANSI support is now off\n\r", ch );
	return;
    }
}

void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_POLYMORPHED))
    { 
      send_to_char("You can't save while polymorphed.\n\r", ch);
      return;
    }

    if ( IS_NPC(ch) )
	return;

    if ( ch->level < 2 )
    {
	set_char_color( AT_BLUE, ch );
	send_to_char( "You must be at least second level to save.\n\r", ch );
	return;
    }

    if ( !IS_SET( ch->affected_by, race_table[ch->race].affected ) )
	SET_BIT( ch->affected_by, race_table[ch->race].affected );
    if ( !IS_SET( ch->resistant, race_table[ch->race].resist ) )
	SET_BIT( ch->resistant, race_table[ch->race].resist );
    if ( !IS_SET( ch->susceptible, race_table[ch->race].suscept ) )
	SET_BIT( ch->susceptible, race_table[ch->race].suscept );	   

    if ( ch->pcdata->deity )
    {
	if ( !IS_SET( ch->affected_by, ch->pcdata->deity->affected ) )
	   SET_BIT( ch->affected_by, ch->pcdata->deity->affected );
	if ( !IS_SET( ch->resistant, ch->pcdata->deity->element ) )
	   SET_BIT( ch->resistant, ch->pcdata->deity->element );
	if ( !IS_SET( ch->susceptible, ch->pcdata->deity->suscept ) )
	   SET_BIT( ch->susceptible, ch->pcdata->deity->suscept );
    }
    save_char_obj( ch );
    saving_char = NULL;
    send_to_char( "Ok.\n\r", ch );
    return;
}


/*
 * Something from original DikuMUD that Merc yanked out.
 * Used to prevent following loops, which can cause problems if people
 * follow in a loop through an exit leading back into the same room
 * (Which exists in many maze areas)			-Thoric
 */
bool circle_follow( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *tmp;

    for ( tmp = victim; tmp; tmp = tmp->master )
	if ( tmp == ch )
	  return TRUE;
    return FALSE;
}


void do_follow( CHAR_DATA *ch, char *argument )
{
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

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master )
    {
	act( AT_PLAIN, "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if ( circle_follow( ch, victim ) )
    {
	send_to_char( "Following in loops is not allowed... sorry.\n\r", ch );
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
    act( AT_ACTION, "$n now follows you.", ch, NULL, master, TO_VICT );

    act( AT_ACTION, "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) )
    act( AT_ACTION, "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
    act( AT_ACTION, "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = first_char; fch; fch = fch->next )
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
    char arg[MAX_INPUT_LENGTH];
    char argbuf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    strcpy( argbuf, argument );
    argument = one_argument( argument, arg );

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

	if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->first_person; och; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	act( AT_ACTION, "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
        sprintf( log_buf, "%s: order %s.", ch->name, argbuf );
        wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, 0);
        log_string(log_buf);
 	send_to_char( "Ok.\n\r", ch );
        WAIT_STATE( ch, 12 );
    }
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}

/*
char *itoa(int foo)
{
  static char bar[256];

  sprintf(bar,"%d",foo);
  return(bar);

}
*/

void do_group( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ch->leader ? ch->leader : ch;
	set_char_color( AT_GREEN, ch );
	ch_printf( ch, "%s's group:\n\r", PERS(leader, ch) );

/* Changed so that no info revealed on possess */
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		set_char_color( AT_DGREEN, ch );
		if (IS_AFFECTED(gch, AFF_POSSESS))
		  ch_printf( ch,
		    "[%2d %s] %-16s %4s/%4s hp %4s/%4s %s %4s/%4s mv %5s xp\n\r",
		    gch->level,
		    IS_NPC(gch) ? "Mob" : class_table[gch->class]->who_name,
		    capitalize( PERS(gch, ch) ),
		    "????",   
		    "????",
		    "????",
		    "????",
		    IS_VAMPIRE(gch) ? "bp" : "mana",
		    "????",  
		    "????",  
		    "?????"    );

		else
		  ch_printf( ch,
		    "[%2d %s] %-16s %4d/%4d hp %4d/%4d %s %4d/%4d mv %5d xp\n\r",
		    gch->level,
		    IS_NPC(gch) ? "Mob" : class_table[gch->class]->who_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   
		    gch->max_hit,
		    IS_VAMPIRE(gch) ? gch->pcdata->condition[COND_BLOODTHIRST]
				    : gch->mana,
		    IS_VAMPIRE(gch) ? 10 + gch->level : gch->max_mana,
		    IS_VAMPIRE(gch) ? "bp" : "mana",
		    gch->move,  
		    gch->max_move,  
		    gch->exp    );
	    }
	}
	return;
    }

    if ( !strcmp( arg, "disband" ))
    {
	CHAR_DATA *gch;
	int count = 0;

	if ( ch->leader || ch->master )
	{
	    send_to_char( "You cannot disband a group if you're following someone.\n\r", ch );
	    return;
	}
	
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( ch, gch )
	    && ( ch != gch ) )
	    {
		gch->leader = NULL;
		gch->master = NULL;
		count++;
		send_to_char( "Your group is disbanded.\n\r", gch );
	    }
	}

	if ( count == 0 )
	   send_to_char( "You have no group members to disband.\n\r", ch );
	else
	   send_to_char( "You disband your group.\n\r", ch );
	
    return;
    }

    if ( !strcmp( arg, "all" ) )
    {
	CHAR_DATA *rch;
	int count = 0;

        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
	{
           if ( ch != rch
           &&   !IS_NPC( rch )
	   &&   can_see( ch, rch )
	   &&   rch->master == ch
	   &&   !ch->master
	   &&   !ch->leader
    	   &&   !is_same_group( rch, ch )
	      )
	   {
		rch->leader = ch;
		count++;
	   }
	}
	
	if ( count == 0 )
	  send_to_char( "You have no eligible group members.\n\r", ch );
	else
	{
     	   act( AT_ACTION, "$n groups $s followers.", ch, NULL, victim, TO_ROOM );
	   send_to_char( "You group your followers.\n\r", ch );
	}
    return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
    act( AT_PLAIN, "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
    act( AT_ACTION, "$n removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "$n removes you from $s group.",  ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	return;
    }

    victim->leader = ch;
    act( AT_ACTION, "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    
    if (( IS_SET(ch->act, PLR_AUTOGOLD)) && (members < 2))
    return;

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    set_char_color( AT_GOLD, ch );
    ch_printf( ch,
	"You split %d gold coins.  Your share is %d gold coins.\n\r",
	amount, share + extra );

    sprintf( buf, "$n splits %d gold coins.  Your share is %d gold coins.",
	amount, share );

    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }
    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
/*    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );*/
    for ( gch = first_char; gch; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	{
	    set_char_color( AT_GTELL, gch );
	    /* Groups unscrambled regardless of clan language.  Other languages
		   still garble though. -- Altrag */
	    if ( knows_language( gch, ch->speaking, gch )
	    ||  (IS_NPC(ch) && !ch->speaking) )
		ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, argument );
	    else
		ch_printf( gch, "%s tells the group '%s'.\n\r", ch->name, scramble(argument, ch->speaking) );
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
    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader;
    return ach == bch;
}

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction (char *argument)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;

    sprintf (buf,"Auction: %s", argument); /* last %s to reset color */

    for (d = first_descriptor; d; d = d->next)
    {
        original = d->original ? d->original : d->character; /* if switched */
        if ((d->connected == CON_PLAYING) && !IS_SET(original->deaf,CHANNEL_AUCTION) 
        && !IS_SET(original->in_room->room_flags, ROOM_SILENCE) && !NOT_AUTHED(original))
            act( AT_AUCTION, buf, original, NULL, NULL, TO_CHAR );
    }
}

/*
 * Language support functions. -- Altrag
 * 07/01/96
 */
bool knows_language( CHAR_DATA *ch, int language, CHAR_DATA *cch )
{
	sh_int sn;

	if ( !IS_NPC(ch) && IS_IMMORTAL(ch) )
		return TRUE;
	if ( IS_NPC(ch) && !ch->speaks ) /* No langs = knows all for npcs */
		return TRUE;
	if ( IS_NPC(ch) && IS_SET(ch->speaks, (language & ~LANG_CLAN)) )
		return TRUE;
	/* everyone KNOWS common tongue */
	if ( IS_SET(language, LANG_COMMON) )
		return TRUE;
	if ( language & LANG_CLAN )
	{
		/* Clan = common for mobs.. snicker.. -- Altrag */
		if ( IS_NPC(ch) || IS_NPC(cch) )
			return TRUE;
		if ( ch->pcdata->clan == cch->pcdata->clan &&
			 ch->pcdata->clan != NULL )
			return TRUE;
	}
	if ( !IS_NPC( ch ) )
	{
	    int lang;
	    
		/* Racial languages for PCs */
	    if ( IS_SET(race_table[ch->race].language, language) )
	    	return TRUE;

	    for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
	      if ( IS_SET(language, lang_array[lang]) &&
	      	   IS_SET(ch->speaks, lang_array[lang]) )
	      {
		  if ( (sn = skill_lookup(lang_names[lang])) != -1
		  &&    ch->pcdata->learned[sn] >= 60 )
		    return TRUE;
	      }
	}
	return FALSE;
}

bool can_learn_lang( CHAR_DATA *ch, int language )
{
	if ( language & LANG_CLAN )
		return FALSE;
	if ( IS_NPC(ch) || IS_IMMORTAL(ch) )
		return FALSE;
	if ( race_table[ch->race].language & language )
		return FALSE;
	if ( ch->speaks & language )
	{
		int lang;
		
		for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
			if ( language & lang_array[lang] )
			{
				int sn;
				
				if ( !(VALID_LANGS & lang_array[lang]) )
					return FALSE;
				if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
				{
					bug( "Can_learn_lang: valid language without sn: %d", lang );
					continue;
				}
				if ( ch->pcdata->learned[sn] >= 99 )
					return FALSE;
			}
	}
	if ( VALID_LANGS & language )
		return TRUE;
	return FALSE;
}

int const lang_array[] = { LANG_COMMON, LANG_ELVEN, LANG_DWARVEN, LANG_PIXIE,
 LANG_OGRE, LANG_ORCISH, LANG_TROLLISH, LANG_RODENT,
 LANG_INSECTOID, LANG_MAMMAL, LANG_REPTILE,
 LANG_DRAGON, LANG_SPIRITUAL, LANG_MAGICAL,
 LANG_GOBLIN, LANG_GOD, LANG_ANCIENT, LANG_HALFLING,
 LANG_CLAN, LANG_GITH, LANG_GIANT, LANG_DEMON, LANG_BODAK,
 LANG_SITHID, LANG_CENTAUR, LANG_DROW, LANG_SATYR, LANG_UNKNOWN };

/* Note: does not count racial language.  This is intentional (for now). */
int countlangs( int languages )
{
	int numlangs = 0;
	int looper;

	for ( looper = 0; lang_array[looper] != LANG_UNKNOWN; looper++ )
	{
		if ( lang_array[looper] == LANG_CLAN )
			continue;
		if ( languages & lang_array[looper] )
			numlangs++;
	}
	return numlangs;
}

char * const lang_names[] = { 
"common", 
"elvish", 
"dwarven", 
"pixie", 
"ogre",
"orcish", 
"trollese", 
"rodent", 
"insectoid",
"mammal", 
"reptile", 
"dragon", 
"spiritual",
"magical", 
"goblin", 
"god", 
"ancient",
"halfling", 
"clan", 
"gith", 
"giant", 
"demon",
"bodak", 
"sithid", 
"centaur", 
"drow", 
"satyr", 
"" };

void do_speak( CHAR_DATA *ch, char *argument )
{
	int langs;
	char arg[MAX_INPUT_LENGTH];
	
	argument = one_argument(argument, arg );
	
	if ( !str_cmp( arg, "all" ) && IS_IMMORTAL( ch ) )
	{
		set_char_color( AT_SAY, ch );
		ch->speaking = ~LANG_CLAN;
		send_to_char( "Now speaking all languages.\n\r", ch );
		return;
	}
	for ( langs = 0; lang_array[langs] != LANG_UNKNOWN; langs++ )
		if ( !str_prefix( arg, lang_names[langs] ) )
			if ( knows_language( ch, lang_array[langs], ch ) )
			{
				if ( lang_array[langs] == LANG_CLAN &&
					(IS_NPC(ch) || !ch->pcdata->clan) )
					continue;
				ch->speaking = lang_array[langs];
				set_char_color( AT_SAY, ch );
				ch_printf( ch, "You now speak %s.\n\r", lang_names[langs] );
				return;
			}
	set_char_color( AT_SAY, ch );
	send_to_char( "You do not know that language.\n\r", ch );
}

void do_languages( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	int lang;
	
	argument = one_argument( argument, arg );
	if ( arg[0] != '\0' && !str_prefix( arg, "learn" ) &&
		!IS_IMMORTAL(ch) && !IS_NPC(ch) )
	{
		CHAR_DATA *sch;
		char arg2[MAX_INPUT_LENGTH];
		int sn;
		int prct;
		int prac;
		
		argument = one_argument( argument, arg2 );
		if ( arg2[0] == '\0' )
		{
			send_to_char( "Learn which language?\n\r", ch );
			return;
		}
		for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
		{
			if ( lang_array[lang] == LANG_CLAN )
				continue;
			if ( !str_prefix( arg2, lang_names[lang] ) )
				break;
		}
		if ( lang_array[lang] == LANG_UNKNOWN )
		{
			send_to_char( "That is not a language.\n\r", ch );
			return;
		}
		if ( !(VALID_LANGS & lang_array[lang]) )
		{
			send_to_char( "You may not learn that language.\n\r", ch );
			return;
		}
		if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
		{
			send_to_char( "That is not a language.\n\r", ch );
			return;
		}
		if ( race_table[ch->race].language & lang_array[lang] ||
			 lang_array[lang] == LANG_COMMON ||
			 ch->pcdata->learned[sn] >= 99 )
		{
			act( AT_PLAIN, "You are already fluent in $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
			return;
		}
		for ( sch = ch->in_room->first_person; sch; sch = sch->next )
			if ( IS_NPC(sch) && IS_SET(sch->act, ACT_SCHOLAR) &&
					knows_language( sch, ch->speaking, ch ) &&
					knows_language( sch, lang_array[lang], sch ) &&
					(!sch->speaking || knows_language( ch, sch->speaking, sch )) )
				break;
		if ( !sch )
		{
			send_to_char( "There is no one who can teach that language here.\n\r", ch );
			return;
		}
		if ( countlangs(ch->speaks) >= (ch->level / 10) &&
			 ch->pcdata->learned[sn] <= 0 )
		{
			act( AT_TELL, "$n tells you 'You may not learn a new language yet.'",
				 sch, NULL, ch, TO_VICT );
			return;
		}
		/* 0..16 cha = 2 pracs, 17..25 = 1 prac. -- Altrag */
		prac = 2 - (get_curr_cha(ch) / 17);
		if ( ch->practice < prac )
		{
			act( AT_TELL, "$n tells you 'You do not have enough practices.'",
				 sch, NULL, ch, TO_VICT );
			return;
		}
		ch->practice -= prac;
		/* Max 12% (5 + 4 + 3) at 24+ int and 21+ wis. -- Altrag */
		prct = 5 + (get_curr_int(ch) / 6) + (get_curr_wis(ch) / 7);
		ch->pcdata->learned[sn] += prct;
		ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn], 99);
		SET_BIT( ch->speaks, lang_array[lang] );
		if ( ch->pcdata->learned[sn] == prct )
			act( AT_PLAIN, "You begin lessons in $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 60 )
			act( AT_PLAIN, "You continue lessons in $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 60 + prct )
			act( AT_PLAIN, "You feel you can start communicating in $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 99 )
			act( AT_PLAIN, "You become more fluent in $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
		else
			act( AT_PLAIN, "You now speak perfect $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		return;
	}
	for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
		if ( knows_language( ch, lang_array[lang], ch ) )
		{
			if ( ch->speaking & lang_array[lang] ||
				(IS_NPC(ch) && !ch->speaking) )
				set_char_color( AT_RED, ch );
			else
				set_char_color( AT_SAY, ch );
			send_to_char( lang_names[lang], ch );
			send_to_char( " ", ch );
		}
	send_to_char( "\n\r", ch );
	return;
}

void do_wartalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_WARTALK, "war" );
    return;
}

/* OOC channel by Beowolf. 1-4-97 */
void do_ooc( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_OOC, "[OOC]:" );
    return;
}

/* New Immtalk by Beowolf */
void do_immtalk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

	if (ch->level == MAX_LEVEL)
	{
           act( AT_IMMORT, "[IMP] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 1))
	{
           act( AT_IMMORT, "[ELD] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 2))
	{
           act( AT_IMMORT, "[ETL] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 3))
	{
           act( AT_IMMORT, "[ACN] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 4))
	{
           act( AT_IMMORT, "[EXD] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL -5))
	{
           act( AT_IMMORT, "[HGH] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 6))
	{
           act( AT_IMMORT, "[GRT] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 7))
	{
           act( AT_IMMORT, "[MOD] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 8))
	{
           act( AT_IMMORT, "[LES] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 9))
	{
           act( AT_IMMORT, "[IMM] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 10))
	{
           act( AT_IMMORT, "[DEM] $n: $t",ch,argument,NULL,TO_CHAR);
	}
	else if (ch->level == (MAX_LEVEL - 11))
	{
           act( AT_IMMORT, "[ANG] $n: $t",ch,argument,NULL,TO_CHAR);
	}

    for ( d = first_descriptor; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && IS_REAL_IMMORTAL (d->character))
	{

	if (ch->level == MAX_LEVEL)
        act( AT_IMMORT, "[IMP] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 1))
        act( AT_IMMORT, "[ELD] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 2))
        act( AT_IMMORT, "[ETL] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 3))
        act( AT_IMMORT, "[ANC] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 4))
        act( AT_IMMORT, "[ETD] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 5))
        act( AT_IMMORT, "[AST] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 6))
        act( AT_IMMORT, "[GRT] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 7))
        act( AT_IMMORT, "[MOD] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 8))
        act( AT_IMMORT, "[LES] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 9))
        act( AT_IMMORT, "[IMM] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 10))
        act( AT_IMMORT, "[DEM] $n: $t",ch,argument,d->character,TO_VICT);

	else if (ch->level == (MAX_LEVEL - 11))
        act( AT_IMMORT, "[ANG] $n: $t",ch,argument,d->character,TO_VICT);

	}
    }

    return;
}

/* Page command written by Beowolf */
void do_beep( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH]; 
    CHAR_DATA *victim;

    argument=one_argument(argument, arg);

    if(arg[0]=='\0')
    {
	send_to_char("Page whom?\n\r",ch);
	return;
    }
    
    /* Can page PC's anywhere just not NPC's in same room.*/

    if((victim=get_char_world(ch,arg))==NULL
    ||(IS_NPC(victim)&&victim->in_room !=ch->in_room))
    {
	send_to_char("They aren't here you moron! Type who...\n\r",ch);
	return;
    }

    if (victim->desc==NULL&&!IS_NPC(victim))
    {
        act(AT_TELL,"$N seems to have misplaced $S link.", ch, NULL, victim, TO_CHAR );
	ch_printf (ch ,"\aYou have been paged by %s!!\n\r\a",PERS(ch,victim));
	return;
    }

    act(AT_RED, "\aYou have paged $N \n\r",ch,argument,victim,TO_CHAR);
    act(AT_RED,"\aYou have been paged by $n!\a\n\r",ch,argument,victim,TO_VICT);
    victim->reply = ch;
    return;
}

/* OSAY Command by Beowolf. Idea from Ethan. */
void do_osay( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *vch;
    int actflags;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Osay what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	{
		char *sbuf = argument;

		if ( vch == ch )
			continue;
		if ( !knows_language(vch, ch->speaking, ch) &&
			 (!IS_NPC(ch) || ch->speaking != 0) )
			sbuf = scramble(argument, ch->speaking);
	      sbuf = drunk_speech( sbuf, ch );

		MOBtrigger = FALSE;
		act( AT_OSAY, "$n says ooc: '$t'", ch, sbuf, vch, TO_VICT );
	}
    ch->act = actflags;
    MOBtrigger = FALSE;
    act( AT_OSAY, "You say ooc: '$T'", ch, NULL, drunk_speech( argument, ch ), TO_CHAR ); 
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    if ( char_died(ch) )
      return;
    oprog_speech_trigger( argument, ch ); 
    if ( char_died(ch) )
      return;
    rprog_speech_trigger( argument, ch ); 
    return;
}

/* Channel for ADMINS by Beowolf */
void do_admin( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
 
     if (ch->level < (MAX_LEVEL ))
     {
 	send_to_char("Sorry, you are not an Admin!",ch);
 	return;
     }
 
     act( AT_ADMIN, "[Admin] $n: '$t'",ch,argument,NULL,TO_CHAR);
     for ( d = first_descriptor; d != NULL; d = d->next )
     {
 	if ( d->connected == CON_PLAYING && IS_ADMIN ( d->character ))
 	{
 	  act( AT_ADMIN, "[Admin] $n: '$t'",ch,argument,d->character,TO_VICT);
 	}
     }
 
     return;
}

/* New channel for AVATARs 4/19/97 */
void do_avspeak( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_AVSPEAK, "[AVATAR]" );
    return;
}

void do_schat( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_SCHAT, "[SOUND]" );
    return;
}

