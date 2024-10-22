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
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mud.h"

#define			MAX_PERM_BLOCK	131072
int			nAllocPerm;
int			sAllocPerm;

/* globals from db.c for load_notes */
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif
extern FILE *                  fpArea;
extern char                    strArea[MAX_INPUT_LENGTH];

/* local procedures */
void loads_thread(char *name, NOTES_DATA **list, int type, time_t free_time);
void parses_note(CHAR_DATA *ch, char *argument, int type);
bool hides_note(CHAR_DATA *ch, NOTES_DATA *newnote);
char *remove_re_subject( char *subject );
char *cut_name( char *str, char *namelist );

NOTES_DATA *notes_list;
NOTES_DATA *ideas_list;
NOTES_DATA *penaltys_list;
NOTES_DATA *newss_list;
NOTES_DATA *changess_list;
NOTES_DATA *notes_free;


char *  cut_name ( char *str, char *namelist )
{   
    char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
    char *list;
    char string[MAX_INPUT_LENGTH];

    string[0] = '\0';
    list = one_argument(namelist,part);
    one_argument(str,name);
    for ( ; part[0] != '\0' ; )
    {
	if ( str_cmp(part,name) )
	{
	    strcat(string,part);
	    strcat(string," ");
	}
	list = one_argument(list,part);	    
    }
    return str_alloc(string);
}


char * remove_re_subject( char * subject )
{
    char * string;

    if ( *subject == '\0' )
	{
	return subject;
	}

    string = subject;
    for (  ; ; string++ )
    {
	for ( ; *string == ' '; string++ );
  	if ( !strncmp("Re:",string,3) || !strncmp("re:",string,3) )
	{
	    string++;
	    string++;
	    string++;
	}
	else
	    return string;
	if ( *string == '\0' )
	{	
	    return " ";
	}
    }
}


NOTES_DATA *news_note()
{
    NOTES_DATA *newnote;

    if (notes_free == NULL)
	newnote = alloc_perm(sizeof(*newnote));
    else
    { 
	newnote = notes_free;
	notes_free = notes_free->next;
    }
    VALIDATE(newnote);
    return newnote;
}

void frees_note(NOTES_DATA *newnote)
{
    if (!IS_VALID(newnote))
	return;

    STRFREE( newnote->text    );
    STRFREE( newnote->subject );
    STRFREE( newnote->to_list );
    STRFREE( newnote->date    );
    STRFREE( newnote->sender  );
    DISPOSE(newnote);
}

int count_spool(CHAR_DATA *ch, NOTES_DATA *spool)
{
    int count = 0;
    NOTES_DATA *newnote;

    for (newnote = spool; newnote != NULL; newnote = newnote->next)
	if (!hides_note(ch,newnote))
	    count++;

    return count;
}

void do_unread(CHAR_DATA *ch)
{
    char buf[MAX_STRING_LENGTH];
    int count;
    bool found = FALSE;

    if (IS_NPC(ch))
	return; 

    if ((count = count_spool(ch,newss_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"There %s %d new news article%s waiting.\n\r",
	    count > 1 ? "are" : "is",count, count > 1 ? "s" : "");
	send_to_char(buf,ch);
    }
    if ((count = count_spool(ch,changess_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"There %s %d change%s waiting to be read.\n\r",
	    count > 1 ? "are" : "is", count, count > 1 ? "s" : "");
        send_to_char(buf,ch);
    }
    if ((count = count_spool(ch,notes_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"You have %d new note%s waiting.\n\r",
	    count, count > 1 ? "s" : "");
	send_to_char(buf,ch);
    }
    if ((count = count_spool(ch,ideas_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"You have %d unread idea%s to peruse.\n\r",
	    count, count > 1 ? "s" : "");
	send_to_char(buf,ch);
    }
    if (IS_IMMORTAL(ch) && (count = count_spool(ch,penaltys_list)) > 0)
    {
	found = TRUE;
	sprintf(buf,"%d %s been added.\n\r",
	    count, count > 1 ? "penalties have" : "penalty has");
	send_to_char(buf,ch);
    }

    if (!found)
	send_to_char("You have no unread notes.\n\r",ch);
}

void do_snote(CHAR_DATA *ch,char *argument)
{
    parses_note(ch,argument,NOTES_NOTE);
}

void do_sidea(CHAR_DATA *ch,char *argument)
{
    parses_note(ch,argument,NOTES_IDEA);
}

void do_spenalty(CHAR_DATA *ch,char *argument)
{
    parses_note(ch,argument,NOTES_PENALTY);
}

void do_snews(CHAR_DATA *ch,char *argument)
{
    parses_note(ch,argument,NOTES_NEWS);
}

void do_schanges(CHAR_DATA *ch,char *argument)
{
    parses_note(ch,argument,NOTES_CHANGES);
}

void saves_notes(int type)
{
    FILE *fp;
    char *name;
    NOTES_DATA *newnote;

    switch (type)
    {
	default:
	    return;
	case NOTES_NOTE:
	    name = NOTES_FILE;
	    newnote = notes_list;
	    break;
	case NOTES_IDEA:
	    name = IDEAS_FILE;
	    newnote = ideas_list;
	    break;
	case NOTES_PENALTY:
	    name = PENALTYS_FILE;
	    newnote = penaltys_list;
	    break;
	case NOTES_NEWS:
	    name = NEWSS_FILE;
	    newnote = newss_list;
	    break;
	case NOTES_CHANGES:
	    name = CHANGESS_FILE;
	    newnote = changess_list;
	    break;
    }

    fclose( fpReserve );
    if ( ( fp = fopen( name, "w" ) ) == NULL )
    {
	perror( name );
    }
    else
    {
	for ( ; newnote != NULL; newnote = newnote->next )
	{
	    fprintf( fp, "Sender  %s~\n", newnote->sender);
	    fprintf( fp, "Date    %s~\n", newnote->date);
	    fprintf( fp, "Stamp   %ld\n", newnote->date_stamp);
	    fprintf( fp, "To      %s~\n", newnote->to_list);
	    fprintf( fp, "Subject %s~\n", newnote->subject);
	    fprintf( fp, "Text\n%s~\n",   newnote->text);
	}
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
   	return;
    }
}
void loads_notes(void)
{
    loads_thread(NOTES_FILE,&notes_list, NOTES_NOTE, 14*24*60*60);
    loads_thread(IDEAS_FILE,&ideas_list, NOTES_IDEA, 28*24*60*60);
    loads_thread(PENALTYS_FILE,&penaltys_list, NOTES_PENALTY, 0);
    loads_thread(NEWSS_FILE,&newss_list, NOTES_NEWS, 0);
    loads_thread(CHANGESS_FILE,&changess_list,NOTES_CHANGES, 0);
}

void loads_thread(char *name, NOTES_DATA **list, int type, time_t free_time)
{
    FILE *fp;
    NOTES_DATA *newnotelast;
 
    if ( ( fp = fopen( name, "r" ) ) == NULL )
	return;
	 
    newnotelast = NULL;
    for ( ; ; )
    {
	NOTES_DATA *newnote;
	char letter;
	 
	do
	{
	    letter = getc( fp );
            if ( feof(fp) )
            {
                fclose( fp );
                return;
            }
        }
        while ( isspace(letter) );
        ungetc( letter, fp );
 
        newnote           = alloc_perm( sizeof(*newnote) );
 
        if ( str_cmp( fread_word( fp ), "sender" ) )
            break;
        newnote->sender   = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "date" ) )
            break;
        newnote->date     = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "stamp" ) )
            break;
        newnote->date_stamp = fread_number(fp);
 
        if ( str_cmp( fread_word( fp ), "to" ) )
            break;
        newnote->to_list  = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "subject" ) )
            break;
        newnote->subject  = fread_string( fp );
 
        if ( str_cmp( fread_word( fp ), "Text" ) )
            break;
        newnote->text     = fread_string( fp );
 
        if (free_time && newnote->date_stamp < current_time - free_time)
        {
	    frees_note(newnote);
            continue;
        }

	newnote->type = type;
 
        if (*list == NULL)
            *list           = newnote;
        else
            newnotelast->next     = newnote;
 
        newnotelast       = newnote;
    }
 
    strcpy( strArea, NOTES_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}

void appends_note(NOTES_DATA *newnote)
{
    FILE *fp;
    char *name;
    NOTES_DATA **list;
    NOTES_DATA *last;

    switch(newnote->type)
    {
	default:
	    return;
	case NOTES_NOTE:
	    name = NOTES_FILE;
	    list = &notes_list;
	    break;
	case NOTES_IDEA:
	    name = IDEAS_FILE;
	    list = &ideas_list;
	    break;
	case NOTES_PENALTY:
	    name = PENALTYS_FILE;
	    list = &penaltys_list;
	    break;
	case NOTES_NEWS:
	     name = NEWSS_FILE;
	     list = &newss_list;
	     break;
	case NOTES_CHANGES:
	     name = CHANGESS_FILE;
	     list = &changess_list;
	     break;
    }

    if (*list == NULL)
	*list = newnote;
    else
    {
	for ( last = *list; last->next != NULL; last = last->next);
	last->next = newnote;
    }

    fclose(fpReserve);
    if ( ( fp = fopen(name, "a" ) ) == NULL )
    {
        perror(name);
    }
    else
    {
        fprintf( fp, "Sender  %s~\n", newnote->sender);
        fprintf( fp, "Date    %s~\n", newnote->date);
        fprintf( fp, "Stamp   %ld\n", newnote->date_stamp);
        fprintf( fp, "To      %s~\n", newnote->to_list);
        fprintf( fp, "Subject %s~\n", newnote->subject);
        fprintf( fp, "Text\n%s~\n",   newnote->text);
        fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
}

bool is_notes_to( CHAR_DATA *ch, NOTES_DATA *newnote )
{
    if ( !str_cmp( ch->name, newnote->sender ) )
	return TRUE;

    if ( is_name( "all", newnote->to_list ) )
	return TRUE;

    if ( IS_IMMORTAL(ch) && is_name( "immortal", newnote->to_list ) )
	return TRUE;

    if ( is_name( ch->name, newnote->to_list ) )
	return TRUE;

    return FALSE;
}


void notes_attach( CHAR_DATA *ch, int type )
{
    NOTES_DATA *newnote;

    if ( ch->newnote )
	return;

    CREATE( newnote, NOTES_DATA, 1 );
    newnote->next		= NULL;
    newnote->sender	        = QUICKLINK( ch->name );
    newnote->date		= STRALLOC( "" );
    newnote->to_list	        = STRALLOC( "" );
    newnote->subject 	        = STRALLOC( "" );
    newnote->text		= STRALLOC( "" );
    newnote->type		= type;
    ch->newnote	         	= newnote;
    return;

}



void notes_remove( CHAR_DATA *ch, NOTES_DATA *newnote, bool delete)
{
    char to_new[MAX_INPUT_LENGTH];
    char to_one[MAX_INPUT_LENGTH];
    NOTES_DATA *prev;
    NOTES_DATA **list;
    char *to_list;

    if (!delete)
    {
	/* make a new list */
        to_new[0]	= '\0';
        to_list	= newnote->to_list;
        while ( *to_list != '\0' )
        {
    	    to_list	= one_argument( to_list, to_one );
    	    if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	    {
	        strcat( to_new, " " );
	        strcat( to_new, to_one );
	    }
        }
        /* Just a simple recipient removal? */
       if ( str_cmp( ch->name, newnote->sender ) && to_new[0] != '\0' )
       {
	   STRFREE( newnote->to_list );
	   newnote->to_list = STRALLOC( to_new + 1 );
	   return;
       }
    }
    /* nuke the whole note */

    switch(newnote->type)
    {
	default:
	    return;
	case NOTES_NOTE:
	    list = &notes_list;
	    break;
	case NOTES_IDEA:
	    list = &ideas_list;
	    break;
	case NOTES_PENALTY:
	    list = &penaltys_list;
	    break;
	case NOTES_NEWS:
	    list = &newss_list;
	    break;
	case NOTES_CHANGES:
	    list = &changess_list;
	    break;
    }

    /*
     * Remove note from linked list.
     */
    if ( newnote == *list )
    {
	*list = newnote->next;
    }
    else
    {
	for ( prev = *list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == newnote )
		break;
	}

	if ( prev == NULL )
	{
	    bug( "Note_remove: newnote not found.", 0 );
	    return;
	}

	prev->next = newnote->next;
    }

    saves_notes(newnote->type);
    frees_note(newnote);
    return;
}

bool hides_note (CHAR_DATA *ch, NOTES_DATA *newnote)
{
    time_t lasts_read;

    if (IS_NPC(ch))
	return TRUE;

    switch (newnote->type)
    {
	default:
	    return TRUE;
	case NOTES_NOTE:
	    lasts_read = ch->pcdata->last2_note;
	    break;
	case NOTES_IDEA:
	    lasts_read = ch->pcdata->last2_idea;
	    break;
	case NOTES_PENALTY:
	    lasts_read = ch->pcdata->last2_penalty;
	    break;
	case NOTES_NEWS:
	    lasts_read = ch->pcdata->last2_news;
	    break;
	case NOTES_CHANGES:
	    lasts_read = ch->pcdata->last2_changes;
	    break;
    }
    
    if (newnote->date_stamp <= lasts_read)
	return TRUE;

    if (!str_cmp(ch->name,newnote->sender))
	return TRUE;

    if (!is_notes_to(ch,newnote))
	return TRUE;

    return FALSE;
}

void updates_read(CHAR_DATA *ch, NOTES_DATA *newnote)
{
    time_t stamp;

    if (IS_NPC(ch))
	return;

    stamp = newnote->date_stamp;

    switch (newnote->type)
    {
        default:
            return;
        case NOTES_NOTE:
	    ch->pcdata->last2_note = UMAX(ch->pcdata->last2_note,stamp);
            break;
        case NOTES_IDEA:
	    ch->pcdata->last2_idea = UMAX(ch->pcdata->last2_idea,stamp);
            break;
        case NOTES_PENALTY:
	    ch->pcdata->last2_penalty = UMAX(ch->pcdata->last2_penalty,stamp);
            break;
        case NOTES_NEWS:
	    ch->pcdata->last2_news = UMAX(ch->pcdata->last2_news,stamp);
            break;
        case NOTES_CHANGES:
	    ch->pcdata->last2_changes = UMAX(ch->pcdata->last2_changes,stamp);
            break;
    }
}

void parses_note( CHAR_DATA *ch, char *argument, int type )
{
    EXTRA_DESCR_DATA *ed = NULL;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    NOTES_DATA *newnote;
    NOTES_DATA **list;
    char *list_name;
    int vnum;
    int anum;

    if ( IS_NPC(ch) )
	return;

    switch(type)
    {
	default:
	    return;
        case NOTES_NOTE:
            list = &notes_list;
	    list_name = "notes";
            break;
        case NOTES_IDEA:
            list = &ideas_list;
	    list_name = "ideas";
            break;
        case NOTES_PENALTY:
            list = &penaltys_list;
	    list_name = "penalties";
            break;
        case NOTES_NEWS:
            list = &newss_list;
	    list_name = "news";
            break;
        case NOTES_CHANGES:
            list = &changess_list;
	    list_name = "changes";
            break;
    }

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    switch( ch->substate )
    {
       default:
         break;

        case SUB_NOTES:
          ed = ch->dest_buf;
	  STRFREE( ch->newnote->text );
	  ch->newnote->text = copy_buffer( ch );
	  stop_editing( ch );	   
	  return;
    }

    if ( arg[0] == '\0' || !str_prefix( arg, "read" ) )
    {
        bool fAll;
 
        if ( !str_cmp( argument, "all" ) )
        {
            fAll = TRUE;
            anum = 0;
        }
 
        else if ( argument[0] == '\0' || !str_prefix(argument, "next"))
        /* read next unread note */
        {
            vnum = 0;
            for ( newnote = *list; newnote != NULL; newnote = newnote->next)
            {
                if (!hides_note(ch,newnote))
                {
                    sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
                        vnum,
                        newnote->sender,
                        newnote->subject,
                        newnote->date,
                        newnote->to_list);
                    send_to_pager( buf, ch );
   		    send_to_pager(newnote->text, ch); 
                    updates_read(ch,newnote);
                    return;
                }
                else if (is_notes_to(ch,newnote))
                    vnum++;
            }
	    sprintf(buf,"You have no unread %s.\n\r",list_name);
	    send_to_char(buf,ch);
            return;
        }
 
        else if ( is_number( argument ) )
        {
            fAll = FALSE;
            anum = atoi( argument );
        }
        else
        {
            send_to_char( "Read which number?\n\r", ch );
            return;
        }
 
        vnum = 0;
        for ( newnote = *list; newnote != NULL; newnote = newnote->next )
        {
            if ( is_notes_to( ch, newnote ) && ( vnum++ == anum || fAll ) )
            {
                sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
                    vnum - 1,
                    newnote->sender,
                    newnote->subject,
                    newnote->date,
                    newnote->to_list
                    );
                send_to_pager( buf, ch );
   		send_to_pager(newnote->text, ch); 
		updates_read(ch,newnote);
                return;
            }
        }
 
	sprintf(buf,"There aren't that many %s.\n\r",list_name);
	send_to_char(buf,ch);
        return;
    }

    if ( !str_prefix( arg, "list" ) )
    {
	vnum = 0;
	for ( newnote = *list; newnote != NULL; newnote = newnote->next )
	{
	    if ( is_notes_to( ch, newnote ) )
	    {
		sprintf( buf, "[%3d%s] %s: %s\n\r",
		    vnum, hides_note(ch,newnote) ? " " : "N", 
		    newnote->sender, newnote->subject );
		send_to_char( buf, ch );
		vnum++;
	    }
	}
	if (!vnum)
	{
	    switch(type)
	    {
		case NOTES_NOTE:	
		    send_to_char("There are no notes for you.\n\r",ch);
		    break;
		case NOTES_IDEA:
		    send_to_char("There are no ideas for you.\n\r",ch);
		    break;
		case NOTES_PENALTY:
		    send_to_char("There are no penalties for you.\n\r",ch);
		    break;
		case NOTES_NEWS:
		    send_to_char("There is no news for you.\n\r",ch);
		    break;
		case NOTES_CHANGES:
		    send_to_char("There are no changes for you.\n\r",ch);
		    break;
	    }
	}
	return;
    }

    if ( !str_prefix( arg, "remove" ) )
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Note remove which number?\n\r", ch );
            return;
        }
 
        anum = atoi( argument );
        vnum = 0;
        for ( newnote = *list; newnote != NULL; newnote = newnote->next )
        {
            if ( is_notes_to( ch, newnote ) && vnum++ == anum )
            {
                notes_remove( ch, newnote, FALSE );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
        }
 
	sprintf(buf,"There aren't that many %s.",list_name);
	send_to_char(buf,ch);
        return;
    }
 


    if ( !str_prefix( arg, "delete" ) && get_trust(ch) >= MAX_LEVEL - 1)
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Note delete which number?\n\r", ch );
            return;
        }
 
        anum = atoi( argument );
        vnum = 0;
        for ( newnote = *list; newnote != NULL; newnote = newnote->next )
        {
            if ( is_notes_to( ch, newnote ) && vnum++ == anum )
            {
                notes_remove( ch, newnote,TRUE );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
        }

 	sprintf(buf,"There aren't that many %s.",list_name);
	send_to_char(buf,ch);
        return;
    }

    if (!str_prefix(arg,"catchup"))
    {
	switch(type)
	{
	    case NOTES_NOTE:	
		ch->pcdata->last2_note = current_time;
		break;
	    case NOTES_IDEA:
		ch->pcdata->last2_idea = current_time;
		break;
	    case NOTES_PENALTY:
		ch->pcdata->last2_penalty = current_time;
		break;
	    case NOTES_NEWS:
		ch->pcdata->last2_news = current_time;
		break;
	    case NOTES_CHANGES:
		ch->pcdata->last2_changes = current_time;
		break;
	}
	return;
    }

    /* below this point only certain people can edit notes */
    if ((type == NOTES_NEWS && !IS_IMMORTAL(ch))
    ||  (type == NOTES_CHANGES && !IS_IMMORTAL(ch)))
    {
	sprintf(buf,"You aren't high enough level to write %s.",list_name);
	send_to_char(buf,ch);
	return;
    }

    if ( !str_prefix( arg, "reply" ) )
    {

	if ( is_number( argument ) )
	{
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note reply which number?\n\r", ch );
	    return;
	}

	vnum = 0;
	for ( newnote = *list; newnote != NULL; newnote = newnote->next )
	{
	    if ( is_notes_to( ch, newnote ) && ( vnum++ == anum ) )
	    {
		notes_attach( ch, newnote->type );
		STRFREE( ch->newnote->subject );
		buf[0] = '\0';
		strcpy( buf, "Re: " );
		strcat( buf, remove_re_subject( newnote->subject ) );
		ch->newnote->subject = str_alloc( buf );
		STRFREE( ch->newnote->to_list );
		buf[0] = '\0';
		strcpy( buf , cut_name( ch->name, newnote->to_list ) );
		strcat( buf, newnote->sender );
		ch->newnote->to_list = str_alloc( buf );
		sprintf( buf, "Replying to note %d\n\r", vnum - 1 );
		send_to_char( buf, ch );
	        ch->substate = SUB_NOTES;
	        ch->dest_buf = ed;
		start_editing( ch, ch->newnote->text );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "write" ) )
    {
	notes_attach( ch, type );
	if (ch->newnote->type != type)
	{
	    send_to_char(
		"You already have a different note in progress.\n\r",ch);
	    return;
	}
        ch->substate = SUB_NOTES;
        ch->dest_buf = ed;
	start_editing( ch, ch->newnote->text );
	return;
    }

    if ( !str_prefix( arg, "subject" ) )
    {
	notes_attach( ch,type );
        if (ch->newnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }

	STRFREE( ch->newnote->subject );
	ch->newnote->subject = STRALLOC( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "to" ) )
    {
	notes_attach( ch,type );
        if (ch->newnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }
	STRFREE( ch->newnote->to_list );
	ch->newnote->to_list = STRALLOC( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "clear" ) )
    {
	if ( ch->newnote != NULL )
	{
	    frees_note(ch->newnote);
	    ch->newnote = NULL;
	}

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "show" ) )
    {
	if ( ch->newnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	if (ch->newnote->type != type)
	{
	    send_to_char("You aren't working on that kind of note.\n\r",ch);
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
	    ch->newnote->sender,
	    ch->newnote->subject,
	    ch->newnote->to_list );
	send_to_char( buf, ch );
	ch_printf(ch, ch->newnote->text);
	return;
    }

    if ( !str_prefix( arg, "post" ) || !str_prefix(arg, "send"))
    {
	char *strtime;

	if ( ch->newnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

        if (ch->newnote->type != type)
        {
            send_to_char("You aren't working on that kind of note.\n\r",ch);
            return;
        }

	if (!str_cmp(ch->newnote->to_list,""))
	{
	    send_to_char(
		"You need to provide a recipient (name, all, or immortal).\n\r",
		ch);
	    return;
	}

	if (!str_cmp(ch->newnote->subject,""))
	{
	    send_to_char("You need to provide a subject.\n\r",ch);
	    return;
	}

	ch->newnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	ch->newnote->date			= STRALLOC( strtime );
	ch->newnote->date_stamp		= current_time;

	appends_note(ch->newnote);
	ch->newnote = NULL;
	return;
    }

    send_to_char( "You can't do that.\n\r", ch );
    return;
}

/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
    static char *pMemPerm;
    static int iMemPerm;
    void *pMem;

    while ( sMem % sizeof(long) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}
