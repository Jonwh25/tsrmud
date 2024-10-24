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
 *			   Object manipulation module			    *
 ****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"
#include "bet.h"

/*double sqrt( double x );*/

/*
 * External functions
 */

void    show_list_to_char  args( ( OBJ_DATA *list, CHAR_DATA *ch,
				bool fShort, bool fShowNothing ) );
/*
 * Local functions.
 */
void	get_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			    OBJ_DATA *container ) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, sh_int wear_bit ) );


/*
 * how resistant an object is to damage				-Thoric
 */
sh_int get_obj_resistance( OBJ_DATA *obj )
{
    sh_int resist;

    resist = number_fuzzy(MAX_ITEM_IMPACT);

    /* magical items are more resistant */
    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
      resist += number_fuzzy(12);
    /* metal objects are definately stronger */
    if ( IS_OBJ_STAT( obj, ITEM_METAL ) )
      resist += number_fuzzy(5);
    /* organic objects are most likely weaker */
    if ( IS_OBJ_STAT( obj, ITEM_ORGANIC ) )
      resist -= number_fuzzy(5);
    /* blessed objects should have a little bonus */
    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
      resist += number_fuzzy(5);
    /* lets make store inventory pretty tough */
    if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
      resist += 20;

    /* okay... let's add some bonus/penalty for item level... */
    resist += (obj->level / 10) - 2;

    /* and lasty... take armor or weapon's condition into consideration */
    if (obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_WEAPON)
      resist += (obj->value[0] / 2) - 2;

    return URANGE(10, resist, 99);
}


void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    CLAN_DATA *clan;
    int weight;

    if ( !CAN_WEAR(obj, ITEM_TAKE)
       && (ch->level < sysdata.level_getobjnotake )  )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE )
    &&  !can_take_proto( ch ) )
    {
	send_to_char( "A godly force prevents you from getting close to it.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( AT_PLAIN, "$d: you can't carry that many items.",
		ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
      weight = obj->weight;
    else
      weight = get_obj_weight( obj );

    if ( ch->carry_weight + weight > can_carry_w( ch ) )
    {
	act( AT_PLAIN, "$d: you can't carry that much weight.",
		ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ( container )
    {
	act( AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ? 
		"You get $p from beneath $P." : "You get $p from $P",
		ch, obj, container, TO_CHAR );
	act( AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ?
		"$n gets $p from beneath $P." : "$n gets $p from $P",
		ch, obj, container, TO_ROOM );
	obj_from_obj( obj );
    }
    else
    {
	act( AT_ACTION, "You get $p.", ch, obj, container, TO_CHAR );
	act( AT_ACTION, "$n gets $p.", ch, obj, container, TO_ROOM );
	obj_from_room( obj );
    }

    /* Clan storeroom checks */
    if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) 
    && (!container || container->carried_by == NULL) )
	for ( clan = first_clan; clan; clan = clan->next )
	  if ( clan->storeroom == ch->in_room->vnum )
	    save_clan_storeroom(ch, clan);

    if ( obj->item_type != ITEM_CONTAINER )
      check_for_trap( ch, obj, TRAP_GET );
    if ( char_died(ch) )
      return;

    if ( obj->item_type == ITEM_MONEY )
    {
	ch->gold += obj->value[0];
	extract_obj( obj );
    }
    else
    {
	obj = obj_to_char( obj, ch );
    }

    if ( char_died(ch) || obj_extracted(obj) )
      return;
    oprog_get_trigger(ch, obj);
    return;
}


void do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    sh_int number;
    bool found;

    argument = one_argument( argument, arg1 );
    if ( is_number(arg1) )
    {
	number = atoi(arg1);
	if ( number < 1 )
	{
	    send_to_char( "That was easy...\n\r", ch );
	    return;
	}
	if ( (ch->carry_number + number) > can_carry_n(ch) )
	{
	    send_to_char( "You can't carry that many.\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg1 );
    }
    else
	number = 0;
    argument = one_argument( argument, arg2 );
    /* munch optional words */
    if ( !str_cmp( arg2, "from" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( arg2[0] == '\0' )
    {
	if ( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->first_content );
	    if ( !obj )
	    {
		act( AT_PLAIN, "I see no $T here.", ch, NULL, arg1, TO_CHAR );
		return;
	    }
	    separate_obj(obj);
	    get_obj( ch, obj, NULL );
	    if ( char_died(ch) )
		return;
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
	else
	{
	    sh_int cnt = 0;
	    bool fAll;
	    char *chk;

	    if ( IS_SET( ch->in_room->room_flags, ROOM_DONATION ) )
	    {
		send_to_char( "The gods frown upon such a display of greed!\n\r", ch );
		return;
	    }
	    if ( !str_cmp(arg1, "all") )
		fAll = TRUE;
	    else
		fAll = FALSE;
	    if ( number > 1 )
		chk = arg1;
	    else
		chk = &arg1[4];
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for ( obj = ch->in_room->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( fAll || nifty_is_name( chk, obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( number && (cnt + obj->count) > number )
			split_obj( obj, number - cnt );
		    cnt += obj->count;
		    get_obj( ch, obj, NULL );
		    if ( char_died(ch)
		    ||   ch->carry_number >= can_carry_n( ch )
		    ||   ch->carry_weight >= can_carry_w( ch )
		    ||   (number && cnt >= number) )
		    {
			if ( IS_SET(sysdata.save_flags, SV_GET)
			&&  !char_died(ch) )
			    save_char_obj(ch);
			return;
		    }
		}
	    }

	    if ( !found )
	    {
		if ( fAll )
		  send_to_char( "I see nothing here.\n\r", ch );
		else
		  act( AT_PLAIN, "I see no $T here.", ch, NULL, chk, TO_CHAR );
	    }
	    else
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    if ( !IS_OBJ_STAT( container, ITEM_COVERING ) )
	    {
		send_to_char( "That's not a container.\n\r", ch );
		return;
	    }
	    if ( ch->carry_weight + container->weight > can_carry_w( ch ) )
	    {
		send_to_char( "It's too heavy for you to lift.\n\r", ch );
		return;
	    }
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	    break;

	case ITEM_CORPSE_PC:
	    {
		char name[MAX_INPUT_LENGTH];
		CHAR_DATA *gch;
		char *pd;

		if ( IS_NPC(ch) )
		{
		    send_to_char( "You can't do that.\n\r", ch );
		    return;
		}

		pd = container->short_descr;
		pd = one_argument( pd, name );
		pd = one_argument( pd, name );
		pd = one_argument( pd, name );
		pd = one_argument( pd, name );
		
		if ( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
		&&  !IS_NPC(ch) && (get_timer( ch, TIMER_PKILLED ) > 0 )
		&& str_cmp( name, ch->name ) )
		{
		   send_to_char( "You cannot loot from that corpse...yet.\n\r", ch );
		   return;
		}

		if ( IS_OBJ_STAT( container, ITEM_CLANCORPSE )
		&&  !IS_NPC(ch) && IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
		   break;
		
		if ( str_cmp( name, ch->name ) && !IS_IMMORTAL(ch) )
		{
		    bool fGroup;

		    fGroup = FALSE;
		    for ( gch = first_char; gch; gch = gch->next )
		    {
			if ( !IS_NPC(gch)
			&&   is_same_group( ch, gch )
			&&   !str_cmp( name, gch->name ) )
			{
			    fGroup = TRUE;
			    break;
			}
		    }

		    if ( !fGroup )
		    {
			send_to_char( "That's someone else's corpse.\n\r", ch );
			return;
		    }
		}
	    }
	}

	if ( !IS_OBJ_STAT(container, ITEM_COVERING )
	&&    IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if ( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->first_content );
	    if ( !obj )
	    {
		act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"I see nothing like that beneath the $T." :
			"I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    separate_obj(obj);
	    get_obj( ch, obj, container );

	    check_for_trap( ch, container, TRAP_GET );
	    if ( char_died(ch) )
	      return;
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
	else
	{
	    int cnt = 0;
	    bool fAll;
	    char *chk;

	    /* 'get all container' or 'get all.obj container' */
	    if ( IS_OBJ_STAT( container, ITEM_DONATION ) )
	    {
		send_to_char( "The gods frown upon such an act of greed!\n\r", ch );
		return;
	    }
	    if ( !str_cmp(arg1, "all") )
		fAll = TRUE;
	    else
		fAll = FALSE;
	    if ( number > 1 )
		chk = arg1;
	    else
		chk = &arg1[4];
	    found = FALSE;
	    for ( obj = container->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( fAll || nifty_is_name( chk, obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( number && (cnt + obj->count) > number )
			split_obj( obj, number - cnt );
		    cnt += obj->count;
		    get_obj( ch, obj, container );
		    if ( char_died(ch)
		    ||   ch->carry_number >= can_carry_n( ch )
		    ||   ch->carry_weight >= can_carry_w( ch )
		    ||   (number && cnt >= number) )
		      return;
		}
	    }

	    if ( !found )
	    {
		if ( fAll )
		  act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"I see nothing beneath the $T." :
			"I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		  act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"I see nothing like that beneath the $T." :
			"I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	    else
	      check_for_trap( ch, container, TRAP_GET );
	    if ( char_died(ch) )
		return;
	    if ( found && IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
    }
    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CLAN_DATA *clan;
    sh_int	count;
    int		number;
    bool	save_char = FALSE;

    argument = one_argument( argument, arg1 );
    if ( is_number(arg1) )
    {
	number = atoi(arg1);
	if ( number < 1 )
	{
	    send_to_char( "That was easy...\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg1 );
    }
    else
	number = 0;
    argument = one_argument( argument, arg2 );
    /* munch optional words */
    if ( (!str_cmp(arg2, "into") || !str_cmp(arg2, "inside") || !str_cmp(arg2, "in"))
    &&   argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
	act( AT_PLAIN, "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	return;
    }

    if ( !container->carried_by && IS_SET( sysdata.save_flags, SV_PUT ) )
	save_char = TRUE;

    if ( IS_OBJ_STAT(container, ITEM_COVERING) )
    {
	if ( ch->carry_weight + container->weight > can_carry_w( ch ) )
	{
	    send_to_char( "It's too heavy for you to lift.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( container->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}

	if ( IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}
    }

    if ( number <= 1 && str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	if ( (IS_OBJ_STAT(container, ITEM_COVERING)
	&&   (get_obj_weight( obj ) / obj->count)
	  > ((get_obj_weight( container ) / container->count)
	  -   container->weight)) )
	{
	    send_to_char( "It won't fit under there.\n\r", ch );
	    return;
	}

	if ( (get_obj_weight( obj ) / obj->count)
	   + (get_obj_weight( container ) / container->count)
	   >  container->value[0] )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	separate_obj(obj);
	separate_obj(container);
	obj_from_char( obj );
	obj = obj_to_obj( obj, container );
	check_for_trap ( ch, container, TRAP_PUT );
	if ( char_died(ch) )
	  return;
	count = obj->count;
	obj->count = 1;
 	act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
 		? "$n hides $p beneath $P." : "$n puts $p in $P.",
		ch, obj, container, TO_ROOM );
	act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
		? "You hide $p beneath $P." : "You put $p in $P.",
		ch, obj, container, TO_CHAR );
	obj->count = count;

	if ( save_char )
	  save_char_obj(ch);
	/* Clan storeroom check */
	if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) 
	&&   container->carried_by == NULL)
	   for ( clan = first_clan; clan; clan = clan->next )
	      if ( clan->storeroom == ch->in_room->vnum )
		save_clan_storeroom(ch, clan);
    }
    else
    {
	bool found = FALSE;
	int cnt = 0;
	bool fAll;
	char *chk;

	if ( !str_cmp(arg1, "all") )
	    fAll = TRUE;
	else
	    fAll = FALSE;
	if ( number > 1 )
	    chk = arg1;
	else
	    chk = &arg1[4];

	separate_obj(container);
	/* 'put all container' or 'put all.obj container' */
	for ( obj = ch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( fAll || nifty_is_name( chk, obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   obj != container
	    &&   can_drop_obj( ch, obj )
	    &&   get_obj_weight( obj ) + get_obj_weight( container )
		 <= container->value[0] )
	    {
		if ( number && (cnt + obj->count) > number )
		    split_obj( obj, number - cnt );
		cnt += obj->count;
		obj_from_char( obj );
		act( AT_ACTION, "$n puts $p in $P.", ch, obj, container, TO_ROOM );
		act( AT_ACTION, "You put $p in $P.", ch, obj, container, TO_CHAR );
		obj = obj_to_obj( obj, container );
		found = TRUE;

		check_for_trap( ch, container, TRAP_PUT );
		if ( char_died(ch) )
		  return;
		if ( number && cnt >= number )
		  break;
	    }
	}

	/*
	 * Don't bother to save anything if nothing was dropped   -Thoric
	 */
	if ( !found )
	{
	    if ( fAll )
	      act( AT_PLAIN, "You are not carrying anything.",
		    ch, NULL, NULL, TO_CHAR );
	    else
	      act( AT_PLAIN, "You are not carrying any $T.",
		    ch, NULL, chk, TO_CHAR );
	    return;
	}

	if ( save_char )
	    save_char_obj(ch);
	/* Clan storeroom check */
        if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) 
	&& container->carried_by == NULL )
	  for ( clan = first_clan; clan; clan = clan->next )
	     if ( clan->storeroom == ch->in_room->vnum )
        	save_clan_storeroom(ch, clan);
    }

    return;
}


void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;
    CLAN_DATA *clan;
    int number;

    argument = one_argument( argument, arg );
    if ( is_number(arg) )
    {
	number = atoi(arg);
	if ( number < 1 )
	{
	    send_to_char( "That was easy...\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg );
    }
    else
	number = 0;

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NODROP ) )
    {
       set_char_color( AT_MAGIC, ch );
       send_to_char( "A magical force stops you!\n\r", ch );
       set_char_color( AT_TELL, ch );
       send_to_char( "Someone tells you, 'No littering here!'\n\r", ch );
       return;
    }

    if ( number > 0 )
    {
	/* 'drop NNNN coins' */

	if ( !str_cmp( arg, "coins" ) || !str_cmp( arg, "coin" ) )
	{
	    if ( ch->gold < number )
	    {
		send_to_char( "You haven't got that many coins.\n\r", ch );
		return;
	    }

	    ch->gold -= number;

	    for ( obj = ch->in_room->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		switch ( obj->pIndexData->vnum )
		{
		case OBJ_VNUM_MONEY_ONE:
		   number += 1;
		   extract_obj( obj );
		   break;

		case OBJ_VNUM_MONEY_SOME:
		   number += obj->value[0];
		   extract_obj( obj );
		   break;
		}
	    }

	    act( AT_ACTION, "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	    obj_to_room( create_money( number ), ch->in_room );
	    send_to_char( "OK.\n\r", ch );
	    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
		save_char_obj( ch );
	    return;
	}
    }

    if ( number <= 1 && str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	/* 'drop obj' */
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	separate_obj( obj );
	act( AT_ACTION, "$n drops $p.", ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "You drop $p.", ch, obj, NULL, TO_CHAR );

	obj_from_char( obj );
	obj = obj_to_room( obj, ch->in_room );
	oprog_drop_trigger ( ch, obj );   /* mudprogs */

        if( char_died(ch) || obj_extracted(obj) )
          return;

	/* Clan storeroom saving */
	if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) )
	   for ( clan = first_clan; clan; clan = clan->next )
 	      if ( clan->storeroom == ch->in_room->vnum )
		save_clan_storeroom(ch, clan);
    }
    else
    {
	int cnt = 0;
	char *chk;
	bool fAll;

	if ( !str_cmp(arg, "all") )
	    fAll = TRUE;
	else
	    fAll = FALSE;
	if ( number > 1 )
	    chk = arg;
	else
	    chk = &arg[4];
	/* 'drop all' or 'drop all.obj' */
	if ( IS_SET( ch->in_room->room_flags, ROOM_NODROPALL ) 
        ||   IS_SET( ch->in_room->room_flags, ROOM_CLANSTOREROOM ) )
	{
	    send_to_char( "You can't seem to do that here...\n\r", ch );
	    return;
	}
	found = FALSE;
	for ( obj = ch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( (fAll || nifty_is_name( chk, obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		if ( obj->pIndexData->progtypes & DROP_PROG && obj->count > 1 ) 
		{
		   ++cnt;
		   separate_obj( obj );
		   obj_from_char( obj );
		   if ( !obj_next )
		     obj_next = ch->first_carrying;
		}
		else
		{
		   if ( number && (cnt + obj->count) > number )
		     split_obj( obj, number - cnt );
		   cnt += obj->count;
		   obj_from_char( obj );
		}
		act( AT_ACTION, "$n drops $p.", ch, obj, NULL, TO_ROOM );
		act( AT_ACTION, "You drop $p.", ch, obj, NULL, TO_CHAR );
		obj = obj_to_room( obj, ch->in_room );
		oprog_drop_trigger( ch, obj );		/* mudprogs */
                if ( char_died(ch) )
                    return;
		if ( number && cnt >= number )
		    break;
	    }
	}

	if ( !found )
	{
	    if ( fAll )
	      act( AT_PLAIN, "You are not carrying anything.",
		    ch, NULL, NULL, TO_CHAR );
	    else
	      act( AT_PLAIN, "You are not carrying any $T.",
		    ch, NULL, chk, TO_CHAR );
	}
    }
    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
	save_char_obj( ch );	/* duping protector */
    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf  [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "to" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( is_number( arg1 ) )
    {
	/* 'give NNNN coins victim' */
	int amount;

	amount   = atoi(arg1);
	if ( amount <= 0
	|| ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( !str_cmp( arg2, "to" ) && argument[0] != '\0' )
	  argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give what to whom?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "Very generous of you, but you haven't got that much gold.\n\r", ch );
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
        strcpy(buf, "$n gives you ");
        strcat(buf, arg1);
        strcat(buf, (amount > 1) ? " coins." : " coin.");

	act( AT_ACTION, buf, ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n gives $N some gold.",  ch, NULL, victim, TO_NOTVICT );
	act( AT_ACTION, "You give $N some gold.",  ch, NULL, victim, TO_CHAR    );
	send_to_char( "OK.\n\r", ch );
	mprog_bribe_trigger( victim, ch, amount );
	if ( IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch) )
	  save_char_obj(ch);
	if ( IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim) )
	  save_char_obj(victim);
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( victim->carry_number + (get_obj_number(obj)/obj->count) > can_carry_n( victim ) )
    {
	act( AT_PLAIN, "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_weight + (get_obj_weight(obj)/obj->count) > can_carry_w( victim ) )
    {
	act( AT_PLAIN, "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( AT_PLAIN, "$N can't see it.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto( victim ) )
    {
	act( AT_PLAIN, "You cannot give that to $N!", ch, NULL, victim, TO_CHAR );
	return;
    }

    separate_obj( obj );
    obj_from_char( obj );
    act( AT_ACTION, "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    act( AT_ACTION, "$n gives you $p.",   ch, obj, victim, TO_VICT    );
    act( AT_ACTION, "You give $p to $N.", ch, obj, victim, TO_CHAR    );
    obj = obj_to_char( obj, victim );
    mprog_give_trigger( victim, ch, obj );
    if ( IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch) )
	save_char_obj(ch);
    if ( IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim) )
	save_char_obj(victim);
    return;
}

/*
 * Damage an object.						-Thoric
 * Affect player's AC if necessary.
 * Make object into scraps if necessary.
 * Send message about damaged object.
 */
obj_ret damage_obj( OBJ_DATA *obj )
{
    CHAR_DATA *ch;
    obj_ret objcode;

    ch = obj->carried_by;
    objcode = rNONE;
  
    separate_obj( obj );
    if ( ch )
      act( AT_OBJECT, "($p gets damaged)", ch, obj, NULL, TO_CHAR );
    else
    if ( obj->in_room && ( ch = obj->in_room->first_person ) != NULL )
    {
	act( AT_OBJECT, "($p gets damaged)", ch, obj, NULL, TO_ROOM );
	act( AT_OBJECT, "($p gets damaged)", ch, obj, NULL, TO_CHAR );
	ch = NULL;
    }

    oprog_damage_trigger(ch, obj);
    if ( obj_extracted(obj) )
      return global_objcode;

    switch( obj->item_type )
    {
	default:
	  make_scraps( obj );
	  objcode = rOBJ_SCRAPPED;
	  break;
	case ITEM_CONTAINER:
	  if (--obj->value[3] <= 0) 
	  {
		make_scraps( obj );
		objcode = rOBJ_SCRAPPED;
	  }
	  break;
	case ITEM_ARMOR:
	  if ( ch && obj->value[0] >= 1 )
	    ch->armor += apply_ac( obj, obj->wear_loc );
	  if (--obj->value[0] <= 0)
	  {
	    if ( !IS_PKILL( ch ) && !in_arena( ch ) )
	    {
		make_scraps( obj );
		objcode = rOBJ_SCRAPPED;
	    }
	    else
		obj->value[0] = 1;
	  }
	  else
	  if ( ch && obj->value[0] >= 1 )
	    ch->armor -= apply_ac( obj, obj->wear_loc );
	  break;
	case ITEM_WEAPON:
	  if (--obj->value[0] <= 0)
	  {
	     if ( !IS_PKILL( ch ) && !in_arena( ch ) )
	     {
		make_scraps( obj );
		objcode = rOBJ_SCRAPPED;
	     }
	     else
		obj->value[0] = 1;
	  }
	  break;
    }
    return objcode;
}


/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj, *tmpobj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace
    &&   ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( AT_PLAIN, "$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR );
	return FALSE;
    }

    if ( !fReplace )
	return FALSE;

    if ( IS_OBJ_STAT(obj, ITEM_NOREMOVE) )
    {
	act( AT_PLAIN, "You can't remove $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    if ( obj == get_eq_char( ch, WEAR_WIELD )
    && ( tmpobj = get_eq_char( ch, WEAR_DUAL_WIELD)) != NULL )
       tmpobj->wear_loc = WEAR_WIELD;

    unequip_char( ch, obj );

    act( AT_ACTION, "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( AT_ACTION, "You stop using $p.", ch, obj, NULL, TO_CHAR );
    oprog_remove_trigger( ch, obj );
    return TRUE;
}

/*
 * See if char could be capable of dual-wielding		-Thoric
 */
bool could_dual( CHAR_DATA *ch )
{
  if ( IS_NPC(ch) )
    return TRUE;
  if ( ch->pcdata->learned[gsn_dual_wield] )
    return TRUE;

  return FALSE;
}

/*
 * See if char can dual wield at this time			-Thoric
 */
bool can_dual( CHAR_DATA *ch )
{
   if ( !could_dual(ch) )
     return FALSE;

   if ( get_eq_char( ch, WEAR_DUAL_WIELD ) )
   {
	send_to_char( "You are already wielding two weapons!\n\r", ch );
	return FALSE;
   }
   if ( get_eq_char( ch, WEAR_SHIELD ) )
   {
	send_to_char( "You cannot dual wield while holding a shield!\n\r", ch );
	return FALSE;
   }
   if ( get_eq_char( ch, WEAR_HOLD ) )
   {
	send_to_char( "You cannot dual wield while holding something!\n\r", ch );
	return FALSE;
   }
   return TRUE;
}


/*
 * Check to see if there is room to wear another object on this location
 * (Layered clothing support)
 */
bool can_layer( CHAR_DATA *ch, OBJ_DATA *obj, sh_int wear_loc )
{
    OBJ_DATA   *otmp;
    sh_int	bitlayers = 0;
    sh_int	objlayers = obj->pIndexData->layers;

    for ( otmp = ch->first_carrying; otmp; otmp = otmp->next_content )
	if ( otmp->wear_loc == wear_loc )
	    if ( !otmp->pIndexData->layers )
		return FALSE;
	    else
		bitlayers |= otmp->pIndexData->layers;

    if ( (bitlayers && !objlayers) || bitlayers > objlayers )
	return FALSE;
    if ( !bitlayers || ((bitlayers & ~objlayers) == bitlayers) )
	return TRUE;
    return FALSE;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 * Restructured a bit to allow for specifying body location	-Thoric
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, sh_int wear_bit )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *tmpobj;
    sh_int bit, tmp;

    separate_obj( obj );
    if ( get_trust( ch ) < obj->level )
    {
	sprintf( buf, "You must be level %d to use this object.\n\r",
	    obj->level );
	send_to_char( buf, ch );
	act( AT_ACTION, "$n tries to use $p, but is too inexperienced.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( !IS_IMMORTAL(ch)
    &&  (( IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR)
        && ch->class == CLASS_WARRIOR					)
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_MAGE)
	&& ch->class == CLASS_MAGE					)
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_THIEF)
	&& ch->class == CLASS_THIEF					)
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_VAMPIRE)
	&& ch->class == CLASS_VAMPIRE					)
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_DRUID)
	&& ch->class == CLASS_DRUID					)
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR)
	&& ch->class == CLASS_RANGER					)
    ||	 ( IS_OBJ_STAT(obj, ITEM_ANTI_MAGE)
	&& ch->class == CLASS_AUGURER					)
    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_CLERIC)
	&& ch->class == CLASS_CLERIC					)) )
    {
	act( AT_MAGIC, "You are forbidden to use that item.", ch, NULL, NULL, TO_CHAR );
	act( AT_ACTION, "$n tries to use $p, but is forbidden to do so.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( wear_bit > -1 )
    {
	bit = wear_bit;
	if ( !CAN_WEAR(obj, 1 << bit) )
	{
	    if ( fReplace )
	    {
		switch( 1 << bit )
		{
		    case ITEM_HOLD:
			send_to_char( "You cannot hold that.\n\r", ch );
			break;
		    case ITEM_WIELD:
			send_to_char( "You cannot wield that.\n\r", ch );
			break;
		    default:
			sprintf( buf, "You cannot wear that on your %s.\n\r",
				w_flags[bit] );
			send_to_char( buf, ch );
		}
	    }
	    return;
	}
    }
    else
    {
	for ( bit = -1, tmp = 1; tmp < 31; tmp++ )
	{
	    if ( CAN_WEAR(obj, 1 << tmp) )
	    {
		bit = tmp;
		break;
	    }
	}
    }

    /* currently cannot have a light in non-light position */
    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;
        if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
        {
  	  act( AT_ACTION, "$n holds $p as a light.", ch, obj, NULL, TO_ROOM );
	  act( AT_ACTION, "You hold $p as your light.",  ch, obj, NULL, TO_CHAR );
        }
	equip_char( ch, obj, WEAR_LIGHT );
        oprog_wear_trigger( ch, obj );
	return;
    }

    if ( obj->item_type == ITEM_SWORD_BELT )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
        if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
        {
  	  act( AT_ACTION, "$n straps $p on $s side.", ch, obj, NULL, TO_ROOM );
	  act( AT_ACTION, "You don $p on your side.",  ch, obj, NULL, TO_CHAR );
        }
	equip_char( ch, obj, WEAR_WAIST );
        oprog_wear_trigger( ch, obj );
	return;
    }

    if ( bit == -1 )
    {
	if ( fReplace )
	  send_to_char( "You can't wear, wield, or hold that.\n\r", ch );
	return;
    }

    switch ( 1 << bit )
    {
	default:
	    bug( "wear_obj: uknown/unused item_wear bit %d", bit );
	    if ( fReplace )
	      send_to_char( "You can't wear, wield, or hold that.\n\r", ch );
	    return;

	case ITEM_WEAR_FINGER:
	    if ( get_eq_char( ch, WEAR_FINGER_L )
	    &&   get_eq_char( ch, WEAR_FINGER_R )
	    &&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	    &&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	      return;

	    if ( !get_eq_char( ch, WEAR_FINGER_L ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
		  act( AT_ACTION, "$n slips $s left finger into $p.",    ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "You slip your left finger into $p.",  ch, obj, NULL, TO_CHAR );
                }
		equip_char( ch, obj, WEAR_FINGER_L );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_FINGER_R ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
		  act( AT_ACTION, "$n slips $s right finger into $p.",   ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "You slip your right finger into $p.", ch, obj, NULL, TO_CHAR );
                }
		equip_char( ch, obj, WEAR_FINGER_R );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    bug( "Wear_obj: no free finger.", 0 );
	    send_to_char( "You already wear something on both fingers.\n\r", ch );
	    return;

	case ITEM_WEAR_NECK:
	    if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	    &&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
	    &&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	    &&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	      return;

	    if ( !get_eq_char( ch, WEAR_NECK_1 ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
		  act( AT_ACTION, "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
                }
		equip_char( ch, obj, WEAR_NECK_1 );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_NECK_2 ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
		  act( AT_ACTION, "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
                }
		equip_char( ch, obj, WEAR_NECK_2 );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    bug( "Wear_obj: no free neck.", 0 );
	    send_to_char( "You already wear two neck items.\n\r", ch );
	    return;

	case ITEM_WEAR_BODY:
	/*
	    if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	      return;
	*/
	    if ( !can_layer( ch, obj, WEAR_BODY ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n fits $p on $s body.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You fit $p on your body.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_BODY );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_HEAD:
	    if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n dons $p upon $s head.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You don $p upon your head.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_HEAD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_EYES:
	    if ( !remove_obj( ch, WEAR_EYES, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n places $p on $s eyes.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You place $p on your eyes.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_EYES );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_EARS:
	    if ( !remove_obj( ch, WEAR_EARS, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wears $p on $s ears.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wear $p on your ears.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_EARS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_LEGS:
/*
	    if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_LEGS ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n slips into $p.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You slip into $p.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_LEGS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_FEET:
/*
	    if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_FEET ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_FEET );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_HANDS:
/*
	    if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_HANDS ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_HANDS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_ARMS:
/*
	    if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_ARMS ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
	    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_ARMS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_ABOUT:
	/*
	    if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	      return;
	*/
	    if ( !can_layer( ch, obj, WEAR_ABOUT ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_ABOUT );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_WAIST:
/*
	    if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_WAIST ) )
	    {
		send_to_char( "It won't fit overtop of what you're already wearing.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_WAIST );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_WRIST:
	    if ( get_eq_char( ch, WEAR_WRIST_L )
	    &&   get_eq_char( ch, WEAR_WRIST_R )
	    &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	    &&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	       return;

	    if ( !get_eq_char( ch, WEAR_WRIST_L ) )
	    {
		if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
		{
		   act( AT_ACTION, "$n fits $p around $s left wrist.",
			ch, obj, NULL, TO_ROOM );
		   act( AT_ACTION, "You fit $p around your left wrist.",
			ch, obj, NULL, TO_CHAR );
		}
		equip_char( ch, obj, WEAR_WRIST_L );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_WRIST_R ) )
	    {
              if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
              {
		act( AT_ACTION, "$n fits $p around $s right wrist.",
			ch, obj, NULL, TO_ROOM );
		act( AT_ACTION, "You fit $p around your right wrist.",
			ch, obj, NULL, TO_CHAR );
              }
		equip_char( ch, obj, WEAR_WRIST_R );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    bug( "Wear_obj: no free wrist.", 0 );
	    send_to_char( "You already wear two wrist items.\n\r", ch );
	    return;

	case ITEM_WEAR_SHIELD:
	    if ( get_eq_char( ch, WEAR_DUAL_WIELD ) )
	    {
		send_to_char( "You can't use a shield AND two weapons!\n\r", ch );
		return;
	    }
	    if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n uses $p as a shield.", ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You use $p as a shield.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_SHIELD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WIELD:
	    if ( (tmpobj  = get_eq_char( ch, WEAR_WIELD )) != NULL
	    &&   !could_dual(ch) )
	    {
		send_to_char( "You're already wielding something.\n\r", ch );
		return;
	    }

	    if ( tmpobj )
	    {
		if ( can_dual(ch) )
		{
		    if ( get_obj_weight( obj ) + get_obj_weight( tmpobj ) > str_app[get_curr_str(ch)].wield )
		    {
			send_to_char( "It is too heavy for you to wield.\n\r", ch );
			return;
	      	    }
                    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                    {
	  	      act( AT_ACTION, "$n dual-wields $p.", ch, obj, NULL, TO_ROOM );
		      act( AT_ACTION, "You dual-wield $p.", ch, obj, NULL, TO_CHAR );
                    }
		    equip_char( ch, obj, WEAR_DUAL_WIELD );
		    oprog_wear_trigger( ch, obj );
		}
	        return;
	    }

	    if ( get_obj_weight( obj ) > str_app[get_curr_str(ch)].wield )
	    {
		send_to_char( "It is too heavy for you to wield.\n\r", ch );
		return;
	    }

            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n wields $p.", ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You wield $p.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_WIELD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_HOLD:
	    if ( get_eq_char( ch, WEAR_DUAL_WIELD ) )
	    {
		send_to_char( "You cannot hold something AND two weapons!\n\r", ch );
		return;
	    }
	    if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	      return;
            if ( obj->item_type == ITEM_WAND
               || obj->item_type == ITEM_STAFF
               || obj->item_type == ITEM_FOOD 
               || obj->item_type == ITEM_PILL
               || obj->item_type == ITEM_POTION
               || obj->item_type == ITEM_SCROLL
               || obj->item_type == ITEM_DRINK_CON 
               || obj->item_type == ITEM_BLOOD
               || obj->item_type == ITEM_PIPE
               || obj->item_type == ITEM_HERB
               || obj->item_type == ITEM_KEY
               || !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "You hold $p in your hands.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_HOLD );
	    oprog_wear_trigger( ch, obj );
	    return;
    }
}


void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    sh_int wear_bit;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( (!str_cmp(arg2, "on")  || !str_cmp(arg2, "upon") || !str_cmp(arg2, "around"))
    &&   argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Wear, wield, or hold what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( !str_cmp( arg1, "all" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
		wear_obj( ch, obj, FALSE, -1 );
	}
	return;
    }
    else
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
	if ( arg2[0] != '\0' )
	  wear_bit = get_wflag(arg2);
	else
	  wear_bit = -1;
	wear_obj( ch, obj, TRUE, wear_bit );
    }

    return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj, *obj_next;


    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

   if ( !str_cmp( arg, "all" ) )  /* SB Remove all */
    {
      for ( obj = ch->first_carrying; obj != NULL ; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( obj->wear_loc != WEAR_NONE && can_see_obj ( ch, obj ) )
          remove_obj ( ch, obj->wear_loc, TRUE );
      }
      return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You are not using that item.\n\r", ch );
	return;
    }
    if ( (obj_next=get_eq_char(ch, obj->wear_loc)) != obj )
    {
	act( AT_PLAIN, "You must remove $p first.", ch, obj_next, NULL, TO_CHAR );
	return;
    }

    remove_obj( ch, obj->wear_loc, TRUE );
    return;
}


void do_bury( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    bool shovel;
    sh_int move;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {    
        send_to_char( "What do you wish to bury?\n\r", ch );
        return;
    }
    
    if ( ms_find_obj(ch) )
        return;
 
    shovel = FALSE;
    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_SHOVEL )
      {
	  shovel = TRUE;
	  break;
      }

    obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
    if ( !obj )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }

    separate_obj(obj);
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	if ( !IS_OBJ_STAT( obj, ITEM_CLANCORPSE )
	|| IS_NPC(ch) || !IS_SET( ch->pcdata->flags, PCFLAG_DEADLY ) )
	   {	
		act( AT_PLAIN, "You cannot bury $p.", ch, obj, 0, TO_CHAR );
        	return;
           }
    }
  
    switch( ch->in_room->sector_type )
    {
	case SECT_CITY:
	case SECT_INSIDE:
	    send_to_char( "The floor is too hard to dig through.\n\r", ch );
	    return;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_UNDERWATER:
	    send_to_char( "You cannot bury something here.\n\r", ch );
	    return;
	case SECT_AIR:
	    send_to_char( "What?  In the air?!\n\r", ch );
	    return;
    }

    if ( obj->weight > (UMAX(5, (can_carry_w(ch) / 10)))
    &&  !shovel )
    {
	send_to_char( "You'd need a shovel to bury something that big.\n\r", ch );
	return;
    }
    
    move = (obj->weight * 50 * (shovel ? 1 : 5)) / UMAX(1, can_carry_w(ch));
    move = URANGE( 2, move, 1000 );
    if ( move > ch->move )
    {
	send_to_char( "You don't have the energy to bury something of that size.\n\r", ch );
	return;
    }
    ch->move -= move;
    if ( obj->item_type == ITEM_CORPSE_NPC
    ||   obj->item_type == ITEM_CORPSE_PC )
	adjust_favor( ch, 6, 1 );

    act( AT_ACTION, "You solemnly bury $p...", ch, obj, NULL, TO_CHAR );
    act( AT_ACTION, "$n solemnly buries $p...", ch, obj, NULL, TO_ROOM );
    SET_BIT( obj->extra_flags, ITEM_BURRIED );
    WAIT_STATE( ch, URANGE( 10, move / 2, 100 ) );
    return;
}

void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char name[50];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
	act( AT_ACTION, "$n offers $mself to $s deity, who graciously declines.",
	    ch, NULL, NULL, TO_ROOM );
	send_to_char( "Your deity appreciates your offer and may accept it later.", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
    if ( !obj )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    separate_obj(obj);
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( AT_PLAIN, "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->deity && ch->pcdata->deity->name[0] != '\0' )
    {
	strcpy( name, ch->pcdata->deity->name );
    }
    else if ( !IS_NPC( ch ) && IS_GUILDED(ch) && sysdata.guild_overseer[0] != '\0' ) 
    {
	strcpy( name, sysdata.guild_overseer );
    }
    else if ( !IS_NPC( ch ) && ch->pcdata->clan && ch->pcdata->clan->deity[0] != '\0' )
    {
	strcpy( name, ch->pcdata->clan->deity );
    }
    else
    {
	strcpy( name, "Ao" );
    }
    ch->gold += 1;
    if ( obj->item_type == ITEM_CORPSE_NPC
    ||   obj->item_type == ITEM_CORPSE_PC )
       adjust_favor( ch, 5, 1 );
    sprintf( buf, "%s gives you one gold coin for your sacrifice.\n\r", name );
    send_to_char( buf, ch );
    sprintf( buf, "$n sacrifices $p to %s.", name );
    act( AT_ACTION, buf, ch, obj, NULL, TO_ROOM );
    oprog_sac_trigger( ch, obj );
    if ( obj_extracted(obj) )
      return;
    if ( cur_obj == obj->serial )
      global_objcode = rOBJ_SACCED;
    extract_obj( obj );
    return;
}

void do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    ch_ret retcode;
    int sn;

    if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }

    if ( ( sn = staff->value[3] ) < 0
    ||   sn >= top_sn
    ||   skill_table[sn]->spell_fun == NULL )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
      if ( !oprog_use_trigger( ch, staff, NULL, NULL, NULL ) )
      {
        act( AT_MAGIC, "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
        act( AT_MAGIC, "You brandish $p.",  ch, staff, NULL, TO_CHAR );
      }
	for ( vch = ch->in_room->first_person; vch; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;
            if ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_WIZINVIS ) 
                  && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
                continue;
            else
	    switch ( skill_table[sn]->target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
		    continue;
		break;
		
	    case TAR_CHAR_DEFENSIVE:
		if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    }

	    retcode = obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
	    if ( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
	    {
		bug( "do_brandish: char died", 0 );
		return;
	    }  
	}
    }

    if ( --staff->value[2] <= 0 )
    {
	act( AT_MAGIC, "$p blazes bright and vanishes from $n's hands!", ch, staff, NULL, TO_ROOM );
	act( AT_MAGIC, "$p blazes bright and is gone!", ch, staff, NULL, TO_CHAR );
	if ( staff->serial == cur_obj )
	  global_objcode = rOBJ_USED;
	extract_obj( staff );
    }

    return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;
    ch_ret retcode;

    one_argument( argument, arg );
    if ( arg[0] == '\0' && !ch->fighting )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }

    if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting )
	{
	    victim = who_fighting( ch );
	}
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
	if ( victim )
	{
          if ( !oprog_use_trigger( ch, wand, victim, NULL, NULL ) )
          {
	    act( AT_MAGIC, "$n aims $p at $N.", ch, wand, victim, TO_ROOM );
	    act( AT_MAGIC, "You aim $p at $N.", ch, wand, victim, TO_CHAR );
          }
	}
	else
	{
          if ( !oprog_use_trigger( ch, wand, NULL, obj, NULL ) )
          {
	    act( AT_MAGIC, "$n aims $p at $P.", ch, wand, obj, TO_ROOM );
	    act( AT_MAGIC, "You aim $p at $P.", ch, wand, obj, TO_CHAR );
          }
	}

	retcode = obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
	if ( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
	{
	   bug( "do_zap: char died", 0 );
	   return;
	}
    }

    if ( --wand->value[2] <= 0 )
    {
      act( AT_MAGIC, "$p explodes into fragments.", ch, wand, NULL, TO_ROOM );
      act( AT_MAGIC, "$p explodes into fragments.", ch, wand, NULL, TO_CHAR );
      if ( wand->serial == cur_obj )
        global_objcode = rOBJ_USED;
      extract_obj( wand );
    }

    return;
}

/*
 * Save items in a clan storage room			-Scryn & Thoric
 */
void save_clan_storeroom( CHAR_DATA *ch, CLAN_DATA *clan )
{
    FILE *fp;
    char filename[256];
    sh_int templvl;
    OBJ_DATA *contents;

    if ( !clan )
    {
	bug( "save_clan_storeroom: Null clan pointer!", 0 );
	return;
    }

    if ( !ch )
    {
	bug ("save_clan_storeroom: Null ch pointer!", 0);
	return;
    }

    sprintf( filename, "%s%s.vault", CLAN_DIR, clan->filename );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
	bug( "save_clan_storeroom: fopen", 0 );
	perror( filename );
    }
    else
    {
	templvl = ch->level;
	ch->level = LEVEL_HERO;		/* make sure EQ doesn't get lost */
        contents = ch->in_room->last_content;
        if (contents)
	  fwrite_obj(ch, contents, fp, 0, OS_CARRY );
	fprintf( fp, "#END\n" );
	ch->level = templvl;
	fclose( fp );
	return;
    }
    return;
}

/* put an item on auction, or see the stats on the current item or bet */
void do_auction (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument (argument, arg1);

    if (IS_NPC(ch)) /* NPC can be extracted at any time and can't auction!*/
	return;

    if (arg1[0] == '\0')
    {
        if (auction->item != NULL)
        {
  	    obj = auction->item;

            /* show item data here */
            if (auction->bet > 0)
                sprintf (buf, "Current bid on this item is %d gold.\n\r",auction->bet);
            else
                sprintf (buf, "No bids on this item have been received.\n\r");
	    set_char_color ( AT_BLUE, ch );
            send_to_char (buf,ch);

	    if (IS_IMMORTAL(ch))
	    {
		sprintf(buf, "Seller: %s.  Bidder: %s.  Round: %d.\n\r",
                        auction->seller->name, auction->buyer->name,
                        (auction->going + 1));
		send_to_char(buf, ch);
		sprintf(buf, "Time left in round: %d.\n\r", auction->pulse);
		send_to_char(buf, ch);
	    }
            return;
	}
	else
	{
	    set_char_color ( AT_LBLUE, ch );
	    send_to_char ( "\n\rThere is nothing being auctioned right now.  What would you like to auction?\n\r", ch );
	    return;
	}
    }

    if ( IS_IMMORTAL(ch) && !str_cmp(arg1,"stop"))
    if (auction->item == NULL)
    {
        send_to_char ("There is no auction to stop.\n\r",ch);
        return;
    }
    else /* stop the auction */
    {
	set_char_color ( AT_LBLUE, ch );
        sprintf (buf,"Sale of %s has been stopped by an Immortal.",
                        auction->item->short_descr);
        talk_auction (buf);
        obj_to_char (auction->item, auction->seller);
	if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
	    save_char_obj(auction->seller);
        auction->item = NULL;
        if (auction->buyer != NULL && auction->buyer != auction->seller) /* return money to the buyer */
        {
            auction->buyer->gold += auction->bet;
            send_to_char ("Your money has been returned.\n\r",auction->buyer);
        }
        return;
    }

    if (!str_cmp(arg1,"bid") )
        if (auction->item != NULL)
        {
            int newbet;

	    if ( ch == auction->seller)
	    {
		send_to_char("You can't bid on your own item!\n\r", ch);
		return;
	    }

            /* make - perhaps - a bet now */
            if (argument[0] == '\0')
            {
                send_to_char ("Bid how much?\n\r",ch);
                return;
            }

            newbet = parsebet (auction->bet, argument);

	    if (newbet < auction->starting)
	    {
		send_to_char("You must place a bid that is higher than the starting bet.\n\r", ch);
		return;
	    }

	    /* to avoid slow auction, use a bigger amount than 100 if the bet
 	       is higher up - changed to 10000 for our high economy
            */

            if (newbet < (auction->bet + 100))
            {
                send_to_char ("You must at least bid 100 coins over the current bid.\n\r",ch);
                return;
            }

            if (newbet > ch->gold)
            {
                send_to_char ("You don't have that much money!\n\r",ch);
                return;
            }

	    if (newbet > 2000000000)
	    {
		send_to_char("You can't bid over 2 billion coins.\n\r", ch);
		return;
	    }

            /* the actual bet is OK! */

            /* return the gold to the last buyer, if one exists */
            if (auction->buyer != NULL && auction->buyer != auction->seller)
                auction->buyer->gold += auction->bet;

            ch->gold -= newbet; /* substract the gold - important :) */
	    if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		save_char_obj(ch);
            auction->buyer = ch;
            auction->bet   = newbet;
            auction->going = 0;
            auction->pulse = PULSE_AUCTION; /* start the auction over again */

            sprintf (buf,"A bid of %d gold has been received on %s.\n\r",newbet,auction->item->short_descr);
            talk_auction (buf);
            return;


        }
        else
        {
            send_to_char ("There isn't anything being auctioned right now.\n\r",ch);
            return;
        }

/* finally... */
    if ( ms_find_obj(ch) )
	return;

    obj = get_obj_carry (ch, arg1); /* does char have the item ? */

    if (obj == NULL)
    {
        send_to_char ("You aren't carrying that.\n\r",ch);
        return;
    }

    if (obj->timer > 0)
    {
	send_to_char ("You can't auction objects that are decaying.\n\r", ch);
	return;
    }

    argument = one_argument (argument, arg2);

    if (arg2[0] == '\0')
    {
      auction->starting = 0;
      strcpy(arg2, "0");
    }

    if ( !is_number(arg2) )
    {
	send_to_char("You must input a number at which to start the auction.\n\r", ch);
	return;
    }

    if ( atoi(arg2) < 0 )
    {
	send_to_char("You can't auction something for less than 0 gold!\n\r", ch);
 	return;
    }

    if (auction->item == NULL)
    switch (obj->item_type)
    {

    default:
        act (AT_TELL, "You cannot auction $Ts.",ch, NULL, item_type_name (obj), TO_CHAR);
        return;

/* insert any more item types here... items with a timer MAY NOT BE 
   AUCTIONED! 
*/
    case ITEM_LIGHT:
    case ITEM_TREASURE:    
    case ITEM_POTION:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_FOOD:
    case ITEM_PEN:
    case ITEM_BOAT:
    case ITEM_PILL:
    case ITEM_PIPE:
    case ITEM_HERB_CON:
    case ITEM_INCENSE:
    case ITEM_FIRE:
    case ITEM_RUNEPOUCH:
    case ITEM_MAP:
    case ITEM_BOOK:
    case ITEM_RUNE:
    case ITEM_MATCH:
    case ITEM_HERB:
    case ITEM_WEAPON:
    case ITEM_ARMOR:
    case ITEM_STAFF:
    case ITEM_WAND:
    case ITEM_SCROLL:
	separate_obj(obj);
	obj_from_char (obj);
	if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
	    save_char_obj(ch);
	auction->item = obj;
	auction->bet = 0;
	auction->buyer = ch;
	auction->seller = ch;
	auction->pulse = PULSE_AUCTION;
	auction->going = 0;
	auction->starting = atoi(arg2);

	if (auction->starting > 0)
	  auction->bet = auction->starting;

	sprintf (buf, "A new item is being auctioned: %s at %d gold.", obj->short_descr, auction->starting);
	talk_auction (buf);

	return;

    } /* switch */
    else
    {
        act (AT_TELL, "Try again later - $p is being auctioned right now!",ch,auction->item,NULL,TO_CHAR);
	WAIT_STATE( ch, 1.5 * PULSE_VIOLENCE );
        return;
    }
}



/* Make objects in rooms that are nofloor fall - Scryn 1/23/96 */

void obj_fall( OBJ_DATA *obj, bool through )
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    static int fall_count;
    char buf[MAX_STRING_LENGTH];
    static bool is_falling; /* Stop loops from the call to obj_to_room()  -- Altrag */
	
    if ( !obj->in_room || is_falling )
    	return;

    if (fall_count > 30)
    {
    	bug( "object falling in loop more than 30 times", 0 );
	extract_obj(obj);
    	fall_count = 0;
	return;
     }

     if ( IS_SET( obj->in_room->room_flags, ROOM_NOFLOOR )
     &&   CAN_GO( obj, DIR_DOWN )
     &&   !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
     {

	pexit = get_exit( obj->in_room, DIR_DOWN );
    	to_room = pexit->to_room;

    	if (through)
	  fall_count++;
	else
	  fall_count = 0;

	if (obj->in_room == to_room)
	{
	    sprintf(buf, "Object falling into same room, room %d",
		to_room->vnum);
	    bug( buf, 0 );
	    extract_obj( obj );
            return;
	}

	if (obj->in_room->first_person)
	{
	  	act( AT_PLAIN, "$p falls far below...",
			obj->in_room->first_person, obj, NULL, TO_ROOM );
		act( AT_PLAIN, "$p falls far below...",
			obj->in_room->first_person, obj, NULL, TO_CHAR );
	}
	obj_from_room( obj );
	is_falling = TRUE;
	obj = obj_to_room( obj, to_room );
	is_falling = FALSE;

	if (obj->in_room->first_person)
	{
	  	act( AT_PLAIN, "$p falls from above...", 
			obj->in_room->first_person, obj, NULL, TO_ROOM );
		act( AT_PLAIN, "$p falls from above...",
			obj->in_room->first_person, obj, NULL, TO_CHAR );
	}

 	if (!IS_SET( obj->in_room->room_flags, ROOM_NOFLOOR ) && through )
	{
/*		int dam = (int)9.81*sqrt(fall_count*2/9.81)*obj->weight/2;
*/		int dam = fall_count*obj->weight/2;
		/* Damage players */
		if ( obj->in_room->first_person && number_percent() > 15 )
		{
			CHAR_DATA *rch;
			CHAR_DATA *vch = NULL;
			int chcnt = 0;
			
			for ( rch = obj->in_room->first_person; rch;
				rch = rch->next_in_room, chcnt++ )
				if ( number_range( 0, chcnt ) == 0 )
					vch = rch;
			act( AT_WHITE, "$p falls on $n!", vch, obj, NULL, TO_ROOM );
			act( AT_WHITE, "$p falls on you!", vch, obj, NULL, TO_CHAR );
			damage( vch, vch, dam*vch->level, TYPE_UNDEFINED );
		}
    	/* Damage objects */
	    switch( obj->item_type )
     	    {
	     	case ITEM_WEAPON:
		case ITEM_ARMOR:
		    if ( (obj->value[0] - dam) <= 0 )
 		    {
   			if (obj->in_room->first_person)
			{
			act( AT_PLAIN, "$p is destroyed by the fall!", 
				obj->in_room->first_person, obj, NULL, TO_ROOM );
			act( AT_PLAIN, "$p is destroyed by the fall!",
				obj->in_room->first_person, obj, NULL, TO_CHAR );
			}
			make_scraps(obj);
	 	    }
		    else
	           	obj->value[0] -= dam;
		    break;
		default:
		    if ( (dam*15) > get_obj_resistance(obj) )
		    {
	              if (obj->in_room->first_person)
		      {
 			    act( AT_PLAIN, "$p is destroyed by the fall!",
			    	obj->in_room->first_person, obj, NULL, TO_ROOM );
			    act( AT_PLAIN, "$p is destroyed by the fall!",
		    		obj->in_room->first_person, obj, NULL, TO_CHAR );
		      }
		      make_scraps(obj);
		    }
		    break;
	    }
     	}
     	obj_fall( obj, TRUE );
    }
    return;
}

/* 
 * Old Sheath and Draw Code added in on 8-7-97 
 */
void do_sheath( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;    /* weapon */
    OBJ_DATA *obj1;   /* belt */
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sheath what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc == WEAR_WIELD )
	send_to_char( "", ch );
    else if ( obj->wear_loc == WEAR_DUAL_WIELD )
	send_to_char( "", ch );
    else
    {
	send_to_char( "You do not have that item.\n\r", ch );
     	return;
    }

    /* is it a belt? if not, returns... */
    if ( ( obj1 = get_eq_char(ch, WEAR_WAIST) ) == NULL
    ||     obj1->item_type != ITEM_SWORD_BELT )
    {
       	send_to_char( "You need a belt to sheath it in.\n\r", ch );
       	return;
    }

    if ( get_eq_char( ch, WEAR_BELT_L ) != NULL
    &&   get_eq_char( ch, WEAR_BELT_R ) != NULL )
    {
	sprintf( buf, "You already have two sheathed weapon on %s.\n\r", obj1->short_descr );
	send_to_char( buf, ch );
	return;
    }

    /* prevents the player from sheathing the first weapon first */
    if ( get_eq_char( ch, WEAR_DUAL_WIELD ) != NULL 
	&& obj->wear_loc == WEAR_WIELD )
    {
	send_to_char( "You need to sheath the weapon in your left hand first.\n\r", ch );
	return;
    }

    unequip_char( ch, obj );  /* removes the weapon and sheath it*/
    if ( get_eq_char( ch, WEAR_BELT_L ) == NULL )
    {
         sprintf( buf, "$n sheaths %s in %s.\n\r", obj->short_descr, obj1->short_descr ); 
         act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
         sprintf( buf, "You sheath %s in %s.\n\r", obj->short_descr, obj1->short_descr ); 
	 send_to_char( buf, ch );
	 equip_char( ch, obj, WEAR_BELT_L );
	 return;
    }

    if ( get_eq_char( ch, WEAR_BELT_R ) == NULL )
    {
        sprintf( buf, "$n sheaths %s in %s.\n\r", obj->short_descr, obj1->short_descr ); 
        act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
        sprintf( buf, "You sheath %s in %s.\n\r", obj->short_descr, obj1->short_descr ); 
	send_to_char( buf, ch );
	equip_char( ch, obj, WEAR_BELT_R );
	return;
    }

    return;

}

void do_draw( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;    /* weapon */
    OBJ_DATA *obj1;   /* belt */
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Draw what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc == WEAR_BELT_L )
	send_to_char( "", ch );
    else if ( obj->wear_loc == WEAR_BELT_R )
	send_to_char( "", ch );
    else
    {
	send_to_char( "You do not have that item.\n\r", ch );
     	return;
    }

    /* are the hands free? */
    if ( get_eq_char( ch, WEAR_WIELD ) != NULL &&
	 get_eq_char( ch, WEAR_DUAL_WIELD ) != NULL )	
    {
	send_to_char( "Your hands are currently busy.\n\r", ch );
	return;
    }

    obj1 = get_eq_char( ch, WEAR_WAIST );
    if ( obj1 == NULL ) return;

    if ( get_eq_char( ch, WEAR_WIELD ) != NULL )
    {
    	if ( (get_obj_weight (obj)*2) > get_obj_weight(get_eq_char(ch,WEAR_WIELD)) )
    	{
            send_to_char ("It is too heavy for you to draw from your belt.\n\r",ch);
            return;
    	}

    	unequip_char( ch, obj );  /* removes the weapon and draw it*/
    	if ( get_eq_char( ch, WEAR_DUAL_WIELD ) == NULL )
        {
	    sprintf( buf, "$n draws $p from %s.", obj1->short_descr );
   	    act( AT_ACTION, buf, ch, obj, NULL, TO_ROOM );

	    sprintf( buf, "You draw $p from %s.", obj1->short_descr );
   	    act( AT_ACTION, buf, ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_DUAL_WIELD );
	    return;
	}

    }
    else
    {
    	unequip_char( ch, obj );  /* removes the weapon and draw it*/
    	if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
        {
	    sprintf( buf, "$n draws $p from %s.", obj1->short_descr );
   	    act( AT_ACTION, buf, ch, obj, NULL, TO_ROOM );

	    sprintf( buf, "You draw $p from %s.", obj1->short_descr );
   	    act( AT_ACTION, buf, ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WIELD );
	    return;
	}
    }
    return;
}
