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
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


/* item type list */
const struct item_type		item_table	[]	=
{
    {	ITEM_LIGHT,	"light"		},
    {	ITEM_SCROLL,	"scroll"	},
    {	ITEM_WAND,	"wand"		},
    {   ITEM_STAFF,	"staff"		},
    {   ITEM_WEAPON,	"weapon"	},
    {   ITEM_TREASURE,	"treasure"	},
    {   ITEM_ARMOR,	"armor"		},
    {	ITEM_POTION,	"potion"	},
    {	ITEM_CLOTHING,	"clothing"	},
    {   ITEM_FURNITURE,	"furniture"	},
    {	ITEM_TRASH,	"trash"		},
    {	ITEM_CONTAINER,	"container"	},
    {	ITEM_DRINK_CON, "drink"		},
    {	ITEM_KEY,	"key"		},
    {	ITEM_FOOD,	"food"		},
    {	ITEM_MONEY,	"money"		},
    {	ITEM_BOAT,	"boat"		},
    {	ITEM_CORPSE_NPC,"npc_corpse"	},
    {	ITEM_CORPSE_PC,	"pc_corpse"	},
    {   ITEM_FOUNTAIN,	"fountain"	},
    {	ITEM_PILL,	"pill"		},
    {	ITEM_PROTECT,	"protect"	},
    {	ITEM_MAP,	"map"		},
    {	ITEM_PORTAL,	"portal"	},
    {	ITEM_WARP_STONE,"warp_stone"	},
    {	ITEM_ROOM_KEY,	"room_key"	},
    {	ITEM_GEM,	"gem"		},
    {	ITEM_JEWELRY,	"jewelry"	},
    {   ITEM_JUKEBOX,	"jukebox"	},
    {   ITEM_SADDLE,	"saddle"	},
    {   ITEM_CARRIAGE,	"wagon"   	},
    {   0,		NULL		}
};

const   struct  lang_type       lang_table      [ MAX_LANGUAGE ] =
{
        { "Common"      },
        { "Dwarvish"    },
        { "Elven"       },
        { "Giant"       },
        { "Demon"       },
        { "Pixie"       },
        { "Undead"      },
        { "Bodak"       },
        { "Sithid"      },
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SWORD,	&gsn_sword	},
   { "mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_MACE,	&gsn_mace 	},
   { "dagger",	OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DAGGER,	&gsn_dagger	},
   { "axe",	OBJ_VNUM_SCHOOL_AXE,	WEAPON_AXE,	&gsn_axe	},
   { "staff",	OBJ_VNUM_SCHOOL_STAFF,	WEAPON_SPEAR,	&gsn_spear	},
   { "flail",	OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_FLAIL,	&gsn_flail	},
   { "whip",	OBJ_VNUM_SCHOOL_WHIP,	WEAPON_WHIP,	&gsn_whip	},
   { "polearm",	OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM,	&gsn_polearm	},
   { NULL,	0,				0,	NULL		}
};

/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "prefix",	WIZ_PREFIX,	IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {	"newbies",	WIZ_NEWBIE,	IM },
   {	"spam",		WIZ_SPAM,	L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",	WIZ_FLAGS,	L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",	WIZ_SACCING,	L5 },
   {	"levels",	WIZ_LEVELS,	IM },
   {	"load",		WIZ_LOAD,	L2 },
   {	"restore",	WIZ_RESTORE,	L2 },
   {	"snoops",	WIZ_SNOOPS,	L2 },
   {	"switches",	WIZ_SWITCHES,	L2 },
   {	"secure",	WIZ_SECURE,	ML },
   {	NULL,		0,		0  }
};

/* attack table  -- not very organized :( */
const 	struct attack_type	attack_table	[MAX_DAMAGE_MESSAGE]	=
{
    { 	"none",		"hit",		-1		},  /*  0 */
    {	"slice",	"slice", 	DAM_SLASH	},	
    {   "stab",		"stab",		DAM_PIERCE	},
    {	"slash",	"slash",	DAM_SLASH	},
    {	"whip",		"whip",		DAM_SLASH	},
    {   "claw",		"claw",		DAM_SLASH	},  /*  5 */
    {	"blast",	"blast",	DAM_BASH	},
    {   "pound",	"pound",	DAM_BASH	},
    {	"crush",	"crush",	DAM_BASH	},
    {   "grep",		"grep",		DAM_SLASH	},
    {	"bite",		"bite",		DAM_PIERCE	},  /* 10 */
    {   "pierce",	"pierce",	DAM_PIERCE	},
    {   "suction",	"suction",	DAM_BASH	},
    {	"beating",	"beating",	DAM_BASH	},
    {   "digestion",	"digestion",	DAM_ACID	},
    {	"charge",	"charge",	DAM_BASH	},  /* 15 */
    { 	"slap",		"slap",		DAM_BASH	},
    {	"punch",	"punch",	DAM_BASH	},
    {	"wrath",	"wrath",	DAM_ENERGY	},
    {	"magic",	"magic",	DAM_ENERGY	},
    {   "divine",	"divine power",	DAM_HOLY	},  /* 20 */
    {	"cleave",	"cleave",	DAM_SLASH	},
    {	"scratch",	"scratch",	DAM_PIERCE	},
    {   "peck",		"peck",		DAM_PIERCE	},
    {   "peckb",	"peck",		DAM_BASH	},
    {   "chop",		"chop",		DAM_SLASH	},  /* 25 */
    {   "sting",	"sting",	DAM_PIERCE	},
    {   "smash",	 "smash",	DAM_BASH	},
    {   "shbite",	"shocking bite",DAM_LIGHTNING	},
    {	"flbite",	"flaming bite", DAM_FIRE	},
    {	"frbite",	"freezing bite", DAM_COLD	},  /* 30 */
    {	"acbite",	"acidic bite", 	DAM_ACID	},
    {	"chomp",	"chomp",	DAM_PIERCE	},
    {  	"drain",	"life drain",	DAM_NEGATIVE	},
    {   "thrust",	"thrust",	DAM_PIERCE	},
    {   "slime",	"slime",	DAM_ACID	},
    {	"shock",	"shock",	DAM_LIGHTNING	},
    {   "thwack",	"thwack",	DAM_BASH	},
    {   "flame",	"flame",	DAM_FIRE	},
    {   "chill",	"chill",	DAM_COLD	},
    {   NULL,		NULL,		0		}
};

/* race table */
const 	struct	race_type	race_table	[]		=
{
/*
    {
	name,		pc_race?,
	act bits,	aff_by bits,	off bits,
	imm,		res,		vuln,
	form,		parts,          act2 bits,
	language #,     Remort? T or F. 
    },
*/
    { "unique",		FALSE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 
	"human",		TRUE, 
	0,		0, 		0,
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	0,		FALSE
    },

    {
	"elf",			TRUE,
	0,		AFF_INFRARED| AFF_DETECT_MAGIC| AFF_SNEAK| AFF_HIDE,
        0,	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	2,		FALSE	
    },

    {
	"dwarf",		TRUE,
	0,		AFF_INFRARED,	0,
	0,		RES_POISON|RES_DISEASE, VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	1,		FALSE
    },

    {
	"giant",		TRUE,
	0,		AFF_REGENERATION,		0,
	0,		RES_FIRE|RES_COLD,	VULN_MENTAL|VULN_LIGHTNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
        3, 		FALSE
    },
    
    {
	"demon",                TRUE,
	0,		AFF_FLYING|AFF_INFRARED|AFF_REGENERATION,   0,
	0,		RES_CHARM| RES_FIRE| RES_COLD| RES_POISON, VULN_HOLY,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
        4,		FALSE
    },

    {
	"sprite",		TRUE,
        0,   	AFF_INVISIBLE|AFF_DETECT_EVIL|AFF_DETECT_GOOD|AFF_FLYING,
	0, IMM_POISON,		RES_CHARM|RES_DISEASE,		VULN_WEAPON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
        5,		FALSE
    },

    {
	"vampire",		TRUE,
	0,   		AFF_INFRARED| AFF_HASTE| AFF_REGENERATION,	0,
	IMM_MENTAL,		RES_CHARM| RES_DISEASE,		VULN_HOLY,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
        6,		FALSE
    },
    
    {
	"bodak",		TRUE,
	0,		AFF_INFRARED,		0,
        IMM_CHARM| IMM_ENERGY,		RES_HOLY,		VULN_WEAPON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	7,		FALSE
    },

    {
	"sithid",		TRUE,
	0,		0,		0,
	0,		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,		FALSE
    },
        /* REMORT RACES */
    {
	"lich",		TRUE,
	0,		AFF_INVISIBLE| AFF_SNEAK| AFF_HIDE| AFF_REGENERATION,
	0, IMM_CHARM| IMM_MENTAL,	RES_DISEASE|RES_POISON,	VULN_HOLY,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
	
    {
	"chaon",		TRUE,
	0,		AFF_DETECT_INVIS| AFF_DETECT_MAGIC,		0,
	IMM_MENTAL,	RES_NEGATIVE| RES_HOLY| RES_ENERGY, VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"balor",		TRUE,
	0,		AFF_INVISIBLE| AFF_DETECT_MAGIC,		0,
	IMM_DISEASE,	RES_POISON| RES_MAGIC,		VULN_ENERGY,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"brownie",		TRUE,
	0,		AFF_SNEAK| AFF_INVISIBLE| AFF_HIDE,		0,
	IMM_POISON,		RES_DISEASE| RES_HOLY,		VULN_ENERGY,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"centaur",		TRUE,
	0,		AFF_HASTE| AFF_REGENERATION,		0,
	0,		RES_WEAPON| RES_FIRE,		VULN_COLD|VULN_MAGIC,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"dragon",		TRUE,
	0, AFF_FLYING|AFF_INFRARED|AFF_DETECT_INVIS|AFF_INVISIBLE, 0,
        IMM_FIRE,	RES_POISON,		VULN_MAGIC| VULN_WEAPON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"halfling",		TRUE,
	0,		AFF_SNEAK,		0,
	0,		RES_POISON,		VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8, 		FALSE 
    },
    
    {
	"werewolf",		TRUE,
	0,		AFF_HASTE,		0,
	0,		RES_CHARM,		VULN_SILVER,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"mind flayer",		TRUE,
	0,		AFF_INFRARED| AFF_HASTE,		0,
	0,		RES_CHARM,		VULN_DISEASE,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"satyr",		TRUE,
	0,		AFF_INVISIBLE| AFF_HIDE| AFF_SNEAK,		0,
	0,		RES_CHARM,		VULN_NEGATIVE,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"archon",		TRUE,
	0,		AFF_INFRARED| AFF_FLYING,		0,
	IMM_CHARM,		RES_POISON,		VULN_FIRE,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"pit fiend",		TRUE,  0,
	AFF_DETECT_MAGIC| AFF_DETECT_INVIS| AFF_INVISIBLE| AFF_FLYING,
	0, IMM_CHARM,		RES_POISON| RES_DISEASE,	VULN_WEAPON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0,
	8,  		TRUE
    },
    
    {
	"bat",			FALSE,
	0,		AFF_FLYING|AFF_DARK_VISION,	OFF_DODGE|OFF_FAST,
	0,		0,		VULN_LIGHT,
	A|G|V,		A|C|D|E|F|H|J|K|P, 0
    },

    {
	"bear",			FALSE,
	0,		0,		OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
	0,		RES_BASH|RES_COLD,	0,
	A|G|V,		A|B|C|D|E|F|H|J|K|U|V, 0
    },

    {
	"cat",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V, 0 
    },

    {
	"centipede",		FALSE,
	0,		AFF_DARK_VISION,	0,
	0,		RES_PIERCE|RES_COLD,	VULN_BASH,
 	A|B|G|O,		A|C|K, 0	
    },

    {
	"dog",			FALSE,
	0,		0,		OFF_FAST,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|U|V,  0
    },

    {
	"doll",			FALSE,
	0,		0,		0,
	IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE
	|IMM_DROWNING,	RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	E|J|M|cc,	A|B|C|G|H|K, 0 
    },

    {
	"fido",			FALSE,
	0,		0,		OFF_DODGE|ASSIST_RACE,
	0,		0,			VULN_MAGIC,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V, 0 
    },		
   
    {
	"fox",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|V, 0
    },

    {
	"goblin",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE,	VULN_MAGIC,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0
    },

    {
	"hobgoblin",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE|RES_POISON,	0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y, 0
    },

    {
	"kobold",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_POISON,	VULN_MAGIC,
	A|B|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q, 0
    },

    {
	"lizard",		FALSE,
	0,		0,		0,
	0,		RES_POISON,	VULN_COLD,
	A|G|X|cc,	A|C|D|E|F|H|K|Q|V, 0
    },

    {
	"modron",		FALSE,
	0,		AFF_INFRARED,		ASSIST_RACE|ASSIST_ALIGN,
	IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
			RES_FIRE|RES_COLD|RES_ACID,	0,
	H,		A|B|C|G|H|J|K, 0
    },

    {
	"orc",			FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE,	VULN_LIGHT,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K, 0
    },

    {
	"pig",			FALSE,
	0,		0,		0,
	0,		0,		0,
	A|G|V,	 	A|C|D|E|F|H|J|K, 0
    },	

    {
	"rabbit",		FALSE,
	0,		0,		OFF_DODGE|OFF_FAST,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K, 0
    },
    
    {
	"school monster",	FALSE,
	ACT_NOALIGN,		0,		0,
	IMM_CHARM|IMM_SUMMON,	0,		VULN_MAGIC,
	A|M|V,		A|B|C|D|E|F|H|J|K|Q|U, 0
    },	

    {
	"snake",		FALSE,
	0,		0,		0,
	0,		RES_POISON,	VULN_COLD,
	A|G|X|Y|cc,	A|D|E|F|K|L|Q|V|X, 0 
    },
 
    {
	"song bird",		FALSE,
	0,		AFF_FLYING,		OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|W,		A|C|D|E|F|H|K|P, 0
    },

    {
	"troll",		FALSE,
	0,		AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN,
	OFF_BERSERK,
 	0,	RES_CHARM|RES_BASH,	VULN_FIRE|VULN_ACID,
	A|B|H|M|V,		A|B|C|D|E|F|G|H|I|J|K|U|V, 0
    },

    {
	"water fowl",		FALSE,
	0,		AFF_SWIM|AFF_FLYING,	0,
	0,		RES_DROWNING,		0,
	A|G|W,		A|C|D|E|F|H|K|P, 0
    },		
  
    {
	"wolf",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,	
	A|G|V,		A|C|D|E|F|J|K|Q|V, 0
    },

    {
	"wyvern",		FALSE,
	0,		AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,
	OFF_BASH|OFF_FAST|OFF_DODGE,
	IMM_POISON,	0,	VULN_LIGHT,
	A|B|G|Z,		A|C|D|E|F|H|J|K|Q|V|X, 0
    },

    {
	"unique",		FALSE,
	0,		0,		0,
	0,		0,		0,		
	0,		0,              0
    },


    {
	NULL, 0, 0, 0, 0, 0, 0
    }
};

const	struct	pc_race_type	pc_race_table	[]	=
{
    { "null race", "", 0, { 100, 100, 100, 100 },
      { "" }, { 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 }, 0 },
 
/*
    {
	"race name", 	who name, 	points,	{ class multipliers },
	{ bonus skills },
	{ base stats },		{ max stats },		size 
    },
*/
    {
	"human",	"Human",	0, 
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 18, 18, 18, 18, 18 },	SIZE_MEDIUM
    },

    { 	
	"elf",		" Elf ",	0, 
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "sneak", "hide", "infravision", "detect magic" },
	{ 12, 14, 13, 15, 11 },	{ 16, 20, 18, 21, 15 }, SIZE_MEDIUM
    },

    {
	"dwarf",	"Dwarf",	0,
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "berserk", "infravision" },
	{ 14, 12, 14, 10, 15 },	{ 20, 16, 19, 14, 21 }, SIZE_MEDIUM
    },

    {
	"giant",	"Giant",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "bash", "fast healing" },
	{ 16, 11, 13, 11, 14 },	{ 22, 15, 18, 15, 20 }, SIZE_LARGE
    },

    {
	"demon",	"Demon",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "enhanced damage", "fly", "infravision" },
	{ 16, 15, 14, 13, 11 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"sprite",	"Sprite",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{"invisibility", "detect evil", "detect good", "fly"},
	{ 12, 10, 13, 16, 13 }, { 18, 16, 20, 25, 20 }, SIZE_MEDIUM
    },

    {
	"vampire",	"Vampire",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
        { "infravision", "haste" },
	{ 16, 16, 10, 12, 11 }, { 25, 25, 16, 18, 16 }, SIZE_MEDIUM
    },

    {
	"bodak",	"Bodak",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "second attack", "infravision"},
	{ 16, 14, 14, 11, 12 }, { 25, 22, 22, 17, 16 }, SIZE_MEDIUM
    },

    {
	"sithid",	"Sithid",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "second attack", "third attack", "berserk"},
	{ 16, 15, 16, 11, 10 }, { 25, 25, 18, 15, 14 }, SIZE_LARGE
    },

    /* REMORT RACES */             
 
    {
	"lich", 	"Lich",	10,
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
          350, 350, 350, 350, 350, 350, 350 },
	{ "invisibility", "sneak", "hide" },
	{ 15, 18, 17, 15, 15 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },
    
    {
	"chaon", 	"Chaon",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
          350, 350, 350, 350, 350, 350, 350 },
	{ "detect invis", "sneak", "hide"},
	{ 23, 15, 15, 20, 15 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },
    
    {
	"balor", 	"Balor",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
          350, 350, 350, 350, 350, 350, 350 },
	{ "invisibility", "detect magic"},
	{ 25, 15, 16, 18, 15 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"brownie", 	"Brownie",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
          350, 350, 350, 350, 350, 350, 350 },
	{ "sneak", "hide", "invisibility"},
	{ 16, 20, 19, 18, 16 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"centaur", 	"Centaur",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
          350, 350, 350, 350, 350, 350, 350 },
	{ "haste" "fast healing"},
	{ 25, 15, 16, 15, 23 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"dragon", 	"Dragon",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
          350, 350, 350, 350, 350, 350, 350 },
	{ "fly", "infravision", "detect invis", "invisibility"},
	{ 18, 19, 17, 16, 14 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"halfling", 	"Halfling",	0,	
        { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,           100, 100, 100, 100, 100, 100, 100 },
	{ "sneak", "hide", "infravision" },
	{ 16, 25, 20, 15, 16 }, { 18, 22, 20, 20, 25 }, SIZE_MEDIUM
    },

    {
	"werewolf", 	"Werewolf",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,           350, 350, 350, 350, 350, 350, 350 },
	{ "second attack", "haste"},
	{ 25, 15, 16, 15, 23 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"mind flayer", 	"Mind Flayer",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,           350, 350, 350, 350, 350, 350, 350 },
	{ "infrared", "haste"},
	{ 15, 20, 22, 15, 15 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"satyr", 	"Satyr",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,           350, 350, 350, 350, 350, 350, 350 },
        { "invisibility", "sneak", "hide" },
	{ 20, 15, 16, 21, 18 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"archon", 	"Archon",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,           350, 350, 350, 350, 350, 350, 350 },
        { "invisibility", "fly" },
	{ 25, 17, 16, 15, 15 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    },

    {
	"pit fiend", 	"Pit Fiend",	10,	
        { 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350,           350, 350, 350, 350, 350, 350, 350 },
        { "detect magic", "fly", "invisibility", "detect invis" }, 
	{ 25, 17, 16, 15, 15 }, { 25, 25, 25, 25, 25 }, SIZE_LARGE
    }

};

	
      	

/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{
    {
	"mage", "Mag",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618 },  75,  20, 6,  6,  8, TRUE,
	"mage basics", "mage default", 3017
    },

    {
	"cleric", "Cle",  STAT_WIS,  OBJ_VNUM_SCHOOL_MACE,
	{ 3003, 9619 },  75,  20, 2,  7, 10, TRUE,
	"cleric basics", "cleric default", 3004
    },

    {
	"thief", "Thi",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3028, 9639 },  75,  20,  -4,  8, 13, FALSE,
	"thief basics", "thief default", 3027
    },

    {
	"barbarian", "Bar",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633 },  75,  20,  -10,  11, 15, FALSE,
	"warrior basics", "warrior default", 3021
    },

    {
	"psionicist", "Psi", STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 404,      0 },   75, 20,    2,   8,   10,  TRUE,
	"psi basics", "psi default", 404
    },

    {
	"monk",	"Mnk", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
	{ 403,      0 },  75, 20,   -4,   10,  15, TRUE ,
	"monk basics", "monk default", 403
    },

    {
	"scout", "Sct", STAT_STR, OBJ_VNUM_SCHOOL_DAGGER,
	{ 402,       0},  75, 20,    -8,  3,   10, TRUE,
	"scout basics", "scout default", 402
    },

    {
	"ranger", "Rgr", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 401,       0},   75, 20,   -10, 5,  14, TRUE,
	"ranger basics", "ranger default", 401
    },

    /* Special RESERVED Class */
    {
	"builder", "BLD", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 0,      0},   75, 20,   -10, 5,  14, FALSE,
	"ranger basics", "ranger default", 468, TRUE
    },

         /* SPECIAL REMORT CLASSES */
    
    {
	"paladin", "Pal", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{ 436, 0 },  80,  20,  -10,  11, 15, TRUE,
	"warrior basics", "paladin default", 423, TRUE
    },
    
    {
	"warlord", "Wld", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 430, 0 },  80,  20,  -10,  11, 15, TRUE,
	"warrior basics", "warlord default", 423, TRUE
    },

    {
	"wizard", "Wiz", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 437, 0 },  75,  20, 6,  6,  8, TRUE,
	"mage basics", "wizard default", 423, TRUE
    },

    {
	"sorcerer", "Sor", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 431, 0 },  75,  20, 6,  6,  8, TRUE,
	"mage basics", "sorcerer default", 423, TRUE
    },

    {
	"overlord", "Ovr", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 439, 0 },  75,  20,  -4,  8, 13, FALSE,
	"thief basics", "overlord default", 423, TRUE
    },

    {
	"underlord", "Und", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 433, 0},  75,  20,  -4,  8, 13, FALSE,
	"thief basics", "underlord default", 423, TRUE
    },

    {
	"abbot", "Abb", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 438, 0 },  75,  20, 2,  7, 10, TRUE,
	"cleric basics", "abbot default", 423, TRUE
    },

    {
	"defiler", "Def", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 432, 0 },  75,  20, 2,  7, 10, TRUE,
	"cleric basics", "defiler default", 423, TRUE
    },

    {
	"mediator", "Med", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 440,      0 },   75, 20,    2,   8,   10,  TRUE,
	"psi basics", "mediator default", 423, TRUE
    },

    {
	"dominator", "Dom", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 434,      0 },   75, 20,    2,   8,   10,  TRUE,
	"psi basics", "dominator default", 423, TRUE
    },

    {
	"samurai", "Sam", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 441,      0 },  75, 20,   -4,   10,  15, TRUE ,
	"monk basics", "samurai default", 423, TRUE
    },

    {
	"assassin", "Asn", STAT_DEX, OBJ_VNUM_SCHOOL_SWORD,
	{ 435,      0 },  75, 20,   -4,   10,  15, TRUE ,
	"monk basics", "assassin default", 423, TRUE
    }

};



/*
 * Titles.
 */
char *	const			title_table	[MAX_CLASS][MAX_LEVEL+1][2] =
{
    {
        /* MAGE */
	{ "Man",			"Woman"				},

	{ "Warlock",	                "Witch"          		},
	{ "Warlock",	                "Witch"          		},
    },

    {
        /* CLERIC */
	{ "Man",			"Woman"				},

	{ "Bishop",	         	"Deconess"			},
	{ "Bishop",	         	"Deconess"			},
    },

    {
        /* THIEF */
	{ "Man",			"Woman"				},

	{ "Crime Lord",			"Crime Mistress"		},
	{ "Crime Lord",			"Crime Mistress"		},
    },

    {
        /* BARBARIAN */
	{ "Man",			"Woman"				},

	{ "barbarian",		        "Adventuress"			},
	{ "barbarian",		        "Adventuress"			},
    },

    {
        /* PSIONICIST */
	{ "Man",			"Woman"				},

	{ "Psychic",	         	"Mind Reader"			},
	{ "Psychic",	         	"Mind Reader"			},
    },

    {
        /* MONK */
	{ "Man",			"Woman"				},

	{ "Meditator",	         	"Monk"   			},
	{ "Meditator",	         	"Monk"   			},
    },
    
    {
        /* SCOUT */
	{ "Man",			"Woman"				},
	
	{ "Covert",			"Covertess"			},
	{ "Covert",			"Covertess"			},
    },

    {
        /* RANGER */
	{ "Man",			"Woman"				},

	{ "Beastmaster",		"Forestress"			},
	{ "Beastmaster",		"Forestress"			},
    },
    
    {
        /* BUILDER */
	{ "Man",			"Woman"				},
	
	{ "Builder",			"Builder"			},
	{ "Builder",			"Builder"			},
    },

    {
        /* PALADIN (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Noble Paladin",		"Beautiful Paladin"		},
	{ "Noble Paladin",		"Beautiful Paladin"		},
    },

    {
        /* WARLORD (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Warlord",		        "Queen of War."      		},
	{ "Warlord",		        "Queen of War."      		},
    },

    {
        /* WIZARD (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Enchanter.",		        "Enchantress."      		},
	{ "Enchanter.",		        "Enchantress."      		},
    },

    {
        /* SORCERER (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Sorcerer.",		        "Sorceress."      		},
	{ "Sorcerer.",		        "Sorceress."      		},
    },

    {
        /* OVERLORD (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "High Overlord.",	        "High Overlordess."   		},
	{ "High Overlord.",	        "High Overlordess."   		},
    },

    {
        /* UNDERLORD (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "High Underlord.",	        "High Underlordess."   		},
	{ "High Underlord.",	        "High Underlordess."   		},
    },

    {
        /* ABBOT (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Abbot.",	                "Abbotess."      		},
	{ "Abbot.",	                "Abbotess."      		},
    },

    {
        /* DEFILER (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Defiler.",	                "Defileress."      		},
	{ "Defiler.",	                "Defileress."      		},
    },

    {
        /* MEDIATOR (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Mediator",	                "Mediatoress."      		},
	{ "Mediator",	                "Mediatoress."      		},
    },

    {
        /* DOMINATOR (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Master of the Mind.",	"Mistress of the Mind."         },
	{ "Master of the Mind.",	"Mistress of the Mind."         },
    },

    {
        /* SAMURAI (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Master Samurai",	       "Mistress Samurai."              },
	{ "Master Samurai",	       "Mistress Samurai."              },
    },

    {
        /* ASSASSIN (REMORT) */
	{ "Man",			"Woman"				},
	
	{ "Master Assassin",	       "Mistress Assassin."             },
	{ "Master Assassin",	       "Mistress Assassin."             },
    }

};



/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 130, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  1, 140, 14 },
    {  1,  1, 150, 15 }, /* 15  */
    {  1,  2, 165, 16 },
    {  2,  3, 180, 22 },
    {  2,  3, 200, 25 }, /* 18  */
    {  3,  4, 225, 30 },
    {  3,  5, 250, 35 }, /* 20  */
    {  4,  6, 300, 40 },
    {  4,  6, 350, 45 },
    {  5,  7, 400, 50 },
    {  5,  8, 450, 55 },
    {  6,  9, 500, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 85 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 2 },	/* 15 */
    { 2 },
    { 2 },
    { 3 },	/* 18 */
    { 3 },
    { 3 },	/* 20 */
    { 3 },
    { 4 },
    { 4 },
    { 4 },
    { 5 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};


const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[]	=
{
/*    name			color	proof, full, thirst, food, ssize */
    { "water",			"clear",	{   0, 1, 10, 0, 16 }	},
    { "beer",			"amber",	{  12, 1,  8, 1, 12 }	},
    { "red wine",		"burgundy",	{  30, 1,  8, 1,  5 }	},
    { "ale",			"brown",	{  15, 1,  8, 1, 12 }	},
    { "dark ale",		"dark",		{  16, 1,  8, 1, 12 }	},

    { "whisky",			"golden",	{ 120, 1,  5, 0,  2 }	},
    { "lemonade",		"pink",		{   0, 1,  9, 2, 12 }	},
    { "firebreather",		"boiling",	{ 190, 0,  4, 0,  2 }	},
    { "local specialty",	"clear",	{ 151, 1,  3, 0,  2 }	},
    { "slime mold juice",	"green",	{   0, 2, -8, 1,  2 }	},

    { "milk",			"white",	{   0, 2,  9, 3, 12 }	},
    { "tea",			"tan",		{   0, 1,  8, 0,  6 }	},
    { "coffee",			"black",	{   0, 1,  8, 0,  6 }	},
    { "blood",			"red",		{   0, 2, -1, 2,  6 }	},
    { "salt water",		"clear",	{   0, 1, -2, 0,  1 }	},

    { "coke",			"brown",	{   0, 2,  9, 2, 12 }	}, 
    { "root beer",		"brown",	{   0, 2,  9, 2, 12 }   },
    { "elvish wine",		"green",	{  35, 2,  8, 1,  5 }   },
    { "white wine",		"golden",	{  28, 1,  8, 1,  5 }   },
    { "champagne",		"golden",	{  32, 1,  8, 1,  5 }   },

    { "mead",			"honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",		"pink",		{  26, 1,  8, 1,  5 }	},
    { "benedictine wine",	"burgundy",	{  40, 1,  8, 1,  5 }   },
    { "vodka",			"clear",	{ 130, 1,  5, 0,  2 }   },
    { "cranberry juice",	"red",		{   0, 1,  9, 2, 12 }	},

    { "orange juice",		"orange",	{   0, 2,  9, 3, 12 }   }, 
    { "absinthe",		"green",	{ 200, 1,  4, 0,  2 }	},
    { "brandy",			"golden",	{  80, 1,  5, 0,  4 }	},
    { "aquavit",		"clear",	{ 140, 1,  5, 0,  2 }	},
    { "schnapps",		"clear",	{  90, 1,  5, 0,  2 }   },

    { "icewine",		"purple",	{  50, 2,  6, 1,  5 }	},
    { "amontillado",		"burgundy",	{  35, 2,  8, 1,  5 }	},
    { "sherry",			"red",		{  38, 2,  7, 1,  5 }   },	
    { "framboise",		"red",		{  50, 1,  7, 1,  5 }   },
    { "rum",			"amber",	{ 151, 1,  4, 0,  2 }	},

    { "cordial",		"clear",	{ 100, 1,  5, 0,  2 }   },
    { NULL,			NULL,		{   0, 0,  0, 0,  0 }	}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

/*
{
   " Name of skill ", { Levels that class can use }, { How diff it is to use },
   " Spell pointer ",  Legal targets,             postion of caster,	
   Pointer for gsn,     SLOT (#),    Min Mana used,    waiting time after use,
   " Damage message ", "Wear off message",  "wear off message for objs"
 },
*/

/* MAGES */


    {
	"reserved",		
	{ 199, 199, 199, 199, 199, 199, 199, 199, 199, 199, 
          199, 199, 199, 199, 199, 199, 199, 199, 199, 199, 199 },	
	{ 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 ,99,
          99, 99, 99, 99, 99, 99, 99, 99, 99},
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),	 0,	 0,
	"",			"",		""
    },

    {
	"acid blast",		
	{ 28, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,   20,  20,  20,  20,  20,  20,  20, 20, 20, 20, 20, 20, 20,
         20, 20, 20, 20, 20, 20, 20 },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(70),	20,	12,
	"acid blast",		"!Acid Blast!"
    },

    {
	"armor",
	{ 7, 152, 152, 152, 152, 152, 152, 152, 152, 15, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,   20,  20,  20,  20,  20,  20,  20, 20, 20, 20, 20, 20, 20,
         20, 20, 20, 20, 20, 20, 20 },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 1),	 5,	12,
	"",			"You feel less armored.",	""
    },

    {
	"bless",
	{ 152, 7, 152, 152, 152, 152, 152, 152, 152, 10, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2 },
	spell_bless,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT( 3),	 5,	12,
	"",			"You feel less righteous.", 
	"$p's holy aura fades."
    },

    {
	"blindness",
	{ 12, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2 },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		SLOT( 4),	 5,	12,
	"",			"You can see again.",	""
    },

    {
	"burning hands",
	{ 7, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2 },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 5),	15,	12,
	"burning hands",	"!Burning Hands!", 	""
    },

    {
	"call lightning",
	{ 26, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  2,  2,  2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2 },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!",	""
    },

    {   "calm",		
	{ 152, 16, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_calm,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(509),	30,	12,
	"",			"You have lost your peace of mind.",	""
    },

    {
	"cancellation",	
	{ 18, 26, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(507),	20,	12,
	""			"!cancellation!",	""
    },

    {
	"cause critical",	
	{ 26, 13, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(63),	20,	12,
	"spell",		"!Cause Critical!",	""
    },

    {
	"cause light",	
	{ 152, 1, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(62),	15,	12,
	"spell",		"!Cause Light!",	""
    },

    {
	"cause serious",	
	{ 152, 7, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(64),	17,	12,
	"spell",		"!Cause Serious!",	""
    },

    {   
	"chain lightning",
	{ 33, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(500),	25,	12,
	"lightning",		"!Chain Lightning!",	""
    }, 

    {
	"change sex",		
	{ 83, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(82),	15,	12,
	"",			"Your body feels familiar again.",	""
    },

    {
	"charm person",	
	{ 50, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 50 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),	 5,	12,
	"",			"You feel more self-confident.",	""
    },

    {
	"chill touch",
	{ 4, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 8),	15,	12,
	"chilling touch",	"You feel less cold.",	""
    },

    {
	"colour spray",	
	{ 16, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(10),	15,	12,
	"colour spray",		"!Colour Spray!",	""
    },

    {
	"continual light",
	{ 10, 4, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(57),	 7,	12,
	"",			"!Continual Light!",	""
    },

    {
	"control weather",
	{ 152, 15, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(11),	25,	12,
	"",			"!Control Weather!",	""
    },

    {
	"create food",	
	{ 152, 5, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(12),	 5,	12,
	"",			"!Create Food!",	""
    },

    {
	"create rose",	
	{ 20, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_create_rose,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(511),	30, 	12,
	"",			"!Create Rose!",	""
    },  

    {
	"create spring",
	{ 152, 14, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(80),	20,	12,
	"",			"!Create Spring!",	""
    },

    {
	"create water",	
	{ 152, 5, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(13),	 5,	12,
	"",			"!Create Water!",	""
    },

    {
	"cure blindness",
	{ 152, 6, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(14),	 5,	12,
	"",			"!Cure Blindness!",	""
    },

    {
	"cure critical",
	{ 152, 13, 152, 152, 152, 152, 152, 152, 152, 12, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(15),	20,	12,
	"",			"!Cure Critical!",	""
    },

    {
	"cure disease",	
	{ 152, 13, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(501),	20,	12,
	"",			"!Cure Disease!",	""
    },

    {
	"cure light",
	{ 152, 1, 152, 152, 152, 152, 152, 152, 152, 10, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(16),	10,	12,
	"",			"!Cure Light!",		""
    },

    {
	"cure poison",	
	{ 152, 14, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(43),	 5,	12,
	"",			"!Cure Poison!",	""
    },

    {
	"cure serious",	
	{ 152, 7, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(61),	15,	12,
	"",			"!Cure Serious!",	""
    },

    {
	"curse",
	{ 152, 18, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_curse,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_curse,		SLOT(17),	20,	12,
	"curse",		"The curse wears off.", 
	"$p is no longer impure."
    },

    {
	"demonfire",	
	{ 152, 34, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_demonfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(505),	20,	12,
	"torments",		"!Demonfire!",		""
    },	

    {
	"detect evil",	
	{ 6, 4, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 6,
          6, 6, 6, 6, 6, 6, 6, 6 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(18),	 5,	12,
	"",			"The red in your vision disappears.",	""
    },

    {
        "detect good",     
	{ 6, 4, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 6,
          6, 6, 6, 6, 6, 6, 6, 6 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SLOT(513),        5,     12,
        "",                     "The gold in your vision disappears.",	""
    },

    {
	"detect hidden",	
	{ 15, 11, 152, 152, 152, 152, 152, 152, 152, 17, 4, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(44),	 5,	12,
	"",			"You feel less aware of your surroundings.",	
	""
    },

    {
	"detect invis",	
	{ 3, 8, 10, 10, 10, 10, 10, 10, 10, 10, 8, 10, 10,
          10, 10, 10, 10, 10, 10, 10, 10 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects.",
	""
    },

    {
	"detect magic",	
	{ 2, 6, 8, 8, 8, 8, 8, 8, 8, 8, 6, 8, 8,
          8, 8, 8, 8, 8, 8, 8, 8 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(20),	 5,	12,
	"",			"The detect magic wears off.",	""
    },

    {
	"detect poison",
	{ 152, 7, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(21),	 5,	12,
	"",			"!Detect Poison!",	""
    },

    {
	"dispel evil",	
	{ 152, 15, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(22),	15,	12,
	"dispel evil",		"!Dispel Evil!",	""
    },

    {
        "dispel good",
	{ 152, 15, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(512),      15,     12,
        "dispel good",          "!Dispel Good!",	""
    },

    {
	"dispel magic",	
	{ 16, 24, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(59),	15,	12,
	"",			"!Dispel Magic!",	""
    },

    {
	"earthquake",
	{ 20, 10, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!",		""
    },

    {
	"enchant armor",
	{ 16, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_enchant_armor,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(510),	100,	24,
	"",			"!Enchant Armor!",	""
    },

    {
	"enchant weapon",
	{ 18, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(24),	100,	24,
	"",			"!Enchant Weapon!",	""
    },

    {
	"energy drain",
	{ 19, 22, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(25),	35,	12,
	"energy drain",		"!Energy Drain!",	""
    },

    {
	"faerie fire",
	{ 152, 3, 152, 152, 152, 152, 152, 5, 152, 17, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	""
    },

    {
	"faerie fog",
	{ 14, 21, 152, 152, 152, 152, 152, 12, 152, 20, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!",		""
    },

    {
	"farsight",
	{ 14, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_farsight,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(521),	36,	20,
	"farsight",		"!Farsight!",		""
    },	

    {
	"fireball",
	{ 22, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(26),	15,	12,
	"fireball",		"!Fireball!",		""
    },
  
    {
	"fireproof",
	{ 13, 12, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_fireproof,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(523),	10,	12,
	"",			"",	"$p's protective aura fades."
    },

    {
	"flamestrike",
	{ 152, 20, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(65),	20,	12,
	"flamestrike",		"!Flamestrike!",		""
    },

    {
	"fly",
	{ 10, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
          12, 12, 12, 12, 12, 12, 12, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(56),	10,	18,
	"",			"You slowly float to the ground.",	""
    },
    
    {
        "flying carpet", 
	{ 35, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_flying_carpet,    TAR_IGNORE,             POS_STANDING,
        NULL,                   SLOT(313),       100,     24,
        "",                     "",		""
    },

    {
        "negate magic", 
	{ 100, 75, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 8,  8,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_mute,    TAR_CHAR_OFFENSIVE,             POS_STANDING,
        NULL,                   SLOT(601),       100,     24,
        "",                     "",		""
    },

    {
	"floating disc",	
	{ 10, 5, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_floating_disc,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(522),	40,	24,
	"",			"!Floating disc!",	""
    },

    {
        "frenzy", 
	{ 152, 24, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SLOT(504),      30,     24,
        "",                     "Your rage ebbs.",	""
    },

    {
	"gate",
	{ 30, 17, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_gate,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(83),	80,	12,
	"",			"!Gate!",		""
    },

    {
	"giant strength",
	{ 11, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(39),	20,	12,
	"",			"You feel weaker.",	""
    },

    {
	"harm",
	{ 152, 23, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(27),	35,	12,
	"harm spell",		"!Harm!,		"""
    },
  
    {
	"haste",
	{ 21, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
          25, 25, 25, 25, 25, 25, 25, 25 }, 
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_haste,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(502),	30,	12,
	"",			"You feel yourself slow down.",	""
    },

    {
	"heal",
	{ 152, 21, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(28),	50,	12,
	"",			"!Heal!",		""
    },
  
    {
	"heat metal",
	{ 152, 152, 152, 152, 10, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_heat_metal,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(516), 	25,	18,
	"spell",		"!Heat Metal!",		""
    },

    {
	"holy word",
	{ 152, 36, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_holy_word,	TAR_IGNORE,	POS_FIGHTING,
	NULL,			SLOT(506), 	200,	24,
	"divine wrath",		"!Holy Word!",		""
    },
    
    {
	"identify",		
	{ 16, 15, 152, 152, 15, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(53),	12,	24,
	"",			"!Identify!",		""
    },

    {
	"infravision",
	{ 9, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
          15, 15, 15, 15, 15, 15, 15, 15 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(77),	 5,	18,
	"",			"You no longer see in the dark.",	""
    },

    {
	"invisibility",	
	{ 10, 13, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
          12, 12, 12, 12, 12, 12, 12, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_invis,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	&gsn_invis,		SLOT(29),	 5,	12,
	"",			"You are no longer invisible.",		
	"$p fades into view."
    },

    {
	"know alignment",
	{ 15, 9, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(58),	 9,	12,
	"",			"!Know Alignment!",	""
    },

    {
	"lightning bolt",	
	{ 25, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(30),	15,	12,
	"lightning bolt",	"!Lightning Bolt!",	""
    },

    {
	"locate object",
	{ 25, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(31),	20,	18,
	"",			"!Locate Object!",	""
    },

    {
	"magic missile",	
	{ 1, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(32),	15,	12,
	"magic missile",	"!Magic Missile!",	""
    },

    {
	"mass healing",	
	{ 152, 152, 152, 152, 50, 20, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_mass_healing,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(508),	100,	36,
	"",			"!Mass Healing!",	""
    },

    {
	"mass invis",	
	{ 25, 152, 152, 152, 20, 20, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),	20,	24,
	"",			"You are no longer invisible.",		""
    },

    {
        "nexus",            
	{ 40, 25, 152, 152, 152, 152, 152, 152, 152, 152, 25, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 26 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   SLOT(520),       150,   36,
        "",                     "!Nexus!",		""
    },

    {
	"pass door",
	{ 24, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(74),	20,	12,
	"",			"You feel solid again.",	""
    },

    {
	"plague",	
	{ 152, 17, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,		SLOT(503),	20,	12,
	"sickness",		"Your sores vanish.",	""
    },

    {
	"poison",
	{ 152, 12, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_poison,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_poison,		SLOT(33),	10,	12,
	"poison",		"You feel less sick.",	
	"The poison on $p dries up."
    },

    {
	"exhaust",
	{ 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_exhausted,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_exhausted,		SLOT(533),	10,	12,
	"safe feeling",	        "You feel less safe.",	
	"The safe feeling on $p vanishes."
    },

    {
        "portal",
	{ 28, 30, 152, 152, 152, 152, 152, 152, 152, 152, 28, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   SLOT(519),       100,     24,
        "",                     "!Portal!",		""
    },

    {
	"protection evil",
	{ 152, 9, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(34), 	5,	12,
	"",			"You feel less protected.",	""
    },

    {
        "protection good",
	{ 152, 9, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SLOT(514),       5,     12,
        "",                     "You feel less protected.",	""
    },

    {
        "ray of truth",  
	{ 80, 35, 152, 152, 152, 152, 152, 152, 152, 22, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(518),      20,     12,
        "ray of truth",         "!Ray of Truth!",	""
    },

    {
	"recharge",
	{ 9, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_recharge,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(517),	60,	24,
	"",			"!Recharge!",		""
    },

    {
	"refresh",
	{ 152, 5, 152, 152, 152, 152, 152, 152, 152, 11, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(81),	12,	18,
	"refresh",		"!Refresh!",		""
    },

    {
	"remove curse",
	{ 152, 18, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_remove_curse,	TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT(35),	 5,	12,
	"",			"!Remove Curse!",	""
    },

    {
	"sanctuary",
	{ 92, 20, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 30 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,		SLOT(36),	75,	12,
	"",			"The white aura around your body fades.",
	""
    },

    {
	"shield",
	{ 20, 35, 152, 152, 152, 152, 152, 152, 152, 37, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away.",
	""
    },

    {
	"shocking grasp",
	{ 10, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	15,	12,
	"shocking grasp",	"!Shocking Grasp!",	""
    },

    {
	"sleep",
	{ 10, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),	15,	12,
	"",			"You feel less tired.",	""
    },

    {
        "slow",
	{ 30, 30, 152, 152, 152, 152, 152, 152, 152, 28, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 32 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(515),      30,     12,
        "",                     "You feel yourself speed up.",	""
    },

    {
	"stone skin",	
	{ 25, 40, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(66),	12,	18,
	"",			"Your skin feels soft again.",	""
    },

    {
	"summon",
	{ 34, 12, 152, 152, 152, 152, 152, 152, 152, 14, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 25 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(40),	50,	12,
	"",			"!Summon!",		""
    },

    {
	"teleport",
	{ 14, 22, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 15 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		SLOT( 2),	35,	12,
	"",			"!Teleport!",		""
    },

    {
	"ventriloquate",
	{ 1, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(41),	 5,	12,
	"",			"!Ventriloquate!",	""
    },

    {
	"water breathe",
	{ 15, 10, 152, 152, 152, 152, 152, 152, 152, 152, 10, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_water_breath,	TAR_CHAR_SELF,  	POS_STANDING,
	NULL,			SLOT(312),   	20,	12,
	"",      		"Your gills vanish!",	""
    },

    {
	"weaken",
	{ 11, 14, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(68),	20,	12,
	"spell",		"You feel stronger.",	""
    },

    {
	"word of recall",
	{  5, 1, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			SLOT(42),	 5,	12,
	"",			"!Word of Recall!",	""
    },

/*
 * Dragon breath
 */
    {
	"acid breath",	
	{  31, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(200),	100,	24,
	"blast of acid",	"!Acid Breath!",	""
    },

    {
	"fire breath",
	{  75, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(201),	200,	24,
	"blast of flame",	"The smoke leaves your eyes.",	""
    },

    {
	"frost breath",
	{  100, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(202),	125,	24,
	"blast of frost",	"!Frost Breath!",	""
    },

    {
	"gas breath",
	{  100, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(203),	175,	24,
	"blast of gas",		"!Gas Breath!",		""
    },

    {
	"lightning breath",	
	{  98, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(204),	150,	24,
	"blast of lightning",	"!Lightning Breath!",	""
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",
	{  152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(401),      0,      12,
        "general purpose ammo", "!General Purpose Ammo!",	""
    },
 
    {
        "high explosive", 
	{  152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(402),      0,      12,
        "high explosive ammo",  "!High Explosive Ammo!",	""
    },

/* Spells converted by Nikki and coded by Echo. Added 5-19-96 */

    {
        "adrenaline control" , 
	{  152, 152, 152, 152, 15, 152, 152, 152, 152, 152, 15, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
         spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
         NULL,                           SLOT(39),      6,      12,
         "",                             "The adrenaline rush wears off.", ""
     },
	
     {
        "biofeedback" , 
	{  152, 152, 152, 152, 10, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
         spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
         NULL,                   SLOT(300),      75,     12,
         "",            "Your biofeedback is no longer effective.", ""
      },
	
      {
        "cell adjustment" , 
	{  152, 152, 152, 152, 1, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
         spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
         NULL,                   SLOT(301),      8,     12,
         "",                     "!Cell Adjustment!",    ""
       },
    
       {
        "combat mind" , 
	{  152, 152, 152, 152, 1, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 15 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
         spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
         NULL,                   SLOT(302),      15,     12,
         "",            "Your battle sense has faded.", ""
        },
     
        {
        "complete healing" , 
	{  152, 152, 152, 152, 30, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 40, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
         spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
         NULL,                   SLOT(28),     100,    12,
         "",                     "!Complete Healing!", ""
         },
     
         {
        "disintegrate" ,
	{  152, 152, 152, 152, 140, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
             spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
             NULL,                   SLOT(303),   150,    18,
             "disintegration",       "!Disintegrate!",  ""
         },
     
         {
        "displacement" ,
	{  152, 152, 152, 152, 14, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
             spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
             NULL,                   SLOT(304),      10,     12,
             "",                     "You are no longer displaced.", ""
         },
     
         {
        "ectoplasmic form" , 
	{  152, 152, 152, 152, 8, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
             spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
             NULL,                   SLOT(305),      20,     12,
             "",                     "You feel solid again."
         },
     
         {
        "ego whip" ,
	{  152, 152, 152, 152, 8, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
             spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
             NULL,                   SLOT(306),      20,     12,
             "",                     "You feel more confident."
         },
     
         {
        "energy containment" ,
	{  152, 152, 152, 152, 8, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
             spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
             NULL,                           SLOT(307),      10,     12,
             "",                             "You no longer absorb energy."
         },
     
         {
        "flesh armor" ,
	{  152, 152, 152, 152, 8, 152, 152, 152, 152, 152, 8, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
             spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
             NULL,                   SLOT(309),      15,     12,
             "",                     "Your skin returns to normal."
         },
        
	 {
	 "enhanced strength", 
	{  152, 152, 152, 152, 8, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	 spell_enhanced_strength,     TAR_CHAR_SELF,           POS_STANDING,
	 NULL,			     SLOT(310),      10,    12,
	 "",			     "Your bones become normal again."
	 },
         
	 {
	 "domination", 
	{  152, 152, 152, 152, 8, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	 spell_domination,     TAR_CHAR_OFFENSIVE,           POS_FIGHTING,
	 NULL,		       SLOT(311),      10,    12,
	 "",		     "!Domination!"
	 },
    
    {
	"disguise",
	{  152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_disguise,          SLOT( 0),        	0,    0,
	"",                     "!Disguise!",               ""
    },

    {
	"bloodlust",           
	{  152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_bloodlust,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(314),        30,    12,
	"lust for blood",       "Your bloodlust subsides.",
    },

    {
	"fear",           
	{  152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_fear,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(315),        180,    12,
	"fear",            "Your fear subsides.",
    },

/* combat and weapons skills */


    {
	"axe",			
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_axe,            	SLOT( 0),       0,      0,
        "",                     "!Axe!",		""
    },

    {
        "dagger",               
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dagger,            SLOT( 0),       0,      0,
        "",                     "!Dagger!",		""
    },
 
    {
	"flail",	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_flail,            	SLOT( 0),       0,      0,
        "",                     "!Flail!",		""
    },

    {
	"mace",			
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_mace,            	SLOT( 0),       0,      0,
        "",                     "!Mace!",		""
    },

    {
	"polearm",		
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_polearm,           SLOT( 0),       0,      0,
        "",                     "!Polearm!",		""
    },
    
    {
	"shield block",	
	{  152, 152, 152, 1, 152, 152, 1, 1, 152, 152, 1, 152, 152,
          152, 152, 1, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shield_block,	SLOT(0),	0,	0,
	"",			"!Shield!",		""
    },
 
    {
	"spear",	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_spear,            	SLOT( 0),       0,      0,
        "",                     "!Spear!",		""
    },

    {
	"sword",	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_sword,            	SLOT( 0),       0,      0,
        "",                     "!sword!",		""
    },

    {
	"whip",
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_whip,            	SLOT( 0),       0,      0,
        "",                     "!Whip!",	""
    },

    {
        "backstab",             
	{  152, 152, 12, 152, 152, 152, 10, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_backstab,          SLOT( 0),        0,     24,
        "backstab",             "!Backstab!",		""
    },

    {
        "circle",         
	{  152, 152, 85, 152, 152, 152, 75, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 88 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_circle,            SLOT( 0),        0,     24,
        "circle",              "!Circle!",		""
    },

    {
	"bash",			
	{  2, 2, 2, 1, 2, 2, 1, 1, 2, 2, 1, 2, 2,
          2, 2, 2, 2, 2, 2, 2, 2 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_bash,            	SLOT( 0),       0,      24,
        "bash",                 "!Bash!",		""
    },

    {
	"berserk",		
	{  152, 152, 152, 18, 152, 152, 18, 18, 152, 152, 18, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,        	SLOT( 0),       0,      24,
        "",                     "You feel your pulse slow down.",	""
    },

    {
	"dirt kicking",		
	{  152, 152, 3, 3, 152, 152, 3, 3, 152, 152, 3, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirt,		SLOT( 0),	0,	24,
	"kicked dirt",		"You rub the dirt out of your eyes.",	""
    },

    {
        "disarm",               
	{  15, 152, 12, 11, 152, 152, 12, 11, 152, 152, 11, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_disarm,            SLOT( 0),        0,     24,
        "",                     "!Disarm!",		""
    },
 
    {
        "dodge",           
	{  15, 152, 1, 11, 152, 152, 12, 11, 152, 152, 11, 152, 152,
          152, 152, 15, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dodge,             SLOT( 0),        0,     0,
        "",                     "!Dodge!",		""
    },
 
    {
        "enhanced damage",      
	{  152, 152, 35, 1, 152, 152, 30, 1, 152, 152, 1, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_enhanced_damage,   SLOT( 0),        0,     0,
        "",                     "!Enhanced Damage!",	""
    },

    {
	"envenom",		
	{  152, 152, 10, 152, 152, 152, 5, 1, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_envenom,		SLOT(0),	0,	36,
	"",			"!Envenom!",		""
    },

    {
	"hand to hand",		
	{  152, 152, 152, 50, 152, 1, 152, 152, 152, 152, 22, 152, 152,
          152, 152, 12, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hand_to_hand,	SLOT( 0),	0,	0,
	"",			"!Hand to Hand!",	""
    },

    {
        "kick",                
	{  152, 152, 152, 8, 152, 1, 5, 5, 152, 152, 5, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_kick,              SLOT( 0),        0,     12,
        "kick",                 "!Kick!",		""
    },

    {
        "parry",                
	{  152, 152, 2, 1, 152, 1, 5, 5, 152, 152, 1, 152, 152,
          152, 152, 5, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_parry,             SLOT( 0),        0,     0,
        "",                     "!Parry!",		""
    },

    {
        "rescue",       
	{  152, 152, 152, 1, 152, 1, 5, 5, 152, 152, 1, 152, 152,
          152, 152, 5, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_rescue,            SLOT( 0),        0,     12,
        "",                     "!Rescue!",		""
    },

    {
	"trip",		
	{  152, 152, 152, 15, 152, 1, 5, 5, 152, 152, 15, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_trip,		SLOT( 0),	0,	24,
	"trip",			"!Trip!",		""
    },

    {
        "second attack", 
	{  7, 7, 12, 5, 7, 1, 5, 5, 7, 7, 5, 7, 7,
          7, 7, 7, 7, 7, 7, 7, 7 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_second_attack,     SLOT( 0),        0,     0,
        "",                     "!Second Attack!",	""
    },

    {
        "third attack",   
	{  152, 152, 24, 12, 152, 1, 5, 5, 152, 152, 12, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_third_attack,      SLOT( 0),        0,     0,
        "",                     "!Third Attack!",	""
    },

/* New skills for TSR by Beowolf...taken from Envy & SillyMUD code. */

    {
	"chameleon",		
	{  152, 152, 44, 152, 152, 1, 5, 5, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_chameleon,        	SLOT( 0),       0,      24,
        "",                     "!Chameleon!",		""
    },

    {
	"heighten",	
	{  152, 152, 44, 152, 152, 1, 5, 5, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_heighten,        	SLOT( 0),       0,      24,
        "",                     "Your senses return to normal.", ""
    },

    {
	"shadow",	
	{  152, 152, 44, 152, 152, 1, 5, 5, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_shadow,        	SLOT( 0),       0,      24,
        "",                     "!Shadow!",		""
    },

    {
	"snare",
	{  152, 152, 44, 152, 152, 1, 5, 5, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_snare,        	SLOT( 0),       0,      24,
        "",                     "!Snare!",		""
    },

    {
	"untangle",
	{  152, 152, 44, 152, 152, 1, 5, 5, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_untangle,        	SLOT( 0),       0,      24,
        "",                     "!Untangle!",		""
    },

/* non-combat skills */

    { 
	"fast healing",		
	{  12, 9, 12, 12, 10, 1, 12, 12, 12, 12, 12, 12, 12,
          12, 12, 12, 12, 12, 12, 12, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_fast_healing,	SLOT( 0),	0,	0,
	"",			"!Fast Healing!",	""
    },

    {
	"haggle",		
	{  7, 152, 1, 152, 152, 152, 2, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_haggle,		SLOT( 0),	0,	0,
	"",			"!Haggle!",		""
    },

    {
	"hide",
        { 4, 4, 1, 4, 4, 2, 2, 2, 4, 4, 4, 4, 4,
          4, 4, 4, 4, 4, 4, 4, 4 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),	 0,	12,
	"",			"!Hide!",		""
    },

    {
	"lore",			
	{  10, 8, 1, 152, 12, 152, 15, 20, 152, 10, 10, 152, 152,
          152, 152, 12, 152, 152, 152, 152, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_lore,		SLOT( 0),	0,	36,
	"",			"!Lore!",		""
    },

    {
	"meditation",
	{  100, 6, 152, 152, 12, 1, 152, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 12 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_meditation,	SLOT( 0),	0,	0,
	"",			"Meditation",		""
    },

    {
	"peek",			
	{  152, 152, 1, 152, 12, 152, 1, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),	 0,	 0,
	"",			"!Peek!",		""
    },

    {
	"pick lock",
	{  152, 152, 1, 152, 12, 152, 1, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),	 0,	12,
	"",			"!Pick!",		""
    },

    {
	"sneak",
        { 14, 14, 4, 14, 12, 14, 1, 14, 14, 14, 14, 14, 14,
          14, 14, 14, 14, 14, 14, 14, 14 },
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),	 0,	12,
	"",			"You no longer feel stealthy.",	""
    },

    {
	"steal",
	{  152, 152, 5, 152, 12, 152, 1, 152, 152, 152, 152, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),	 0,	24,
	"",			"!Steal!",		""
    },

    {
	"scrolls",		
	{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
          10, 10, 10, 10, 10, 10, 10, 10 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scrolls,		SLOT( 0),	0,	24,
	"",			"!Scrolls!",		""
    },

    {
	"staves",
	{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
          10, 10, 10, 10, 10, 10, 10, 10 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_staves,		SLOT( 0),	0,	12,
	"",			"!Staves!",		""
    },
    
    {
	"wands",
	{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
          10, 10, 10, 10, 10, 10, 10, 10 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_wands,		SLOT( 0),	0,	12,
	"",			"!Wands!",		""
    },

    {
	"dual wield",		
	{  152, 152, 110, 35, 152, 152, 152, 25, 152, 152, 35, 152, 152,
          152, 152, 152, 152, 152, 152, 152, 152 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dual_wield,	SLOT( 0),	0,	12,
	"",			"!Second!",		""
    },
    

    {
	"sign",
	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_sign,        	SLOT( 0),       0,      24,
        "",                     "",		""
    },

    {
	"ride",
	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_ride,        	SLOT( 0),       0,      24,
        "",                     "",		""
    },

    {
	"godspeak",  	
	{  153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153,
          153, 153, 153, 153, 153, 153, 153, 153 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_godspeak,        	SLOT( 0),       0,      24,
        "",                     "",		""
    },

    {
	"recall",
	{  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
          10, 10, 10, 10, 10, 10, 10, 10 }, 	
	{ 1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 
         1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_recall,		SLOT( 0),	0,	12,
	"",			"!Recall!",		""
    }
};

const   struct  group_type      group_table     [MAX_GROUP]     =
{
      
    /* Class Basics  9 of em */

    {
	"rom basics",		{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ "scrolls", "staves", "wands", "recall" }
    },

    {
	"mage basics",		{ 0, -1, -1, -1, -1, -1, -1, -1 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },

    {
	"cleric basics",	{ -1, 0, -1, -1, -1, -1, -1, -1 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },
   
    {
	"thief basics",		{ -1, -1, 0, -1, -1, -1, -1, -1 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },

    {
	"warrior basics",	{ -1, -1, -1, 0, -1, -1, -1, -1 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },

    {
	"psi basics", 		{ -1, -1, -1, -1, 0, -1, -1, -1 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },

    {
	"monk basics",		{ -1, -1, -1, -1, -1, 0, -1, -1 },
	{ "whip" }
    },

    {
	"scout basics", 	{ -1, -1, -1, -1, -1, -1, 0, -1 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },

    {
	"ranger basics", 	{ -1, -1, -1, -1, -1, -1, -1, 0 },
	{ "sword", "mace", "dagger", "axe", "staff", "flail", "whip", 
	  "polearm", "exotic" }
    },

    /* Class Defaults 21 of em so far */

    {
	"mage default",		{ 40, -1, -1, -1, -1, -1, -1, -1 },
	{ "acid blast", "armor", "blindness", "burning hands", "call lightning",
	  "cancellation", "chain lightning", "change sex", "charm person",
	  "chill touch", "colour spray", "create rose", "detect hidden", 
	  "detect invis", "detect magic", "dispel magic", "earthquake", 
	  "enchant armor", "enchant weapon", "energy drain", "faerie fog",
	  "farsight", "fireball", "fireproof", "fly", "flying carpet", 
	  "floating disk", "gate", "giant strength", "haste", "negate magic",
	  "identify", "infravision", "invisibility", "know align", 
	  "locate obj", "magic missile", "mass invis", "nexus", "pass door",
	  "portal", "ray of truth", "recharge", "shield", "shocking grasp",
	  "sleep", "slow", "stone skin", "summon", "teleport", "ventriloquate",
	  "water breath", "weaken", "recall", "acid breath", "fire breath", 
	  "frost breath", "gas breath", "lightning breath", "haggle", "lore",
          "lightning bolt" }
    },

    {
	"cleric default",	{ -1, 40, -1, -1, -1, -1, -1, -1 },
	{ "bless", "calm", "cancellation", "cause critical", "cause light",
	  "cause serious", "continual light", "control weather", "create food",
	  "create spring", "create water", "cure blind", "cure critical", 
	  "cure disease", "cure light", "cure poison", "cure serious", 
	  "curse", "demon fire", "detect evil", "detect good", "detect hidden",
	  "detect invis", "detect magic", "detect poison", "dispell evil",
	  "dispell good", "dispell magic", "earthquake", "energy drain",
	  "fairy fire", "faerie fog", "fireproof", "flamestrike","negate magic",
	  "floating disk", "frenzy", "gate", "harm", "heal", "holy word",
	  "identify", "infravision", "invisibility", "know align", "nexus",
	  "poison", "portal", "protect evil", "protect good", "ray of truth",
	  "refresh", "remove curse", "sanctuary", "shield", "slow", 
	  "stone skin", "summon", "teleport", "water breath", "weaken",
	  "recall", "fast healing", "lore", "meditation", "plague" }
    },
 
    {
	"thief default",	{ -1, -1, 40, -1, -1, -1, -1, -1 },
	{ "mace", "sword", "backstab", "disarm", "dodge", "second attack",
	  "trip", "hide", "peek", "pick lock", "sneak", "circle",
	  "dirt kick", "enhanced damage", "envenom", "rescue", "third attack",
	  "chameleon", "heighten", "shadow", "snare", "untangle", "haggle", 
	  "lore", "steal", "dual wield" }
    },

    {
	"warrior default",	{ -1, -1, -1, 40, -1, -1, -1, -1 },
	{ "weaponsmaster", "shield block", "bash", "disarm", "enhanced damage", 
	  "parry", "rescue", "third attack", "berserk", "dirt kick", "dodge",
	  "hand to hand", "kick", "trip", "second attack", "dual wield" }
    },

    {
	"psi default", { -1, -1, -1, -1, 40, -1, -1, -1 },
	{ "heat metal", "mass healing", "mass invis", "adrenaline control",
	  "biofeedback", "cell adjustment", "combat mind", "complete healing",
	  "disintigrate", "displacement", "ectoplasmic form", "ego whip",
	  "energy containment", "flesh armor", "enhanced strength", 
	  "domination", "fast healing", "meditation", "peek", "pick lock",
	  "sneak", "steal" }
    
    },

    {
      "monk default", { -1, -1, -1, -1, -1, 40, -1, -1 },
      { "mass healing", "mass invis", "hand to hand", "kick", "parry", 
	"rescue", "trip", "second attack", "third attack", "chameleon", 
	"heighten", "shadow", "snare", "untangle", "fast healing",
	"hide", "lore", "hide", "meditation" }
    },

    {
      "scout default", { -1, -1, -1, -1, -1, -1, 40, -1 },
      {"shield block", "backstab", "circle", "bash", "berserk"
       "dirt kicking", "disarm", "dodge", "enhanced damage", "envenom",
       "hand to hand", "kick", "parry", "rescue", "trip", "second attack",
       "third attack", "chameleon", "heighten", "shadow", "snare", "untangle",
       "haggle", "hide", "lore", "peek", "pick lock", "sneak", "steal" }
    },
    
    { 
      "ranger default", {-1, -1, -1, -1, -1, -1, -1, 40 },
      {"shield block", "bash", "berserk", "meditation",
       "dirt kicking", "disarm", "dodge", "enhanced damage", "envenom",
       "hand to hand", "kick", "parry", "rescue", "trip", "second attack",
       "third attack", "chameleon", "heighten", "shadow", "snare", "untangle",
       "faerie fire", "faerie fog", "dual wield", "hide", "lore" }
    },

    /*
     * No Builder default needed. 
     * Begin REMORT defaults
     */

    {
	"paladin default",	{ -1, -1, -1, 40, -1, -1, -1, -1 },
	{ "weaponsmaster", "shield block", "bash", "disarm", "enhanced damage", 
	  "parry", "rescue", "third attack", "berserk", "dirt kick", "dodge",
	  "hand to hand", "kick", "trip", "second attack", "dual wield",
          "detect good", "detect evil", "cure light",
          "lore", "bless", "cure critical", "faerie fog", "faerie fire"
          "detect hidden", "slow", "haste", "refresh", "ride", "summon", 
          "ray of truth", "shield", "armor" }
    },

    {
	"warlord default",	{ -1, -1, -1, 40, -1, -1, -1, -1 },
	{ "weaponsmaster", "shield block", "bash", "disarm", "enhanced damage", 
	  "parry", "rescue", "third attack", "berserk", "dirt kick", "dodge",
	  "hand to hand", "kick", "trip", "second attack", "dual wield", "lore",
          "detect invis", "detect good", "detect evil", "heighten", 
          "detect hidden", "ride", "detect magic", "adrenaline control", 
          "flesh armor", "nexus", "portal", "water breath" }
    },

    {
	"wizard default",		{ 40, -1, -1, -1, -1, -1, -1, -1 },
	{ "acid blast", "armor", "blindness", "burning hands", "call lightning",
	  "cancellation", "chain lightning", "change sex", "charm person",
	  "chill touch", "colour spray", "create rose", "detect hidden", 
	  "detect invis", "detect magic", "dispel magic", "earthquake", 
	  "enchant armor", "enchant weapon", "energy drain", "faerie fog",
	  "farsight", "fireball", "fireproof", "fly", "flying carpet", 
	  "floating disk", "gate", "giant strength", "haste", "negate magic",
	  "identify", "infravision", "invisibility", "know align", 
	  "locate obj", "magic missile", "mass invis", "nexus", "pass door",
	  "portal", "ray of truth", "recharge", "shield", "shocking grasp",
	  "sleep", "slow", "stone skin", "summon", "teleport", "ventriloquate",
	  "water breath", "weaken", "recall", "acid breath", "fire breath", 
	  "frost breath", "gas breath", "lightning breath", "haggle", "lore",
          "lightning bolt", "disguise", "lore", "parry", "detect good",
          "detect evil", "detect magic", "fast healing", "detect hidden",
          "shadow", "detect invis" }
    },

    {
	"sorcerer default",	{ -1, 40, -1, -1, -1, -1, -1, -1 },
	{ "bless", "calm", "cancellation", "cause critical", "cause light",
	  "cause serious", "continual light", "control weather", "create food",
	  "create spring", "create water", "cure blind", "cure critical", 
	  "cure disease", "cure light", "cure poison", "cure serious", 
	  "curse", "demon fire", "detect evil", "detect good", "detect hidden",
	  "detect invis", "detect magic", "detect poison", "dispell evil",
	  "dispell good", "dispell magic", "earthquake", "energy drain",
	  "fairy fire", "faerie fog", "fireproof", "flamestrike","negate magic",
	  "floating disk", "frenzy", "gate", "harm", "heal", "holy word",
	  "identify", "infravision", "invisibility", "know align", "nexus",
	  "poison", "portal", "protect evil", "protect good", "ray of truth",
	  "refresh", "remove curse", "sanctuary", "shield", "slow", 
	  "stone skin", "summon", "teleport", "water breath", "weaken",
	  "recall", "fast healing", "lore", "meditation", "plague",
          "colour spray", "lore", "burning hands", "detect magic", "haggle",
          "detect good", "detect evil", "charm person", "detect hidden", 
          "detect invis", "slow", "gas breath", "fire breath" }
    },
 
    {
	"overlord default",	{ -1, -1, -1, 40, -1, -1, -1, -1 },
	{ "weaponsmaster", "shield block", "bash", "disarm", "enhanced damage", 
	  "parry", "rescue", "third attack", "berserk", "dirt kick", "dodge",
	  "hand to hand", "kick", "trip", "second attack", "dual wield",
          "detect good", "fly", "cure critical", "bless", "cure blindness", 
          "cure poison", "locate object", "ray of truth" }
    },

    {
	"underlord default",	{ -1, -1, 40, -1, -1, -1, -1, -1 },
	{ "mace", "sword", "backstab", "disarm", "dodge", "second attack",
	  "trip", "hide", "peek", "pick lock", "sneak", "circle",
	  "dirt kick", "enhanced damage", "envenom", "rescue", "third attack",
	  "chameleon", "heighten", "shadow", "snare", "untangle", "haggle", 
	  "lore", "steal", "dual wield", "detect hidden", "disguise", 
          "detect evil", "cloak", "detect invis", "fast healing", "circle",
          "combat mind", "ego whip", "flesh armor" }
    },

    {
	"abbot default",	{ -1, 40, -1, -1, -1, -1, -1, -1 },
	{ "bless", "calm", "cancellation", "cause critical", "cause light",
	  "cause serious", "continual light", "control weather", "create food",
	  "create spring", "create water", "cure blind", "cure critical", 
	  "cure disease", "cure light", "cure poison", "cure serious", 
	  "curse", "demon fire", "detect evil", "detect good", "detect hidden",
	  "detect invis", "detect magic", "detect poison", "dispell evil",
	  "dispell good", "dispell magic", "earthquake", "energy drain",
	  "fairy fire", "faerie fog", "fireproof", "flamestrike","negate magic",
	  "floating disk", "frenzy", "gate", "harm", "heal", "holy word",
	  "identify", "infravision", "invisibility", "know align", "nexus",
	  "poison", "portal", "protect evil", "protect good", "ray of truth",
	  "refresh", "remove curse", "sanctuary", "slow", "shield",
	  "stone skin", "summon", "teleport", "water breath", "weaken",
	  "recall", "fast healing", "lore", "meditation", "plague", 
          "parry", "lore", "dodge", "rescue", "shield block", "hand to hand",
          "second attack", "complete healing" }
    },
 
    {
	"defiler default",	{ -1, -1, 40, -1, -1, -1, -1, -1 },
	{ "mace", "sword", "backstab", "disarm", "dodge", "second attack",
	  "trip", "hide", "peek", "pick lock", "sneak", "circle",
	  "dirt kick", "enhanced damage", "envenom", "rescue", "third attack",
	  "chameleon", "heighten", "shadow", "snare", "untangle", "haggle", 
	  "lore", "steal", "dual wield", "detect hidden", "detect invis",
          "create food", "cause critical", "heal critical", "bless", 
          "cure poison", "cure disease", "enchant weapon", "gate", 
          "detect magic" }
    },

    {
	"mediator default", { -1, -1, -1, -1, 40, -1, -1, -1 },
	{ "heat metal", "mass healing", "mass invis", "adrenaline control",
	  "biofeedback", "cell adjustment", "combat mind", "complete healing",
	  "disintigrate", "displacement", "ectoplasmic form", "ego whip",
	  "energy containment", "flesh armor", "enhanced strength", 
	  "domination", "fast healing", "meditation", "peek", "pick lock",
	  "sneak", "steal", "trip", "ride", "detect magic", "lore", "dodge",
          "hide", "meditation", "detect hidden", "parry", "detect invis", 
          "fast healing", "gate", "summon", "charm person" }
    },

    {
	"dominator default", { -1, -1, -1, -1, 40, -1, -1, -1 },
	{ "heat metal", "mass healing", "mass invis", "adrenaline control",
	  "biofeedback", "cell adjustment", "combat mind", "complete healing",
	  "disintigrate", "displacement", "ectoplasmic form", "ego whip",
	  "energy containment", "flesh armor", "enhanced strength", 
	  "domination", "fast healing", "meditation", "peek", "pick lock",
	  "sneak", "steal", "detect magic", "create food", "detect good",
          "detect evil", "lore", "stone skin", "detect hidden", "summon",
          "detect invis", "gate", "charm", "fireproof", "slow", "weaken",
          "combat mind", "gas breath" }
    
    },

    {
      "samurai default", { -1, -1, -1, -1, -1, 40, -1, -1 },
      { "mass healing", "mass invis", "hand to hand", "kick", "parry", 
	"rescue", "trip", "second attack", "third attack", "chameleon", 
	"heighten", "shadow", "snare", "untangle", "fast healing",
	"hide", "lore", "hide", "meditation", "detect good", "detect evil",
        "detect magic", "haggle", "lore", "beserk", "cure light", 
        "cure critical", "detect invis", "haste", "enchant weapon",
        "enchant armor", "charm", "combat mind" }
    },

    {
      "assassin default", { -1, -1, -1, -1, -1, 40, -1, -1 },
      { "mass healing", "mass invis", "hand to hand", "kick", "parry", 
	"rescue", "trip", "second attack", "third attack", "chameleon", 
	"heighten", "shadow", "snare", "untangle", "fast healing",
	"hide", "lore", "hide", "meditation", "detect good", "detect evil",
        "detect magic", "detect hidden", "backstab", "sanctuary", "hide",
        "invisible", "meditation", "lore", "summon", "teleport", "nexus",
        "detect invis", "slow", "circle", "charm", "combat mind" }

    }

};
