#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "time.h"
#include "finger.h"


void do_finger( CHAR_DATA *ch, char *argument )
{
        char   arg [ MAX_INPUT_LENGTH ]="";
	char   buf [ MAX_STRING_LENGTH ]="";
 	char   file[ 256 ]="";
 	char  *header="";
	long   last;
 	FILE  *fp;
 	struct tmpFingerData FingerData;

        argument = one_argument ( argument, arg ); 
 
        if ( arg[0] == '\0' )
 	{
          send_to_char( "Syntax: finger <player>\n\r", ch );
 	  return;
 	}

 	sprintf( file, "%s%s", PLAYER_DIR, capitalize( arg ) );

 	/* try to open the player file */
        if (!( fp = fopen( file, "r" ) ) )
 	{
          send_to_char( "No player by that name exists!\n\r", ch );
 	  return;
 	}
 
 	while ( !feof( fp ) && str_cmp( header, "End"))
       	{
        header = fread_word( fp );
 	  /* set up different ones for fields you want etc.. */
 	  if (!str_cmp( header, "Name" ) )
            FingerData.Name = fread_string( fp );
 
 	  else if (!str_cmp( header, "Race" ) )
 	    FingerData.Race = fread_string( fp );		

 	  else if (!str_cmp( header, "Cla" ) )
  	    FingerData.Class = fread_number( fp );		

	  else if (!str_cmp( header, "Last" ) )
 	    FingerData.Last = fread_number( fp );		

	  else if (!str_cmp( header, "Email" ) )
 	    FingerData.Email = fread_string( fp );		

          /*
           * If we hit an object section we stop looking because we know
           * that we have gotten through all the player stuff. 
           */

	  else if (!str_cmp( header, "#O" ) ) /* <-- Object */
	    fclose( fp ); 
 		
          else if (!str_cmp( header, "#Pet" ) )
	    fclose( fp );

    	  else
 		fread_to_eol( fp );

       }
 
	fclose( fp ); /* you got your info, now close the file */
	last = FingerData.Last;
 	/* show the stuff to the player 
	 * Format it however you wnat*/
 	sprintf( buf, "\n\rName :   %s\n\r"
		      "Race :   %s\n\r"
		      "Class:   %s\n\r",
                FingerData.Name, 
                FingerData.Race, 
	        class_table[FingerData.Class].name);
 	send_to_char( buf, ch );
 
  					  
/* You want it to show the time!
					   * not the number in the pfile hehe
					   * and we put it into a seperate 
					   * var */
        sprintf( buf, "Last On: %s\n\r", ((last!=0) ? ctime( &last ) : "Not since we installed this code." ));
  	send_to_char( buf, ch );
        if(IS_IMMORTAL(ch))
        {
           sprintf( buf, "Email-Id: %s\n\r", FingerData.Email);
           send_to_char(buf, ch);
        }
        
 	/* Your free! */
 	free_string( FingerData.Name  );
 	free_string( FingerData.Race  );
        free_string( FingerData.Email );
 	return;
}

