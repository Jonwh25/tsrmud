/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
*	Alias code modifications by Ron Yacketta (dajy@telmaron.com)	   *
***************************************************************************/
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
 *			     Special boards module			    *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "mud.h"

void alias_sub( DESCRIPTOR_DATA *d, char *alias_sub, char *arg );

/* does aliasing and other fun stuff */
void substitute_alias(DESCRIPTOR_DATA *d, char *argument)
{
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH],name[MAX_INPUT_LENGTH];
    char *point;
    int alias;

    ch = d->original ? d->original : d->character;

    if (IS_NPC(ch) || ch->pcdata->alias[0] == NULL
    ||	!str_prefix("alias",argument) || !str_prefix("una",argument) )
    /*||  !str_prefix("prefix",argument)) */
    {
	interpret(d->character, argument);
	return;
    }

    strcpy(buf,argument);

    for (alias = 0; alias < MAX_ALIAS; alias++)	 /* go through the aliases */
    {
	if (ch->pcdata->alias[alias] == NULL)
	    break;

	if (!str_prefix(ch->pcdata->alias[alias],argument))
	{
	    point = one_argument(argument,name);
	    if (!strcmp(ch->pcdata->alias[alias],name))
	    {
                alias_sub(ch->desc, ch->pcdata->alias_sub[alias], point);
                return;

	    }
	    if (strlen(buf) > MAX_INPUT_LENGTH)
	    {
		send_to_char("Alias substitution too long. Truncated.\r\n",ch);
		buf[MAX_INPUT_LENGTH -1] = '\0';
	    }
	}
    }
    interpret(d->character,buf);
}

bool check_alias(CHAR_DATA *ch, char * alias, bool alias_used[MAX_ALIAS] )
{
    char buf[ MAX_STRING_LENGTH ];
    char * pbuf;
    int pos;

    while ( *alias == ' ' ) alias++;

    if ( *alias == '%' )
    {
	alias++;
	if ( *alias >= '0' && *alias <= '9' )
	    return FALSE;
	alias--;
    }

    for ( pos = 0 ; pos <= MAX_ALIAS && ch->pcdata->alias[ pos ] != NULL ; pos++ )
        if ( !str_prefix( ch->pcdata->alias[ pos ], alias ) )
	{
	    if ( alias_used[ pos ] )
		return FALSE;
	    else
	    {
		alias_used[ pos ] = TRUE;
		if ( !check_alias( ch, ch->pcdata->alias_sub[ pos ], alias_used ) )
		    return FALSE;
	    }
	}

    strcpy( buf, alias );
    
    pbuf = buf;    
    while ( ( pbuf = strchr( pbuf, '%' ) ) != NULL  && *(++pbuf) != ';' );

    if ( pbuf != NULL && *pbuf != '\0' )
    {
	pbuf++;
	if ( !check_alias( ch, pbuf, alias_used ) )
	    return FALSE;
    }

    return TRUE;
}

void do_alia(CHAR_DATA *ch, char *argument)
{
    send_to_char("I'm sorry, alias must be entered in full.\n\r",ch);
    return;
}

void do_alias(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    int pos;
    bool alias_used[MAX_ALIAS];

    if (ch->desc == NULL)
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC(rch))
	return;

    smash_tilde( argument );
    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {

	if (rch->pcdata->alias[0] == NULL)
	{
	    send_to_char("You have no aliases defined.\n\r",ch);
	    return;
	}
	send_to_char("Your current aliases are:\n\r",ch);

	for (pos = 0; pos < MAX_ALIAS; pos++)
	{
	    if (rch->pcdata->alias[pos] == NULL
	    ||	rch->pcdata->alias_sub[pos] == NULL)
		break;

	    sprintf(buf,"    %s  %s\n\r",rch->pcdata->alias[pos],
		    rch->pcdata->alias_sub[pos]);
	    send_to_char(buf,ch);
	}
	return;
    }

    if (!str_prefix("una",arg) || !str_cmp("alias",arg))
    {
	send_to_char("Sorry, that word is reserved.\n\r",ch);
	return;
    }

    if (argument[0] == '\0')
    {
	for (pos = 0; pos < MAX_ALIAS; pos++)
	{
	    if (rch->pcdata->alias[pos] == NULL
	    ||	rch->pcdata->alias_sub[pos] == NULL)
		break;

	    if (!str_cmp(arg,rch->pcdata->alias[pos]))
	    {
		sprintf(buf,"%s aliases to '%s'.\n\r",rch->pcdata->alias[pos],
			rch->pcdata->alias_sub[pos]);
		send_to_char(buf,ch);
		return;
	    }
	}

	send_to_char("That alias is not defined.\n\r",ch);
	return;
    }

    if (!str_prefix(argument,"delete") || !str_prefix(argument,"prefix"))
    {
	send_to_char("That shall not be done!\n\r",ch);
	return;
    }

    for ( pos = 0 ; pos < MAX_ALIAS ; pos++ )
	alias_used[ pos ] = FALSE;

    for (pos = 0; pos < MAX_ALIAS; pos++)
    {
	if (rch->pcdata->alias[pos] == NULL)
	    break;

	if (!str_cmp(arg,rch->pcdata->alias[pos])) /* redefine an alias */
	{
	    strcpy( buf, rch->pcdata->alias_sub[pos]  );
	    DISPOSE(rch->pcdata->alias_sub[pos]);
	    rch->pcdata->alias_sub[pos] = str_dup(argument);
	    if ( !check_alias( rch, argument, alias_used ) )
	    {
		send_to_char( "Invalid alias (possibly recursive)\n\r", rch );
		DISPOSE( rch->pcdata->alias_sub[pos] );
		rch->pcdata->alias_sub[pos] = str_dup( buf );
		return;
	    }
	    sprintf(buf,"%s is now realiased to '%s'.\n\r",arg,argument);
		    send_to_char(buf,ch);
	    return;
	}
     }

     if (pos >= MAX_ALIAS)
     {
	send_to_char("Sorry, you have reached the alias limit.\n\r",ch);
	return;
     }

     /* make a new alias */
     rch->pcdata->alias[pos]		= str_dup(arg);
     rch->pcdata->alias_sub[pos]	= str_dup(argument);

    if ( !check_alias( rch, argument, alias_used ) )
    {
	send_to_char( "Invalid alias (possibly recursive)\n\r", rch );
	DISPOSE( rch->pcdata->alias[pos] );
	DISPOSE( rch->pcdata->alias_sub[pos] );
	return;
    }
  
     sprintf(buf,"%s is now aliased to '%s'.\n\r",arg,argument);
     send_to_char(buf,ch);
}


void do_unalias(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH];
    int pos;
    bool found = FALSE;
 
    if (ch->desc == NULL)
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;
 
    if (IS_NPC(rch))
	return;
 
    argument = one_argument(argument,arg);

    if (arg == '\0')
    {
	send_to_char("Unalias what?\n\r",ch);
	return;
    }

    for (pos = 0; pos < MAX_ALIAS; pos++)
    {
	if (rch->pcdata->alias[pos] == NULL)
	    break;

	if (found)
	{
	    rch->pcdata->alias[pos-1]		= rch->pcdata->alias[pos];
	    rch->pcdata->alias_sub[pos-1]	= rch->pcdata->alias_sub[pos];
	    rch->pcdata->alias[pos]		= NULL;
	    rch->pcdata->alias_sub[pos]		= NULL;
	    continue;
	}

	if(!strcmp(arg,rch->pcdata->alias[pos]))
	{
	    DISPOSE(rch->pcdata->alias[pos]);
	    DISPOSE(rch->pcdata->alias_sub[pos]);
	    rch->pcdata->alias[pos] = NULL;
	    rch->pcdata->alias_sub[pos] = NULL;
	    send_to_char("Alias removed.\n\r",ch);
	    found = TRUE;
	}
    }

    if (!found)
	send_to_char("No alias of that name to remove.\n\r",ch);
}

void alias_sub( DESCRIPTOR_DATA *d, char *alias_sub, char *arg )
{
    char alias_args[10][MAX_INPUT_LENGTH];
    char *arguments = arg;
    char buf[MAX_STRING_LENGTH];
    char c;
    int i;

    bzero( buf, sizeof( buf ) );
    strcpy( alias_args[ 0 ], arg );
    for ( i = 1 ; i < 10 ; i++ )
	arguments = one_argument( arguments, alias_args[ i ] );

    i = 0;
    while( ( c = *alias_sub ) != '\0' )
    {
	alias_sub++;
	if ( c != '%' )
	    buf[ i++ ] = c;
	else
	{
	    switch( *alias_sub )
	    {
		default :
			buf[i++] = '%';
			buf[i++] = *alias_sub;
			break;
	  	case '%':
			buf[i++] = '%';
			break;
	  	case ';':
			buf[i++] = '\n';
			break;
	    	case '0':
			strcat( buf, alias_args[ 0 ] );
			i = strlen( buf );
			break;
		case '1':
			strcat( buf, alias_args[ 1 ] );
			i = strlen( buf );
			break;
		case '2':
			strcat( buf, alias_args[ 2 ] );
			i = strlen( buf );
			break;
		case '3':
			strcat( buf, alias_args[ 3 ] );
			i = strlen( buf );
			break;
		case '4':
			strcat( buf, alias_args[ 4 ] );
			i = strlen( buf );
			break;
		case '5':
			strcat( buf, alias_args[ 5 ] );
			i = strlen( buf );
			break;
		case '6':
			strcat( buf, alias_args[ 6 ] );
			i = strlen( buf );
			break;
		case '7':
			strcat( buf, alias_args[ 7 ] );
			i = strlen( buf );
			break;
		case '8':
			strcat( buf, alias_args[ 8 ] );
			i = strlen( buf );
			break;
		case '9':
			strcat( buf, alias_args[ 9 ] );
			i = strlen( buf );
			break;
	    }
	    alias_sub++;
	}
    }
    buf[ i ] = '\0';

    i = 0;
    while( ( c = buf[ i++ ] ) != '\n' && c != '\0' );

    if ( buf[ --i ] == '\n' )
    {
	strcat( buf, "\n" );
	strcat( buf, d->inbuf );
	strcpy( d->inbuf, &buf[ i ] );
    }	

    buf[ i ] = '\0';

    substitute_alias( d, buf );
}
