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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /* unlink() */
#include "merc.h"
#include "interp.h"


bool	check_social	args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );
bool  check_disabled (const struct cmd_type *command);

DISABLED_DATA *disabled_first;

#define END_MARKER    "END" /* for load_disabled() and save_disabled() */



/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2



/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{

    /*
     * Common movement commands.
     */
    { "north",		do_north,	POS_STANDING,    0,  LOG_NEVER, 0 },
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "sail",		do_sail,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "port",		do_port,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "ride",		do_ride,	POS_STANDING,	 0,  LOG_NEVER, 0 },
    { "stop",		do_stop,	POS_STANDING,	 0,  LOG_NEVER, 0 },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "auction",        do_auction,     POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "buy",		do_buy,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "channels",       do_channels,    POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "group",          do_group,       POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "hit",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "lore",		do_lore,	POS_RESTING,	 0,  LOG_NORMAL, 1 }, 
    { "dev",		do_devotiontalk,POS_SLEEPING,	10,  LOG_NORMAL, 1 },
    { "music",          do_music,   	POS_SLEEPING,    0,  LOG_NORMAL, 1 }, 
    { "order",		do_order,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "practice",       do_practice,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "rest",		do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "sit",		do_sit,		POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "btell",		do_btell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "unlock",         do_unlock,      POS_RESTING,     0,  LOG_NORMAL, 1 },
    { "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "quest",		do_quest,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "disguise",	do_disguise,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
   
    /*
     * COLOR
     */

     { "color",		do_color,	POS_DEAD,     0,  LOG_NORMAL, 1 },

     /*
      * Arena Commands
      */

    { "bet",        do_bet,       POS_RESTING,   0,   LOG_NORMAL, 0 },
    { "accept",     do_accept,    POS_STANDING,  0,   LOG_NORMAL, 0 },
    { "decline",    do_decline,   POS_SLEEPING,  0,   LOG_NORMAL, 0 },
    { "challenge",  do_challenge, POS_STANDING,  0,   LOG_NORMAL, 0 },

    /*
     * Informational commands.
     */
    { "affects",	do_affects,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "letter",	        do_letter,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "count",		do_count,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "finger", 	do_finger,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "motd",		do_motd,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "news",		do_news,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "read",		do_read,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "report",		do_report,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "rules",		do_rules,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "scan",		do_scan,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "skills",		do_skills,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "show",		do_show,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "spells",		do_spells,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "story",		do_story,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "wizlist",	do_wizlist,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "worth",		do_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },

    /*
     * Configuration commands.
     */
    { "alia",		do_alia,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "alias",		do_alias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "autolist",	do_autolist,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "autoassist",	do_autoassist,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autoexit",	do_autoexit,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autogold",	do_autogold,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autoloot",	do_autoloot,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autosac",	do_autosac,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autosplit",	do_autosplit,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "brief",		do_brief,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "combine",	do_combine,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "compact",	do_compact,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "delet",		do_delet,	POS_DEAD,	 0,  LOG_ALWAYS, 0 },
    { "delete",		do_delete,	POS_STANDING,	 0,  LOG_ALWAYS, 1 },
    { "pkill",          do_pkill,       POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "nofollow",	do_nofollow,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "noloot",		do_noloot,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "nosummon",	do_nosummon,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "outfit",		do_outfit,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER,  1 },
    { "prompt",		do_prompt,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "scroll",		do_scroll,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "unalias",	do_unalias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 1 },

    /*
     * Communication commands.
     */
    { "afk",		do_afk,		POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "deaf",		do_deaf,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pmote",		do_pmote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { ".",		do_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "gossip",		do_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "grats",		do_grats,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "info",		do_info,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "ooc",         	do_ooc,          POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "quote",		do_quote,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "quiet",		do_quiet,	POS_SLEEPING, 	 0,  LOG_NORMAL, 1 },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "replay",		do_replay,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "shout",		do_shout,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "unread",		do_unread,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "yell",		do_yell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pray",		do_pray,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "newbie",		do_newbie,	POS_RESTING,	 0,  LOG_NORMAL, 1 },

    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "envenom",	do_envenom,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "heal",		do_heal,	POS_RESTING,	 0,  LOG_NORMAL, 1 }, 
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "list",		do_list,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pour",		do_pour,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sell",		do_sell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "dual",		do_second,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "donate",  	do_donate,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "junk",           do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, 0 },
    { "tap",      	do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, 0 },   
    { "value",		do_value,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL, 1 },

    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "bash",		do_bash,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },
    { "bs",		do_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "circle",	        do_circle,	POS_FIGHTING,   75,  LOG_NORMAL, 1 },
    { "dirt",		do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "murde",		do_murde,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "murder",		do_murder,	POS_FIGHTING,	 5,  LOG_ALWAYS, 1 },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "track",		do_track,	POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "trip",		do_trip,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },

    { "chameleon", 	do_chameleon, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "heighten",	do_heighten, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "shadow",         do_heighten, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "snare", 		do_snare, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "untangle", 	do_untangle, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    
     /*
      * Mob command interpreter (placed here for faster scan...)
      */
   { "mob",		do_mob,		POS_DEAD,	 0,  LOG_NEVER,  0 },
   { "surrender",	do_surrender,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },
  
 
    /*
     * Miscellaneous commands.
     */
    { "enter", 		do_enter, 	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "gain",		do_gain,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "groups",		do_groups,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "go",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, 0 },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "play",		do_play,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "save",		do_save,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "push",           do_push,        POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "drag",           do_drag,        POS_STANDING,    0,  LOG_NORMAL, 1 },
   
    /* 
     * Bank Commands Added on 5-22-96
     */

    { "balance",	do_balance,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "deposit",	do_deposit,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "withdraw",	do_withdraw,	POS_RESTING,	 0,  LOG_NORMAL, 1 },

    /*
     * Language Code
     */

    { "learn",          do_learn,       POS_RESTING,     0,  LOG_NORMAL, 1 },
    { "speak",          do_speak,       POS_RESTING,     0,  LOG_NORMAL, 1 },

    { "sign",           do_sign,        POS_RESTING,     0,  LOG_NORMAL, 1 },
    { "mount",          do_mount,       POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "dismount",       do_dismount,    POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "draw",           do_draw,        POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "sheath",         do_sheath,      POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "remor",		do_remor,	POS_DEAD,	 0,  LOG_ALWAYS, 0 },
    { "remort",		do_remort,	POS_STANDING,	 0,  LOG_ALWAYS, 1 },
    
    /*
     * Immortal commands.
     */

     /* ADMIN COMMANDS */
    { "reboo",		do_reboo,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "reboot",		do_reboot,	POS_DEAD,	ML,  LOG_ALWAYS, 1 },
    { "shutdow",	do_shutdow,	POS_DEAD,	ML,  LOG_NORMAL, 0 },
    { "shutdown",	do_shutdown,	POS_DEAD,	ML,  LOG_ALWAYS, 1 },


     /* IMP COMMANDS */
    { "admin",	        do_admintalk,	POS_DEAD,	L1,  LOG_NORMAL, 1 },
    { "[",	        do_admintalk,	POS_DEAD,	L1,  LOG_NORMAL, 1 },
    { "advance",	do_advance,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "disable",	do_disable,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "dump",		do_dump,	POS_DEAD,	L1,  LOG_ALWAYS, 0 },
    { "permban",	do_permban,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "protect",	do_protect,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "trust",		do_trust,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "violate",	do_violate,	POS_DEAD,	L1,  LOG_ALWAYS, 1 },

    /* CREATOR */
    { "log",		do_log,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },

    /* DEIETY */
    { "allow",		do_allow,	POS_DEAD,	L3,  LOG_ALWAYS, 1 },
    { "wizlock",	do_wizlock,	POS_DEAD,	L3,  LOG_ALWAYS, 1 },

    /* GOD */
    { "ban",		do_ban,		POS_DEAD,	L4,  LOG_ALWAYS, 1 },
    { "restore",	do_restore,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },

    /* IMMORTAL */
    { "deny",		do_deny,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
    { "flag",		do_flag,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
    { "inform",		do_info_god,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
    { "load",		do_load,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },
    { "set",		do_set,		POS_DEAD,	L5,  LOG_ALWAYS, 1 },
    { "sla",		do_sla,		POS_DEAD,	L5,  LOG_NORMAL, 0 },
    { "slay",		do_slay,	POS_DEAD,	L5,  LOG_ALWAYS, 1 },

    /* DEMIGOD */
    { "clone",		do_clone,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "disconnect",	do_disconnect,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "force",		do_force,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "gecho",		do_echo,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "newlock",	do_newlock,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "pardon",		do_pardon,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "snoop",		do_snoop,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "string",		do_string,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "teleport",	do_transfer,    POS_DEAD,	L6,  LOG_ALWAYS, 1 },	
    { "transfer",	do_transfer,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },

    /* ANGEL */
    { "arenaset",       do_arena,       POS_DEAD,       L7,  LOG_NORMAL, 0 },
    { "at",             do_at,          POS_DEAD,       L7,  LOG_NORMAL, 1 },
    { "join",		do_guild,	POS_DEAD,	L7,  LOG_ALWAYS, 1 },
    { "peace",		do_peace,	POS_DEAD,	L7,  LOG_NORMAL, 1 },
    { "sockets",        do_sockets,	POS_DEAD,       L7,  LOG_NORMAL, 1 },
    { "zecho",		do_zecho,	POS_DEAD,	L7,  LOG_ALWAYS, 1 },

    /* AVATAR */
    { "aset",	        do_aset,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "authorize",	do_authorize,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "echo",		do_recho,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "freeze",		do_freeze,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "godspeak",       do_godspeak,    POS_STANDING,   IM,  LOG_NORMAL, 1 },
    { "goto",           do_goto,        POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "immtalk",	do_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "imotd",          do_imotd,       POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { ":",		do_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "invis",		do_invis,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "lag",		do_lag, 	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "lstat",          do_lstat,       POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "lset",           do_lset,        POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "memory",		do_memory,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "mpdump",		do_mpdump,	POS_DEAD,	IM,  LOG_NEVER,  1 },
    { "mpedit",		do_mpedit,	POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "mplist",         do_mplist,      POS_DEAD,       IM,  LOG_NEVER,  1 },
    { "mpsave",         do_mpsave,      POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "mpstat",		do_mpstat,	POS_DEAD,	IM,  LOG_NEVER,  1 },
    { "mwhere",		do_mwhere,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "nochannels",	do_nochannels,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "noemote",	do_noemote,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "noshout",	do_noshout,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "notell",		do_notell,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "owhere",		do_owhere,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "page",           do_page,	POS_DEAD,	IM,  LOG_NORMAL, 1 }, 
    { "pecho",		do_pecho,	POS_DEAD,	IM,  LOG_ALWAYS, 1 }, 
    { "penalty",	do_penalty,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "poofin",		do_bamfin,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "poofout",	do_bamfout,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "prefi",		do_prefi,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "prefix",		do_prefix,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "purge",		do_purge,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "return",         do_return,      POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "smote",		do_smote,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "stat",		do_stat,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "switch",		do_switch,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
    { "vnum",		do_vnum,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "whois",		do_whois,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "wizinvis",	do_invis,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "wiznet",		do_wiznet,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "zlock",   	do_zlock,	POS_DEAD,	IM,  LOG_ALWAYS, 1 },
  
    /*
     * OLC Building Stuff
     */

    { "alist",		do_alist,	POS_DEAD,       152,  LOG_NORMAL, 1 },
    { "areastat",       do_areastat,    POS_DEAD,       152,  LOG_NORMAL, 1 },
    { "asave",          do_asave,	POS_DEAD,       152,  LOG_ALWAYS, 1 },
    { "bmotd",          do_bmotd,       POS_DEAD,       152,  LOG_NORMAL, 1 },
    { "buildtalk",	do_buildtalk,	POS_DEAD,	152,  LOG_NORMAL, 1 },
    { "-",	        do_buildtalk,	POS_DEAD,	152,  LOG_NORMAL, 1 },
    { "clean",		do_clean,	POS_DEAD,	152,  LOG_NORMAL, 1 },
    { "edit",		do_olc,		POS_DEAD,       152,  LOG_NORMAL, 1 },
    { "forget",         do_forget,      POS_DEAD,       152,  LOG_NORMAL, 1 },
    { "holylight",	do_holylight,	POS_DEAD,	152,  LOG_NORMAL, 1 },
    { "incognito",	do_incognito,	POS_DEAD,	152,  LOG_NORMAL, 1 },
    { "resets",		do_resets,	POS_DEAD,       152,  LOG_NORMAL, 1 },
    { "title",		do_title,	POS_DEAD,	152,  LOG_NORMAL, 1 },
    { "wizhelp",	do_wizhelp,	POS_DEAD,	152,  LOG_NORMAL, 1 },


    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 0 }
};




/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /* reset idle timer */
    ch->idle_start_time = time(NULL);
    
    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name )
	&&   cmd_table[cmd].level <= trust )
	{
	    found = TRUE;
	    break;
	}
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	    send_to_char( "Huh?\n\r", ch );
	return;
    }
    else /* a normal valid command.. check if it is disabled */
      if (check_disabled (&cmd_table[cmd]))
      {
          send_to_char ("This command has been temporarily disabled.\n\r",ch);
              return;
      }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_SITTING:
	    send_to_char( "Better stand up first.\n\r",ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;

	}
	return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
	act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
	act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

	if ( !IS_NPC(ch) && IS_NPC(victim)
	&&   !IS_AFFECTED(victim, AFF_CHARM)
	&&   IS_AWAKE(victim) 
	&&   victim->desc == NULL)
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act( social_table[cmd].others_found,
		    victim, NULL, ch, TO_NOTVICT );
		act( social_table[cmd].char_found,
		    victim, NULL, ch, TO_CHAR    );
		act( social_table[cmd].vict_found,
		    victim, NULL, ch, TO_VICT    );
		break;

	    case 9: case 10: case 11: case 12:
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
		break;
	    }
	}
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{
 
    if ( *arg == '\0' )
        return FALSE;
 
    if ( *arg == '+' || *arg == '-' )
        arg++;
 
    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }
 
    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}

/* 
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    }
 
    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    int cmd;
    int col;
 
    buffer = new_buf(); 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) 
	&&   cmd_table[cmd].show)
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    add_buf( buffer, buf );
	    if ( ++col % 6 == 0 )
	        add_buf( buffer, "\n\r" );
	}
    }
 
    if ( col % 6 != 0 )
	add_buf( buffer, "\n\r" );

   page_to_char (buf_string (buffer), ch);
   free_buf(buffer);
    return;
}

/* void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level >= LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) 
        &&   cmd_table[cmd].show)
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
} */

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  int cmd;
  int col;
  char *tile="";
  
  col = 0;
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
    if ( cmd_table[cmd].level >= LEVEL_HERO
    &&   cmd_table[cmd].level <= get_trust( ch ) 
    &&   cmd_table[cmd].show)
    {
      switch(cmd_table[cmd].level)
      {
	case (MAX_LEVEL - 9):tile = "%"; break;
	case (MAX_LEVEL - 8):tile = "!"; break;
	case (MAX_LEVEL - 7):tile = "9"; break;
	case (MAX_LEVEL - 6):tile = "8"; break;
	case (MAX_LEVEL - 5):tile = "5"; break;
	case (MAX_LEVEL - 4):tile = "4"; break;
	case (MAX_LEVEL - 3):tile = "3"; break;
	case (MAX_LEVEL - 2):tile = "2"; break;
	case (MAX_LEVEL - 1):tile = "1"; break;
	case       MAX_LEVEL:tile = "0"; break;
      }

      sprintf( buf, "(`%s%-2d``) %-14s",tile, cmd_table[cmd].level, cmd_table[cmd].name );
      send_to_char( buf, ch );
      if ( ++col % 4 == 0 )
        send_to_char( "\n\r", ch );
    }
  }
  if ( col % 4 != 0 )
    send_to_char( "\n\r", ch );
  return;
}
/* Syntax is:
disable - shows disabled commands
disable <command> - toggles disable status of command
*/

void do_disable (CHAR_DATA *ch, char *argument)
{
        int i;
        DISABLED_DATA *p,*q;
        char buf[100];
        
        if (IS_NPC(ch))
        {
                send_to_char ("RETURN first.\n\r",ch);
                return;
        }
        
        if (!argument[0]) /* Nothing specified. Show disabled commands. */
        {
                if (!disabled_first) /* Any disabled at all ? */
                {
                        send_to_char ("There are no commands disabled.\n\r",ch)
;
                        return;
                }

                send_to_char ("Disabled commands:\n\r"
                              "Command      Level   Disabled by\n\r",ch);
                
                for (p = disabled_first; p; p = p->next)
                {
                        sprintf (buf, "%-12s %5d   %-12s\n\r",p->command->name,
 p->level, p->disabled_by);
                        send_to_char (buf,ch);
                }
                return;
        }
        
        /* command given */

        /* First check if it is one of the disabled commands */
        for (p = disabled_first; p ; p = p->next)
                if (!str_cmp(argument, p->command->name))
                        break;
                        
        if (p) /* this command is disabled */
        {
        /* Optional: The level of the imm to enable the command must equal or e
xceed level
           of the one that disabled it */
        
                if (get_trust(ch) < p->level)
                {
                        send_to_char ("This command was disabled by a higher po
wer.\n\r",ch);
                        return;
                }
                
                /* Remove */
                
                if (disabled_first == p) /* node to be removed == head ? */
                        disabled_first = p->next;
                else /* Find the node before this one */
                {
                        for (q = disabled_first; q->next != p; q = q->next); /*
 empty for */
                        q->next = p->next;
                }
                
                free_string (p->disabled_by); /* free name of disabler */
                free_mem (p,sizeof(DISABLED_DATA)); /* free node */
                save_disabled(); /* save to disk */
                send_to_char ("Command enabled.\n\r",ch);
        }
        else /* not a disabled command, check if that command exists */
        {
                /* IQ test */
                if (!str_cmp(argument,"disable"))
                {
                        send_to_char ("You cannot disable the disable command.\
n\r",ch);
                        return;
                }

                /* Search for the command */
                for (i = 0; cmd_table[i].name[0] != '\0'; i++)
                        if (!str_cmp(cmd_table[i].name, argument))
                                break;

                /* Found? */
                if (cmd_table[i].name[0] == '\0')
                {
                        send_to_char ("No such command.\n\r",ch);
                        return;
                }

                /* Can the imm use this command at all ? */
        
                if (cmd_table[i].level > get_trust(ch))
                {
                        send_to_char ("You dot have access to that command; you
 cannot disable it.\n\r",ch);
                        return;
                }
                
                /* Disable the command */
                
                p = alloc_mem (sizeof(DISABLED_DATA));

                p->command = &cmd_table[i];
                p->disabled_by = str_dup (ch->name); /*save name of disabler */
                p->level = get_trust(ch); /* save trust */
                p->next = disabled_first;
                disabled_first = p; /* add before the current first element */
                
                send_to_char ("Command disabled.\n\r",ch);
                save_disabled(); /* save to disk */
        }
}

/* Check if that command is disabled
   Note that we check for equivalence of the do_fun pointers; this means
   that disabling 'chat' will also disable the '.' command
*/
bool check_disabled (const struct cmd_type *command)
{
        DISABLED_DATA *p;
        
        for (p = disabled_first; p ; p = p->next)
                if (p->command->do_fun == command->do_fun)
                        return TRUE;

        return FALSE;
}

/* Load disabled commands */
void load_disabled()
{
        FILE *fp;
        DISABLED_DATA *p;
        char *name;
        int i;
        
        disabled_first = NULL;
        
        fp = fopen (DISABLED_FILE, "r");
        
        if (!fp) /* No disabled file.. no disabled commands : */
                return;
                
        name = fread_word (fp);
        
        while (str_cmp(name, END_MARKER)) /* as long as name is NOT END_MARKER
:) */
        {
                /* Find the command in the table */
                for (i = 0; cmd_table[i].name[0] ; i++)
                        if (!str_cmp(cmd_table[i].name, name))
                                break;
                                
                if (!cmd_table[i].name[0]) /* command does not exist? */
                {
                        bug ("Skipping uknown command in " DISABLED_FILE " file
.",0);
                        fread_number(fp); /* level */
                        fread_word(fp); /* disabled_by */
                }
                else /* add new disabled command */
                {
                        p = alloc_mem(sizeof(DISABLED_DATA));
                        p->command = &cmd_table[i];
                        p->level = fread_number(fp);
                        p->disabled_by = str_dup(fread_word(fp));
                        p->next = disabled_first;
                        
                        disabled_first = p;

                }
                
                name = fread_word(fp);
        }

        fclose (fp);
}

/* Save disabled commands */
void save_disabled()
{
        FILE *fp;
        DISABLED_DATA *p;
        
        if (!disabled_first) /* delete file if no commands are disabled */
        {
                unlink (DISABLED_FILE);
                return;
        }
        
        fp = fopen (DISABLED_FILE, "w");
        
        if (!fp)
        {
                bug ("Could not open " DISABLED_FILE " for writing",0);
                return;
        }
        
        for (p = disabled_first; p ; p = p->next)
        fprintf (fp, "%s %d %s\n", p->command->name, p->level, p->disabled_by);
                
        fprintf (fp, "%s\n",END_MARKER);
                
        fclose (fp);
}
