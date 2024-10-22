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
/***************************************************************************
 *  File: string.c                                                         *
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

#define unix 1
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
#include "merc.h"
#include "interp.h"

/* Decklarean */
extern const char colors[];
void string_number( CHAR_DATA *ch, char *pString )
{

  char buf2[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *posline;
  int count = 0;
/*   int cl; */

  buf[0] = '\0';

  if ( pString == NULL )
   {
     send_to_char(  "(blank)\n\r", ch );
     return;
   }

  for ( posline = pString; *posline != '\0'; )
  {
    char oneline[MAX_INPUT_LENGTH];
    int curr = 0;

    for ( ; *posline != '\0' && *posline != '\n'; posline++, curr++ )
    {
      oneline[curr] = *posline;
      /* show the color codes */
/*      if ( *posline == '&' )  Can't use until  is redone right. -Deck
      {
        if(*(posline+1) == '&' )
        {    
          oneline[++curr] = *(++posline);
        }
        else
        {
          for ( cl = 0; cl < 16; cl++ )
           if ( *(posline+1) == colors[cl] )
           {
             oneline[++curr] = *(++posline);
             oneline[++curr] = '&';
             oneline[++curr] = '&';
             oneline[++curr] = *posline;
             break;
           }
        }
      }*/
    }

    if ( *posline == '\n' )
    {
      oneline[curr] = *posline;
      posline++, curr++;
    }

    if ( *posline == '\r' )
    {
      oneline[curr] = *posline;
      posline++, curr++;
    }
    oneline[curr] = '\0';

    sprintf( buf2, "%2d] %s", count, oneline );
    strcat(buf, buf2);

    count++;
  }

  send_to_char( buf, ch);

  return;

}



/*****************************************************************************
 Name:		string_append
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit( CHAR_DATA *ch, char **pString )
{
    act( "$n begins editing some text.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "-========- Entering EDIT Mode -=========-\n\r", ch );
    send_to_char( "    Type .h on a new line for help\n\r", ch );
    send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
    send_to_char( "-=======================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }
    else
    {
        **pString = '\0';
    }

    ch->desc->pString = pString;

    return;
}



/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append( CHAR_DATA *ch, char **pString )
{
    act(  "$n begins editing some text.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "-=======- Entering APPEND Mode -========-\n\r", ch );
    send_to_char( "    Type .h on a new line for help\n\r", ch );
    send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
    send_to_char( "-=======================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }
    
    /*send_to_char( *pString, ch ); */
    string_number( ch, *pString );

    if ( *(*pString + strlen( *pString ) - 1) != '\r' )
    send_to_char( "\n\r", ch );

    ch->desc->pString = pString;

    return;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char * string_replace( char * orig, char * old, char * new )
{
    char xbuf[MAX_STRING_LENGTH];
    int i;

    xbuf[0] = '\0';
    strcpy( xbuf, orig );
    if ( strstr( orig, old ) != NULL )
    {
        i = strlen( orig ) - strlen( strstr( orig, old ) );
        xbuf[i] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[i+strlen( old )] );
    }

    free_string( orig );

    return str_dup( xbuf );
}


/* OLC 1.1b */
/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    /*
     * Thanks to James Seng
     */
    smash_tilde( argument );

    if ( *argument == '.' )
    {
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg1 );

	if ( !str_cmp( arg1, ".c" ) )
	{
	    send_to_char( "String cleared.\n\r", ch );
	    **ch->desc->pString = '\0';
	    return;
	}

	if ( !str_cmp( arg1, ".i" ) )
	{
	    *ch->desc->pString = string_insline( ch, argument,
			*ch->desc->pString );
	    return;
	}

	/* prevent conflict with string_insline */
	argument = first_arg( argument, arg2, FALSE );
	argument = first_arg( argument, arg3, FALSE );

	if ( !str_cmp( arg1, ".s" ) )
	{
	    send_to_char( "String so far:\n\r", ch );
/*            send_to_char( *ch->desc->pString, ch ); */
	    string_number( ch, *ch->desc->pString );
	    return;
	}

	if ( !str_cmp( arg1, ".r" ) )
	{
	    if ( arg2[0] == '\0' )
	    {
		send_to_char(
		    "usage:  .r \"old string\" \"new string\"\n\r", ch );
		return;
	    }

	    *ch->desc->pString =
		string_replace( *ch->desc->pString, arg2, arg3 );
	    sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
	    send_to_char( buf, ch );
	    return;
	}

	if ( !str_cmp( arg1, ".d" ) )
	{
	    *ch->desc->pString = string_delline( ch, arg2,
			*ch->desc->pString );
	    return;
	}

        if ( !str_cmp( arg1, ".f" ) )
        {
            *ch->desc->pString = format_string( *ch->desc->pString );
            send_to_char( "String formatted.\n\r", ch );
            return;
        }
        
        if ( !str_cmp( arg1, ".h" ) )
        {
	    send_to_char( "Sedit help (commands on blank line):   \n\r", ch );
            send_to_char( ".r 'old' 'new'   - replace a substring \n\r", ch );
            send_to_char( "                   (requires '', \"\") \n\r", ch );
            send_to_char( ".h               - get help (this info)\n\r", ch );
            send_to_char( ".s               - show string so far  \n\r", ch );
            send_to_char( ".f               - (word wrap) string  \n\r", ch );
	    send_to_char( ".c               - clear string so far \n\r", ch );
	    send_to_char( ".i # <string>    - insert string       \n\r", ch );
	    send_to_char( ".i #             - blank line          \n\r", ch );
            send_to_char( ".d #             - delete line number #\n\r", ch );
            send_to_char( "                   ( 0 - ? )           \n\r", ch );
            send_to_char( "@                - end string          \n\r", ch );
            return;
        }
            

        send_to_char( "SEdit:  Invalid dot command.\n\r", ch );
        return;
    }

    if ( *argument == '@' )
    {
	ch->desc->pString = NULL;
        return;
    }

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     * --------------------------------------
     */
    if ( strlen( *ch->desc->pString ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 100 ) )
    {
        send_to_char( "String too long, last line skipped.\n\r", ch );

	/* Force character out of editing mode. */
        ch->desc->pString = NULL;
        return;
    }

    strcpy( buf, *ch->desc->pString );
    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( *ch->desc->pString );
    *ch->desc->pString = str_dup( buf );
    return;
}



/*
 *  Thanks to Kalgen for the new procedure (no more bug!)
 *  Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string( char *oldstring /*, bool fSpace */)
{
  char xbuf[MAX_STRING_LENGTH];
  char xbuf2[MAX_STRING_LENGTH];
  char *rdesc;
  int i=0;
  int colcount=0;
  bool cap=TRUE;
  
  xbuf[0]=xbuf2[0]=0;
  
  i=0;
  
  if ( strlen(oldstring) >= (MAX_STRING_LENGTH - 100) )	/* OLC 1.1b */
  {
     bug( "String to format_string() longer than MAX_STRING_LENGTH.", 0 );
     return (oldstring);
  }

  for (rdesc = oldstring; *rdesc; rdesc++)
  {
    if (*rdesc=='\n')
    {
      if (xbuf[i-1] != ' ')
      {
	xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc=='\r') ;
    else if (*rdesc==' ')
    {
      if (xbuf[i-1] != ' ')
      {
        xbuf[i]=' ';
        i++;
      }
    }
    else if (*rdesc==')')
    {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!'))
      {
        xbuf[i-2]=*rdesc;
        xbuf[i-1]=' ';
	xbuf[i]=' ';
        i++;
      }
      else
      {
        xbuf[i]=*rdesc;
        i++;
      }
    }
    else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!') {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
        xbuf[i-2]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i-1]=' ';
          xbuf[i]=' ';
          i++;
        }
        else
	{
          xbuf[i-1]='\"';
          xbuf[i]=' ';
          xbuf[i+1]=' ';
          i+=2;
          rdesc++;
        }
      }
      else
      {
        xbuf[i]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i+1]=' ';
          xbuf[i+2]=' ';
          i += 3;
        }
        else
        {
          xbuf[i+1]='\"';
	  xbuf[i+2]=' ';
          xbuf[i+3]=' ';
          i += 4;
          rdesc++;
        }
      }
      cap = TRUE;
    }
    else
    {
      xbuf[i]=*rdesc;
      if ( cap )
        {
          cap = FALSE;
          xbuf[i] = UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i]=0;
  strcpy(xbuf2,xbuf);
  
  rdesc=xbuf2;
  
  xbuf[0]=0;
  
  for ( ; ; )
  {
    /*TSRCHANGE: added +colcount to numerous things below  */
    for (i=0,colcount=0; i<77+colcount; i++)
    {
      if (!*(rdesc+i)) break;

      /*TSRCHANGE*/
      /*If we find any {x things then dont count as chars...
       *Unless it is a {{ then we count it as one.
       *So we add 2 for each { and if the next char is also {
       *then we subtract one from our color count.
       *In the case of three {{{ in a row, we want to count 2
       *for the first, and -1 for the second and 2 for the 3rd, etc...
       */
      if (*(rdesc+i)=='{')
      {
        if (i>0 && *(rdesc+i-1)=='{')
          {
          if (i>1 && *(rdesc+i-2)=='{') colcount+=2;
          else colcount-=1;
          }
        else colcount+=2;
      }
      /*ENDCHANGE*/
    }
    if (i<77+colcount)
    {
      break;
    }
    for (i=(xbuf[0]?76+colcount:73+colcount) ; i ; i--)
    {
      if (*(rdesc+i)==' ') break;
    }
    if (i)
    {
      *(rdesc+i)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"\n\r");
      rdesc += i+1;
      while (*rdesc == ' ') rdesc++;
    }
    else
    {
      bug ("No spaces", 0);
      *(rdesc+75+colcount)=0;
      strcat(xbuf,rdesc);
      strcat(xbuf,"-\n\r");
      rdesc += 76+colcount;
    }
  }
  /*ENDCHANGE*/
  while (*(rdesc+i) && (*(rdesc+i)==' '||
                        *(rdesc+i)=='\n'||
                        *(rdesc+i)=='\r'))
    i--;
  *(rdesc+i+1)=0;
  strcat(xbuf,rdesc);
  if (xbuf[strlen(xbuf)-2] != '\n')
    strcat(xbuf,"\n\r");

  free_string(oldstring);
  return(str_dup(xbuf));
}



/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
      || *argument == '%'  || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
	}
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
    if ( fCase ) *arg_first = LOWER(*argument);
            else *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
	argument++;

    return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
	{
            s++;
        }
    }

    return argument;
}



/*
 * Returns an all-caps string.		OLC 1.1b
 */
char* all_capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
           int  i;
    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = UPPER( str[i] );
    strcap[i] = '\0';
    return strcap;
}

char *string_delline( CHAR_DATA *ch, char *argument, char *old )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *dline;
  int count = 0;
  int ln;

  argument = one_argument( argument, arg );
  if ( !is_number( arg ) )
  {
    send_to_char( "Delete which line?\n\r",ch);
    return old;
  }
  ln = atoi(arg);

  buf[0] = '\0';
  for ( dline = old; *dline != '\0'; )
  {
    char oneline[MAX_INPUT_LENGTH];
    int curr = 0;

    for ( ; *dline != '\0' && *dline != '\n'; dline++, curr++ )
    {
      oneline[curr] = *dline;
    }

    if ( *dline == '\n' )
    {
      oneline[curr] = *dline;
      dline++, curr++;
    }
    if ( *dline == '\r' )
    {
      oneline[curr] = *dline;
      dline++, curr++;
    }
    oneline[curr] = '\0';
    curr++;

    if ( count != ln )
      strcat(buf, oneline);

    count++;
  }

  if ( count <= ln )
  {
    send_to_char( "Line doesn't exist.\n\r",ch);
    return old;
  }
  send_to_char( "Line deleted.\n\r", ch );
  free_string(old);
  return str_dup(buf);
}

char *string_insline( CHAR_DATA *ch, char *argument, char *old )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *dline;
  int count = 0;
  int ln = 0;

  argument = one_argument( argument, arg );
  if ( is_number( arg ) )
    ln = atoi( arg );
   else
    {
      send_to_char(  "Syntax: .i # [newstring]\n\r", ch );
      return old;
    }
  buf[0] = '\0';
  for ( dline = old; *dline != '\0'; )
  {
    char oneline[MAX_INPUT_LENGTH];
    int curr = 0;
    
    for ( ; *dline != '\0' && *dline != '\n'; dline++,  curr++ )
    {
      oneline[curr] = *dline;
    }

    if ( *dline == '\n' )
    {
      oneline[curr] = *dline;
      dline++, curr++;
    }

    if ( *dline == '\r' )
    {
      oneline[curr] = *dline;
      dline++, curr++;
    }

    oneline[curr] = '\0';
    curr++;

    if ( ln == count )
    {
      if ( argument[0] != '\0' )
       strcat( buf, argument);
      strcat( buf, "\n\r\0" );

      count++;
    }

    strcat(buf, oneline);

    count++;
  }
 
  if ( count <= ln )
  {
    send_to_char(  "Line doesn't exist.\n\r", ch );
    return old;
  }
  send_to_char(  "Line inserted.\n\r", ch );
  free_string( old );
  return str_dup( buf ); 
}

