/***************************************************************************
 *  File: olc.h                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  -Jason Dinkel
 */



/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define VERSION	"ILAB Online Creation [Beta 1.1]"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)"
#define DATE	"     (May. 15, 1995)"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"



/*
 * New typedefs.
 */
typedef bool OLC_FUN args((CHAR_DATA * ch, char *argument));
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun



/*
 * Connected states for editor.
 */
#define ED_AREA         1
#define ED_ROOM         2
#define ED_OBJECT       3
#define ED_MOBILE       4
#define ED_CLAN         5
#define ED_MPROG        6
#define ED_HELP         7
#define ED_OPROG        8
#define ED_RPROG        9
#define ED_EPROG        10
#define ED_SOCIAL       11
#define RENAME_OBJECT   12
#define FORGE_OBJECT    13
#define ED_MRESET       14
#define ED_SPEDIT       15
#define ED_NEWBIE       16
#define RACE_EDIT       17


/*
 * Interpreter Prototypes
 */
void aedit args((CHAR_DATA * ch, char *argument));
void redit args((CHAR_DATA * ch, char *argument));
void medit args((CHAR_DATA * ch, char *argument));
void oedit args((CHAR_DATA * ch, char *argument));
void cedit args((CHAR_DATA * ch, char *argument));
void mpedit args((CHAR_DATA * ch, char *argument));
void tedit args((CHAR_DATA * ch, char *argument));
void sedit args((CHAR_DATA * ch, char *argument));
void rename_object args((CHAR_DATA * ch, char *argument));
void forge_obj args((CHAR_DATA * ch, OBJ_DATA * to_forge));
void forge_object args((CHAR_DATA * ch, char *argument));
void mreset args((CHAR_DATA * ch, char *argument));
void spedit args((CHAR_DATA * ch, char *argument));
void nedit args((CHAR_DATA * ch, char *argument));
void race_edit args((CHAR_DATA * ch, char *argument));

/*
 * OLC Constants
 */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */



/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type {
    char *const name;
    OLC_FUN *olc_fun;
};

/*
 * Structure for an OLC editor startup command.
 */
struct editor_cmd_type {
    char *const name;
    DO_FUN *do_fun;
};



/*
 * Utils.
 */
AREA_DATA *get_vnum_area args((int vnum));
AREA_DATA *get_area_data args((int vnum));
int flag_value args((const struct flag_type * flag_table,
		     char *argument));
char *flag_string args((const struct flag_type * flag_table,
			int bits));
void add_reset args((ROOM_INDEX_DATA * room,
		     RESET_DATA * pReset, int index));



/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type aedit_table[];
extern const struct olc_cmd_type redit_table[];
extern const struct olc_cmd_type oedit_table[];
extern const struct olc_cmd_type medit_table[];
extern const struct olc_cmd_type cedit_table[];
extern const struct olc_cmd_type mpedit_table[];
extern const struct olc_cmd_type tedit_table[];
extern const struct olc_cmd_type sedit_table[];
extern const struct olc_cmd_type rename_obj_table[];
extern const struct olc_cmd_type forge_obj_table[];
extern const struct olc_cmd_type mreset_table[];
extern const struct olc_cmd_type spedit_table[];
extern const struct olc_cmd_type nedit_table[];
extern const struct olc_cmd_type race_edit_table[];

/*
 * General Functions
 */
bool show_commands args((CHAR_DATA * ch, char *argument));
bool show_help args((CHAR_DATA * ch, char *argument));
bool edit_done args((CHAR_DATA * ch));
bool show_version args((CHAR_DATA * ch, char *argument));


/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN(aedit_show);
DECLARE_OLC_FUN(aedit_links);
DECLARE_OLC_FUN(aedit_create);
DECLARE_OLC_FUN(aedit_name);
DECLARE_OLC_FUN(aedit_file);
DECLARE_OLC_FUN(aedit_age);
DECLARE_OLC_FUN(aedit_recall);
DECLARE_OLC_FUN(aedit_sounds);
DECLARE_OLC_FUN(aedit_prototype);
DECLARE_OLC_FUN(aedit_clan_hq);
DECLARE_OLC_FUN(aedit_reset);
DECLARE_OLC_FUN(aedit_security);
DECLARE_OLC_FUN(aedit_builder);
DECLARE_OLC_FUN(aedit_vnum);
DECLARE_OLC_FUN(aedit_lvnum);
DECLARE_OLC_FUN(aedit_uvnum);
DECLARE_OLC_FUN(aedit_llevel);
DECLARE_OLC_FUN(aedit_ulevel);
DECLARE_OLC_FUN(aedit_noquest);
DECLARE_OLC_FUN(aedit_mudschool);

/*
 * Clan Editor Prototypes
 */

DECLARE_OLC_FUN(cedit_show);
DECLARE_OLC_FUN(cedit_create);
DECLARE_OLC_FUN(cedit_name);
DECLARE_OLC_FUN(cedit_diety);
DECLARE_OLC_FUN(cedit_recall);
DECLARE_OLC_FUN(cedit_members);
DECLARE_OLC_FUN(cedit_mkills);
DECLARE_OLC_FUN(cedit_civil);
DECLARE_OLC_FUN(cedit_pkilled);
DECLARE_OLC_FUN(cedit_pkills);
DECLARE_OLC_FUN(cedit_object);
DECLARE_OLC_FUN(cedit_pkill);
DECLARE_OLC_FUN(cedit_clist);
DECLARE_OLC_FUN(cedit_desc);
DECLARE_OLC_FUN(cedit_power);
DECLARE_OLC_FUN(cedit_induct);

/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN(redit_rdamage);
DECLARE_OLC_FUN(redit_show);
DECLARE_OLC_FUN(redit_create);
DECLARE_OLC_FUN(redit_delet);
DECLARE_OLC_FUN(redit_delete);
DECLARE_OLC_FUN(redit_name);
DECLARE_OLC_FUN(redit_desc);
DECLARE_OLC_FUN(redit_ed);
DECLARE_OLC_FUN(redit_format);
DECLARE_OLC_FUN(redit_north);
DECLARE_OLC_FUN(redit_south);
DECLARE_OLC_FUN(redit_east);
DECLARE_OLC_FUN(redit_west);
DECLARE_OLC_FUN(redit_up);
DECLARE_OLC_FUN(redit_down);
DECLARE_OLC_FUN(redit_move);
DECLARE_OLC_FUN(redit_mreset);
DECLARE_OLC_FUN(redit_oreset);
DECLARE_OLC_FUN(redit_rreset);
DECLARE_OLC_FUN(redit_mlist);
DECLARE_OLC_FUN(redit_olist);
DECLARE_OLC_FUN(redit_mshow);
DECLARE_OLC_FUN(redit_oshow);
DECLARE_OLC_FUN(redit_proglist);
DECLARE_OLC_FUN(redit_rlist);
DECLARE_OLC_FUN(redit_rpedit);
DECLARE_OLC_FUN(redit_rplist);
DECLARE_OLC_FUN(redit_rpremove);
DECLARE_OLC_FUN(redit_epedit);
DECLARE_OLC_FUN(redit_eplist);
DECLARE_OLC_FUN(redit_epremove);
DECLARE_OLC_FUN(redit_owner);


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN(oedit_show);
DECLARE_OLC_FUN(oedit_create);
DECLARE_OLC_FUN(oedit_delet);
DECLARE_OLC_FUN(oedit_delete);
DECLARE_OLC_FUN(oedit_name);
DECLARE_OLC_FUN(oedit_short);
DECLARE_OLC_FUN(oedit_long);
DECLARE_OLC_FUN(oedit_addaffect);
DECLARE_OLC_FUN(oedit_delaffect);
DECLARE_OLC_FUN(oedit_join);	/* TRI */
DECLARE_OLC_FUN(oedit_sepone);	/* TRI */
DECLARE_OLC_FUN(oedit_septwo);	/* TRI */
DECLARE_OLC_FUN(oedit_opedit);
DECLARE_OLC_FUN(oedit_oplist);
DECLARE_OLC_FUN(oedit_opremove);
DECLARE_OLC_FUN(oedit_value0);
DECLARE_OLC_FUN(oedit_value1);
DECLARE_OLC_FUN(oedit_value2);
DECLARE_OLC_FUN(oedit_value3);
DECLARE_OLC_FUN(oedit_weight);
DECLARE_OLC_FUN(oedit_cost);
DECLARE_OLC_FUN(oedit_level);
DECLARE_OLC_FUN(oedit_ed);
DECLARE_OLC_FUN(set_ac_type);
DECLARE_OLC_FUN(set_ac_vnum);
DECLARE_OLC_FUN(set_ac_v1);
DECLARE_OLC_FUN(set_ac_v2);
DECLARE_OLC_FUN(set_ac_setspell);


/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN(medit_show);
DECLARE_OLC_FUN(medit_create);
DECLARE_OLC_FUN(medit_delet);
DECLARE_OLC_FUN(medit_delete);
DECLARE_OLC_FUN(medit_name);
DECLARE_OLC_FUN(medit_short);
DECLARE_OLC_FUN(medit_long);
DECLARE_OLC_FUN(medit_shop);
DECLARE_OLC_FUN(medit_desc);
DECLARE_OLC_FUN(medit_level);
DECLARE_OLC_FUN(medit_hitpoint);
DECLARE_OLC_FUN(medit_gold);

#ifdef NEW_MONEY
DECLARE_OLC_FUN(medit_silver);
DECLARE_OLC_FUN(medit_copper);
#endif

DECLARE_OLC_FUN(medit_align);
DECLARE_OLC_FUN(medit_spec);
DECLARE_OLC_FUN(medit_immune);	/* XOR */
DECLARE_OLC_FUN(medit_class);
DECLARE_OLC_FUN(medit_mpedit);	/* Altrag */
DECLARE_OLC_FUN(medit_mplist);	/* Altrag */
DECLARE_OLC_FUN(medit_mpremove);	/* Altrag */

/*
 * MPEditor functions for MobProgs
 * -- Altrag
 */
DECLARE_OLC_FUN(mpedit_show);
DECLARE_OLC_FUN(mpedit_create);
DECLARE_OLC_FUN(mpedit_arglist);
DECLARE_OLC_FUN(mpedit_comlist);

/*
 * Trap Editor Prototypes
 */
DECLARE_OLC_FUN(tedit_show);
DECLARE_OLC_FUN(tedit_create);
DECLARE_OLC_FUN(tedit_disarmable);
DECLARE_OLC_FUN(tedit_arglist);
DECLARE_OLC_FUN(tedit_comlist);

/* Social editor prototype  -Decklarean */

DECLARE_OLC_FUN(sedit_name);
DECLARE_OLC_FUN(sedit_delete);
DECLARE_OLC_FUN(sedit_show);
DECLARE_OLC_FUN(sedit_char_no_arg);
DECLARE_OLC_FUN(sedit_others_no_arg);
DECLARE_OLC_FUN(sedit_char_found);
DECLARE_OLC_FUN(sedit_others_found);
DECLARE_OLC_FUN(sedit_vict_found);
DECLARE_OLC_FUN(sedit_char_auto);
DECLARE_OLC_FUN(sedit_others_auto);

/* rename_obj editor - Decklarean */
DECLARE_OLC_FUN(rename_show);
DECLARE_OLC_FUN(rename_keyword);
DECLARE_OLC_FUN(rename_short);
DECLARE_OLC_FUN(rename_long);
/* Forge stuff  */
DECLARE_OLC_FUN(forge_show);
DECLARE_OLC_FUN(forge_addaffect);
DECLARE_OLC_FUN(forge_type);


/*
 * mreset editor by Decklarean
 */
DECLARE_OLC_FUN(mreset_add);
DECLARE_OLC_FUN(mreset_show);
DECLARE_OLC_FUN(mreset_delet);
DECLARE_OLC_FUN(mreset_delete);
DECLARE_OLC_FUN(mreset_max_in_room);

/* spedit editor -Decklarean */
DECLARE_OLC_FUN(spedit_damage_msg);
DECLARE_OLC_FUN(spedit_spell_ends);
DECLARE_OLC_FUN(spedit_spell_ends_room);
DECLARE_OLC_FUN(spedit_dispelable);
DECLARE_OLC_FUN(spedit_min);
DECLARE_OLC_FUN(spedit_mana);
DECLARE_OLC_FUN(spedit_name);
DECLARE_OLC_FUN(spedit_show);
DECLARE_OLC_FUN(spedit_wait);

/* Newbie help editor -Angi */
DECLARE_OLC_FUN(nedit_keyword);
DECLARE_OLC_FUN(nedit_answer1);
DECLARE_OLC_FUN(nedit_answer2);
DECLARE_OLC_FUN(nedit_show);

/* Social editor prototype  -Decklarean */

DECLARE_OLC_FUN(race_edit_name);
DECLARE_OLC_FUN(race_edit_full);
DECLARE_OLC_FUN(race_edit_mcon);
DECLARE_OLC_FUN(race_edit_mstr);
DECLARE_OLC_FUN(race_edit_mdex);
DECLARE_OLC_FUN(race_edit_mint);
DECLARE_OLC_FUN(race_edit_mwis);
DECLARE_OLC_FUN(race_edit_show);
DECLARE_OLC_FUN(race_edit_delete);



/*
 * Macros
 */
#define IS_BUILDER(ch, Area)	( ( ch->pcdata->security >= Area->security  \
				|| strstr( Area->builders, ch->name )	    \
				|| strstr( Area->builders, "All" ) )	    \
				&& !IS_SWITCHED( ch ) )

/* #define TOGGLE_BIT(var, bit)    ((var) ^= (bit)) */

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)Ch->desc->pEdit )
#define EDIT_CLAN(Ch, Clan)     ( Clan = (CLAN_DATA *)Ch->desc->pEdit )
#define EDIT_MPROG(Ch, MProg)   ( MProg = (MPROG_DATA *)Ch->desc->pEdit )
#define EDIT_HELP(Ch, Help)     ( Help = (HELP_DATA *)Ch->desc->pEdit )
#define EDIT_TRAP(Ch, Trap)     ( Trap = (TRAP_DATA *)Ch->desc->pEdit )
#define EDIT_SOCIAL(Ch, Social) ( Social = (SOCIAL_DATA *)Ch->desc->pEdit )
#define RENAME_OBJ(Ch, Obj)  ( Obj = (OBJ_DATA *)Ch->desc->pEdit )
#define FORGE_OBJ(Ch, Obj)	( Obj = (OBJ_DATA*)Ch->desc->pEdit )
#define EDIT_SPELL( Ch, Spell)  ( Spell = (SKILL_TYPE *)Ch->desc->pEdit )
#define EDIT_NEWBIE( Ch, Newbie ) ( Newbie = (NEWBIE_DATA *)Ch->desc->pEdit )
#define EDIT_RACE(Ch, pRace) ( pRace = (RACE_DATA *)Ch->desc->pEdit )

/*
 * Prototypes
 */
/* mem.c - memory prototypes. */
#define ED	EXTRA_DESCR_DATA
RESET_DATA *new_reset_data args((void));
void free_reset_data args((RESET_DATA * pReset));
AREA_DATA *new_area args((void));
void free_area args((AREA_DATA * pArea));
EXIT_DATA *new_exit args((void));
void free_exit args((EXIT_DATA * pExit));
ED *new_extra_descr args((void));
void free_extra_descr args((ED * pExtra));
ROOM_INDEX_DATA *new_room_index args((void));
void free_room_index args((ROOM_INDEX_DATA * pRoom));
AFFECT_DATA *new_affect args((void));
void free_affect args((AFFECT_DATA * pAf));
SHOP_DATA *new_shop args((void));
void free_shop args((SHOP_DATA * pShop));
OBJ_INDEX_DATA *new_obj_index args((void));
void free_obj_index args((OBJ_INDEX_DATA * pObj));
MOB_INDEX_DATA *new_mob_index args((void));
void free_mob_index args((MOB_INDEX_DATA * pMob));
CLAN_DATA *new_clan_index args((void));
void free_clan_index args((CLAN_DATA * pClan));
/* -- Altrag */
MPROG_DATA *new_mprog_data args((void));
void free_mprog_data args((MPROG_DATA * pMProg));
TRAP_DATA *new_trap_data args((void));
void free_trap_data args((TRAP_DATA * pTrap));
/* Decklarean */
SOCIAL_DATA *new_social_index args((void));
void free_social_index args((SOCIAL_DATA * pSocial));
/* Angi */
NEWBIE_DATA *new_newbie_index args((void));
/* Decklarean */
RACE_DATA *new_race_data args((void));
void free_race_data args((RACE_DATA * pRace));

#undef	ED


void hedit args((CHAR_DATA * ch, char *argument));

extern const struct olc_cmd_type hedit_table[];

DECLARE_DO_FUN(do_hedit);

/* Help Editor - kermit 1/98 */
DECLARE_OLC_FUN(hedit_show);
DECLARE_OLC_FUN(hedit_make);
DECLARE_OLC_FUN(hedit_desc);
DECLARE_OLC_FUN(hedit_level);
DECLARE_OLC_FUN(hedit_keywords);
DECLARE_OLC_FUN(hedit_delete);

#define EDIT_HELP(Ch, Help)     ( Help = (HELP_DATA *)Ch->desc->pEdit )

HELP_DATA *new_help args((void));
void free_help args((HELP_DATA * pHelp));
void save_helps args((void));
