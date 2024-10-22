/*************************************************************************
* Dyre MUD 1.0 is copyright 1996                                         *
* Dyre MUD is brought to you by :                                        *
*  George Zolas     - Froth                                              *
*  Nathan Lefebvre  - Pinto                                              *
*  Joe Fresch       - JEB                                                *
*  Jonathan Gray    - Hordar                                             *
* Mobguard was written by George Zolas.  It is based on Helix's          *
* trap.c code.                                                           *
* We stayed up really, really  late for this thing...don't screw us.     *
*                                                                        *
*                              Filename:                                 *
*                             mobguard.c                                 *
*************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"

/* decalare local functions */

void mob_guard_act(CHAR_DATA *mob, CHAR_DATA *ch);
bool char_has_pass ( CHAR_DATA *ch, CHAR_DATA *mob );

bool checkmobguard(CHAR_DATA *ch, int dir)
{
  CHAR_DATA *mob ;
  CHAR_DATA *mob_next;
  bool found =0;
  

  if (IS_NPC(ch) )
	return FALSE;

 for (mob = ch->in_room->people; mob != NULL; mob = mob_next)

	 {

	 mob_next = mob->next_in_room;

	  if( (IS_SET (mob->off_flags   , MOB_IS_GUARD            ))
	  &&  (!IS_SET(mob->guard_action, GUARD_ACTION_CHECK_ITEM )) )
	  found = TRUE;

	  if( (IS_SET (mob->off_flags   , MOB_IS_GUARD            ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_ITEM ))
	  &&  ( !char_has_pass( ch, mob )                 ) )
	  found = TRUE;

	  if( (IS_SET (mob->off_flags   , MOB_IS_GUARD            ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_ITEM ))
	  &&  ( char_has_pass( ch, mob )                 ) )
	  {
	  act("You show $N your pass, and $E let's you by. ", ch, NULL, mob, 
		TO_CHAR);
	  act("$N checks $n's pass.  It's good.", ch, NULL, mob, TO_ROOM);
	  found = FALSE;
 	  }

/* all above checks were released to mailing list */
/* stuff specific to Dyre MUD added below - Froth */

/* check character's class */
	
	if( (IS_SET (mob->off_flags   , MOB_IS_GUARD        ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_CLASS ))
	  &&  ( ch->class == mob->pass_vnum                         ) )
	  {
	  act("$N considers your profession for a moment, then $E let's you by. ", 
		ch, NULL, mob, TO_CHAR);
	  act("$N considers $n's profession.", ch, NULL, mob, TO_ROOM);

	  found = FALSE;
 	  }

	if( (IS_SET (mob->off_flags   , MOB_IS_GUARD        ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_CLASS ))
	  &&  !( ch->class == mob->pass_vnum                         ) )
	  found = TRUE;

/* check char's race */
	
	if( (IS_SET (mob->off_flags   , MOB_IS_GUARD        ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_RACE ))
	  &&  ( ch->race == mob->pass_vnum         ) )
	  {
	  act(
"$N checks your race, nods at you, then let's you by. ", 
		ch, NULL, mob, TO_CHAR);
	  act("$N considers $n's race.", ch, NULL, mob, TO_ROOM);

	  found = FALSE;
 	  }

	if( (IS_SET (mob->off_flags   , MOB_IS_GUARD        ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_RACE ))
	  &&  !( ch->race == mob->pass_vnum         ) )
	  found = TRUE;

/* check a character's sect affiliation!!! */	

	if( (IS_SET (mob->off_flags, MOB_IS_GUARD           ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_SECT ))
	  &&  ( ch->clan == mob->pass_vnum         ) )
	  {
	  act("$N asks for your gods True Name, then $E let's you by. ", 
		ch, NULL, mob, TO_CHAR);
	  act("$N considers what god $n's worships .", ch, NULL, mob, TO_ROOM);
	  found = FALSE;
 	  }
	if( (IS_SET (mob->off_flags, MOB_IS_GUARD           ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_SECT ))
	  &&  !( ch->clan == mob->pass_vnum         ) )
	found = TRUE;

/* check a character's level!! */

	if( (IS_SET (mob->off_flags, MOB_IS_GUARD           ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_LEVEL ))
	  &&  ( ch->level <= mob->pass_vnum         ) )
	  {
	  act("$N guages your level of experience, then $E let's you by. ", 
		ch, NULL, mob, TO_CHAR);
	  act("$N considers $n's level of experience.", ch, NULL, mob, TO_ROOM);
	  found = FALSE;
 	  }
	if( (IS_SET (mob->off_flags, MOB_IS_GUARD           ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_LEVEL ))
	  &&  !( ch->level <= mob->pass_vnum         ) )
	  found = TRUE;

/* check a character's age */

	if( (IS_SET (mob->off_flags, MOB_IS_GUARD           ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_AGE ))
	  &&  ( GET_AGE(ch) <= mob->pass_vnum         ) )
	  {
	  act("$N ponders your age for a moment, then $E let's you by. ", 
		ch, NULL, mob, TO_CHAR);
	  act("$N considers $n's age.", ch, NULL, mob, TO_ROOM);
	  found = FALSE;
 	  }

	if( (IS_SET (mob->off_flags, MOB_IS_GUARD           ))
	  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_AGE ))
	  &&  !( GET_AGE(ch) <= mob->pass_vnum         ) )
	found = TRUE;

/* check a character's alignment - has three sections */

  if  ( 
      (IS_SET (mob->off_flags, MOB_IS_GUARD)                )
  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_ALIGN) )
  &&  (  
      ( (mob->pass_vnum >= 1  ) && ch->alignment >= 500    )  
  ||  ( (mob->pass_vnum == 0  ) && ( ch->alignment > -500 
	                        &&   ch->alignment < 500  )) 
  ||  ( (mob->pass_vnum <= -1 ) && ch->alignment <= -500   )    )   )
  {
  act("$N considers how you view the world, then $E let's you by. ", 
	ch, NULL, mob, TO_CHAR);
  act("$N considers how $n views the world.", ch, NULL, mob, TO_ROOM);
  found = FALSE;
  }
  if  ( 
      (IS_SET (mob->off_flags, MOB_IS_GUARD)                )
  &&  (IS_SET (mob->guard_action, GUARD_ACTION_CHECK_ALIGN) )
  &&  !(  
      ( (mob->pass_vnum >= 1  ) && ch->alignment >= 500    )  
  ||  ( (mob->pass_vnum == 0  ) && ( ch->alignment > -500 
	                        &&   ch->alignment < 500  )) 
  ||  ( (mob->pass_vnum <= -1 ) && ch->alignment <= -500   )    )   )

	found = TRUE;

/* call appropriate function if player tried to move */

		 if (found == TRUE)
		 {
		  if (IS_SET(mob->guard_dir, MOB_GUARD_DIR_NORTH)
		  && dir == 0)
		  {
		  mob_guard_act(mob, ch);
		  return TRUE;
		  }

		  if (IS_SET(mob->guard_dir, MOB_GUARD_DIR_EAST)
		  && dir == 1)
		  {
		  mob_guard_act(mob, ch);
		  return TRUE;
		  }

		  if (IS_SET(mob->guard_dir, MOB_GUARD_DIR_SOUTH)
		  && dir == 2)
		  {
			mob_guard_act(mob, ch);
			 return TRUE;
		  }

		  if (IS_SET(mob->guard_dir, MOB_GUARD_DIR_WEST)
		  && dir == 3)
		  {
			 mob_guard_act(mob, ch);
			 return TRUE;
		  }

		  if (IS_SET(mob->guard_dir, MOB_GUARD_DIR_UP)
		  && dir == 4)
		  {
			mob_guard_act(mob, ch);
			 return TRUE;
		  }

		  if (IS_SET(mob->guard_dir, MOB_GUARD_DIR_DOWN)
		  && dir == 5)
		  {
			mob_guard_act(mob, ch);
			return TRUE;
		  }

		  } 
	  } 
	return FALSE;
}

/* Player tried to enter!!! time to act!!! */


void mob_guard_act(CHAR_DATA *mob, CHAR_DATA *ch)
{
	 ROOM_INDEX_DATA *pRoomIndex;

	  switch(mob->guard_action)
	  {
	  case GUARD_ACTION_STOP_CHAR:
	  act("$N shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$N shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  break;

	  case GUARD_ACTION_ATTACK:
	  act("$N growls at you menacingly.", ch, NULL, mob, TO_CHAR);
	  act("$N growls at $n menacingly.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "You'll get past me over my dead body!");
	  do_emote(mob,"{Bscreams{x and{R attacks{x!");
	  multi_hit(mob, ch,TYPE_UNDEFINED);
	  break;

	  case GUARD_ACTION_TELEPORT:
	  do_say (mob, "You may not enter! BEGONE!!!");
	  act("$N waves his hands at you and chants softly.", 
		ch, NULL, mob, TO_CHAR);
	  act("$N waves his hands at $n and chants softly." , 
		ch, NULL, mob, TO_ROOM);

/* allow for directed teleportation of characters!!!       */
/* this was not included in public release of code - Froth */

	  if (mob->pass_vnum != 0)
	  pRoomIndex = get_room_index(mob->pass_vnum);
	  else
	  pRoomIndex = get_random_room (ch);

	  send_to_char ("You have been teleported!!!\n\r", ch);
	  act("$n vanishes!!!\n\r", ch, NULL, NULL, TO_ROOM);
	  char_from_room(ch);
	  char_to_room(ch, pRoomIndex);
	  act("$n slowly fades into existence.\n\r", ch, NULL, NULL, TO_ROOM);
	  do_look(ch, "auto");
	  break;

	  case GUARD_ACTION_CHECK_ITEM:
	  act("$N asks you for your pass.  You don't have one.", ch, NULL, mob,
TO_CHAR);
	  act("$N then shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$N asks $n for his pass.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "Come back when you have a pass!");
	  break;

/* the above stuff was released tothe world on mailing list */
/* the stuff below is unique to Dyre MUD - Froth            */

	  case GUARD_ACTION_CHECK_CLASS:
	  act("$N considers your class.", ch, NULL, mob, TO_CHAR);
	  act("$N then shoves you back into the room!", ch, NULL, mob, TO_CHAR);
	  act("$N considers $n's class.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room!", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "Your prefession is not allowed in there!");
	  break;

	  case GUARD_ACTION_CHECK_RACE:
	  act("$N looks you up and down.", ch, NULL, mob, TO_CHAR);
	  act("$N then shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$N looks at $n closely.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "Your kind is not allowed in there!");
	  break;

	  case GUARD_ACTION_CHECK_SECT:
	  act("$N asks for your gods True Name.", ch, NULL, mob, TO_CHAR);
	  act("$N then shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$n whispers into $N's ear.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "You aren't a True follower! Get out before I slay you!");
	  break;

	  case GUARD_ACTION_CHECK_LEVEL:
	  act("$N scrutinizes you.", ch, NULL, mob, TO_CHAR);
	  act("$N then shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$N scrutinizes $n.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "Come back when you are more experienced!");
	  break;

	  case GUARD_ACTION_CHECK_AGE:
	  act("$N studies your face intently.", ch, NULL, mob, TO_CHAR);
	  act("$N denies you admittance.", ch, NULL, mob, TO_CHAR);
	  act("$N asks $n for $e age.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "Come back when you are older!");
	  break;

	  case GUARD_ACTION_CHECK_ALIGN:
	  act("$N considers your alignment.", ch, NULL, mob, TO_CHAR);
	  act("$N then shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$N considers $n's alignment.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "People like you aren't allowed in there!");
	  break;

/* added a default, too! */

	  default:
	  act("$N looks you up and down.", ch, NULL, mob, TO_CHAR);
	  act("$N then shoves you back into the room.", ch, NULL, mob, TO_CHAR);
	  act("$N looks at $n closely.", ch, NULL, mob, TO_ROOM);
	  act("$N then shoves $n back into the room.", ch, NULL, mob, TO_ROOM);
	  do_say (mob, "You're not allowed in there!");
	  break;
	}  
return;
}     

bool char_has_pass ( CHAR_DATA *ch, CHAR_DATA *mob )
{
	OBJ_DATA * obj;
	OBJ_DATA * obj_next;
/* 
 *  for bug checking
 *  char buf [MAX_STRING_LENGTH];
 */
	/* this function checks a players inventory for the pass item */

	for (obj = ch->carrying; obj != NULL ; obj = obj_next )
	{
	obj_next = obj->next_content;

/* for bug checking	
 *	sprintf ( buf, "ch->carrying->vnum: %d Mob pass vnum; %d \n\r",
 *	obj->pIndexData->vnum, mob->pass_vnum );
 *	send_to_char(buf, ch );
 * not used now
 */

	if ( obj->pIndexData->vnum == mob->pass_vnum )
	break;
	}
	if (!obj)
	return FALSE;
	else
	return TRUE;
   	
}
