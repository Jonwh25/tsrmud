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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/* this is a listing of all the commands and command related data */

/* for command types */
#define ML 	MAX_LEVEL	/* implementor */
#define L1	MAX_LEVEL - 1	/* creator */
#define L2	MAX_LEVEL - 2	/* supreme being */
#define L3	MAX_LEVEL - 3	/* deity */
#define L4 	MAX_LEVEL - 4	/* god */
#define L5	MAX_LEVEL - 5	/* immortal */
#define L6	MAX_LEVEL - 6	/* demigod */
#define L7	MAX_LEVEL - 7	/* angel */
#define L8	MAX_LEVEL - 8	/* avatar */
#define L9	MAX_LEVEL - 9	/* apprentice */
#define IM	LEVEL_IMMORTAL	/* avatar */
#define HE	LEVEL_HERO	/* hero */

#define COM_INGORE	1


/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type {
    char *const name;
    DO_FUN *do_fun;
    int position;
    int level;
    int log;
};


/* the command table itself */
extern struct cmd_type cmd_table[];


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(do_clanquest);
DECLARE_DO_FUN(do_quest);
DECLARE_DO_FUN(do_qecho);
DECLARE_DO_FUN(do_qset);
DECLARE_DO_FUN(do_questflag);
DECLARE_DO_FUN(do_mpasound);
DECLARE_DO_FUN(do_mpat);
DECLARE_DO_FUN(do_mpecho);
DECLARE_DO_FUN(do_mpechoaround);
DECLARE_DO_FUN(do_mpechoat);
DECLARE_DO_FUN(do_mpforce);
DECLARE_DO_FUN(do_mpgoto);
DECLARE_DO_FUN(do_mpjunk);
DECLARE_DO_FUN(do_mpkill);
DECLARE_DO_FUN(do_mpmload);
DECLARE_DO_FUN(do_mpoload);
DECLARE_DO_FUN(do_mppurge);
DECLARE_DO_FUN(do_mpstat);
DECLARE_DO_FUN(do_mpcommands);
DECLARE_DO_FUN(do_mpteleport);
DECLARE_DO_FUN(do_mptransfer);
DECLARE_DO_FUN(do_opstat);
DECLARE_DO_FUN(do_oscore);
DECLARE_DO_FUN(do_rpstat);
DECLARE_DO_FUN(do_accept);
DECLARE_DO_FUN(do_account);
DECLARE_DO_FUN(do_advance);
DECLARE_DO_FUN(do_relevel);
DECLARE_DO_FUN(do_affectedby);
DECLARE_DO_FUN(do_raffect);
DECLARE_DO_FUN(do_afk);
DECLARE_DO_FUN(do_coding);
DECLARE_DO_FUN(do_afkmes);
DECLARE_DO_FUN(do_allow);
DECLARE_DO_FUN(do_answer);
DECLARE_DO_FUN(do_antidote);
DECLARE_DO_FUN(do_areas);
DECLARE_DO_FUN(do_assasinate);
DECLARE_DO_FUN(do_astat);
DECLARE_DO_FUN(do_astrip);
DECLARE_DO_FUN(do_at);
DECLARE_DO_FUN(do_auction);
DECLARE_DO_FUN(do_auto);
DECLARE_DO_FUN(do_autoexit);
DECLARE_DO_FUN(do_autocoins);
DECLARE_DO_FUN(do_autoloot);
DECLARE_DO_FUN(do_autosac);
DECLARE_DO_FUN(do_autosplit);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_bash);
DECLARE_DO_FUN(do_beep);
DECLARE_DO_FUN(do_berserk);
DECLARE_DO_FUN(do_bid);
DECLARE_DO_FUN(do_blank);
DECLARE_DO_FUN(do_bodybag);
DECLARE_DO_FUN(do_bounty);
DECLARE_DO_FUN(do_brandish);
DECLARE_DO_FUN(do_brief);
DECLARE_DO_FUN(do_bug);
DECLARE_DO_FUN(do_buy);
DECLARE_DO_FUN(do_challenge);
DECLARE_DO_FUN(do_chameleon);
DECLARE_DO_FUN(do_champlist);
DECLARE_DO_FUN(do_cast);
DECLARE_DO_FUN(do_changes);
DECLARE_DO_FUN(do_channels);
DECLARE_DO_FUN(do_chat);
DECLARE_DO_FUN(do_clan);
DECLARE_DO_FUN(do_clone);
DECLARE_DO_FUN(do_cinfo);
DECLARE_DO_FUN(do_circle);
DECLARE_DO_FUN(do_clans);
DECLARE_DO_FUN(do_clandesc);
DECLARE_DO_FUN(do_class);
DECLARE_DO_FUN(do_cloak);
DECLARE_DO_FUN(do_close);
DECLARE_DO_FUN(do_colour);	/* Colour Command By Lope */
DECLARE_DO_FUN(do_combat);
DECLARE_DO_FUN(do_combine);
DECLARE_DO_FUN(do_commands);
DECLARE_DO_FUN(do_compare);
DECLARE_DO_FUN(do_conference);
DECLARE_DO_FUN(do_config);
DECLARE_DO_FUN(do_consider);
DECLARE_DO_FUN(do_copyover);
DECLARE_DO_FUN(do_cocount);	/* Counter for Copyover in 5 ticks by Hunter */
DECLARE_DO_FUN(do_countcmds);
DECLARE_DO_FUN(do_credits);
DECLARE_DO_FUN(do_cross_punch);
DECLARE_DO_FUN(do_cset);
DECLARE_DO_FUN(do_darkinvis);
DECLARE_DO_FUN(do_delet);
DECLARE_DO_FUN(do_delete);
DECLARE_DO_FUN(do_deny);
DECLARE_DO_FUN(do_deposit);
DECLARE_DO_FUN(do_description);
DECLARE_DO_FUN(do_detract);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_disconnect);
DECLARE_DO_FUN(do_donate);
DECLARE_DO_FUN(do_down);
DECLARE_DO_FUN(do_drink);
DECLARE_DO_FUN(do_drop);
DECLARE_DO_FUN(do_ds);
DECLARE_DO_FUN(do_dual);
DECLARE_DO_FUN(do_east);
DECLARE_DO_FUN(do_eat);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_email);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_empower);
DECLARE_DO_FUN(do_enter);
DECLARE_DO_FUN(do_equipment);
DECLARE_DO_FUN(do_examine);
DECLARE_DO_FUN(do_exits);
DECLARE_DO_FUN(do_farsight);
DECLARE_DO_FUN(do_feed);
DECLARE_DO_FUN(do_fighting);
DECLARE_DO_FUN(do_fill);
DECLARE_DO_FUN(do_finger);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_flury);
DECLARE_DO_FUN(do_follow);
DECLARE_DO_FUN(do_force);
DECLARE_DO_FUN(do_freeze);
DECLARE_DO_FUN(do_fullname);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_give);
DECLARE_DO_FUN(do_gorge);
DECLARE_DO_FUN(do_gossip);
DECLARE_DO_FUN(do_quote);
DECLARE_DO_FUN(do_gratz);
DECLARE_DO_FUN(do_goto);
DECLARE_DO_FUN(do_group);
DECLARE_DO_FUN(do_gtell);
DECLARE_DO_FUN(do_guard);
DECLARE_DO_FUN(do_heighten);
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_hero);
DECLARE_DO_FUN(do_hide);
DECLARE_DO_FUN(do_high_kick);
DECLARE_DO_FUN(do_hlist);
DECLARE_DO_FUN(do_hnumber);
DECLARE_DO_FUN(do_holylight);
DECLARE_DO_FUN(do_idea);
DECLARE_DO_FUN(do_ideas);
DECLARE_DO_FUN(do_immlist);
DECLARE_DO_FUN(do_immtalk);
DECLARE_DO_FUN(do_indestructable);
DECLARE_DO_FUN(do_induct);
DECLARE_DO_FUN(do_info);
DECLARE_DO_FUN(do_inventory);
DECLARE_DO_FUN(do_invoke);
DECLARE_DO_FUN(do_invis);
DECLARE_DO_FUN(do_irongrip);
DECLARE_DO_FUN(do_jab_punch);
DECLARE_DO_FUN(do_join);
DECLARE_DO_FUN(do_jump_kick);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_kidney_punch);
DECLARE_DO_FUN(do_kill);
DECLARE_DO_FUN(do_list);
DECLARE_DO_FUN(do_lock);
DECLARE_DO_FUN(do_chest);
DECLARE_DO_FUN(do_log);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_lowrecall);
DECLARE_DO_FUN(do_memory);
DECLARE_DO_FUN(do_mental_drain);
DECLARE_DO_FUN(do_mfind);
DECLARE_DO_FUN(do_mload);
DECLARE_DO_FUN(do_mset);
DECLARE_DO_FUN(do_mstat);
DECLARE_DO_FUN(do_mwhere);
DECLARE_DO_FUN(do_murde);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_music);
DECLARE_DO_FUN(do_newbie);
DECLARE_DO_FUN(do_newcorpse);
DECLARE_DO_FUN(do_newlock);
DECLARE_DO_FUN(do_noemote);
DECLARE_DO_FUN(do_north);
DECLARE_DO_FUN(do_note);
DECLARE_DO_FUN(do_notell);
DECLARE_DO_FUN(do_nukerep);
DECLARE_DO_FUN(do_numlock);
DECLARE_DO_FUN(do_ofind);
DECLARE_DO_FUN(do_olist);
DECLARE_DO_FUN(do_oload);
DECLARE_DO_FUN(do_ooc);
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_order);
DECLARE_DO_FUN(do_oset);
DECLARE_DO_FUN(do_ostat);
DECLARE_DO_FUN(do_outcast);
DECLARE_DO_FUN(do_owhere);
DECLARE_DO_FUN(do_pagelen);
DECLARE_DO_FUN(do_pardon);
DECLARE_DO_FUN(do_password);
DECLARE_DO_FUN(do_patch);
DECLARE_DO_FUN(do_peace);
DECLARE_DO_FUN(do_pick);
DECLARE_DO_FUN(do_pkill);
DECLARE_DO_FUN(do_plan);
DECLARE_DO_FUN(do_playerlist);
DECLARE_DO_FUN(do_pload);
DECLARE_DO_FUN(do_poison_weapon);
DECLARE_DO_FUN(do_poof);
DECLARE_DO_FUN(do_pose);
DECLARE_DO_FUN(do_practice);
DECLARE_DO_FUN(do_pray);
DECLARE_DO_FUN(do_prompt);
DECLARE_DO_FUN(do_punch);
DECLARE_DO_FUN(do_purge);
DECLARE_DO_FUN(do_put);
DECLARE_DO_FUN(do_pwhere);
DECLARE_DO_FUN(do_quaff);
DECLARE_DO_FUN(do_question);
DECLARE_DO_FUN(do_qui);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_race_edit);
DECLARE_DO_FUN(do_racelist);
DECLARE_DO_FUN(do_reboo);
DECLARE_DO_FUN(do_reboot);
DECLARE_DO_FUN(do_rebuild);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_recho);
DECLARE_DO_FUN(do_recite);
DECLARE_DO_FUN(do_remake);
DECLARE_DO_FUN(do_remote);
DECLARE_DO_FUN(do_remove);
DECLARE_DO_FUN(do_rent);
DECLARE_DO_FUN(do_repair);
DECLARE_DO_FUN(do_reply);
DECLARE_DO_FUN(do_report);
DECLARE_DO_FUN(do_rescue);
DECLARE_DO_FUN(do_rest);
DECLARE_DO_FUN(do_restrict);
DECLARE_DO_FUN(do_restore);
DECLARE_DO_FUN(do_retreat);
DECLARE_DO_FUN(do_retrieve);
DECLARE_DO_FUN(do_return);
DECLARE_DO_FUN(do_roundhouse_punch);
DECLARE_DO_FUN(do_rset);
DECLARE_DO_FUN(do_rstat);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_save);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_score);
/* Added for scan functionality by Akasha 03-11-2001 */
DECLARE_DO_FUN(do_scan);
DECLARE_DO_FUN(do_sell);
DECLARE_DO_FUN(do_seize);
DECLARE_DO_FUN(do_separate);
DECLARE_DO_FUN(do_setlev);
DECLARE_DO_FUN(do_shadow);
DECLARE_DO_FUN(do_shadow_walk);
DECLARE_DO_FUN(do_shutdow);
DECLARE_DO_FUN(do_shutdown);
DECLARE_DO_FUN(do_silence);
DECLARE_DO_FUN(do_sla);
DECLARE_DO_FUN(do_slay);
DECLARE_DO_FUN(do_slaymes);
DECLARE_DO_FUN(do_sleep);
DECLARE_DO_FUN(do_slist);
DECLARE_DO_FUN(do_slookup);
DECLARE_DO_FUN(do_smash);
DECLARE_DO_FUN(do_snatch);
DECLARE_DO_FUN(do_sneak);
DECLARE_DO_FUN(do_snoop);
DECLARE_DO_FUN(do_sockets);
DECLARE_DO_FUN(do_socials);
DECLARE_DO_FUN(do_soulstrike);
DECLARE_DO_FUN(do_south);
DECLARE_DO_FUN(do_spells);
DECLARE_DO_FUN(do_spin_kick);
DECLARE_DO_FUN(do_split);
DECLARE_DO_FUN(do_sset);
DECLARE_DO_FUN(do_sstat);
DECLARE_DO_FUN(do_sstime);
DECLARE_DO_FUN(do_stand);
DECLARE_DO_FUN(do_steal);
DECLARE_DO_FUN(do_store);
DECLARE_DO_FUN(do_stun);
DECLARE_DO_FUN(do_switch);
DECLARE_DO_FUN(do_tell);
DECLARE_DO_FUN(do_telnetga);
DECLARE_DO_FUN(do_throw);
DECLARE_DO_FUN(do_trip);
DECLARE_DO_FUN(do_time);
DECLARE_DO_FUN(do_title);
DECLARE_DO_FUN(do_todo);
DECLARE_DO_FUN(do_track);
DECLARE_DO_FUN(do_train);
DECLARE_DO_FUN(do_transfer);
DECLARE_DO_FUN(do_transport);
DECLARE_DO_FUN(do_trmes);
DECLARE_DO_FUN(do_trust);
DECLARE_DO_FUN(do_typo);
DECLARE_DO_FUN(do_typos);
DECLARE_DO_FUN(do_unlock);
DECLARE_DO_FUN(do_up);
DECLARE_DO_FUN(do_uppercut_punch);
DECLARE_DO_FUN(do_value);
DECLARE_DO_FUN(do_vent);
DECLARE_DO_FUN(do_visible);
DECLARE_DO_FUN(do_voodo);
DECLARE_DO_FUN(do_vused);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_ward);
DECLARE_DO_FUN(do_wear);
DECLARE_DO_FUN(do_weather);
DECLARE_DO_FUN(do_west);
DECLARE_DO_FUN(do_where);
DECLARE_DO_FUN(do_who);
DECLARE_DO_FUN(do_huntwho);

DECLARE_DO_FUN(do_whotype);
DECLARE_DO_FUN(do_wimpy);
DECLARE_DO_FUN(do_withdraw);
DECLARE_DO_FUN(do_wizhelp);
DECLARE_DO_FUN(do_wizify);
/*DECLARE_DO_FUN( do_wizlist      ); */
DECLARE_DO_FUN(do_wizlock);
DECLARE_DO_FUN(do_wiznet);
DECLARE_DO_FUN(do_worth);
DECLARE_DO_FUN(do_wrlist);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_zap);
DECLARE_DO_FUN(do_stare);
/* XOR */
/*DECLARE_DO_FUN( do_vnum               ); */
DECLARE_DO_FUN(do_load);
DECLARE_DO_FUN(do_push);
DECLARE_DO_FUN(do_drag);
DECLARE_DO_FUN(do_guild);
DECLARE_DO_FUN(do_guilds);
DECLARE_DO_FUN(do_unguild);
DECLARE_DO_FUN(do_setrank);
DECLARE_DO_FUN(do_gdt);
DECLARE_DO_FUN(do_drain_life);
DECLARE_DO_FUN(do_gouge);
DECLARE_DO_FUN(do_alchemy);
DECLARE_DO_FUN(do_scribe);
DECLARE_DO_FUN(do_multiburst);
/* Necro & Werewolf skills by Hannibal */
DECLARE_DO_FUN(do_gravebind);
DECLARE_DO_FUN(do_rage);
DECLARE_DO_FUN(do_bite);
DECLARE_DO_FUN(do_rush);
DECLARE_DO_FUN(do_howl_of_fear);
DECLARE_DO_FUN(do_scent);
DECLARE_DO_FUN(do_frenzy);
DECLARE_DO_FUN(do_reflex);
DECLARE_DO_FUN(do_palm);
DECLARE_DO_FUN(do_rake);
/*
 * Racial Skills Start Here -- Hannibal
 */
DECLARE_DO_FUN(do_headbutt);	/* Minotaur */
DECLARE_DO_FUN(do_globedarkness);	/* Drow */
DECLARE_DO_FUN(do_drowfire);	/* Drow */
DECLARE_DO_FUN(do_forge);	/* Dwarf */
DECLARE_DO_FUN(do_spit);	/* Liz. */
/* Monk skills -- Hannibal */
DECLARE_DO_FUN(do_flamehand);
DECLARE_DO_FUN(do_frosthand);
DECLARE_DO_FUN(do_chaoshand);
DECLARE_DO_FUN(do_bladepalm);
DECLARE_DO_FUN(do_flyingkick);
DECLARE_DO_FUN(do_nerve);
DECLARE_DO_FUN(do_ironfist);
/* Bard skill */
DECLARE_DO_FUN(do_shriek);
/* Ban Functions -- Hannibal */
DECLARE_DO_FUN(do_permban);
DECLARE_DO_FUN(do_tempban);
DECLARE_DO_FUN(do_newban);
DECLARE_DO_FUN(do_banlist);
DECLARE_DO_FUN(do_home);
DECLARE_DO_FUN(do_avatar);
DECLARE_DO_FUN(do_search);
DECLARE_DO_FUN(do_rental);
DECLARE_DO_FUN(do_checkout);

DECLARE_DO_FUN(do_aedit);	/* OLC 1.1b */
DECLARE_DO_FUN(do_redit);	/* OLC 1.1b */
DECLARE_DO_FUN(do_oedit);	/* OLC 1.1b */
DECLARE_DO_FUN(do_medit);	/* OLC 1.1b */
DECLARE_DO_FUN(do_cedit);	/* IchiCode 1.1b */
DECLARE_DO_FUN(do_hedit);	/* XOR 3.14159265359r^2 */
DECLARE_DO_FUN(do_sedit);	/* Decklarean */
DECLARE_DO_FUN(do_spedit);	/* Decklarean */
DECLARE_DO_FUN(do_rename_obj);	/* Decklarean */
DECLARE_DO_FUN(do_race_edit);	/* Decklarean */
DECLARE_DO_FUN(do_mreset);	/* Decklarean */
DECLARE_DO_FUN(do_nedit);	/* Angi */
DECLARE_DO_FUN(do_asave);
DECLARE_DO_FUN(do_alist);
DECLARE_DO_FUN(do_resets);
DECLARE_DO_FUN(do_alias);
DECLARE_DO_FUN(do_clear);	/* Angi */

DECLARE_DO_FUN(do_recent);
DECLARE_DO_FUN(do_resetpass);
DECLARE_DO_FUN(do_exlist);
DECLARE_DO_FUN(do_vlist);
DECLARE_DO_FUN(do_rename);
DECLARE_DO_FUN(do_for);
DECLARE_DO_FUN(do_board);
DECLARE_DO_FUN(do_authorize);
DECLARE_DO_FUN(do_aset);
DECLARE_DO_FUN(do_ignore);
