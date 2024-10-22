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
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "merc.h"
#include "interp.h"

void separator(char* buf, char sign, int len)
{
	char tbuf[MSL];
	int i;
	tbuf[0] = '\0';
	for (i=0;i < len;i++)
		tbuf[i] = sign;
	tbuf[len] = '\0';
	strcat(tbuf,"\n\r\0");
	strcat(buf,tbuf);
	return;
}

void endstr(char* instr)
{
	strcat(instr,"\n\r\0");
	return;
}

void midstr(char* outstr, char* instr, int len)
{
	int left, right, modif;
	int i;
	char tbuf[MSL];
	// first calculate the left and right space
	modif = (len-strlen(instr)) / 2;
	left = modif;
	right = modif;
	for(i = 0; i < left;i++)
	{
		tbuf[i] = ' ';
	}
	tbuf[left] = '\0';
	strcat(tbuf,instr);
	right = right + strlen(tbuf);
	for (i = strlen(tbuf);i < right;i++)
		tbuf[i] = ' ';
	if ((modif % 2) != 0)
	{ 
		right = i+1;
		tbuf[right] = ' ';
		right++;
	}
	tbuf[right] = '\0';
	outstr[0] = '\0';
	strcpy(outstr,tbuf);
	return;
}

void whorace(CHAR_DATA* ch, char* out)
{
	char buf[MSL];
	strcpy(buf,(get_race_data(ch->race))->race_name);
	strcpy(out,buf);
	return;
}

void whoclass(CHAR_DATA* wch, char* out)
{
	char* class;
	class = class_short(wch);
	if (wch->level >= LEVEL_HERO)
	    switch (wch->level) 
	    {
	    default:
		break;
	    case L_IMP:
		class = wch->sex == 2 ? "{MShards QUEEN{x"
		    : "{MShards KING{x";
		break;
	    case L_CON:
		class = "CODER";
		break;
	    case L_DIR:
		class = "COUNCIL";
		break;
	    case L_GOD:
		class = "GUARDIAN";
		break;
	    case L_SEN:
		class = wch->sex == 2 ? "GODDESS"
		    : "GOD";
		break;
	    case L_DEM:
		class = "BUILDER";
		break;
	    case L_JUN:
		class = "IMMORTAL";
		break;
	    case L_APP:
		class = "AVATAR";
		break;
	    case L_CHAMP5:
	    case L_CHAMP4:
	    case L_CHAMP3:
	    case L_CHAMP2:
	    case L_CHAMP1:
	    case LEVEL_HERO:
		switch (prime_class(wch)) 
		{
		default:
		    class = "CHAMPION";
		    break;
		case 0:
		    class = "{BARCH MAGUS{x";
		    break;
		case 1:
		    class = wch->sex == 2 ? "{CMATRIARCH{x"
			: "{CPATRIARCH{x";
		    break;
		case 2:
		    class = "{kBLACKGUARD{x";
		    break;
		case 3:
		    class = "{WKNIGHT{x";
		    break;
		case 4:
		    class = "{GMINDBENDER{x";
		    break;
		case 5:
		    class = "{gHIEROPHANT{x";
		    break;
		case 6:
		    class = wch->sex == 2 ? "{gLADYRANGER{x"
			: "{gLORDRANGER{x";
		    break;
		case 7:
		    class = "{wCRUSADER{x";
		    break;
		case 8:
		    class = "{MLOREMASTER{x";
		    break;
		case 9:
		    class = "{rKINDRED{x";
		    break;
		case 10:
		    class = "{cLICHMASTER{x ";
		    break;
		case 11:
		    class = "{yLYCANTHROPE{x";
		    break;
		case 12:
		    class = "{cSENSEI{x";
		    break;
		}
	}
	    
	if (wch->level >= L_APP
	    && wch->pcdata->whotype
	    && str_cmp(wch->pcdata->whotype, "!!!!!!!!!!!!")
	    && str_cmp(wch->pcdata->whotype, "(null)"))
	    class = wch->pcdata->whotype;
	strcpy(out,class);
	return;
}

void whostatus(CHAR_DATA* wch, char* out)
{
	char status[MSL];
	status[0] = '\0';
	if (IS_SET(wch->act, PLR_QUEST)) 
	    strcat(status, "Q");
	if (IS_SET(wch->act, PLR_QUESTOR))
	    strcat(status,"q");
	
	if (wch->desc && wch->desc->editor != 0) 
	{
	    if (wch->desc->editor == 13)	/* forging eq */
		strcat(status,"F"); 
	    else
		strcat(status,"B");
	}

	if (IS_SET(wch->act, PLR_PKILLER)) strcat(status,"K");
        if (IS_SET(wch->act, PLR_AFK)) strcat(status,"A");
	if (IS_SET(wch->act, PLR_WIZINVIS)) strcat(status,"W");
	if (IS_SET(wch->act, PLR_CLOAKED)) strcat(status,"C");
	strcpy(out,status);
	return;
}		

void wholevel(CHAR_DATA* ch, char* out)
{
	char buf[MSL];
	sprintf(buf,"%d",ch->level);
	strcpy(out,buf);
	return;
}

void whoname(CHAR_DATA* wch, char* out)
{
	char buf[MSL];
	if (wch->pcdata->lname)
	    sprintf(buf, "%s%s%s%s\n\r",
		    wch->name, (wch->pcdata->lname[0] != '\0') ? " " : "",
		    wch->pcdata->lname, wch->pcdata->title);
	else
	   sprintf(buf, "%s%s\n\r", wch->name, wch->pcdata->title);
	strcpy(out,buf);
	return;
}

void do_huntwho(CHAR_DATA* ch, char* argument)
{
	/* Hunter's who listing v1.00 
	 * 
	 * To keep it simple, every task is split up in separate
	 * functions (see above)
	 *
	 * We want to see following info:
	 *
	 * Name of player
	 * Title of player
	 * Race of player
	 * Class of player
	 * Level of player
	 * Status of player
	 *
	 */
     	
     	DESCRIPTOR_DATA* d;
	CHAR_DATA* wch;
     	int tot_imm, tot_plr, tot_invis, tot_newb;
     	char buf[MSL];
     	char temp[MSL];
     	char class[MSL];
     	char race[MSL];
     	char status[MSL];
     	char level[MSL];
     	char name[MSL];
     	/* First lets send the boundaries */
     	buf[0] = '\0';
     	temp[0] = '\0';
     	class[0] = '\0';
     	race[0] = '\0';
     	status[0] = '\0';
     	level[0] = '\0';
     	name[0] = '\0';
     	separator(buf,'*', 75);
     	send_to_char(buf,ch);
     	buf[0] = '\0';
     	midstr(buf,"Shards of Destiny - Age of Decadence", 75);
	endstr(buf);
     	send_to_char(buf,ch);
     	buf[0] = '\0';
     	separator(buf,'*', 75);
     	send_to_char(buf,ch);
	buf[0] = '\0';
	/* Now lets start gathering the stats */
	tot_plr = tot_invis = tot_imm = tot_newb = 0;
	
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected != CON_PLAYING) continue;
		tot_plr++;
		wch = (d->original) ? d->original : d->character;
		if (!can_see(ch, wch)) tot_invis++;
		if (wch->level >= L_APP) tot_imm++;
		if (wch->level < 10) tot_newb++;
	}
	
	/* Now lets go thru the characterlist and see if we can get
	 * them to display
	 */
        for (d = descriptor_list; d; d = d->next) 
        {
		CHAR_DATA *wch;
		if (d->connected != CON_PLAYING) continue;
		wch = (d->original) ? d->original : d->character;
		if (!can_see(ch, wch)) continue;
     		class[0] = '\0';
     		race[0] = '\0';
     		status[0] = '\0';
     		level[0] = '\0';
     		name[0] = '\0';
		whorace(wch, race);
		whoclass(wch, class);
		midstr(class, class, 12);
		class[15] = '\0';		// fix for copyover bug
		whostatus(wch, status);
		// if a coder is coding, set status to blinking coding sign
		if (IS_SET(wch->act2, PLR2_CODING))
			strcpy(status,"CODING");
		
		wholevel(wch, level);
		midstr(level, level, 3);
		whoname(wch, name);
		sprintf(buf,"{c[%-3s] {b[%s] {c[%-6s] {b[%-3s] {w%s{x",
		race, class, status, level, name);
		send_to_char(buf,ch);
	}
	// good and now we are done, only need to output the totals
	buf[0] = '\0';
	separator(buf,'-',75);
	send_to_char(buf,ch);
	sprintf(buf,"Online: %d - Invis: %d - Immortals: %d - Newbies: %d", tot_plr, tot_invis, tot_imm, tot_newb);
	midstr(buf, buf, 75);
	endstr(buf);
	send_to_char(buf,ch);
	buf[0] = '\0';
	separator(buf,'-',75);
	send_to_char(buf,ch);
	send_to_char("3rd Column: <W> Wizinvis   <C> Cloaked  <Q> Questing   <q> Running a quest\n\r", ch);
	send_to_char("            <F> Forging EQ <B> Building <K> Plr.Killer <A> AFK\n\r", ch);
	buf[0] = '\0';
	separator(buf,'=',75);
	send_to_char(buf,ch);
	return;
}
	