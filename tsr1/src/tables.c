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
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

/* for sects */
const struct clan_type clan_table[MAX_CLAN] =
{
    /*  
	name,		
	who entry,	
	god,
	clan room vnum,
	death-transfer room,	
        independent should be FALSE if is a real clan 
    */

    {	
	"",		
	"",		
	"Ao",
	ROOM_VNUM_CIRCLE,
	ROOM_VNUM_ALTAR,
	TRUE
    },

    {	
	"traitor",	
	" `%[```5Traitor```%]`` ",	
	"`5The King of Thieves``",
	ROOM_VNUM_CIRCLE,
	ROOM_VNUM_ALTAR,	
	TRUE
    },

    {	
	"Chaos",	
	" `%[```8War & Chaos```%]`` ",	
	"`8Beowolf``",
	ROOM_VNUM_SECT_CHAOS,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Pride",	
	" `%[```4Pride```%]`` ",	
	"`4Allanon``",
	ROOM_VNUM_SECT_PRIDE,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Oblivion",	
	" `0[```6Oblivion```0]`` ",	
	"`6Raff``",
	ROOM_VNUM_SECT_OBLIVION,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Strife",	
	" [`0Strife``] ",	
	"`0Thallin``",
	ROOM_VNUM_SECT_STRIFE,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Retribution",	
	" `%[```3Retribution```%]`` ",	
	"`3Variant``",
	ROOM_VNUM_SECT_RETRIBUTION,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Balance",	
	" `0[```3Balance```0]`` ",	
	"`3Draven``",
	ROOM_VNUM_SECT_BALANCE,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Rebellion",	
	" `0[```%Rebellion```0]`` ",	
	"`%Bryelle``",
	ROOM_VNUM_SECT_FAITH,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    },

    {	
	"Deception",	
	" `6[```@Deception```6]`` ",	
	"`@Remik``",
	ROOM_VNUM_SECT_APOCALYPSE,	
	ROOM_VNUM_ALTAR,	
	FALSE	
    }

};

/* for position */
const struct position_type position_table[] =
{
    {   "dead",                 "dead"  },
    {   "mortally wounded",     "mort"  },
    {   "incapacitated",        "incap" },
    {   "stunned",              "stun"  },
    {   "sleeping",             "sleep" },
    {   "resting",              "rest"  },
    {   "sitting",              "sit"   },
    {   "fighting",             "fight" },
    {   "standing",             "stand" },
    {   NULL,                   NULL    }
};

/* for sex */
const struct sex_type sex_table[] =
{
   {    "none"          },
   {    "male"          },
   {    "female"        },
   {    "either"        },
   {    NULL            }
};

/* for sizes */
const struct size_type size_table[] =
{ 
    {   "tiny"          },
    {   "small"         },
    {   "medium"        },
    {   "large"         },
    {   "huge",         },
    {   "giant"         },
    {   NULL            }
};

/* various flag tables */
const struct flag_type act_flags[] =
{
    {   "npc",                  A,      FALSE   },
    {   "sentinel",             B,      TRUE    },
    {   "scavenger",            C,      TRUE    },
    {   "aggressive",           F,      TRUE    },
    {   "stay_area",            G,      TRUE    },
    {   "wimpy",                H,      TRUE    },
    {   "pet",                  I,      TRUE    },
    {   "train",                J,      TRUE    },
    {   "practice",             K,      TRUE    },
    {   "undead",               O,      TRUE    },
    {   "cleric",               Q,      TRUE    },
    {   "mage",                 R,      TRUE    },
    {   "thief",                S,      TRUE    },
    {   "warrior",              T,      TRUE    },
    {   "noalign",              U,      TRUE    },
    {   "nopurge",              V,      TRUE    },
    {   "outdoors",             W,      TRUE    },
    {   "indoors",              Y,      TRUE    },
    {   "healer",               aa,     TRUE    },
    {   "gain",                 bb,     TRUE    },
    {   "update_always",        cc,     TRUE    },
    {   "changer",              dd,     TRUE    },
    {   "banker",               ee,     TRUE    },
    {   NULL,                   0,      FALSE   }
};

/* New act bits table */

const struct flag_type act2_flags[] =
{
    {   "challenger",           A,     TRUE    },
    {   "challenged",           B,     TRUE    },
    {   "mountable",            C,     TRUE    },
    {   "teacher",              D,     TRUE    },
    {   NULL,                   0,      FALSE   }
};

const struct flag_type plr_flags[] =
{
    {   "npc",                  A,      FALSE   },
    {   "autoassist",           C,      FALSE   },
    {   "autoexit",             D,      FALSE   },
    {   "autoloot",             E,      FALSE   },
    {   "autosac",              F,      FALSE   },
    {   "autogold",             G,      FALSE   },
    {   "autosplit",            H,      FALSE   },
    {   "holylight",            N,      FALSE   },
    {   "can_loot",             P,      FALSE   },
    {   "nosummon",             Q,      FALSE   },
    {   "nofollow",             R,      FALSE   },
    {   "permit",               U,      TRUE    },
    {   "log",                  W,      FALSE   },
    {   "deny",                 X,      FALSE   },
    {   "freeze",               Y,      FALSE   },
    {   "thief",                Z,      FALSE   },
    {   "killer",               aa,     FALSE   },
    {   NULL,                   0,      0       }
};

const struct flag_type affect_flags[] =
{
    {   "blind",                A,      TRUE    },
    {   "invisible",            B,      TRUE    },
    {   "detect_evil",          C,      TRUE    },
    {   "detect_invis",         D,      TRUE    },
    {   "detect_magic",         E,      TRUE    },
    {   "detect_hidden",        F,      TRUE    },
    {   "detect_good",          G,      TRUE    },
    {   "sanctuary",            H,      TRUE    },
    {   "faerie_fire",          I,      TRUE    },
    {   "infrared",             J,      TRUE    },
    {   "curse",                K,      TRUE    },
    {   "poison",               M,      TRUE    },
    {   "protect_evil",         N,      TRUE    },
    {   "protect_good",         O,      TRUE    },
    {   "sneak",                P,      TRUE    },
    {   "hide",                 Q,      TRUE    },
    {   "sleep",                R,      TRUE    },
    {   "charm",                S,      TRUE    },
    {   "flying",               T,      TRUE    },
    {   "pass_door",            U,      TRUE    },
    {   "haste",                V,      TRUE    },
    {   "calm",                 W,      TRUE    },
    {   "plague",               X,      TRUE    },
    {   "weaken",               Y,      TRUE    },
    {   "dark_vision",          Z,      TRUE    },
    {   "berserk",              aa,     TRUE    },
    {   "swim",                 bb,     TRUE    },
    {   "regeneration",         cc,     TRUE    },
    {   "slow",                 dd,     TRUE    },
    {   NULL,                   0,      0       }
};

const struct flag_type off_flags[] =
{
    {   "area_attack",          A,      TRUE    },
    {   "backstab",             B,      TRUE    },
    {   "bash",                 C,      TRUE    },
    {   "berserk",              D,      TRUE    },
    {   "disarm",               E,      TRUE    },
    {   "dodge",                F,      TRUE    },
    {   "fade",                 G,      TRUE    },
    {   "fast",                 H,      TRUE    },
    {   "kick",                 I,      TRUE    },
    {   "dirt_kick",            J,      TRUE    },
    {   "parry",                K,      TRUE    },
    {   "rescue",               L,      TRUE    },
    {   "tail",                 M,      TRUE    },
    {   "trip",                 N,      TRUE    },
    {   "crush",                O,      TRUE    },
    {   "assist_all",           P,      TRUE    },
    {   "assist_align",         Q,      TRUE    },
    {   "assist_race",          R,      TRUE    },
    {   "assist_players",       S,      TRUE    },
    {   "assist_guard",         T,      TRUE    },
    {   "assist_vnum",          U,      TRUE    },
    {   "mob_is_guard",         V,      TRUE    },
    {   NULL,                   0,      0       }
};

const struct flag_type guard_direction[] =
{
    {"guard_north",             A,      TRUE},
    {"guard_south",             B,      TRUE},
    {"guard_east",              C,      TRUE},
    {"guard_west",              D,      TRUE},
    {"guard_up",                E,      TRUE},
    {"guard_down",              F,      TRUE},
    {NULL,			0,	0  }
};
   
const struct flag_type guard_action[] =
{
    {"stop",                    A,     TRUE},
    {"attack",                  B,     TRUE},
    {"teleport",                C,     TRUE},
    {"item",                    D,     TRUE},
    {"check_class",             E,     TRUE},
    {"check_race",              F,     TRUE},
    {"check_sect",              G,     TRUE},
    {"check_level",             H,     TRUE},
    {"check_age",               I,     TRUE},
    {"check_align",             J,     TRUE},
    {NULL,			0,	0  }
};


const struct flag_type imm_flags[] =
{
    {   "summon",               A,      TRUE    },
    {   "charm",                B,      TRUE    },
    {   "magic",                C,      TRUE    },
    {   "weapon",               D,      TRUE    },
    {   "bash",                 E,      TRUE    },
    {   "pierce",               F,      TRUE    },
    {   "slash",                G,      TRUE    },
    {   "fire",                 H,      TRUE    },
    {   "cold",                 I,      TRUE    },
    {   "lightning",            J,      TRUE    },
    {   "acid",                 K,      TRUE    },
    {   "poison",               L,      TRUE    },
    {   "negative",             M,      TRUE    },
    {   "holy",                 N,      TRUE    },
    {   "energy",               O,      TRUE    },
    {   "mental",               P,      TRUE    },
    {   "disease",              Q,      TRUE    },
    {   "drowning",             R,      TRUE    },
    {   "light",                S,      TRUE    },
    {   "sound",                T,      TRUE    },
    {   "wood",                 X,      TRUE    },
    {   "silver",               Y,      TRUE    },
    {   "iron",                 Z,      TRUE    },
    {   NULL,                   0,      0       }
};

const struct flag_type form_flags[] =
{
    {   "edible",               FORM_EDIBLE,            TRUE    },
    {   "poison",               FORM_POISON,            TRUE    },
    {   "magical",              FORM_MAGICAL,           TRUE    },
    {   "instant_decay",        FORM_INSTANT_DECAY,     TRUE    },
    {   "other",                FORM_OTHER,             TRUE    },
    {   "animal",               FORM_ANIMAL,            TRUE    },
    {   "sentient",             FORM_SENTIENT,          TRUE    },
    {   "undead",               FORM_UNDEAD,            TRUE    },
    {   "construct",            FORM_CONSTRUCT,         TRUE    },
    {   "mist",                 FORM_MIST,              TRUE    },
    {   "intangible",           FORM_INTANGIBLE,        TRUE    },
    {   "biped",                FORM_BIPED,             TRUE    },
    {   "centaur",              FORM_CENTAUR,           TRUE    },
    {   "insect",               FORM_INSECT,            TRUE    },
    {   "spider",               FORM_SPIDER,            TRUE    },
    {   "crustacean",           FORM_CRUSTACEAN,        TRUE    },
    {   "worm",                 FORM_WORM,              TRUE    },
    {   "blob",                 FORM_BLOB,              TRUE    },
    {   "mammal",               FORM_MAMMAL,            TRUE    },
    {   "bird",                 FORM_BIRD,              TRUE    },
    {   "reptile",              FORM_REPTILE,           TRUE    },
    {   "snake",                FORM_SNAKE,             TRUE    },
    {   "dragon",               FORM_DRAGON,            TRUE    },
    {   "amphibian",            FORM_AMPHIBIAN,         TRUE    },
    {   "fish",                 FORM_FISH ,             TRUE    },
    {   "cold_blood",           FORM_COLD_BLOOD,        TRUE    },
    {   NULL,                   0,                      0       }
};

const struct flag_type part_flags[] =
{
    {   "head",                 PART_HEAD,              TRUE    },
    {   "arms",                 PART_ARMS,              TRUE    },
    {   "legs",                 PART_LEGS,              TRUE    },
    {   "heart",                PART_HEART,             TRUE    },
    {   "brains",               PART_BRAINS,            TRUE    },
    {   "guts",                 PART_GUTS,              TRUE    },
    {   "hands",                PART_HANDS,             TRUE    },
    {   "feet",                 PART_FEET,              TRUE    },
    {   "fingers",              PART_FINGERS,           TRUE    },
    {   "ear",                  PART_EAR,               TRUE    },
    {   "eye",                  PART_EYE,               TRUE    },
    {   "long_tongue",          PART_LONG_TONGUE,       TRUE    },
    {   "eyestalks",            PART_EYESTALKS,         TRUE    },
    {   "tentacles",            PART_TENTACLES,         TRUE    },
    {   "fins",                 PART_FINS,              TRUE    },
    {   "wings",                PART_WINGS,             TRUE    },
    {   "tail",                 PART_TAIL,              TRUE    },
    {   "claws",                PART_CLAWS,             TRUE    },
    {   "fangs",                PART_FANGS,             TRUE    },
    {   "horns",                PART_HORNS,             TRUE    },
    {   "scales",               PART_SCALES,            TRUE    },
    {   "tusks",                PART_TUSKS,             TRUE    },
    {   NULL,                   0,                      0       }
};

const struct flag_type comm_flags[] =
{
    {   "quiet",                COMM_QUIET,             TRUE    },
    {   "deaf",                 COMM_DEAF,              TRUE    },
    {   "nowiz",                COMM_NOWIZ,             TRUE    },
    {   "noclangossip",         COMM_NOAUCTION,         TRUE    },
    {   "nogossip",             COMM_NOGOSSIP,          TRUE    },
    {   "noooc",                COMM_NOOOC,             TRUE    },
    {   "nomusic",              COMM_NOMUSIC,           TRUE    },
    {   "noclan",               COMM_NOCLAN,            TRUE    },
    {   "noquote",              COMM_NOQUOTE,           TRUE    },
    {   "shoutsoff",            COMM_SHOUTSOFF,         TRUE    },
    {   "compact",              COMM_COMPACT,           TRUE    },
    {   "brief",                COMM_BRIEF,             TRUE    },
    {   "prompt",               COMM_PROMPT,            TRUE    },
    {   "combine",              COMM_COMBINE,           TRUE    },
    {   "telnet_ga",            COMM_TELNET_GA,         TRUE    },
    {   "show_affects",         COMM_SHOW_AFFECTS,      TRUE    },
    {   "nograts",              COMM_NOGRATS,           TRUE    },
    {   "noemote",              COMM_NOEMOTE,           FALSE   },
    {   "noshout",              COMM_NOSHOUT,           FALSE   },
    {   "notell",               COMM_NOTELL,            FALSE   },
    {   "nochannels",           COMM_NOCHANNELS,        FALSE   },
    {   "snoop_proof",          COMM_SNOOP_PROOF,       FALSE   },
    {   "afk",                  COMM_AFK,               TRUE    },
    {   NULL,                   0,                      0       }
};

/* MOBPROGS */
const struct flag_type mprog_flags[] =
{
    {	"act",			TRIG_ACT,		TRUE	},
    {	"bribe",		TRIG_BRIBE,		TRUE 	},
    {	"death",		TRIG_DEATH,		TRUE    },
    {	"entry",		TRIG_ENTRY,		TRUE	},
    {	"fight",		TRIG_FIGHT,		TRUE	},
    {	"give",			TRIG_GIVE,		TRUE	},
    {	"greet",		TRIG_GREET,		TRUE    },
    {	"grall",		TRIG_GRALL,		TRUE	},
    {	"kill",			TRIG_KILL,		TRUE	},
    {	"hpcnt",		TRIG_HPCNT,		TRUE    },
    {	"random",		TRIG_RANDOM,		TRUE	},
    {	"speech",		TRIG_SPEECH,		TRUE	},
    {	"exit",			TRIG_EXIT,		TRUE    },
    {	"exall",		TRIG_EXALL,		TRUE    },
    {	"delay",		TRIG_DELAY,		TRUE    },
    {	"surr",			TRIG_SURR,		TRUE    },
    {   NULL,                   0,                      0       }
};
