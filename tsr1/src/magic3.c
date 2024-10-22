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
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/***************************************************************************
 *									   *
 *       TSR MUD spells recoded and redone by Nikki & Beowolf. The Code    *
 *	 was taken from Envy and Silly MUD code & special thanks goes      *
 *	 out to those very cool code bases.				   *
 *									   *
 **************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"

/*
 * Local Functions needed by spells or skills
 */ 

void    exec_fear       args( ( CHAR_DATA *ch ) );


/*
 * Code for Psionicist spells/skills by Thelonius
 */
void spell_adrenaline_control ( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA  *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
     
   if ( is_affected( victim, sn ) )
   return;
     
         af.type      = sn;
         af.duration  = level - 5;
         af.location  = APPLY_DEX;
         af.modifier  = 2;
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         af.location = APPLY_CON;
         affect_to_char( victim, &af );
     
         send_to_char( "You have given yourself an adrenaline rush!\n\r", 
     ch );
         act( "$n has given $mself an adrenaline rush!", ch, NULL, NULL,
             TO_ROOM );
        
         return;
     }

void spell_biofeedback ( int sn, int level, CHAR_DATA *ch, void *vo, int target)
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
        
         if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
             return;
     
         af.type      = sn;
         af.duration  = number_fuzzy( level / 8 );
         af.location  = APPLY_NONE;
         af.modifier  = 0;
         af.bitvector = AFF_SANCTUARY;
         affect_to_char( victim, &af );
     
         send_to_char( "You are surrounded by a white aura.\n\r", victim );
         act( "$n is surrounded by a white aura.", victim, NULL, NULL, 
     TO_ROOM );
         return;
     }
     
     void spell_cell_adjustment ( int sn, int level, CHAR_DATA *ch, void 
     *vo, int target)
     {
         CHAR_DATA *victim = (CHAR_DATA *) vo;
        
         if ( is_affected( victim, gsn_poison ) )
         {
             affect_strip( victim, gsn_poison );
             send_to_char( "A warm feeling runs through your body.\n\r", 
     victim );
             act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );
         }
         if ( is_affected( victim, gsn_curse  ) )
         {
             affect_strip( victim, gsn_curse  );
             send_to_char( "You feel better.\n\r", victim );
         }   
         send_to_char( "Ok.\n\r", ch );
         return;
     }
     
void spell_combat_mind ( int sn, int level, CHAR_DATA *ch, void *vo, int target)
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( is_affected( victim, sn ) )
         {
             if ( victim == ch )
               send_to_char( "You already understand battle tactics.\n\r",
                            victim );
             else
               act( "$N already understands battle tactics.",
                   ch, NULL, victim, TO_CHAR );
             return;
         }
     
         af.type      = sn;
         af.duration  = level + 3;
         af.location  = APPLY_HITROLL;
         af.modifier  = level / 6;
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         af.location  = APPLY_AC;
         af.modifier  = - level/2 - 5;
         affect_to_char( victim, &af );
     
         if ( victim != ch )
             send_to_char( "OK.\n\r", ch );
         send_to_char( "You gain a keen understanding of battle 
     tactics.\n\r",
                      victim );
         return;
     }
     
void spell_complete_healing ( int sn, int level, CHAR_DATA *ch, void 
     *vo, int target)
     {
         CHAR_DATA *victim = (CHAR_DATA *) vo;
     
         victim->hit = victim->max_hit;
         update_pos( victim );
         if ( ch != victim )
             send_to_char( "Ok.\n\r", ch );
         send_to_char( "Ahhhhhh...You are completely healed!\n\r", victim 
     );
         return;
     }

void spell_disintegrate ( int sn, int level, CHAR_DATA *ch, void *vo, int target)
     {
         CHAR_DATA *victim = (CHAR_DATA *) vo;
         OBJ_DATA  *obj_lose;
         OBJ_DATA  *obj_next;
     
         if ( number_percent( ) < 2 * level && !saves_spell( level, victim, DAM_ENERGY ) )
           for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next 
     )
           {
               obj_next = obj_lose->next_content;
     
               if ( number_bits( 2 ) != 0 )
                   continue;
     
               act( "$p disintegrates!",      victim, obj_lose, NULL, 
     TO_CHAR );
               act( "$n's $p disintegrates!", victim, obj_lose, NULL, 
     TO_ROOM );
               extract_obj( obj_lose ) ;
           }
     
         if ( !saves_spell( level, victim, DAM_ENERGY ) )
         /*
          * Disintegrate char, do not generate a corpse, do not
          * give experience for kill.  Extract_char will take care
          * of items carried/wielded by victim.  Needless to say,
          * it would be bad to be a target of this spell!
          * --- Thelonius (Monk)
          */
         {
             act( "You have DISINTEGRATED $N!",         ch, NULL, victim, 
     TO_CHAR );
             act( "You have been DISINTEGRATED by $n!", ch, NULL, victim, 
     TO_VICT );
             act( "$n's spell DISINTEGRATES $N!",       ch, NULL, victim, 
     TO_ROOM );
             
             if ( IS_NPC( victim ) )
                 extract_char( victim, TRUE );
             else
                 extract_char( victim, FALSE );
         }
         return;
     }
     
     
void spell_displacement ( int sn, int level, CHAR_DATA *ch, void *vo, int target)
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( is_affected( victim, sn ) )
             return;
     
         af.type      = sn;
         af.duration  = level - 4;
         af.location  = APPLY_AC;
         af.modifier  = 4 - level;
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         send_to_char( "Your form shimmers, and you appear displaced.\n\r",
                      victim );
         act( "$N shimmers and appears in a different location.",
             ch, NULL, victim, TO_NOTVICT );
         return;
     }

     
void spell_domination ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( victim == ch )
         {
             send_to_char( "Dominate yourself?  You're weird.\n\r", ch );
             return;
         }
     
         if (   IS_AFFECTED( victim, AFF_CHARM )
             || IS_AFFECTED( ch,     AFF_CHARM )
             || level < victim->level
             || saves_spell( level, victim, DAM_ENERGY ) )
             return;
     
         if ( victim->master )
             stop_follower( victim );
         add_follower( victim, ch );
     
         af.type      = sn;
         af.duration  = number_fuzzy( level / 4 );
         af.location  = APPLY_NONE;
         af.modifier  = 0;
         af.bitvector = AFF_CHARM;
         affect_to_char( victim, &af );
     
         act( "Your will dominates $N!", ch, NULL, victim, TO_CHAR );
         act( "Your will is dominated by $n!", ch, NULL, victim, TO_VICT );
         return;
     }
     
void spell_ectoplasmic_form ( int sn, int level, CHAR_DATA *ch, void 
     *vo , int target )
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
             return;
     
         af.type      = sn;
         af.duration  = number_fuzzy( level / 4 );
         af.location  = APPLY_NONE;
         af.modifier  = 0;
         af.bitvector = AFF_PASS_DOOR;
         affect_to_char( victim, &af );
     
         send_to_char( "You turn translucent.\n\r", victim );
         act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
         return;
     }
     
     
void spell_ego_whip ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( is_affected( victim, sn ) || saves_spell( level, victim, DAM_ENERGY ) )
             return;
     
         af.type      = sn;
         af.duration  = level;
         af.location  = APPLY_HITROLL;
         af.modifier  = -2;
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         af.location  = APPLY_SAVING_SPELL;
         af.modifier  = 2;
         affect_to_char( victim, &af );
     
         af.location  = APPLY_AC;
         af.modifier  = level / 2;
         affect_to_char( victim, &af );
     
         act( "You ridicule $N about $S childhood.", ch, NULL, victim, 
     TO_CHAR    );
         send_to_char( "Your ego takes a beating.\n\r", victim );
         act( "$N's ego is crushed by $n!",          ch, NULL, victim, 
     TO_NOTVICT );
     
         return;
     }
     
     
     
void spell_energy_containment ( int sn, int level, CHAR_DATA *ch, void 
     *vo , int target )
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( is_affected( victim, sn ) )
             return;
     
         af.type      = sn;
         af.duration  = level / 2 + 7;
         af.modifier  = -level / 5;
         af.location  = APPLY_SAVING_SPELL;
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         send_to_char( "You can now absorb some forms of energy.\n\r", ch 
     );
         return;
     }
     
void spell_enhanced_strength ( int sn, int level, CHAR_DATA *ch, void 
     *vo , int target )
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( is_affected( victim, sn ) )
             return;
     
         af.type      = sn;
         af.duration  = level;
         af.location  = APPLY_STR;
         af.modifier  = 1 + ( level >= 15 ) + ( level >= 25 );
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         send_to_char( "You are HUGE!\n\r", victim );
         return;
     }
     
     
     
void spell_flesh_armor ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
     {
         CHAR_DATA  *victim = (CHAR_DATA *) vo;
         AFFECT_DATA af;
     
         if ( is_affected( victim, sn ) )
             return;
     
         af.type      = sn;
         af.duration  = level;
         af.location  = APPLY_AC;
         af.modifier  = -40;
         af.bitvector = 0;
         affect_to_char( victim, &af );
     
         send_to_char( "Your flesh turns to steel.\n\r", victim );
         act( "$N's flesh turns to steel.", ch, NULL, victim, TO_NOTVICT);
         return;
     }
     
void spell_water_breath(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected( victim, sn ) )
    {
	if (victim == ch)
	  send_to_char("You can already breathe underwater.\n\r", ch);
        else
	  act("$N can already breathe underwater.",ch,NULL,victim,TO_CHAR);
	return;
    }

af.where	= TO_AFFECTS;
af.type		= sn;
af.level	= level;
af.duration	= level * 2;
af.modifier	= 0;
af.location	= 0;
af.bitvector	= AFF_WATER_BREATH;
affect_to_char(victim, &af);
send_to_char("You are now ready to breathe underwater!\n\r", victim);
if ( ch != victim )
    act("$N develops gills on $s neck.", ch, NULL,victim,TO_CHAR);
return;
}

/* 
 * Flying Carpet spell from Sabrina Saraco (orion@shadow.net) in trade
 * for OLC.
 */

void spell_flying_carpet( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *flyingcarpet; 
    /*OBJ_DATA *obj;*/
    int caster;
    caster = get_curr_stat(ch,STAT_INT);

if ( caster < 18)
    {
    send_to_char("Your mind is not strong enough to create a magical object.\n\r",ch);
    ch->mana = ch->mana -dice(10,5);
    WAIT_STATE(ch,number_range(3,6) * PULSE_VIOLENCE);    
    return;
    }
/*if ( IS_AFFECTED(ch, AFF_FROZEN))
     { 
       send_to_char("Magical ice can't cast spells.\n\r",ch);     
       ch->position = POS_STUNNED;
       return;
     }*/
    flyingcarpet = create_object( get_obj_index( OBJ_VNUM_FLYINGCARPET ), 0 );
    obj_to_room( flyingcarpet, ch->in_room );
    flyingcarpet->timer = caster*3;
    act( "$n gestures and $p materializes.", ch, flyingcarpet, NULL, TO_ROOM );
    act( "You gesture and $p materializes.", ch, flyingcarpet, NULL, TO_CHAR );
    ch->mana = ch->mana -(ch->max_mana/3);
    if (ch->level < 91)
      WAIT_STATE(ch,number_range(3,6) * PULSE_VIOLENCE);
    return;
}

void spell_mute( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_FLAG2 ( victim, AFF_MUTE ) )
    {
	if (victim == ch)
	  send_to_char("You have already cast that!\n\r", ch);
        else
	  act("$N tries to mute the room again!.",ch,NULL,victim,TO_CHAR);
	return;
    }

af.where	= TO_FLAG2;
af.type		= sn;
af.level	= level;
af.duration	= level;
af.modifier	= 0;
af.location	= 0;
af.bitvector	= AFF_MUTE;
affect_to_char(victim, &af);
send_to_char("You have silenced $N!\n\r", victim);
if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    act( "$n has silenced you!",  ch, NULL, victim, TO_VICT    );
    act( "$n has silenced $N!",   ch, NULL, victim, TO_NOTVICT );
return;
}

void do_disguise( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    char arg1[MAX_INPUT_LENGTH];    

    smash_tilde( arg1 );
    strcpy( arg1, argument );
    
    if (IS_IMMORTAL(ch)) /* always works */
     send_to_char( "", ch );

    else
    {
	send_to_char( "You don't know how to disguise yourself.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
        if (is_affected(ch, skill_lookup("disguise")))
        {
	    send_to_char("You change back into your normal appearance.\n\r", ch);
            affect_strip( ch, gsn_disguise );
            /* free_string( ch->short_descr );*/
	    ch->short_descr = str_dup( ch->name );
	}
	else
	    send_to_char("Disguise yourself as what?\n\r", ch );

    	return;
    }

    if ( number_percent( ) < get_skill(ch,gsn_disguise))
    {
	af.where     = TO_AFFECTS;
	af.type      = gsn_disguise;
	af.level     = ch->level; 
	af.duration  = -1;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = 0;
	affect_to_char( ch, &af );

    	/* ok, lets change it */
    	free_string( ch->short_descr );
    	ch->short_descr = str_dup( arg1 );
    	act( "You change your appearance to $n.", ch, NULL, NULL, TO_CHAR );
    }
    else
    {
	send_to_char( "You failed to disguise yourself perfectly.\n\r", ch );
	check_improve(ch,gsn_disguise,FALSE,3);
    }
    return;
}

void spell_bloodlust( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim,DAM_NEGATIVE) )
    {
	send_to_char("The spell fizzles and dies.\n\r", ch);
	return;
    }

    if ( IS_AFFECTED(victim, AFF_BLOODLUST) )
    {
	send_to_char("They already lust for blood.\n\r",ch);
	return;
    }
   
    af.where     = TO_FLAG2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 8;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_BLOODLUST;
    affect_to_char( victim, &af );
    send_to_char( "You have an overwhelming thirst for blood.\n\r", victim );
    act( "$n clutches $s throat and gasps for blood.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_fear( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim,DAM_NEGATIVE) )
    {
	send_to_char("The spell fizzles and dies.\n\r", ch);
	return;
    }

    if ( IS_FLAG2(victim, AFF_FEAR) )
	{
	send_to_char("They are already running for their lives.\n\r",ch);
	return;
	}

    af.where     = TO_FLAG2;
    af.type      = sn;
    af.level     = level;
    af.duration  = 1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FEAR;
    affect_to_char( victim, &af );
    send_to_char( "You panic as you are gripped by an incredible fear.\n\r", victim );
    act( "$n screams and runs away.", victim, NULL, NULL, TO_ROOM );
    exec_fear(victim);
    return;
}

void spell_exhausted( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;


    victim = (CHAR_DATA *) vo;

  if ( saves_spell( level, victim,DAM_NONE) )
  {
      act("$n starts to feel tired, but it passes.",victim,NULL,NULL,TO_ROOM);
      send_to_char("You feel momentarily exhausted, but it passes.\n\r",victim);
      return;
  }
    af.where     = TO_FLAG2;
    af.type      = sn;
    af.level     = level;
    af.duration  = 20;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_EXHAUSTED;
    affect_join( victim, &af );
    send_to_char( "You feel very safe.\n\r", victim );
    act("$n looks very safe.",victim,NULL,NULL,TO_ROOM);
    return;
}
