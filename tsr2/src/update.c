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
 *			      Regular update module			    *
 ****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"


/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );
void	room_act_update	args( ( void ) );
void	obj_act_update	args( ( void ) );
void	char_check	args( ( void ) );
void    quest_update    args( ( void ) ); /* Vassago - quest.c */
void    drunk_randoms	args( ( CHAR_DATA *ch ) );
void    halucinations	args( ( CHAR_DATA *ch ) );
void	subtract_times	args( ( struct timeval *etime,
				struct timeval *stime ) );

/*
 * Global Variables
 */

CHAR_DATA *	gch_prev;
OBJ_DATA *	gobj_prev;

CHAR_DATA *	timechar;

char * corpse_descs[] =
   { 
     "The corpse of %s is in the last stages of decay.", 
     "The corpse of %s is crawling with vermin.",
     "The corpse of %s fills the air with a foul stench.",
     "The corpse of %s is buzzing with flies.",
     "The corpse of %s lies here."
   };

extern int      top_exit;

/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;

/*	save_char_obj( ch );*/
    sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
    set_title( ch, buf );

    add_hp	= con_app[get_curr_con(ch)].hitp + number_range(
		    class_table[ch->class]->hp_min,
		    class_table[ch->class]->hp_max );
    add_mana	= class_table[ch->class]->fMana
		    ? number_range(2, (2*get_curr_int(ch)+get_curr_wis(ch))/8)
		    : 0;
    add_move	= number_range( 5, (get_curr_con(ch)+get_curr_dex(ch))/4 );
    add_prac	= wis_app[get_curr_wis(ch)].practice;

    add_hp	= UMAX(  1, add_hp   );
    add_mana	= UMAX(  0, add_mana );
    add_move	= UMAX( 10, add_move );

    /* bonus for deadlies */
    if(IS_PKILL(ch)){
        add_mana = add_mana + add_mana*.3;
        add_move = add_move + add_move*.3;
        add_hp +=1; /* bitch at blod if you don't like this :) */
        sprintf(buf,"Ao's Pandect steels your sinews.\n\r");
    }

    ch->max_hit 	+= add_hp;
    ch->max_mana	+= add_mana;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;

    if ( !IS_NPC(ch) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    if ( ch->level == LEVEL_AVATAR )
    {
	DESCRIPTOR_DATA *d;

	sprintf( buf, "%s has just achieved Avatarhood!", ch->name );
	for ( d = first_descriptor; d; d = d->next )
	   if ( d->connected == CON_PLAYING && d->character != ch )
	   {
		set_char_color( AT_IMMORT, d->character );
		send_to_char( buf,	d->character );
		send_to_char( "\n\r",	d->character );
	   }
	set_char_color( AT_WHITE, ch );
	do_help( ch, "M_ADVHERO_" );
    }
    if ( ch->level < LEVEL_IMMORTAL )
    {
      if ( IS_VAMPIRE(ch) )
        sprintf( buf,
	  "Your gain is: %d/%d hp, %d/%d bp, %d/%d mv %d/%d prac.\n\r",
	  add_hp,	ch->max_hit,
	  1,	        ch->level + 10,
	  add_move,	ch->max_move,
	  add_prac,	ch->practice
	  );
      else
        sprintf( buf,
	  "Your gain is: %d/%d hp, %d/%d mana, %d/%d mv %d/%d prac.\n\r",
	  add_hp,	ch->max_hit,
 	  add_mana,	ch->max_mana,
	  add_move,	ch->max_move,
	  add_prac,	ch->practice
	  );
      set_char_color( AT_WHITE, ch );
      send_to_char( buf, ch );
      sprintf(buf1, "The ground shakes as %s becomes more powerful in this realm.", ch->name);
      do_info( ch, buf1);
      sound_to_all( AT_SAY, "!!SOUND(Tsr/Fanfare3 V=100)", ECHOTAR_ALL);
    }
    return;
}   



void gain_exp( CHAR_DATA *ch, int gain )
{
    int modgain;
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) || ch->level >= LEVEL_AVATAR )
	return;


    /* Bonus for deadly lowbies */
    modgain = gain;
    if(modgain>0 && IS_PKILL(ch) && ch->level<17){
       if(ch->level<=6){
          sprintf(buf,"The Favor of Ao fosters your learning.\n\r");
          modgain*=2;
       }
       if(ch->level<=10 && ch->level>=7){
          sprintf(buf,"The Hand of Ao hastens your learning.\n\r");
          modgain*=1.75;
       }
       if(ch->level<=13 && ch->level>=11){
          sprintf(buf,"The Cunning of Ao succors your learning.\n\r");
          modgain*=1.5;
       }
       if(ch->level<=16 && ch->level>=14){
          sprintf(buf,"The Patronage of Ao reinforces your learning.\n\r");
          modgain*=1.25;
       }
       if (IS_SOUND(ch))
         send_to_char("!!SOUND(Tsr/Fanfare2 V=100)\n\r", ch);
	send_to_char(buf, ch);
    }

    /* Deadly exp loss floor is exp floor of level */
    if(IS_PKILL(ch)&& modgain<0){
       if( ch->exp + modgain < exp_level(ch, ch->level)){
          modgain = exp_level(ch, ch->level) - ch->exp;
          sprintf(buf,"Ao's Pandect protects your insight.\n\r");
       }
    }

    ch->exp = UMAX( 0, ch->exp + modgain );

    if (NOT_AUTHED(ch) && ch->exp >= exp_level(ch, ch->level+1))
    {
	send_to_char("You can not ascend to a higher level until you are authorized.\n\r", ch);
	ch->exp = (exp_level(ch, (ch->level+1)) - 1);
	return;
    }

    while ( ch->level < LEVEL_AVATAR && ch->exp >= exp_level(ch, ch->level+1))
    {
	set_char_color( AT_WHITE + AT_BLINK, ch );
	ch_printf( ch, "You have now obtained experience level %d!\n\r", ++ch->level );
        if (IS_SOUND(ch))
         send_to_char("!!SOUND(Tsr/Fanfare1 V=100)\n\r", ch);

	advance_level( ch );
    }

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->level * 3 / 2;
    }
    else
    {
	gain = UMIN( 5, ch->level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -1;
	case POS_INCAP:    return -1;
	case POS_STUNNED:  return 1;
	case POS_SLEEPING: gain += get_curr_con(ch) * 1.5;	break;
	case POS_RESTING:  gain += get_curr_con(ch); 		break;
	}

        if ( IS_VAMPIRE(ch) ) {
            if ( ch->pcdata->condition[COND_BLOODTHIRST] <= 1 )
		gain /= 2;
	    else
	    if ( ch->pcdata->condition[COND_BLOODTHIRST] >= (8 + ch->level) )
                gain *= 2;
            if ( IS_OUTSIDE( ch ) )
  	    {
    	       switch(weather_info.sunlight)
    	       {
    	          case SUN_RISE:
    		  case SUN_SET:
      	            gain /= 2;
      	            break;
    		  case SUN_LIGHT:
      		    gain /= 4;
      		    break;
      	       }
      	    }
        }

	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMIN( 5, ch->level / 2 );

	if ( ch->position < POS_SLEEPING )
	  return 0;
	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_int(ch) * 3;	break;
	case POS_RESTING:  gain += get_curr_int(ch) * 1.5;	break;
	}

	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, 2 * ch->level );

	switch ( ch->position )
	{
	case POS_DEAD:	   return 0;
	case POS_MORTAL:   return -1;
	case POS_INCAP:    return -1;
	case POS_STUNNED:  return 1;
	case POS_SLEEPING: gain += get_curr_dex(ch) * 2;	break;
	case POS_RESTING:  gain += get_curr_dex(ch);		break;
	}

        if ( IS_VAMPIRE(ch) ) {
            if ( ch->pcdata->condition[COND_BLOODTHIRST] <= 1 )
		gain /= 2;
	    else
	    if ( ch->pcdata->condition[COND_BLOODTHIRST] >= (8 + ch->level) )
                gain *= 2;
            if ( IS_OUTSIDE( ch ) )
  	    {
    	       switch(weather_info.sunlight)
    	       {
    	          case SUN_RISE:
    		  case SUN_SET:
      	            gain /= 2;
      	            break;
    		  case SUN_LIGHT:
      		    gain /= 4;
      		    break;
      	       }
      	    }
        }

	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    return UMIN(gain, ch->max_move - ch->move);
}


void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;
    ch_ret retcode;

    if ( value == 0 || IS_NPC(ch) || ch->level >= LEVEL_IMMORTAL || NOT_AUTHED(ch))
	return;

    condition				= ch->pcdata->condition[iCond];
    if ( iCond == COND_BLOODTHIRST )
      ch->pcdata->condition[iCond]    = URANGE( 0, condition + value,
                                                10 + ch->level );
    else
      ch->pcdata->condition[iCond]    = URANGE( 0, condition + value, 48 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are STARVING!\n\r",  ch );
            act( AT_HUNGRY, "$n is starved half to death!", ch, NULL, NULL, TO_ROOM);
	    if ( !IS_PKILL(ch) || number_bits(1) == 0 )
		worsen_mental_state( ch, 1 );
	    retcode = damage(ch, ch, 1, TYPE_UNDEFINED);
          }
          break;

	case COND_THIRST:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You are DYING of THIRST!\n\r", ch );
            act( AT_THIRSTY, "$n is dying of thirst!", ch, NULL, NULL, TO_ROOM);
	    worsen_mental_state( ch, IS_PKILL(ch) ? 1: 2 );
	    retcode = damage(ch, ch, 2, TYPE_UNDEFINED);
          }
          break;

        case COND_BLOODTHIRST:
          if ( ch->level < LEVEL_AVATAR )
          {
            set_char_color( AT_BLOOD, ch );
            send_to_char( "You are starved to feast on blood!\n\r", ch );
            act( AT_BLOOD, "$n is suffering from lack of blood!", ch,
                 NULL, NULL, TO_ROOM);
	    worsen_mental_state( ch, 2 );
	    retcode = damage(ch, ch, ch->max_hit / 20, TYPE_UNDEFINED);
          }
          break;
	case COND_DRUNK:
	    if ( condition != 0 ) {
                set_char_color( AT_SOBER, ch );
		send_to_char( "You are sober.\n\r", ch );
	    }
	    retcode = rNONE;
	    break;
	default:
	    bug( "Gain_condition: invalid condition type %d", iCond );
	    retcode = rNONE;
	    break;
	}
    }

    if ( retcode != rNONE )
      return;

    if ( ch->pcdata->condition[iCond] == 1 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are really hungry.\n\r",  ch );
            act( AT_HUNGRY, "You can hear $n's stomach growling.", ch, NULL, NULL, TO_ROOM);
	    if ( number_bits(1) == 0 )
		worsen_mental_state( ch, 1 );
          } 
	  break;

	case COND_THIRST:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You are really thirsty.\n\r", ch );
	    worsen_mental_state( ch, 1 );
	    act( AT_THIRSTY, "$n looks a little parched.", ch, NULL, NULL, TO_ROOM);
          } 
	  break;

        case COND_BLOODTHIRST:
          if ( ch->level < LEVEL_AVATAR )
          {
            set_char_color( AT_BLOOD, ch );
            send_to_char( "You have a growing need to feast on blood!\n\r", ch );
            act( AT_BLOOD, "$n gets a strange look in $s eyes...", ch,
                 NULL, NULL, TO_ROOM);
	    worsen_mental_state( ch, 1 );
          }
          break;
	case COND_DRUNK:
	    if ( condition != 0 ) {
                set_char_color( AT_SOBER, ch );
		send_to_char( "You are feeling a little less light headed.\n\r", ch );
            }
	    break;
	}
    }


    if ( ch->pcdata->condition[iCond] == 2 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are hungry.\n\r",  ch );
          } 
	  break;

	case COND_THIRST:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You are thirsty.\n\r", ch );
          } 
	  break;

        case COND_BLOODTHIRST:
          if ( ch->level < LEVEL_AVATAR )
          {
            set_char_color( AT_BLOOD, ch );
            send_to_char( "You feel an urgent need for blood.\n\r", ch );
          }  
          break;
	}
    }

    if ( ch->pcdata->condition[iCond] == 3 )
    {
	switch ( iCond )
	{
	case COND_FULL:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_HUNGRY, ch );
	    send_to_char( "You are a mite peckish.\n\r",  ch );
          } 
	  break;

	case COND_THIRST:
          if ( ch->level < LEVEL_AVATAR && ch->class != CLASS_VAMPIRE )
          {
            set_char_color( AT_THIRSTY, ch );
	    send_to_char( "You could use a sip of something refreshing.\n\r", ch );
          } 
	  break;

        case COND_BLOODTHIRST:
          if ( ch->level < LEVEL_AVATAR )
          {
            set_char_color( AT_BLOOD, ch );
            send_to_char( "You feel an aching in your fangs.\n\r", ch );
          }
          break;
	}
    }
    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    int door;
    ch_ret     retcode;

    retcode = rNONE;

    /* Examine all mobs. */
    for ( ch = last_char; ch; ch = gch_prev )
    {
	set_cur_char( ch );
	if ( ch == first_char && ch->prev )
	{
	    bug( "mobile_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	  
	gch_prev = ch->prev;
	
	if ( gch_prev && gch_prev->next != ch )
	{
	    sprintf( buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.",
		ch->name );
	    bug( buf, 0 );	    
	    bug( "Short-cutting here", 0 );
	    gch_prev = NULL;
	    ch->prev = NULL;
	    do_shout( ch, "Ao says, 'Prepare for the worst!'" );
	}

	if ( !IS_NPC(ch) )
	{
	    drunk_randoms(ch);
	    halucinations(ch);
	    continue;
	}

	if ( !ch->in_room
	||   IS_AFFECTED(ch, AFF_CHARM)
	||   IS_AFFECTED(ch, AFF_PARALYSIS) )
	    continue;

/* Clean up 'animated corpses' that are not charmed' - Scryn */

        if ( ch->pIndexData->vnum == 5 && !IS_AFFECTED(ch, AFF_CHARM) )
	{
	  if(ch->in_room->first_person)
	    act(AT_MAGIC, "$n returns to the dust from whence $e came.", ch, NULL, NULL, TO_ROOM);
          
   	  if(IS_NPC(ch)) /* Guard against purging switched? */
	    extract_char(ch, TRUE);
	  continue;
	}

	if ( !IS_SET( ch->act, ACT_RUNNING )
	&&   !IS_SET( ch->act, ACT_SENTINEL )
	&&   !ch->fighting && ch->hunting )
	{
	  WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	  /* Commented out temporarily to avoid spam - Scryn 
	  sprintf( buf, "%s hunting %s from %s.", ch->name,
	  	ch->hunting->name,
		ch->in_room->name );
	  log_string( buf ); */
	  hunt_victim( ch );
	  continue;
	}  

	/* Examine call for special procedure */
	if ( !IS_SET( ch->act, ACT_RUNNING )
	&&    ch->spec_fun )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	    if ( char_died(ch) )
		continue;
	}

	/* Check for mudprogram script on mob */
	if ( IS_SET( ch->pIndexData->progtypes, SCRIPT_PROG ) )
	{
	    mprog_script_trigger( ch );
	    continue;
	}

	if ( ch != cur_char )
	{
	    bug( "Mobile_update: ch != cur_char after spec_fun", 0 );
	    continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;

	if ( IS_SET(ch->act, ACT_MOUNTED ) )
	{
	    if ( IS_SET(ch->act, ACT_AGGRESSIVE) )
		do_emote( ch, "snarls and growls." );
	    continue;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE )
	&&   IS_SET(ch->act, ACT_AGGRESSIVE) )
	    do_emote( ch, "glares around and snarls." );


	/* MOBprogram random trigger */
	if ( ch->in_room->area->nplayer > 0 )
	{
	    mprog_random_trigger( ch );
	    if ( char_died(ch) )
		continue;
	    if ( ch->position < POS_STANDING )
	        continue;
	}

        /* MOBprogram hour trigger: do something for an hour */
        mprog_hour_trigger(ch);

	if ( char_died(ch) )
	  continue;

	rprog_hour_trigger(ch);
	if ( char_died(ch) )
	  continue;

	if ( ch->position < POS_STANDING )
	  continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->first_content
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = NULL;
	    for ( obj = ch->in_room->first_content; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max 
		&& !IS_OBJ_STAT( obj, ITEM_BURRIED ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( AT_ACTION, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_RUNNING)
	&&   !IS_SET(ch->act, ACT_SENTINEL)
	&&   !IS_SET(ch->act, ACT_PROTOTYPE)
	&& ( door = number_bits( 5 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room, door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_DEATH)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    retcode = move_char( ch, pexit, 0 );
						/* If ch changes position due
						to it's or someother mob's
						movement via MOBProgs,
						continue - Kahn */
	    if ( char_died(ch) )
	      continue;
	    if ( retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL)
	    ||    ch->position < POS_STANDING )
	        continue;
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 4 ) ) <= 9
	&& ( pexit = get_exit(ch->in_room,door) ) != NULL
	&&   pexit->to_room
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = ch->in_room->first_person;
		  rch;
		  rch  = rch->next_in_room )
	    {
		if ( is_fearing(ch, rch) )
		{
		    switch( number_bits(2) )
		    {
			case 0:
			  sprintf( buf, "Get away from me, %s!", rch->name );
			  break;
			case 1:
			  sprintf( buf, "Leave me be, %s!", rch->name );
			  break;
			case 2:
			  sprintf( buf, "%s is trying to kill me!  Help!", rch->name );
			  break;
			case 3:
			  sprintf( buf, "Someone save me from %s!", rch->name );
			  break;
		    }
		    do_yell( ch, buf );
		    found = TRUE;
		    break;
		}
	    }
	    if ( found )
		retcode = move_char( ch, pexit, 0 );
	}
    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;
    sh_int AT_TEMP = AT_PLAIN;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun." );
        sound_to_all( AT_YELLOW, "!!SOUND(Tsr/Fallow V=100)", ECHOTAR_ALL);
        AT_TEMP = AT_YELLOW;
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east." );
        AT_TEMP = AT_ORANGE;
	break;

    case 12:
        weather_info.sunlight = SUN_LIGHT;
        strcat( buf, "It's noon." ); 
        sound_to_all( AT_YELLOW, "!!SOUND(Tsr/Bellrng V=100)", ECHOTAR_ALL);
        AT_TEMP = AT_YELLOW;
   	break;
	
    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west." );
        AT_TEMP = AT_BLOOD;
  	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun." );
        sound_to_all( AT_YELLOW, "!!SOUND(Tsr/crickets V=100)", ECHOTAR_ALL);
        AT_TEMP = AT_DGREY;
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;
    }

    if ( time_info.day   >= 30 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = first_descriptor; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		act( AT_TEMP, buf, d->character, 0, 0, TO_CHAR );
	}
        buf[0] = '\0';
    }
    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    AT_TEMP = AT_GREY;
    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy." );
            sound_to_all( AT_YELLOW, "!!SOUND(Tsr/wind3 V=100)", ECHOTAR_ALL);
	    weather_info.sky = SKY_CLOUDY;
            AT_TEMP = AT_GREY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain." );
            sound_to_all( AT_YELLOW, "!!SOUND(Tsr/rain V=100)", ECHOTAR_ALL);
	    weather_info.sky = SKY_RAINING;
            AT_TEMP = AT_BLUE;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear." );
	    weather_info.sky = SKY_CLOUDLESS;
            AT_TEMP = AT_WHITE;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky." );
            sound_to_all( AT_YELLOW, "!!SOUND(Tsr/thunder3 V=100)",ECHOTAR_ALL);
	    weather_info.sky = SKY_LIGHTNING;
	    AT_TEMP = AT_YELLOW;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped." );
	    weather_info.sky = SKY_CLOUDY;
	    AT_TEMP = AT_WHITE;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped." );
	    weather_info.sky = SKY_RAINING;
            AT_TEMP = AT_GREY;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = first_descriptor; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		act( AT_TEMP, buf, d->character, 0, 0, TO_CHAR );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_save;
    sh_int save_count = 0;

    ch_save	= NULL;
    for ( ch = last_char; ch; ch = gch_prev )
    {
	if ( ch == first_char && ch->prev )
	{
	    bug( "char_update: first_char->prev != NULL... fixed", 0 );
	    ch->prev = NULL;
	}
	gch_prev = ch->prev;
	set_cur_char( ch );
	if ( gch_prev && gch_prev->next != ch )
	{
	    bug( "char_update: ch->prev->next != ch", 0 );
	    return;
	}

        /*
	 *  Do a room_prog rand check right off the bat
	 *   if ch disappears (rprog might wax npc's), continue
	 */
	if(!IS_NPC(ch))
	    rprog_random_trigger( ch );

	if( char_died(ch) )
	    continue;

	if(IS_NPC(ch))
	    mprog_time_trigger(ch);   

	if( char_died(ch) )
	    continue;

	rprog_time_trigger(ch);

	if( char_died(ch) )
	    continue;

	/*
	 * See if player should be auto-saved.
	 */
	if ( !IS_NPC(ch)
	&& ( !ch->desc || ch->desc->connected == CON_PLAYING )
	&&    ch->level >= 2
	&&    current_time - ch->save_time > (sysdata.save_frequency*60) )
	    ch_save	= ch;
	else
	    ch_save	= NULL;

	if ( ch->position >= POS_STUNNED )
	{
	    if ( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain(ch);

	    if ( ch->mana < ch->max_mana )
		ch->mana += mana_gain(ch);

	    if ( ch->move < ch->max_move )
		ch->move += move_gain(ch);
	}

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room )
		{
		    ch->in_room->light -= obj->count;
		    act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( AT_ACTION, "$p goes out.", ch, obj, NULL, TO_CHAR );
		    if ( obj->serial == cur_obj )
		      global_objcode = rOBJ_EXPIRED;
		    extract_obj( obj );
		}
	    }

	    if ( ++ch->timer >= 12 )
	    {
		if ( !ch->was_in_room && ch->in_room )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting )
			stop_fighting( ch, TRUE );
		    act( AT_ACTION, "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    if ( IS_SET( sysdata.save_flags, SV_IDLE ) )
			save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( ch->pcdata->condition[COND_DRUNK] > 8 )
		worsen_mental_state( ch, ch->pcdata->condition[COND_DRUNK]/8 );
	    if ( ch->pcdata->condition[COND_FULL] > 1 )
	    {
		switch( ch->position )
		{
		    case POS_SLEEPING:  better_mental_state( ch, 4 );	break;
		    case POS_RESTING:   better_mental_state( ch, 3 );	break;
		    case POS_SITTING:
		    case POS_MOUNTED:   better_mental_state( ch, 2 );	break;
		    case POS_STANDING:  better_mental_state( ch, 1 );	break;
		    case POS_FIGHTING:
			if ( number_bits(2) == 0 )
			    better_mental_state( ch, 1 );
			break;
		}
	    }
	    if ( ch->pcdata->condition[COND_THIRST] > 1 )
	    {
		switch( ch->position )
		{
		    case POS_SLEEPING:  better_mental_state( ch, 5 );	break;
		    case POS_RESTING:   better_mental_state( ch, 3 );	break;
		    case POS_SITTING:
		    case POS_MOUNTED:   better_mental_state( ch, 2 );	break;
		    case POS_STANDING:  better_mental_state( ch, 1 );	break;
		    case POS_FIGHTING:
			if ( number_bits(2) == 0 )
			    better_mental_state( ch, 1 );
			break;
		}
	    }
	    gain_condition( ch, COND_DRUNK,  -1 );
	    gain_condition( ch, COND_FULL,   -1 );
            if ( ch->class == CLASS_VAMPIRE && ch->level >= 10 )
	    {
		if ( time_info.hour < 21 && time_info.hour > 5 )
		  gain_condition( ch, COND_BLOODTHIRST, -1 );
	    }
	    if ( ch->in_room )
	      switch( ch->in_room->sector_type )
	      {
		default:
		    gain_condition( ch, COND_THIRST, -1 );  break;
		case SECT_DESERT:
		    gain_condition( ch, COND_THIRST, -2 );  break;
		case SECT_UNDERWATER:
		case SECT_OCEANFLOOR:
		    if ( number_bits(1) == 0 )
			gain_condition( ch, COND_THIRST, -1 );  break;
	      }

	}

	if ( !char_died(ch) )
	{
	    /*
	     * Careful with the damages here,
	     *   MUST NOT refer to ch after damage taken,
	     *   as it may be lethal damage (on NPC).
	     */
	    if ( IS_AFFECTED(ch, AFF_POISON) )
	    {
		act( AT_POISON, "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
		act( AT_POISON, "You shiver and suffer.", ch, NULL, NULL, TO_CHAR );
		ch->mental_state = URANGE( 20, ch->mental_state
				 + (IS_PKILL(ch) ? 3 : 4), 100 );
		damage( ch, ch, 6, gsn_poison );
	    }
	    else
	    if ( ch->position == POS_INCAP )
		damage( ch, ch, 1, TYPE_UNDEFINED );
	    else
	    if ( ch->position == POS_MORTAL )
		damage( ch, ch, 4, TYPE_UNDEFINED );
	    if ( char_died(ch) )
		continue;
	    if ( ch->mental_state >= 30 )
		switch( (ch->mental_state+5) / 10 )
		{
		    case  3:
		    	send_to_char( "You feel feverish.\n\r", ch );
			act( AT_ACTION, "$n looks kind of out of it.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  4:
		    	send_to_char( "You do not feel well at all.\n\r", ch );
			act( AT_ACTION, "$n doesn't look too good.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  5:
		    	send_to_char( "You need help!\n\r", ch );
			act( AT_ACTION, "$n looks like $e could use your help.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  6:
		    	send_to_char( "Seekest thou a cleric.\n\r", ch );
			act( AT_ACTION, "Someone should fetch a healer for $n.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  7:
		    	send_to_char( "You feel reality slipping away...\n\r", ch );
			act( AT_ACTION, "$n doesn't appear to be aware of what's going on.", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  8:
		    	send_to_char( "You begin to understand... everything.\n\r", ch );
			act( AT_ACTION, "$n starts ranting like a madman!", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case  9:
		    	send_to_char( "You are ONE with the universe.\n\r", ch );
			act( AT_ACTION, "$n is ranting on about 'the answer', 'ONE' and other mumbo-jumbo...", ch, NULL, NULL, TO_ROOM );
		    	break;
		    case 10:
		    	send_to_char( "You feel the end is near.\n\r", ch );
			act( AT_ACTION, "$n is muttering and ranting in tongues...", ch, NULL, NULL, TO_ROOM );
		    	break;
		}
	    if ( ch->mental_state <= -30 )
		switch( (abs(ch->mental_state)+5) / 10 )
		{
		    case  10:
			if ( ch->position > POS_SLEEPING )
			{
			   if ( (ch->position == POS_STANDING
			   ||    ch->position < POS_FIGHTING)
			   &&    number_percent()+10 < abs(ch->mental_state) )
				do_sleep( ch, "" );
			   else
				send_to_char( "You're barely conscious.\n\r", ch );
			}
			break;
		    case   9:
			if ( ch->position > POS_SLEEPING )
			{
			   if ( (ch->position == POS_STANDING
			   ||    ch->position < POS_FIGHTING)
			   &&   (number_percent()+20) < abs(ch->mental_state) )
				do_sleep( ch, "" );
			   else
				send_to_char( "You can barely keep your eyes open.\n\r", ch );
			}
			break;
		    case   8:
			if ( ch->position > POS_SLEEPING )
			{
			   if ( ch->position < POS_SITTING
			   &&  (number_percent()+30) < abs(ch->mental_state) )
				do_sleep( ch, "" );
			   else
				send_to_char( "You're extremely drowsy.\n\r", ch );
			}
			break;
		    case   7:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel very unmotivated.\n\r", ch );
			break;
		    case   6:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel sedated.\n\r", ch );
			break;
		    case   5:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel sleepy.\n\r", ch );
			break;
		    case   4:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You feel tired.\n\r", ch );
			break;
		    case   3:
			if ( ch->position > POS_RESTING )
			   send_to_char( "You could use a rest.\n\r", ch );
			break;
		}
	    if ( ch->timer > 24 )
		do_quit( ch, "" );
	    else
	    if ( ch == ch_save && IS_SET( sysdata.save_flags, SV_AUTO )
	    &&   ++save_count < 10 )	/* save max of 10 per tick */
		save_char_obj( ch );
	}
    }

    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    sh_int AT_TEMP;

    for ( obj = last_object; obj; obj = gobj_prev )
    {
	CHAR_DATA *rch;
	char *message;

	if ( obj == first_object && obj->prev )
	{
	    bug( "obj_update: first_object->prev != NULL... fixed", 0 );
	    obj->prev = NULL;
	}
	gobj_prev = obj->prev;
	if ( gobj_prev && gobj_prev->next != obj )
	{
	    bug( "obj_update: obj->prev->next != obj", 0 );
	    return;
	}
	set_cur_obj( obj );
	if ( obj->carried_by )
	  oprog_random_trigger( obj ); 
	else
	if( obj->in_room && obj->in_room->area->nplayer > 0 )
	  oprog_random_trigger( obj ); 

        if( obj_extracted(obj) )
	  continue;

	if ( obj->item_type == ITEM_PIPE )
	{
	    if ( IS_SET( obj->value[3], PIPE_LIT ) )
	    {
		if ( --obj->value[1] <= 0 )
		{
		  obj->value[1] = 0;
		  REMOVE_BIT( obj->value[3], PIPE_LIT );
		}
		else
		if ( IS_SET( obj->value[3], PIPE_HOT ) )
		  REMOVE_BIT( obj->value[3], PIPE_HOT );
		else
		{
		  if ( IS_SET( obj->value[3], PIPE_GOINGOUT ) )
		  {
		    REMOVE_BIT( obj->value[3], PIPE_LIT );
		    REMOVE_BIT( obj->value[3], PIPE_GOINGOUT );
		  }
		  else
		    SET_BIT( obj->value[3], PIPE_GOINGOUT );
		}
		if ( !IS_SET( obj->value[3], PIPE_LIT ) )
		  SET_BIT( obj->value[3], PIPE_FULLOFASH );
	    }
	    else
	      REMOVE_BIT( obj->value[3], PIPE_HOT );
	}


/* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

        if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
        {
          sh_int timerfrac = UMAX(1, obj->timer - 1);
          if ( obj->item_type == ITEM_CORPSE_PC )
	    timerfrac = (int)(obj->timer / 8 + 1);

	  if ( obj->timer > 0 && obj->value[2] > timerfrac )
	  {
            char buf[MAX_STRING_LENGTH];
            char name[MAX_STRING_LENGTH];
            char *bufptr;
            bufptr = one_argument( obj->short_descr, name ); 
            bufptr = one_argument( bufptr, name ); 
            bufptr = one_argument( bufptr, name ); 

	    separate_obj(obj);
            obj->value[2] = timerfrac; 
            sprintf( buf, corpse_descs[ UMIN( timerfrac - 1, 4 ) ], 
                          capitalize( bufptr ) ); 

            STRFREE( obj->description );
            obj->description = STRALLOC( buf ); 
          }  
        }
       
	/* don't let inventory decay */
	if ( IS_OBJ_STAT(obj, ITEM_INVENTORY) )
	  continue;

	if ( ( obj->timer <= 0 || --obj->timer > 0 ) )
	  continue;
	
	/* if we get this far, object's timer has expired. */
 
         AT_TEMP = AT_PLAIN;
	 switch ( obj->item_type )
	 {
	 default:
	   message = "$p mysteriously vanishes.";
           AT_TEMP = AT_PLAIN;
	   break;
	 case ITEM_PORTAL:
	   message = "$p winks out of existence.";
           remove_portal(obj);
	   obj->item_type = ITEM_TRASH;		/* so extract_obj	 */
           AT_TEMP = AT_MAGIC;			/* doesn't remove_portal */
	   break;
	 case ITEM_FOUNTAIN:
	   message = "$p dries up.";
           AT_TEMP = AT_BLUE;
	   break;
	 case ITEM_CORPSE_NPC:
	   message = "$p decays into dust and blows away.";
           AT_TEMP = AT_OBJECT;
	   break;
	 case ITEM_CORPSE_PC:
	   message = "$p is sucked into a swirling vortex of colors...";
           AT_TEMP = AT_MAGIC;
	   break;
	 case ITEM_FOOD:
	   message = "$p is devoured by a swarm of maggots.";
           AT_TEMP = AT_HUNGRY;
	   break;
         case ITEM_BLOOD:
           message = "$p slowly seeps into the ground.";
           AT_TEMP = AT_BLOOD;
           break;
         case ITEM_BLOODSTAIN:
           message = "$p dries up into flakes and blows away.";
           AT_TEMP = AT_BLOOD;
	   break;
         case ITEM_SCRAPS:
           message = "$p crumbles and decays into nothing.";
           AT_TEMP = AT_OBJECT;
	   break;
	 case ITEM_FIRE:
	   if (obj->in_room)
	     --obj->in_room->light;
	   message = "$p burns out.";
	   AT_TEMP = AT_FIRE;
	 }

	if ( obj->carried_by )
	{
	    act( AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room
	&&      ( rch = obj->in_room->first_person ) != NULL
	&&	!IS_OBJ_STAT( obj, ITEM_BURRIED ) )
	{
	    act( AT_TEMP, message, rch, obj, NULL, TO_ROOM );
            act( AT_TEMP, message, rch, obj, NULL, TO_CHAR );
	}

	if ( obj->serial == cur_obj )
	  global_objcode = rOBJ_EXPIRED;
	extract_obj( obj );
    }
    return;
}


/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check( void )
{
    CHAR_DATA *ch, *ch_next;
    OBJ_DATA *obj;
    EXIT_DATA *pexit;
    static int cnt = 0;
    int door, retcode;

    cnt = (cnt+1) % 2;

    for ( ch = first_char; ch; ch = ch_next )
    {
	set_cur_char(ch);
	ch_next = ch->next;
	will_fall(ch, 0);

	if ( char_died( ch ) )
	  continue;

	if ( IS_NPC( ch ) )
	{
	    if ( cnt != 0 )
		continue;

	    /* running mobs	-Thoric */
	    if ( IS_SET(ch->act, ACT_RUNNING) )
	    {
		if ( !IS_SET( ch->act, ACT_SENTINEL )
		&&   !ch->fighting && ch->hunting )
		{
		    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
		    hunt_victim( ch );
		    continue;
		}

		if ( ch->spec_fun )
		{
		    if ( (*ch->spec_fun) ( ch ) )
			continue;
		    if ( char_died(ch) )
			continue;
		}

		if ( !IS_SET(ch->act, ACT_SENTINEL)
		&&   !IS_SET(ch->act, ACT_PROTOTYPE)
		&& ( door = number_bits( 4 ) ) <= 9
		&& ( pexit = get_exit(ch->in_room, door) ) != NULL
		&&   pexit->to_room
		&&   !IS_SET(pexit->exit_info, EX_CLOSED)
		&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		&&   !IS_SET(pexit->to_room->room_flags, ROOM_DEATH)
		&& ( !IS_SET(ch->act, ACT_STAY_AREA)
		||   pexit->to_room->area == ch->in_room->area ) )
		{
		    retcode = move_char( ch, pexit, 0 );
		    if ( char_died(ch) )
			continue;
		    if ( retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL)
		    ||    ch->position < POS_STANDING )
			continue;
		}
	    }
	    continue;
	}
	else
	{
	    if ( ch->mount
	    &&   ch->in_room != ch->mount->in_room )
	    {
		REMOVE_BIT( ch->mount->act, ACT_MOUNTED );
		ch->mount = NULL;
		ch->position = POS_STANDING;
		send_to_char( "No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch );
	    }

	    if ( ( ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER )
	    || ( ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR ) )
	    {
		if ( !IS_AFFECTED( ch, AFF_AQUA_BREATH ) )
		{
		    if ( ch->level < LEVEL_IMMORTAL )
		    {
			int dam;

        		/* Changed level of damage at Brittany's request. -- Narn */	
			dam = number_range( ch->max_hit / 100, ch->max_hit / 50 );
			dam = UMAX( 1, dam );
			if ( number_bits(3) == 0 )
			  send_to_char( "You cough and choke as you try to breathe water!\n\r", ch );
			damage( ch, ch, dam, TYPE_UNDEFINED );
		    }
		}
	    }
	
	    if ( char_died( ch ) )
		continue; 

	    if ( ch->in_room
	    && (( ch->in_room->sector_type == SECT_WATER_NOSWIM )
	    ||  ( ch->in_room->sector_type == SECT_WATER_SWIM ) ) )
	    {
		if ( !IS_AFFECTED( ch, AFF_FLYING )
		&& !IS_AFFECTED( ch, AFF_FLOATING ) 
		&& !IS_AFFECTED( ch, AFF_AQUA_BREATH )
		&& !ch->mount )
		{
		    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
		      if ( obj->item_type == ITEM_BOAT ) break;

		    if ( !obj )
		    {
			if ( ch->level < LEVEL_IMMORTAL )
			{
			    int mov;
			    int dam;

			    if ( ch->move > 0 )
			    {
				mov = number_range( ch->max_move / 20, ch->max_move / 5 );
				mov = UMAX( 1, mov );

				if ( ch->move - mov < 0 )
				    ch->move = 0;
				else
				    ch->move -= mov;
			    }
			    else
			    {
				dam = number_range( ch->max_hit / 20, ch->max_hit / 5 );
				dam = UMAX( 1, dam );

				if ( number_bits(3) == 0 )
				   send_to_char( "Struggling with exhaustion, you choke on a mouthful of water.\n\r", ch );
				damage( ch, ch, dam, TYPE_UNDEFINED );
			    }
			}
		    }
		}
	    }

	    /* beat up on link dead players */
	    if ( !ch->desc )
	    {
		CHAR_DATA *wch, *wch_next;

		for ( wch = ch->in_room->first_person; wch; wch = wch_next )
		{
		    wch_next	= wch->next_in_room;

		    if (!IS_NPC(wch)
		    ||   wch->fighting
		    ||   IS_AFFECTED(wch, AFF_CHARM)
		    ||   !IS_AWAKE(wch)
		    || ( IS_SET(wch->act, ACT_WIMPY) && IS_AWAKE(ch) )
		    ||   !can_see( wch, ch ) )
			continue;

		    if ( is_hating( wch, ch ) )
		    {
			found_prey( wch, ch );
			continue;
		    }

		    if ( !IS_SET(wch->act, ACT_AGGRESSIVE)
		    ||    IS_SET(wch->act, ACT_MOUNTED)
		    ||    IS_SET(wch->in_room->room_flags, ROOM_SAFE ) )
			continue;
		    global_retcode = multi_hit( wch, ch, TYPE_UNDEFINED );
		}
	    }
	}
    }
}


/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update( void )
{
    DESCRIPTOR_DATA *d, *dnext;
    CHAR_DATA *wch;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;
    struct act_prog_data *apdtmp;

#ifdef UNDEFD
  /*
   *  GRUNT!  To do
   *
   */
        if ( IS_NPC( wch ) && wch->mpactnum > 0
	    && wch->in_room->area->nplayer > 0 )
        {
            MPROG_ACT_LIST * tmp_act, *tmp2_act;
	    for ( tmp_act = wch->mpact; tmp_act;
		 tmp_act = tmp_act->next )
	    {
                 oprog_wordlist_check( tmp_act->buf,wch, tmp_act->ch,
				      tmp_act->obj, tmp_act->vo, ACT_PROG );
                 DISPOSE( tmp_act->buf );
            }
	    for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
	    {
                 tmp2_act = tmp_act->next;
                 DISPOSE( tmp_act );
            }
            wch->mpactnum = 0;
            wch->mpact    = NULL;
        }
#endif

    /* check mobprog act queue */
    while ( (apdtmp = mob_act_list) != NULL )
    {
	wch = mob_act_list->vo;
	if ( !char_died(wch) && wch->mpactnum > 0 )
	{
	    MPROG_ACT_LIST * tmp_act;

	    while ( (tmp_act = wch->mpact) != NULL )
	    {
		if ( tmp_act->obj && obj_extracted(tmp_act->obj) )
		  tmp_act->obj = NULL;
		if ( tmp_act->ch && !char_died(tmp_act->ch) )
		  mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch,
					tmp_act->obj, tmp_act->vo, ACT_PROG );
		wch->mpact = tmp_act->next;
		DISPOSE(tmp_act->buf);
		DISPOSE(tmp_act);
	    }
	    wch->mpactnum = 0;
	    wch->mpact    = NULL;
        }
	mob_act_list = apdtmp->next;
	DISPOSE( apdtmp );
    }


    /*
     * Just check descriptors here for victims to aggressive mobs
     * We can check for linkdead victims to mobile_update	-Thoric
     */
    for ( d = first_descriptor; d; d = dnext )
    {
	dnext = d->next;
	if ( d->connected != CON_PLAYING || (wch=d->character) == NULL )
	   continue;

	if ( char_died(wch)
	||   IS_NPC(wch)
	||   wch->level >= LEVEL_IMMORTAL
	||  !wch->in_room )
	    continue;

	for ( ch = wch->in_room->first_person; ch; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   ch->fighting
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch ) )
		continue;

	    if ( is_hating( ch, wch ) )
	    {
		found_prey( ch, wch );
		continue;
	    }

	    if ( !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||    IS_SET(ch->act, ACT_MOUNTED)
	    ||    IS_SET(ch->in_room->room_flags, ROOM_SAFE ) )
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->first_person; vch; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( !IS_NPC(vch)
		&&   vch->level < LEVEL_IMMORTAL
		&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( !victim )
	    {
		bug( "Aggr_update: null victim.", count );
		continue;
	    }

	    if ( IS_NPC(ch) && IS_SET(ch->attacks, ATCK_BACKSTAB ) )
	    {
		OBJ_DATA *obj;

		if ( !ch->mount
    		&& (obj = get_eq_char( ch, WEAR_WIELD )) != NULL
    		&& obj->value[3] == 11
		&& !victim->fighting
		&& victim->hit >= victim->max_hit )
		{
		    check_attacker( ch, victim );
		    WAIT_STATE( ch, skill_table[gsn_backstab]->beats );
		    if ( !IS_AWAKE(victim)
		    ||   number_percent( )+5 < ch->level )
		    {
			global_retcode = multi_hit( ch, victim, gsn_backstab );
			continue;
		    }
		    else
		    {
			global_retcode = damage( ch, victim, 0, gsn_backstab );
			continue;
		    }
		}
	    }
	    global_retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}

/* From interp.c */
bool check_social  args( ( CHAR_DATA *ch, char *command, char *argument ) );

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms( CHAR_DATA *ch )
{
    CHAR_DATA *rvch = NULL;
    CHAR_DATA *vch;
    sh_int drunk;
    sh_int position;

    if ( IS_NPC( ch ) || ch->pcdata->condition[COND_DRUNK] <= 0 )
	return;

    if ( number_percent() < 30 )
	return;

    drunk = ch->pcdata->condition[COND_DRUNK];
    position = ch->position;
    ch->position = POS_STANDING;

    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "burp", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "hiccup", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "drool", "" );
    else
    if ( number_percent() < (2*drunk / 20) )
	check_social( ch, "fart", "" );
    else
    if ( drunk > (10+(get_curr_con(ch)/5))
    &&   number_percent() < ( 2 * drunk / 18 ) )
    {
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	    if ( number_percent() < 10 )
		rvch = vch;
	check_social( ch, "puke", (rvch ? rvch->name : "") );
    }

    ch->position = position;
    return;
}

void halucinations( CHAR_DATA *ch )
{
    if ( ch->mental_state >= 30 && number_bits(5 - (ch->mental_state >= 50) - (ch->mental_state >= 75)) == 0 )
    {
	char *t;

	switch( number_range( 1, UMIN(20, (ch->mental_state+5) / 5)) )
	{
	    default:
	    case  1: t = "You feel very restless... you can't sit still.\n\r";		break;
	    case  2: t = "You're tingling all over.\n\r";				break;
	    case  3: t = "Your skin is crawling.\n\r";					break;
	    case  4: t = "You suddenly feel that something is terribly wrong.\n\r";	break;
	    case  5: t = "Those damn little fairies keep laughing at you!\n\r";		break;
	    case  6: t = "You can hear your mother crying...\n\r";			break;
	    case  7: t = "Have you been here before, or not?  You're not sure...\n\r";	break;
	    case  8: t = "Painful childhood memories flash through your mind.\n\r";	break;
	    case  9: t = "You hear someone call your name in the distance...\n\r";	break;
	    case 10: t = "Your head is pulsating... you can't think straight.\n\r";	break;
	    case 11: t = "The ground... seems to be squirming...\n\r";			break;
	    case 12: t = "You're not quite sure what is real anymore.\n\r";		break;
	    case 13: t = "It's all a dream... or is it?\n\r";				break;
	    case 14: t = "They're coming to get you... coming to take you away...\n\r";	break;
	    case 15: t = "You begin to feel all powerful!\n\r";				break;
	    case 16: t = "You're light as air... the heavens are yours for the taking.\n\r";	break;
	    case 17: t = "Your whole life flashes by... and your future...\n\r";	break;
	    case 18: t = "You are everywhere and everything... you know all and are all!\n\r";	break;
	    case 19: t = "You feel immortal!\n\r";					break;
	    case 20: t = "Ahh... the power of a Supreme Entity... what to do...\n\r";	break;
	}
	send_to_char( t, ch );
    }
    return;
}

void tele_update( void )
{
    TELEPORT_DATA *tele, *tele_next;

    if ( !first_teleport )
      return;
    
    for ( tele = first_teleport; tele; tele = tele_next )
    {
	tele_next = tele->next;
	if ( --tele->timer <= 0 )
	{
	    if ( tele->room->first_person )
	    {
		if ( IS_SET( tele->room->room_flags, ROOM_TELESHOWDESC ) )
		  teleport( tele->room->first_person, tele->room->tele_vnum,
			TELE_SHOWDESC | TELE_TRANSALL );
		else
		  teleport( tele->room->first_person, tele->room->tele_vnum,
			TELE_TRANSALL );
	    }
	    UNLINK( tele, first_teleport, last_teleport, next, prev );
	    DISPOSE( tele );
	}
    }
}

#if FALSE
/* 
 * Write all outstanding authorization requests to Log channel - Gorog
 */ 
void auth_update( void ) 
{ 
  CHAR_DATA *victim; 
  DESCRIPTOR_DATA *d; 
  char log_buf [MAX_INPUT_LENGTH];
  bool first_time = TRUE;         /* so titles are only done once */

  for ( d = first_descriptor; d; d = d->next ) 
      {
      victim = d->character;
      if ( victim && IS_WAITING_FOR_AUTH(victim) )
         {
         if ( first_time )
            {
            first_time = FALSE;
            strcpy (log_buf, "Pending authorizations:" ); 
            wiznet( log_buf, NULL, NULL, WIZ_NEWBIE, 0, 0);
            }
         sprintf( log_buf, " %s@%s new %s %s", victim->name,
            victim->desc->host, race_table[victim->race].race_name, 
            class_table[victim->class]->who_name ); 
         wiznet( log_buf, NULL, NULL, WIZ_NEWBIE, 0, 0);
         }
      }
} 
#endif

void auth_update( void ) 
{ 
    CHAR_DATA *victim; 
    DESCRIPTOR_DATA *d; 
    char buf [MAX_INPUT_LENGTH], log_buf [MAX_INPUT_LENGTH];
    bool found_hit = FALSE;         /* was at least one found? */

    strcpy (log_buf, "Pending authorizations:\n\r" );
    for ( d = first_descriptor; d; d = d->next ) 
    {
	if ( (victim = d->character) && IS_WAITING_FOR_AUTH(victim) )
	{
	    found_hit = TRUE;
	    sprintf( buf, " %s@%s new %s %s\n\r", victim->name,
		victim->desc->host, race_table[victim->race].race_name, 
		class_table[victim->class]->who_name );
	    strcat (log_buf, buf);
	}
    }
    if (found_hit)
    {
	log_string( log_buf ); 
	wiznet( log_buf, NULL, NULL, WIZ_NEWBIE, 0, 0);
    }
} 

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int	    pulse_second;
    struct timeval stime;
    struct timeval etime;

    if ( timechar )
    {
      set_char_color(AT_PLAIN, timechar);
      send_to_char( "Starting update timer.\n\r", timechar );
      gettimeofday(&stime, NULL);
    }
    
    if ( --pulse_area     <= 0 )
    {
	pulse_area	= PULSE_AREA;
        quest_update    ( );
	area_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update  ( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK * 0.75, PULSE_TICK * 1.25 );

        auth_update     ( );			/* Gorog */
	weather_update	( );
	char_update	( );
	obj_update	( );
	clear_vrooms	( );			/* remove virtual rooms */
    }

    if ( --pulse_second   <= 0 )
    {
	pulse_second	= PULSE_PER_SECOND;
	char_check( );
 	/*reboot_check( "" ); Disabled to check if its lagging a lot - Scryn*/
 	/* Much faster version enabled by Altrag..
 	   although I dunno how it could lag too much, it was just a bunch
 	   of comparisons.. */
 	reboot_check(0);
    }

    if ( auction->item && --auction->pulse <= 0 )
    {                                                  
	auction->pulse = PULSE_AUCTION;                     
	auction_update( );
    }

    tele_update( );
    aggr_update( );
    obj_act_update ( );
    room_act_update( );
    clean_obj_queue();		/* dispose of extracted objects */
    clean_char_queue();		/* dispose of dead mobs/quitting chars */
    if ( timechar )
    {
      gettimeofday(&etime, NULL);
      set_char_color(AT_PLAIN, timechar);
      send_to_char( "Update timing complete.\n\r", timechar );
      subtract_times(&etime, &stime);
      ch_printf( timechar, "Timing took %d.%06d seconds.\n\r",
          etime.tv_sec, etime.tv_usec );
      timechar = NULL;
    }
    tail_chain( );
    return;
}


void remove_portal( OBJ_DATA *portal )
{
    ROOM_INDEX_DATA *fromRoom, *toRoom;
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    bool found;

    if ( !portal )
    {
	bug( "remove_portal: portal is NULL", 0 );
	return;
    }

    fromRoom = portal->in_room;
    found = FALSE;
    if ( !fromRoom )
    {
	bug( "remove_portal: portal->in_room is NULL", 0 );
	return;
    }

    for ( pexit = fromRoom->first_exit; pexit; pexit = pexit->next )
	if ( IS_SET( pexit->exit_info, EX_PORTAL ) )
	{
	    found = TRUE;
	    break;
	}

    if ( !found )
    {
	bug( "remove_portal: portal not found in room %d!", fromRoom->vnum );
	return;
    }

    if ( pexit->vdir != DIR_PORTAL )
	bug( "remove_portal: exit in dir %d != DIR_PORTAL", pexit->vdir );

    if ( ( toRoom = pexit->to_room ) == NULL )
      bug( "remove_portal: toRoom is NULL", 0 );
 
    extract_exit( fromRoom, pexit );
    /* rendunancy */
    /* send a message to fromRoom */
    /* ch = fromRoom->first_person; */
    /* if(ch!=NULL) */
    /* act( AT_PLAIN, "A magical portal below winks from existence.", ch, NULL, NULL, TO_ROOM ); */

    /* send a message to toRoom */
    if ( toRoom && (ch = toRoom->first_person) != NULL )
      act( AT_PLAIN, "A magical portal above winks from existence.", ch, NULL, NULL, TO_ROOM );

    /* remove the portal obj: looks better to let update_obj do this */
    /* extract_obj(portal);  */

    return;
}

void reboot_check( time_t reset )
{
  static char *tmsg[] =
  { "You feel the ground shake as the end comes near!",
    "Lightning crackles in the sky above!",
    "Crashes of thunder sound across the land!",
    "The sky has suddenly turned midnight black.",
    "You notice the life forms around you slowly dwindling away.",
    "The seas across the realm have turned frigid.",
    "The aura of magic that surrounds the realms seems slightly unstable.",
    "You sense a change in the magical forces surrounding you."
  };
  static const int times[] = { 60, 120, 180, 240, 300, 600, 900, 1800 };
  static const int timesize =
      UMIN(sizeof(times)/sizeof(*times), sizeof(tmsg)/sizeof(*tmsg));
  char buf[MAX_STRING_LENGTH];
  static int trun;
  static bool init;
  
  if ( !init || reset >= current_time )
  {
    for ( trun = timesize-1; trun >= 0; trun-- )
      if ( reset >= current_time+times[trun] )
        break;
    init = TRUE;
    return;
  }
  
  if ( (current_time % 1800) == 0 )
  {
    sprintf(buf, "%.24s: %d players", ctime(&current_time), num_descriptors);
    append_to_file(USAGE_FILE, buf);
  }
  
  if ( new_boot_time_t - boot_time < 60*60*18 &&
      !set_boot_time->manual )
    return;
  
  if ( new_boot_time_t <= current_time )
  {
    CHAR_DATA *vch;
    extern bool mud_down;
    
    if ( auction->item )
    {
      sprintf(buf, "Sale of %s has been stopped by mud.",
          auction->item->short_descr);
      talk_auction(buf);
      obj_to_char(auction->item, auction->seller);
      auction->item = NULL;
      if ( auction->buyer && auction->buyer != auction->seller )
      {
        auction->buyer->gold += auction->bet;
        send_to_char("Your money has been returned.\n\r", auction->buyer);
      }
    }
    echo_to_all(AT_YELLOW, "You are forced from these realms by a strong "
        "magical presence\n\ras life here is reconstructed.", ECHOTAR_ALL);
    
    for ( vch = first_char; vch; vch = vch->next )
      if ( !IS_NPC(vch) )
        save_char_obj(vch);
    mud_down = TRUE;
    return;
  }
  
  if ( trun != -1 && new_boot_time_t - current_time <= times[trun] )
  {
    echo_to_all(AT_YELLOW, tmsg[trun], ECHOTAR_ALL);
    if ( trun <= 5 )
      sysdata.DENY_NEW_PLAYERS = TRUE;
    --trun;
    return;
  }
  return;
}
  
#if 0
void reboot_check( char *arg )
{
    char buf[MAX_STRING_LENGTH];
    extern bool mud_down;
    CHAR_DATA *vch;

    /*Bools to show which pre-boot echoes we've done. */
    static bool thirty  = FALSE;
    static bool fifteen = FALSE;
    static bool ten     = FALSE;
    static bool five    = FALSE;
    static bool four    = FALSE;
    static bool three   = FALSE;
    static bool two     = FALSE;
    static bool one     = FALSE;

    /* This function can be called by do_setboot when the reboot time
       is being manually set to reset all the bools. */
    if ( !str_cmp( arg, "reset" ) )
    {
      thirty  = FALSE;
      fifteen = FALSE;
      ten     = FALSE;
      five    = FALSE;
      four    = FALSE;
      three   = FALSE;
      two     = FALSE;
      one     = FALSE;
      return;
    }

    /* If the mud has been up less than 18 hours and the boot time 
       wasn't set manually, forget it. */ 
/* Usage monitor */

if ((current_time % 1800) == 0)
{
  sprintf(buf, "%s: %d players", ctime(&current_time), num_descriptors);  
  append_to_file(USAGE_FILE, buf);
}

/* Change by Scryn - if mud has not been up 18 hours at boot time - still 
 * allow for warnings even if not up 18 hours 
 */
    if ( new_boot_time_t - boot_time < 60*60*18 
         && set_boot_time->manual == 0 )
    {
      return;
    }
/*
    timestruct = localtime( &current_time);

    if ( timestruct->tm_hour == set_boot_time->hour        
         && timestruct->tm_min  == set_boot_time->min )*/
    if ( new_boot_time_t <= current_time )
    {
       /* Return auction item to seller */
       if (auction->item != NULL)
       {
        sprintf (buf,"Sale of %s has been stopped by mud.",
                 auction->item->short_descr);
        talk_auction (buf);
        obj_to_char (auction->item, auction->seller);
        auction->item = NULL;
        if (auction->buyer != NULL && auction->seller != auction->buyer) /* return money to the buyer */
        {
            auction->buyer->gold += auction->bet;
            send_to_char ("Your money has been returned.\n\r",auction->buyer);
        }
       }      

       sprintf( buf, "You are forced from these realms by a strong magical presence" ); 
       echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
       sprintf( buf, "as life here is reconstructed." );
       echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );

       /* Save all characters before booting. */
       for ( vch = first_char; vch; vch = vch->next )
       {
         if ( !IS_NPC( vch ) )
           save_char_obj( vch );
       }
       mud_down = TRUE;
    }

  /* How many minutes to the scheduled boot? */
/*  timecheck = ( set_boot_time->hour * 60 + set_boot_time->min )
              - ( timestruct->tm_hour * 60 + timestruct->tm_min );

  if ( timecheck > 30  || timecheck < 0 ) return;

  if ( timecheck <= 1 ) */
  if ( new_boot_time_t - current_time <= 60 )
  {
    if ( one == FALSE )
    {
	sprintf( buf, "You feel the ground shake as the end comes near!" );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	one = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 2 )*/
  if ( new_boot_time_t - current_time <= 120 )
  {
    if ( two == FALSE )
    {
	sprintf( buf, "Lightning crackles in the sky above!" );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	two = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 3 )*/ 
  if (new_boot_time_t - current_time <= 180 )
  {
    if ( three == FALSE )
    {
	sprintf( buf, "Crashes of thunder sound across the land!" );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	three = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 4 )*/
  if( new_boot_time_t - current_time <= 240 )
  {
    if ( four == FALSE )
    {
	sprintf( buf, "The sky has suddenly turned midnight black." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	four = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 5 )*/
  if( new_boot_time_t - current_time <= 300 )
  {
    if ( five == FALSE )
    {
	sprintf( buf, "You notice the life forms around you slowly dwindling away." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	five = TRUE;
	sysdata.DENY_NEW_PLAYERS = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 10 )*/
  if( new_boot_time_t - current_time <= 600 )
  {
    if ( ten == FALSE )
    {
	sprintf( buf, "The seas across the realm have turned frigid." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	ten = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 15 )*/
  if( new_boot_time_t - current_time <= 900 )
  {
    if ( fifteen == FALSE )
    {
	sprintf( buf, "The aura of magic which once surrounded the realms seems slightly unstable." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	fifteen = TRUE;
    }
    return;   
  }

/*  if ( timecheck == 30 )*/
  if( new_boot_time_t - current_time <= 1800 )
  { 
    if ( thirty == FALSE )
    {
	sprintf( buf, "You sense a change in the magical forces surrounding you." );
	echo_to_all( AT_YELLOW, buf, ECHOTAR_ALL );
	thirty = TRUE;
    }
    return;   
  }

  return;
}
#endif

/* the auction update*/

void auction_update (void)
{
    int tax, pay;
    char buf[MAX_STRING_LENGTH];

    switch (++auction->going) /* increase the going state */
    {
	case 1 : /* going once */
	case 2 : /* going twice */
	    if (auction->bet > auction->starting)
		sprintf (buf, "%s: going %s for %d.", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"), auction->bet);
	    else
		sprintf (buf, "%s: going %s (bid not received yet).", auction->item->short_descr,
			((auction->going == 1) ? "once" : "twice"));

	    talk_auction (buf);
	    break;

	case 3 : /* SOLD! */
	    if (!auction->buyer && auction->bet)
	    {
		bug( "Auction code reached SOLD, with NULL buyer, but %d gold bid", auction->bet );
		auction->bet = 0;
	    }
	    if (auction->bet > 0 && auction->buyer != auction->seller)
	    {
		sprintf (buf, "%s sold to %s for %d.",
			auction->item->short_descr,
			IS_NPC(auction->buyer) ? auction->buyer->short_descr : auction->buyer->name,
			auction->bet);
		talk_auction(buf);

		act(AT_ACTION, "The auctioneer materializes before you, and hands you $p.",
			auction->buyer, auction->item, NULL, TO_CHAR);
		act(AT_ACTION, "The auctioneer materializes before $n, and hands $m $p.",
			auction->buyer, auction->item, NULL, TO_ROOM);

		if ( (auction->buyer->carry_weight 
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->buyer ) )
		{
		    act( AT_PLAIN, "$p is too heavy for you to carry with your current inventory.", auction->buyer, auction->item, NULL, TO_CHAR );
    		    act( AT_PLAIN, "$n is carrying too much to also carry $p, and $e drops it.", auction->buyer, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->buyer->in_room );
		}
		else
		    obj_to_char( auction->item, auction->buyer );
	        pay = (int)auction->bet * 0.9;
		tax = (int)auction->bet * 0.1;
		boost_economy( auction->seller->in_room->area, tax );
                auction->seller->gold += pay; /* give him the money, tax 10 % */
		sprintf(buf, "The auctioneer pays you %d gold, charging an auction fee of %d.\n\r", pay, tax);
		send_to_char(buf, auction->seller);
                auction->item = NULL; /* reset item */
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		{
		    save_char_obj( auction->buyer );
		    save_char_obj( auction->seller );
		}
            }
            else /* not sold */
            {
                sprintf (buf, "No bids received for %s - object has been removed from auction\n\r.",auction->item->short_descr);
                talk_auction(buf);
                act (AT_ACTION, "The auctioneer appears before you to return $p to you.",
                      auction->seller,auction->item,NULL,TO_CHAR);
                act (AT_ACTION, "The auctioneer appears before $n to return $p to $m.",
                      auction->seller,auction->item,NULL,TO_ROOM);
		if ( (auction->seller->carry_weight
		+     get_obj_weight( auction->item ))
		>     can_carry_w( auction->seller ) )
		{
		    act( AT_PLAIN, "You drop $p as it is just too much to carry"
			" with everything else you're carrying.", auction->seller,
			auction->item, NULL, TO_CHAR );
		    act( AT_PLAIN, "$n drops $p as it is too much extra weight"
			" for $m with everything else.", auction->seller,
			auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->seller->in_room );
		}
		else
		    obj_to_char (auction->item,auction->seller);
		tax = (int)auction->item->cost * 0.05;
		boost_economy( auction->seller->in_room->area, tax );
		sprintf(buf, "The auctioneer charges you an auction fee of %d.\n\r", tax );
		send_to_char(buf, auction->seller);
		if ((auction->seller->gold - tax) < 0)
		  auction->seller->gold = 0;
		else
		  auction->seller->gold -= tax;
		if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		    save_char_obj( auction->seller );
	    } /* else */
	    auction->item = NULL; /* clear auction */
    } /* switch */
} /* func */

void subtract_times(struct timeval *etime, struct timeval *stime)
{
  etime->tv_sec -= stime->tv_sec;
  etime->tv_usec -= stime->tv_usec;
  while ( etime->tv_usec < 0 )
  {
    etime->tv_usec += 1000000;
    etime->tv_sec--;
  }
  return;
}
