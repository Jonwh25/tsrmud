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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <signal.h>
#include <netdb.h>

#include "merc.h"
#include "recycle.h"

extern  char    *color_table[];

/* command procedures needed */
DECLARE_DO_FUN(do_help		);
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_skills	);
DECLARE_DO_FUN(do_outfit	);
DECLARE_DO_FUN(do_unread	);
DECLARE_DO_FUN(do_info_char     );
DECLARE_DO_FUN(do_autoexit      );
DECLARE_DO_FUN(do_autogold      );
DECLARE_DO_FUN(do_autoloot      );
DECLARE_DO_FUN(do_autosac       );

int  bind_connect( int s, struct sockaddr *local, struct sockaddr *remote, int length );

int AUTHORIZE_LOGINS = FALSE;

/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if     defined(macintosh) || defined(MSDOS)
const char echo_off_str[] = {'\0'};
const char echo_on_str[] = {'\0'};
const char go_ahead_str[] = {'\0'};
#endif

#if     defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
#include <arpa/telnet.h>
#if !defined( STDOUT_FILENO )
#define STDOUT_FILENO 1
#endif
const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };
 
/* rfc stuff
   for user authing via tap ident method
   added by madgnome */
 
extern char *strchr();
extern char *inet_ntoa();
 
#define RFC931_PORT     113
#define ANY_PORT        0
 
int     rfc931_timeout = 7;
static jmp_buf timebuf;

extern unsigned int alarm __P ((unsigned int __seconds)); 
extern unsigned int alarm (unsigned int __seconds);
char   *rfcauth(struct sockaddr_in *rmt_sin, struct sockaddr_in *our_sin);
 
#endif

/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen
) ); 
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );

int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void
*optval, int optlen ) ); 
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if	defined(linux)
 
/*    Linux shouldn't need these. If you have a problem compiling, try
    uncommenting accept and bind. 
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
*/

int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
/*int	read		args( ( int fd, char *buf, int nbyte ) ); */
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
/*int	write		args( ( int fd, char *buf, int nbyte ) ); */
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int setsockopt		args( ( int s, int level, int optname,
			    const char *optval, int optlen ) );
#else
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif



/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
bool		    newlock;		/* Game is newlocked		*/
bool                zlock;              /* Game is buildlocked          */
char		    str_boot_time[MAX_INPUT_LENGTH];
time_t		    current_time;	/* time of this pulse           */	
bool		    MOBtrigger = TRUE;  /* act() switch                 */

/* NEW FEATURES */
double  diff_time;
extern  int     shutdown_time;
extern  int     shutdown_notify_time;
extern  char    shutdown_text[256];
extern  char    shutdown_mud;
extern  char    reboot_mud;
extern  time_t  shutdown_start_time;
extern  CHAR_DATA *shutdown_char;



/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	init_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif




/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );
void    do_shutdown             args( (CHAR_DATA *ch, char *argument ));
void    do_reboot               args( (CHAR_DATA *ch, char *argument ));


int main( int argc, char **argv )
{
    struct timeval now_time;
    int port;

#if defined(unix)
    int control;
#endif

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time 	= (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 4000;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }

    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db( );
    log_string( "TSR is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined(unix)
    control = init_socket( port );
    boot_db( );
    sprintf( log_buf, "TSR is ready to rock on port %d.", port );
    log_string( log_buf );
    game_loop_unix( control );
    close (control);
#endif

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close(fd);
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close(fd);
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror("Init socket: bind" );
	close(fd);
	exit(1);
    }


    if ( listen( fd, 3 ) < 0 )
    {
	perror("Init socket: listen");
	close(fd);
	exit(1);
    }

    return fd;
}
#endif



#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.connected	= CON_DETECT_ANSI;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 2000;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.next		= descriptor_list;
    dcon.showstr_head	= NULL;
    dcon.showstr_point	= NULL;
    dcon.pEdit          = NULL;          /* OLC */
    dcon.pString        = NULL;          /* OLC */
    dcon.editor         = 0;             /* OLC */
    descriptor_list	= &dcon;

    /*
     * Send the greeting.
     */
    {
	extern char *help_greeting;
	if (help_greeting[0] == '.')
	    write_to_buffer(&dcon, help_greeting + 1, 0);
	else
	    write_to_buffer(&dcon, help_greeting, 0);
    }

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if (d->character != NULL && d->character->daze > 0)
		--d->character->daze;

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		if ( d->connected == CON_PLAYING )
		    substitute_alias( d, d->incomm );
		else
		    nanny( d, d->incomm );

		d->incomm[0]	= '\0';
	    }
		
		/* OLC */
		if (d->showstr_point)
		    show_string(d, d->incomm);
		else if (d->pString)
		    string_add(d->character, d->incomm);
		else
		    switch (d->connected)
		    {
			case CON_PLAYING:
			    if (!run_olc_editor(d))
				interpret(d->character, d->incomm);
			    break;
			default:
			    nanny(d, d->incomm);
			    break;
		    }

		d->incomm[0] = '\0';
	    }
	}


	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Busy wait (blargh).
	 */
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( dcon.character != NULL )
		    dcon.character->timer = 0;
		if ( !read_from_descriptor( &dcon ) )
		{
		    if ( dcon.character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    dcon.outtop	= 0;
		    close_socket( &dcon );
		}
#if defined(MSDOS)
		break;
#endif
	    }

	    gettimeofday( &now_time, NULL );
	    delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
		  + ( now_time.tv_usec - last_time.tv_usec );
	    if ( delta >= 1000000 / PULSE_PER_SECOND )
		break;
	}
	last_time    = now_time;
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif

#define CNUM(x) ch->pcdata->x
/*
 * Write to one char.
 */
void process_color(CHAR_DATA *ch, char a) 
{
    int c = 0;

    if (ch->desc && ch->desc->color && ch->pcdata!=NULL ) 
    {
	switch(a) 
        {
		case '`': /* off color */
			c=0xf;
			break;
		case 'C': /* Charmed mobs */
		        c=CNUM(color_charmed); break;
		case 'F': /* Devotion channel */
			c=CNUM(color_dev); break;
		case 'P': /* Prompt */
		        c=CNUM(color_prompt); break;
		case 'h': /* hidden mobs */
		        c=CNUM(color_hidden); break;
		case 'i': /* invis */
		        c=CNUM(color_invis); break;
		case 'O': /* Ooc channel */
		        c=CNUM(color_ooc); break;
		case 'R': /* reply channel */
		        c=CNUM(color_reply); break;
		case 'S': /* Say channel */
		        c=CNUM(color_say); break;
		case 'T': /* tell channel */
		        c=CNUM(color_tell); break;
		case 'W': /* Wizi */
		        c=CNUM(color_wizi); break;
		case 'X': /* shout channel */
		        c=CNUM(color_shout); break;
		case 'Y': /* Auction channel */
		        c=CNUM(color_auction); break;
		case 'y': /* yell channel */
		        c=CNUM(color_yell); break;
	 	case 'G': /* gossip channel */
			c=CNUM(color_gossip); break;
		case 'g': /* grats channel */
			c=CNUM(color_grats); break;
		case 'I': /* info channel */
			c=CNUM(color_info); break;
		case 'M': /* music channel */
			c=CNUM(color_music); break;
		case 'Q': /* quote channel */
			c=CNUM(color_quote); break;
		case 'A': /* immortal channel */
			c=CNUM(color_immortal); break;
		case 'B': /* builder channel */
			c=CNUM(color_build); break;
		case 'n': /* newbie channel */
			c=CNUM(color_newbie); break;
		case 'Z': /* Admin channel */
			c=CNUM(color_admin); break;
		case '0': c = 0; break;
		case '1': c = 1; break;
		case '2': c = 2; break;
		case '3': c = 3; break;
		case '4': c = 4; break;
		case '5': c = 5; break;
		case '6': c = 6; break;
		case '7': c = 7; break;
		case '8': c = 8; break;
		case '9': c = 9; break;
		case '!': c = 10; break;
		case '@': c = 11; break;
		case '#': c = 12; break;
		case '$': c = 13; break;		
		case '%': c = 14; break;
		case '^': c = 15; break;		
		default: /* print unknown */
     		      write_to_buffer(ch->desc,&a,1);		
		      return;
	}
	write_to_buffer(ch->desc,color_table[c], strlen(color_table[c]));
   }
}
#undef CNUM(x)

#if defined(unix)
void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;
	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    init_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character && d->connected == CON_PLAYING)
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if (d->character != NULL && d->character->daze > 0)
		--d->character->daze;

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		/* OLC */
		if (d->showstr_point)
		    show_string(d, d->incomm);
		else if (d->pString)
		    string_add(d->character, d->incomm);
		else
		    switch (d->connected)
		    {
			case CON_PLAYING:
			    if (!run_olc_editor(d))
				substitute_alias(d, d->incomm);
			    break;
			default:
			    nanny(d, d->incomm);
			    break;
		    }

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif

#if defined(unix)
void init_descriptor( int control )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct sockaddr_in oursock;              /* IDENT */
    struct hostent *from;
    int desc;
    int size;
    int len;                                 /* IDENT */

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }
    /* IDENT */
    len = sizeof(oursock);
    getsockname(desc, (struct sockaddr *) &oursock, &len);

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    dnew = new_descriptor();

    dnew->descriptor	= desc;
    dnew->connected	= CON_DETECT_ANSI;
    dnew->showstr_head	= NULL;
    dnew->showstr_point = NULL;
    dnew->outsize	= 2000;
    dnew->pEdit         = NULL;        /* OLC */
    dnew->pString       = NULL;        /* OLC */
    dnew->editor        = 0;           /* OLC */
    dnew->outbuf	= alloc_mem( dnew->outsize );

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
        dnew->rfc = str_dup( "(unknown)" );    /* IDENT */
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;
 	char *rfc;    /* IDENT */

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
 
         rfc=rfcauth(&sock,&oursock);
         dnew->rfc=str_dup(rfc);
 
	log_string( log_buf );
	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
	dnew->host = str_dup( from ? from->h_name : buf );
    }

    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if ( check_ban(dnew->host,BAN_ALL))
    {
	write_to_descriptor( desc,
	    "Your site has been banned from this mud.\n\r", 0 );
	close( desc );
	free_descriptor(dnew);
	return;
    }

    /* SHUTDOWN AND REBOOT INFORMATION FOR NEW CONNECTIONS */
 
    if (shutdown_mud)
    {
	diff_time=difftime(current_time,shutdown_start_time);
	   shutdown_notify_time += 30;
sprintf(buf, "
The Shadow Realms is currently shutting down in %d minute(s) %d seconds.
You may enter our realm if you wish, but be warned that a shutdown is
currently active.\n\r", 
	      (int)(shutdown_time-diff_time)/60,
	      (int)(shutdown_time-diff_time) % 60);
	   write_to_descriptor( desc, buf, 0 );
    }

    if (reboot_mud)
    {
	diff_time=difftime(current_time,shutdown_start_time);
	   shutdown_notify_time += 30;
sprintf(buf, "
The Shadow Realms is currently rebooting in %d minute(s) %d seconds.
You may enter our realm if you wish, but be warned that a reboot is 
currently active.\n\r", 
	      (int)(shutdown_time-diff_time)/60,
	      (int)(shutdown_time-diff_time) % 60);
	   write_to_descriptor( desc, buf, 0 );
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Send the greeting.
     */
     { 
     write_to_buffer(dnew,"Do you want Ansi color? (Y,N) ",0);
     dnew->connected = CON_DETECT_ANSI;
     }
    return;
}
#endif



void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf );
	/* cut down on wiznet spam when rebooting */
	if ( dclose->connected == CON_PLAYING && !merc_down)
	{
	  act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
	  wiznet("[`4WIZNET``] `8Dropped Link:`` `@$N``",ch,NULL,WIZ_LINKS,0,0);
	  ch->desc = NULL;
	}
	else
	{
	    free_char(dclose->original ? dclose->original : 
		dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    close( dclose->descriptor );
    free_descriptor(dclose);
#if defined(MSDOS) || defined(macintosh)
    exit(1);
#endif
    return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT BEFORE YOUR DELETED!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ; ; )
    {
	int c;
	c = getc( stdin );
	if ( c == '\0' || c == EOF )
	    break;
	putc( c, stdout );
	if ( c == '\r' )
	    putc( '\n', stdout );
	d->inbuf[iStart++] = c;
	if ( iStart > sizeof(d->inbuf) - 10 )
	    break;
    }
#endif

#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read." );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */

    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if (++d->repeat >= 25 && d->character
	    &&  d->connected == CON_PLAYING)
	    {
 		sprintf( log_buf, "%s@%s input spamming!", d->rfc, d->host);
		log_string( log_buf );
	        if(d->character==NULL)
		{
		  sprintf(log_buf,"Some freak is trying to bad-name-spam-crash the mud from %s",d->host);
		  wiznet(log_buf,NULL,NULL,WIZ_SPAM,0,0);
                }
		else
		wiznet("[`4WIZNET``] `!$N`` `@is SPAMMING the MUD!``",
		    d->character, NULL, WIZ_SPAM, 0, get_trust(d->character));
		if (d->incomm[0] == '!')
		    wiznet(d->inlast,d->character,NULL,WIZ_SPAM,0,
			get_trust(d->character));
		else
		    wiznet(d->incomm,d->character,NULL,WIZ_SPAM,0,
			get_trust(d->character));

		d->repeat = 0;
	    }
	}
    }


    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}

/*
 * Low level output function.
 */
bool process_output(DESCRIPTOR_DATA * d, bool fPrompt)
{
    extern bool merc_down;

    /*
       Bust a prompt.
    */

    if (!merc_down)
	if (d->showstr_point)
	    write_to_buffer(d,
        "[Press return to continue]", 0);
	else if (fPrompt && d->pString && d->connected == CON_PLAYING)
	    write_to_buffer(d, "> ", 2);
	else if (fPrompt && d->connected == CON_PLAYING)
	{
	    CHAR_DATA *ch;


	    CHAR_DATA *victim;

	    ch = d->character;

	    /* battle prompt */
	    if ((victim = ch->fighting) != NULL && can_see(ch, victim))
	    {
		int percent;
		char wound[100];
		char buf[MAX_STRING_LENGTH];

		if (victim->max_hit > 0)
		    percent = victim->hit * 100 / victim->max_hit;
		else
		    percent = -1;

		if (percent >= 100)
		    sprintf(wound, "is in excellent condition.");
		else if (percent >= 90)
		    sprintf(wound, "has a few scratches.");
		else if (percent >= 75)
		    sprintf(wound, "has some small wounds and bruises.");
		else if (percent >= 50)
		    sprintf(wound, "has quite a few wounds.");
		else if (percent >= 30)
		    sprintf(wound, "has some big nasty wounds and scratches.");
		else if (percent >= 15)
		    sprintf(wound, "looks pretty hurt.");
		else if (percent >= 0)
		    sprintf(wound, "is in awful condition.");
		else
		    sprintf(wound, "is bleeding to death.");

		if ( IS_AFFECTED(ch, AFF_BLIND) ) 
		{
		   sprintf(buf,"Someone %s \n\r", wound);
	    	   buf[0] = UPPER(buf[0]);
	    	   send_to_char_ansi(ch, buf, strlen(buf));
		}
		else
		{
		   sprintf(buf,"%s %s \n\r", 
		   IS_NPC(victim) ? victim->short_descr : victim->name,wound);
	    	   buf[0] = UPPER(buf[0]);
	    	   send_to_char_ansi(ch, buf, strlen(buf));
		}
	 }


	    ch = d->original ? d->original : d->character;
	    if (!IS_SET(ch->comm, COMM_COMPACT))
		write_to_buffer(d, "\n\r", 2);


  	    send_to_char( "`P", d->character );
	    if (IS_SET(ch->comm, COMM_PROMPT))
	    bust_a_prompt(d->character);
 	    send_to_char( "``", d->character );

	    if (IS_SET(ch->comm, COMM_TELNET_GA))
		write_to_buffer(d, go_ahead_str, 0);
	}

    /*
       Short-circuit if nothing to write.
    */
    if (d->outtop == 0)
	return TRUE;

    /*
       Snoop-o-rama.
    */
    if (d->snoop_by != NULL)
    {
	if (d->character != NULL)
	    write_to_buffer(d->snoop_by, d->character->name, 0);
	write_to_buffer(d->snoop_by, "> ", 2);
	write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
    }

    /*
       OS-dependent output.
    */
    if (!write_to_descriptor(d->descriptor, d->outbuf, d->outtop))
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char immprompt[MAX_STRING_LENGTH];
    char olcprompt[MAX_STRING_LENGTH] ;
    const char *str;
    const char *i;
    char *point;
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    const char *dir_name[] = {"N", "E", "S", "W", "U", "D"};
    int door;

    point = buf;
    str = ch->prompt;
    immprompt[0] = '\0';

if (IS_BUILD(ch))
    {    

    switch (ch->desc->editor)
    {

    case ED_AREA:
    strcpy( olcprompt, " <AEDIT> ") ;
    break ;

    case ED_ROOM:
    strcpy (olcprompt, " <REDIT> ") ;
    break ;

    case ED_OBJECT:
    strcpy (olcprompt, " <OEDIT> ") ;
    break ;

    case ED_MOBILE:
    strcpy (olcprompt, " <MEDIT> ") ;
    break ;

    case ED_HELP:
    strcpy (olcprompt, " <HEDIT> ") ;
    break ;

    case ED_MPCODE:
    strcpy (olcprompt, " <MPCODE> ") ;
    break ;

    default:
    olcprompt[0] = '\0' ;
    break ;

    }
    

	if (ch->invis_level > 0)
	    strcat(immprompt, " {WIZ} ");

	if (ch->incog_level > 0)
	    strcat(immprompt, " {INCOG} ");

if (olcprompt[0] != '\0')
strcat (immprompt, olcprompt) ;

    }


    if (str == NULL || str[0] == '\0')
    {
	sprintf(buf, "<%dhp %dm %dmv> %s",
		ch->hit, ch->mana, ch->move, ch->prefix);
        
	if (immprompt[0] != '\0')
	    send_to_char(buf, ch);
	else
	{
	    strcat(immprompt, buf);
	    send_to_char(immprompt, ch);
	}
	return;
    }

    if (IS_SET(ch->comm, COMM_AFK))
    {
	if (immprompt[0] != '\0')
	    send_to_char(strcat(immprompt, " <AFK> "), ch);
	else
	    send_to_char("<AFK> ", ch);

	return;
    }

    while (*str != '\0')
    {
	if (*str != '%')
	{
	    *point++ = *str++;
	    continue;
	}
	++str;
	switch (*str)
	{
	    default:
		i = " ";
		break;
	    case 'e':
		found = FALSE;
		doors[0] = '\0';
		for (door = 0; door < 6; door++)
		{
		    if ((pexit = ch->in_room->exit[door]) != NULL
			&& pexit->u1.to_room != NULL
			&& (can_see_room(ch, pexit->u1.to_room)
			    || (IS_AFFECTED(ch, AFF_INFRARED)
				&& !IS_AFFECTED(ch, AFF_BLIND)))
			&& !IS_SET(pexit->exit_info, EX_CLOSED))
		    {
			found = TRUE;
			strcat(doors, dir_name[door]);
		    }
		}
		if (!found)
		    strcat(buf, "none");
		sprintf(buf2, "%s", doors);
		i = buf2;
		break;
	    case 'c':
		sprintf(buf2, "%s", "\n\r");
		i = buf2;
		break;
	    case 'h':
		sprintf(buf2, "%d", ch->hit);
		i = buf2;
		break;
	    case 'H':
		sprintf(buf2, "%d", ch->max_hit);
		i = buf2;
		break;
	    case 'm':
		sprintf(buf2, "%d", ch->mana);
		i = buf2;
		break;
	    case 'M':
		sprintf(buf2, "%d", ch->max_mana);
		i = buf2;
		break;
	    case 'v':
		sprintf(buf2, "%d", ch->move);
		i = buf2;
		break;
	    case 'V':
		sprintf(buf2, "%d", ch->max_move);
		i = buf2;
		break;
	    case 'S':
		sprintf(buf2, "%d", ch->ship_fuel);
		i = buf2;
		break;
	    case 'W':
		sprintf(buf2, "%d", ch->wagon_fuel);
		i = buf2;
		break;
	    case 'x':
		sprintf(buf2, "%d", ch->exp);
		i = buf2;
		break;
	    case 'X':
		sprintf(buf2, "%d",IS_NPC(ch) ? 0:
			(ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp);
		i = buf2;
		break;
	    case 'g':
		sprintf(buf2, "%ld", ch->gold);
		i = buf2;
		break;
	    case 's':
		sprintf(buf2, "%ld", ch->silver);
		i = buf2;
		break;
	    case 'a':
		if (ch->level > 9)
		    sprintf(buf2, "%d", ch->alignment);
		else
		    sprintf(buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
			    "evil" : "neutral");
		i = buf2;
		break;
	    case 'r':
		if (ch->in_room != NULL)
		    sprintf(buf2, "%s",
			    ((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) ||
			     (!IS_AFFECTED(ch, AFF_BLIND) && !room_is_dark(ch->in_room)))
			    ? ch->in_room->name : "darkness");
		else
		    sprintf(buf2, " ");
		i = buf2;
		break;
	    case 'R':
		if (IS_BUILD(ch) && ch->in_room != NULL)
		    sprintf(buf2, "%d", ch->in_room->vnum);
		else
		    sprintf(buf2, " ");
		i = buf2;
		break;
	    case 'z':
		if (IS_BUILD(ch) && ch->in_room != NULL)
		    sprintf(buf2, "%s", ch->in_room->area->name);
		else
		    sprintf(buf2, " ");
		i = buf2;
		break;
	    case '%':
		sprintf(buf2, "%%");
		i = buf2;
		break;
	}
	++str;
	while ((*point = *i) != '\0')
	    ++point, ++i;
    }

    if ((IS_BUILD(ch))
	|| (immprompt[0] != '\0'))
	write_to_buffer(ch->desc, immprompt, 0);

    write_to_buffer(ch->desc, buf, point - buf);

    if (ch->prefix[0] != '\0')
	write_to_buffer(ch->desc, ch->prefix, 0);
    return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

        if (d->outsize >= 32000)
	{
	    bug("Buffer overflow. Closing.\n\r",0);
	    close_socket(d);
	    return;
 	}
	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strcpy( d->outbuf + d->outtop, txt );
    d->outtop += length;
    return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
	desc = 1;
#endif

    if ( length <= 0 )
	length = strlen(txt);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    int iClass,race,i,weapon;
    bool fOld;
    extern char * help_greeting;

    while ( isspace(*argument) )
	argument++;

    ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_DETECT_ANSI:
	    switch (*argument)
	    {
		case 'y':
		case 'Y':
        	          d->color=1;    
        	          d->connected	= CON_GET_NAME;
	      
	      if ( help_greeting[0] == '.' )
	         write_to_buffer( d, help_greeting+1, 0 );
     	      else	    
                 write_to_buffer( d, help_greeting, 0 );
		    break;

		case 'n':
		case 'N':
       	 	          d->color=0;
        	          d->connected	= CON_GET_NAME;
	      
	      if ( help_greeting[0] == '.' )
	         write_to_buffer( d, help_greeting+1, 0 );
     	      else	    
                 write_to_buffer( d, help_greeting, 0 );
		    break;

		default:
		    write_to_buffer(d, "Please type Yes or No? ", 0);
		    break;
	    
	    }
             return;

    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

	argument[0] = UPPER(argument[0]);
	if ( !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
	    return;
	}

	fOld = load_char_obj( d, argument );
	ch   = d->character;

	if (IS_SET(ch->act, PLR_DENY))
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	if (check_ban(d->host,BAN_PERMIT) && !IS_SET(ch->act,PLR_PERMIT))
	{
	    write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    close_socket(d);
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    if ( wizlock && !IS_IMMORTAL(ch)) 
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
            sprintf(buf,
	    "[`4WIZNET``] `!Old Player:`` `@%s``@`8%s``",ch->name,d->host);
            wiznet(buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));

            sprintf( log_buf, 
	    "Player attempting login: %s (%s)@%s.", ch->name, d->rfc, d->host );
	    log_string( log_buf );

	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */
 	    if (newlock)
	    {
                write_to_buffer( d, "The game is newlocked.\n\r", 0 );
                close_socket( d );
                return;
            }

	    if (check_ban(d->host,BAN_NEWBIES))
	    {
		write_to_buffer(d,
		    "New players are not allowed from your site.\n\r",0);
		close_socket(d);
		return;
	    }
	        
            write_to_buffer(d,
"    
    __^__                                                            __^__
   ( ___ )----------------------------------------------------------( ___ )
    | / |                                                            | \\ |
    | / |                   The Shadow Realms MUD                    | \\ |
    | / |------------------------------------------------------------| \\ |
    | / |   TSR is a mud based on Role-Playing and is set in the     | \\ |
    | / |   Medievial/Fantasy setting. Your name is one of the most  | \\ |
    | / |   important processes when making your character here.     | \\ |
    | / |   So please choose a name that will reflect role-playing   | \\ |
    | / |   and the Medievial/Fantasy theme of this MUD. We are      | \\ |
    | / |   trying to create a unique mythos. We prefer that you     | \\ |
    | / |   do NOT use names such as Elminster or Sparhawk or any    | \\ |
    | / |   other name found in a Fantasy book, myth, or legend.     | \\ |
    | / |   Please be creative and make up a good name. If your      | \\ |
    | / |   name does not reflect this you will not be admitted      | \\ |
    | / |   into the mud and/or you will be asked to change it.      | \\ |
    | / |   TRANSGRESSORS WILL BE DENIED WITHOUT WARNING. Use        | \\ |
    | / |   your head.                                               | \\ |
    |___|                                                            |___|
   (_____)----------------------------------------------------------(_____)
                \n\r", 0);
	
	    sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ))
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    
	    /* LOG those litle shits who try to hack. */
	    sprintf( log_buf, 
	    "%s (%s)@%s just fucked his password up.",
	    ch->name,d->rfc, d->host );
            log_string( log_buf );

            /* Wiznet for password added with Color */ 
	    sprintf( buf,
	    "[`4WIZNET``] `!Wrong Password:`` `@%s`` (%s)@`8%s``", 
	    ch->name,d->rfc, d->host );
	    wiznet(buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));
	    close_socket( d );
	    return;
	}
 
	write_to_buffer( d, echo_on_str, 0 );

	if (check_playing(d,ch->name))
	    return;

	if ( check_reconnect( d, ch->name, TRUE ) )
	    return;
 	
	sprintf( log_buf, "%s (%s)@%s has logged in.\r\n",
		 ch->name, d->rfc, d->host );
        log_string(log_buf);
        sprintf(buf, 
	    "[`4WIZNET``] `!%s`` (%s)@%s has logged in.\r\n", 
	    ch->name,d->rfc, d->host);
        wiznet(buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));
        
        if (ch->level == 152)
        {
   	    do_help(ch, "bmotd");
	    d->connected = CON_READ_BMOTD;
	}
	if ( IS_IMMORTAL(ch) )
	{
	    do_help( ch, "imotd" );
            d->connected = CON_READ_IMOTD;
 	}
	else
	{
	    do_help( ch, "motd" );
	    d->connected = CON_READ_MOTD;
	}
	break;

/* RT code for breaking link */
 
    case CON_BREAK_CONNECT:
	switch( *argument )
	{
	case 'y' : case 'Y':
            for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
	    {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
		    continue;

		if (str_cmp(ch->name,d_old->original ?
		    d_old->original->name : d_old->character->name))
		    continue;

		close_socket(d_old);
	    }
	    if (check_reconnect(d,ch->name,TRUE))
	    	return;
	    write_to_buffer(d,"Reconnect attempt failed.\n\rName: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	case 'n' : case 'N':
	    write_to_buffer(d,"Name: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d,"Please type Y or N? ",0);
	    break;
	}
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}
	/* New Wiznet for newbies */
        sprintf( log_buf, 
	"New Player connecting: %s (%s)@%s", ch->name,d->rfc, d->host );
	log_string( log_buf );
        sprintf( buf, 
	"[`4WIZNET``] `0Newbie Alert:`` `!%s`` (%s)@`@%s``",
	ch->name,d->rfc, d->host );
	wiznet(buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));
     	sprintf(buf,"Please enter your email address or none (Ex. user@%s).\n\rEmail: ",d->host);
     	write_to_buffer(d,buf,0);
	d->connected = CON_GET_EMAIL;
 	break;
    
    case CON_GET_EMAIL:
    if (argument[0]=='\0' || !(!str_cmp(argument,"none") || strstr(argument,"@"))) 
    {
	  write_to_buffer(d, "Invalid! try again.\n\r", 0);
          if (strlen(d->host)<200) 
	  {
     	     sprintf(buf,"Please enter your email address or none (user@%s).\n\rEmail: ",d->host);
     	     write_to_buffer(d,buf,0);
          }
    }
    free_string(d->character->pcdata->email);
    d->character->pcdata->email=str_dup(argument);
	    
	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer(d,"The following races are available:\n\r  ",0);
	for ( race = 1; race_table[race].name != NULL; race++ )
	{
	    if (!race_table[race].pc_race)
		break;
	    if ( !race_table[race].remort_race ||
	       (race_table[race].remort_race && IS_SET(ch->act,PLR_REMORT)))
	    {
		write_to_buffer(d,race_table[race].name,0);
		write_to_buffer(d," ",1);
	    }
	}
	write_to_buffer(d,"\n\r",0);
	write_to_buffer(d,"What is your race (help for more information)? ",0);
	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	one_argument(argument,arg);

	if (!strcmp(arg,"help"))
	{
	    argument = one_argument(argument,arg);
	    if (argument[0] == '\0')
		do_help(ch,"race help");
	    else
		do_help(ch,argument);
            write_to_buffer(d,
		"What is your race (help for more information)? ",0);
	    break;
  	}

	race = race_lookup(argument);

	if (race == 0 || !race_table[race].pc_race ||
	   ( race_table[race].remort_race && !IS_SET( ch->act, PLR_REMORT ) ) )
	{
	    write_to_buffer(d,"That is not a valid race.\n\r",0);
            write_to_buffer(d,"The following races are available:\n\r  ",0);
            for ( race = 1; race_table[race].name != NULL; race++ )
            {
            	if (!race_table[race].pc_race)
                    break;
		if ( !race_table[race].remort_race ||
		   (race_table[race].remort_race && IS_SET(ch->act,PLR_REMORT)))
		{
		    write_to_buffer(d,race_table[race].name,0);
		    write_to_buffer(d," ",1);
		}
            }
            write_to_buffer(d,"\n\r",0);
            write_to_buffer(d,
		"What is your race? (help for more information) ",0);
	    break;
	}

        ch->race = race;
	/* initialize stats */
	for (i = 0; i < MAX_STATS; i++)
	ch->perm_stat[i]         = pc_race_table[race].stats[i];
	ch->affected_by          = ch->affected_by|race_table[race].aff;
	ch->imm_flags	         = ch->imm_flags|race_table[race].imm;
	ch->res_flags	         = ch->res_flags|race_table[race].res;
	ch->vuln_flags	         = ch->vuln_flags|race_table[race].vuln;
	ch->form	         = race_table[race].form;
	ch->parts	         = race_table[race].parts;
        ch->act2                 = race_table[ch->race].act2; 
        /*ch->pcdata->plr_wager    = 0; * arena betting amount 
        ch->pcdata->awins        = 0; * arena wins           
        ch->pcdata->alosses      = 0; * arena losses        
        ch->gladiator            = NULL;  set player to bet on to NULL */

	/* add skills */
	for (i = 0; i < 5; i++)
	{
	    if (pc_race_table[race].skills[i] == NULL)
	 	break;
	    group_add(ch,pc_race_table[race].skills[i],FALSE);
	}
	/* add cost */
	ch->pcdata->points = pc_race_table[race].points;
	ch->size = pc_race_table[race].size;

        write_to_buffer( d, "What is your sex (M/F)? ", 0 );
        d->connected = CON_GET_NEW_SEX;
        break;
        

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    
			    ch->pcdata->true_sex = SEX_MALE;
			    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE; 
			    ch->pcdata->true_sex = SEX_FEMALE;
			    break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}

	strcpy( buf, "=-=-=-=-=-=-=-=Please Select a class=-=-=-=-=-=-=\n\r" );
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if (!class_table[iClass].remort_class ||
	       (class_table[iClass].remort_class && IS_SET(ch->act,PLR_REMORT)))
	    {
		if ( iClass > 0 )
		    strcat( buf, " " );
		strcat( buf, class_table[iClass].name );
	    }
	}
	strcat( buf, "\n\rEnter your choice here -> " );

        write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_NEW_CLASS;
	break;

    case CON_GET_NEW_CLASS:
	iClass = class_lookup(argument);

	if ( iClass == -1 ||
	   ( class_table[iClass].remort_class && !IS_SET(ch->act,PLR_REMORT)))
	{
	    write_to_buffer( d,
		"That's not a class.\n\rWhat IS your class? ", 0 );
	    return;
	}

	if ( iClass == class_lookup ("builder"))
	{
	    write_to_buffer( d,
	    "I'm sorry that class is reserved.\n\rWhat IS your class? ", 0 );
	    return;
	}

        ch->class = iClass;

	sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	log_string( log_buf );
	wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
        wiznet(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));

	write_to_buffer( d, "\n\r", 2 );
	write_to_buffer( d, "You may be good, neutral, or evil.\n\r",0);
	write_to_buffer( d, "Which alignment (G/N/E)? ",0);
	d->connected = CON_GET_ALIGNMENT;
	break;

case CON_GET_ALIGNMENT:
	switch( argument[0])
	{
	    case 'g' : case 'G' : ch->alignment = 750;  break;
	    case 'n' : case 'N' : ch->alignment = 0;	break;
	    case 'e' : case 'E' : ch->alignment = -750; break;
	    default:
		write_to_buffer(d,"That's not a valid alignment.\n\r",0);
		write_to_buffer(d,"Which alignment (G/N/E)? ",0);
		return;
	}

	write_to_buffer(d,"\n\r",0);

        group_add(ch,"rom basics",FALSE);
        group_add(ch,class_table[ch->class].base_group,FALSE);
        ch->pcdata->learned[gsn_recall] = 50;
        ch->pcdata->language[0] = 85; 
        ch->pcdata->last = 0; 
        
	/* Added when Customize was taken out */
	group_add(ch,class_table[ch->class].default_group,TRUE);
        write_to_buffer( d, "\n\r", 2 );
	write_to_buffer(d,
		"Please pick a weapon from the following choices:\n\r",0);
        buf[0] = '\0';
        for ( i = 0; weapon_table[i].name != NULL; i++)
	 if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
	 {
	    strcat(buf,weapon_table[i].name);
	    strcat(buf," ");
	 }
	 strcat(buf,"\n\rYour choice? ");
	 write_to_buffer(d,buf,0);
         d->connected = CON_PICK_WEAPON;
	break;

    case CON_PICK_WEAPON:
	write_to_buffer(d,"\n\r",2);
	weapon = weapon_lookup(argument);
	if (weapon == -1 || ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0)
	{
	    write_to_buffer(d,
		"That's not a valid selection. Choices are:\n\r",0);
            buf[0] = '\0';
            for ( i = 0; weapon_table[i].name != NULL; i++)
                if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
                {
                    strcat(buf,weapon_table[i].name);
		    strcat(buf," ");
                }
            strcat(buf,"\n\rYour choice? ");
            write_to_buffer(d,buf,0);
	    return;
	}

	ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
	write_to_buffer(d,"\n\r",2);
	
        if (AUTHORIZE_LOGINS == TRUE) 
        {
           d->connected = CON_AUTHORIZE_NAME;
           sprintf(buf, "%s (%s)@%s needs to be AUTHORIZED!", 
	           ch->name, d->rfc, d->host);
           wiznet( buf, d->character, NULL, WIZ_LOGINS, 0, 0 );
           write_to_buffer(d, 
	      "Now, please wait for an immortal to authorize you.\n\r", 0);
        } 
	else
        { 
	   do_help(ch,"motd");
 	   d->connected = CON_READ_MOTD;
	}
	break;

    case CON_BEGIN_REMORT:
	write_to_buffer( d, "Now beginning the remorting process.\n\r\n\r", 0 );
	write_to_buffer( d, 
  "Congradulations. You may choose from new races and classes.\n\r\n\r", 0 );
	write_to_buffer( d, "The following races are available:\n\r  ", 0 );
	for ( race = 1; race_table[race].name != NULL; race++ )
	{
	    if (!race_table[race].pc_race)
		break;
	    write_to_buffer(d,race_table[race].name,0);
	    write_to_buffer(d," ",1);
	}
	write_to_buffer(d,"\n\r",0);
	write_to_buffer(d,"What is your race (help for more information)? ",0);
	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_READ_IMOTD:
	write_to_buffer(d,"\n\r",2);
        do_help( ch, "bmotd" );
        d->connected = CON_READ_BMOTD;
	break;

    case CON_READ_BMOTD:
	write_to_buffer(d,"\n\r",2);
        do_help( ch, "motd" );
        d->connected = CON_READ_MOTD;
	break;

   case CON_AUTHORIZE_NAME:
   case CON_AUTHORIZE_NAME1:
   case CON_AUTHORIZE_NAME2:
   case CON_AUTHORIZE_NAME3:
        write_to_buffer(d, 
                       "Please wait for an immortal to authorize you.\n\r", 0);
        sprintf(log_buf, 
	       "%s (%s)@%s needs be to AUTHORIZED.", ch->name, d->rfc, d->host);
           d->connected++;
           
	   if(d->connected == CON_AUTHORIZE_LOGOUT)
           {
             write_to_buffer( d, "Auto exit to prevent spam.\n\r", 0 );
             sprintf(log_buf, 
	     "%s (%s)@%s auto logged off.", ch->name, d->rfc, d->host);
             close_socket( d );
             return;
            }
            break;
 
    case CON_READ_MOTD:
        if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
        {
            write_to_buffer( d, "Warning! Null password!\n\r",0 );
            write_to_buffer( d, "Please report old password with bug.\n\r",0);
            write_to_buffer( d,
                "Type 'password null <new password>' to fix.\n\r",0);
        }

	write_to_buffer( d, 
	    "\n\rWelcome to TSR. We Addict players for their enjoyment.\n\r",
	    0 );
	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;
	reset_char(ch);

	if ( ch->level == 0 )
	{

	    ch->perm_stat[class_table[ch->class].attr_prime] += 3;

	    ch->level	= 1;
	    ch->exp	= exp_per_level(ch,ch->pcdata->points);
	    ch->hit	= ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
	    ch->train	 = 3;
	    ch->practice = 5;
	    sprintf( buf, "the %s",
		title_table [ch->class] [ch->level]
		[ch->sex == SEX_FEMALE ? 1 : 0] );
	    set_title( ch, buf );

            ch->pcdata->speaking = race_table[ch->race].race_lang;
            ch->pcdata->language[race_table[ch->race].race_lang] = 100;
            ch->pcdata->learn = 5;
            ch->pcdata->language[0] = 85; 
	    
            do_outfit(ch,"");

         /* Turn everything on */
	    do_autoexit(ch,"");
	    do_autogold(ch,"");
	    do_autoloot(ch,"");
	    do_autosac(ch,"");

	    obj_to_char(create_object(get_obj_index(OBJ_VNUM_MAP),0),ch);

	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    send_to_char("\n\r",ch);
	    do_help(ch,"NEWBIE INFO");
	    send_to_char("\n\r",ch);
	}
	else if ( ch->in_room != NULL )
	{
	    char_to_room( ch, ch->in_room );
	}
	else if ( IS_IMMORTAL(ch) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	}
    
	sprintf(buf, "the name of %s into your ear.",ch->name);
        if(!IS_IMMORTAL(ch)) 
        do_info_char(ch,buf);

	act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
	do_look( ch, "auto" );

	wiznet("$N has left real life behind.",ch,NULL,
		WIZ_LOGINS,WIZ_SITES,get_trust(ch));

	if (ch->pet != NULL)
	{
	    char_to_room(ch->pet,ch->in_room);
	    act("$n has entered the game.",ch->pet,NULL,NULL,TO_ROOM);
	}

	do_unread(ch,"");
	break;
    }

    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if ( is_name( name, "all everyone auto immortal self someone something the you demise
balance circle loner honor") )
	return FALSE;
	
    if (str_cmp(capitalize(name),"Alander") && (!str_prefix("Alan",name)
    || !str_suffix("Alander",name)))
	return FALSE;

    /*
     * Length restrictions.
     */
     
    if ( strlen(name) <  2 )
	return FALSE;

#if defined(MSDOS)
    if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
    if ( strlen(name) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll,adjcaps = FALSE,cleancaps = FALSE;
 	int total_caps = 0;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;

	    if ( isupper(*pc)) /* ugly anti-caps hack */
	    {
		if (adjcaps)
		    cleancaps = TRUE;
		total_caps++;
		adjcaps = TRUE;
	    }
	    else
		adjcaps = FALSE;

	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;

	if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&&   (!fConn || ch->desc == NULL)
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char(
		    "Reconnecting. Type replay to see missed tells.\n\r", ch );
		act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );

		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
		wiznet("$N groks the fullness of $S link.",
		    ch,NULL,WIZ_LINKS,0,0);
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
	    write_to_buffer( d, "That character is already playing.\n\r",0);
	    write_to_buffer( d, "Do you wish to connect anyway (Y/N)?",0);
	    d->connected = CON_BREAK_CONNECT;
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL 
    ||   ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}



/*
 * Write to one char.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
      send_to_char_ansi( ch, txt, strlen(txt) );
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL)


    if (ch->lines == 0 )
    {
	send_to_char(txt,ch);
	return;
    }
	
#if defined(macintosh)
	send_to_char(txt,ch);
#else
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head,txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc,"");
#endif
}

void show_string(struct descriptor_data *d, char *input)
{
    char buffer[4*MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;

    one_argument(input,buf);
    if (buf[0] != '\0')
    {
	if (d->showstr_head)
	{
	    free_mem(d->showstr_head,strlen(d->showstr_head));
	    d->showstr_head = 0;
	}
    	d->showstr_point  = 0;
	return;
    }

    if (d->character)
	show_lines = d->character->lines;
    else
	show_lines = 0;

    for (scan = buffer; ; scan++, d->showstr_point++)
    {
	if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
	    && (toggle = -toggle) < 0)
	    lines++;

	else if (!*scan || (show_lines > 0 && lines >= show_lines))
	{
	    *scan = '\0';
           
            /* Possible COLOR FIX */
 	    if( d->character ) 
 	       send_to_char( buffer, d->character );
 	    else
	    write_to_buffer(d,buffer,strlen(buffer));
	    for (chk = d->showstr_point; isspace(*chk); chk++);
	    {
		if (!*chk)
		{
		    if (d->showstr_head)
        	    {
            		free_mem(d->showstr_head,strlen(d->showstr_head));
            		d->showstr_head = 0;
        	    }
        	    d->showstr_point  = 0;
    		}
	    }
	    return;
	}
    }
    return;
} 

/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
    if (ch->sex < 0 || ch->sex > 2)
    	ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

void act (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2,
	  int type)
{
    /* to be compatible with older code */
    act_new(format,ch,arg1,arg2,type,POS_RESTING);
}

void act_new( const char *format, CHAR_DATA *ch, const void *arg1, 
	      const void *arg2, int type, int min_pos)
{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
 
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;
 
    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return;

    /* discard null rooms and chars */
    if (ch == NULL || ch->in_room == NULL)
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
        if ( vch == NULL )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
            return;
        }

	if (vch->in_room == NULL)
	    return;

        to = vch->in_room->people;
    }
 
    for ( ; to != NULL; to = to->next_in_room )
    {
	/* 
	 * MOBPROGS
         * if ( to->desc == NULL || to->position < min_pos )
	 */

 	if ( (!IS_NPC(to) && to->desc == NULL )
 	||   ( IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT) )
 	||    to->position < min_pos )
            continue;

         /* end mobprogs */

        if ( (type == TO_CHAR) && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && (to == ch || to == vch) )
            continue;
 
        point   = buf;
        str     = format;
        while ( *str != '\0' )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }
            ++str;
 
            if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
            {
                bug( "Act: missing arg2 for code %d.", *str );
                i = " <@@@> ";
            }
            else
            {
                switch ( *str )
                {
                default:  bug( "Act: bad code %d.", *str );
                          i = " <@@@> ";                                break;
                /* Thx alex for 't' idea */
                case 't': i = (char *) arg1;                            break;
                case 'T': i = (char *) arg2;                            break;
                case 'n': i = PERS( ch,  to  );                         break;
                case 'N': i = PERS( vch, to  );                         break;
                case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
                case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
                case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
                case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;
 
                case 'p':
                    i = can_see_obj( to, obj1 )
                            ? obj1->short_descr
                            : "something";
                    break;
 
                case 'P':
                    i = can_see_obj( to, obj2 )
                            ? obj2->short_descr
                            : "something";
                    break;
 
                case 'd':
                    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    {
                        i = "door";
                    }
                    else
                    {
                        one_argument( (char *) arg2, fname );
                        i = fname;
                    }
                    break;
                }
            }
 
            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }
 
        *point++ = '\n';
        *point++ = '\r';
	*point   = '\0';             /* MOBPROGS */
        buf[0]   = UPPER(buf[0]);
 	if ( to->desc != NULL )     /* MOBPROGS */
        send_to_char_ansi( to, buf, point - buf );
 	else                        /* MOBPROGS */
 	if ( MOBtrigger )
 	    mp_act_trigger( buf, to, ch, arg1, arg2, TRIG_ACT );
           
       /* end mobprogs */	
    }
 
    return;
}



/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif
 
void send_to_char_ansi( CHAR_DATA *ch, const char *txt, int length )
{   
    const char *a,*b;
    int l,curlen=0;
    a=txt;
    if ( txt != NULL && ch!=NULL && ch->desc != NULL) 
    {
       	while(curlen<length && ch!=NULL) 
	{
     	    b=a;
     	    l=0;
            while(curlen<length && *a!='`') 
	    {
   	     	l++;
   	     	curlen++;
   	     	a++;
   	    }
   		 
            if (l)	
	    {
         	write_to_buffer(ch->desc,b,l); 
            }
            if (*a)
            {
          	a++;
          	curlen++;
          	if (curlen<length) 
		{
         	     process_color(ch, *a++); 
        	     curlen++;
          	}
            }
     	}
    }
}

void do_authorize(CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int mode = 0;

  if(argument[0] != '\0')
  {
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
         if(arg2[0] == '\0' || !str_prefix(arg2, "yes"))
      mode = 1;
    else if(!str_prefix(arg2, "no"))
      mode = 2;
    else
      mode = 3;
  }
  else
  {
    send_to_char("People waiting for authorization:\n\r",ch);
    send_to_char("---------------------------------\n\r",ch);
  }
  for(d = descriptor_list;d;d = d_next)
  {
    d_next      = d->next;
    if(!d->character) continue;
    if(!d->character->name) continue;
    if(mode == 0)
    {
      if(d->connected >= CON_AUTHORIZE_NAME
        && d->connected <= CON_AUTHORIZE_LOGOUT)
      {
        sprintf(buf, "  %s (%s)@%s\n\r", d->character->name, d->rfc, d->host);
        send_to_char( buf, ch );
      }
    }
    else if(d->connected >= CON_AUTHORIZE_NAME
     && d->connected <= CON_AUTHORIZE_LOGOUT
     && is_exact_name(d->character->name, arg1))
    {
      if(mode == 1)
      {
        send_to_char( "Character authorized.\n\r", ch);
        d->connected = CON_READ_MOTD;
        write_to_buffer(d, "You have been authorized.\n\r", 0 );
        sprintf(buf, "%s (%s)@%s AUTHORIZED by %s.",
         d->character->name, d->rfc, d->host, ch->name);
        log_string( buf );
        wiznet( buf, ch, NULL, WIZ_LOGINS, 0, 0 );
        return;
      }
      else
      if(mode == 2)
      {
        send_to_char( "Character denied.\n\r", ch);
        write_to_buffer(d, "Please choose a more medieval name.\n\r", 0 );
        sprintf(buf, "%s (%s)@%s was DENIED (DOH!) by %s.",
         d->character->name, d->rfc, d->host, ch->name);
        log_string( buf );
        wiznet( buf, ch, NULL, WIZ_LOGINS, 0, 0 );
        close_socket(d);
        return;
      }
      else
      {
        send_to_char( "Ok.\n\r", ch);
        sprintf(buf, "%s %s.\n\r", arg2, argument);
        write_to_buffer(d, buf, 0);
        sprintf(buf, "%s (%s)@%s was DENIED (DOH!) by %s.",
         d->character->name, d->rfc, d->host, ch->name);
        wiznet( buf, ch, NULL, WIZ_LOGINS, 0, 0 );
        close_socket(d);
        return;
      }
    }
  }
  if(mode)
    send_to_char( "No such unauthorized person.\n\r", ch);
  return;
}

static FILE *fsocket(domain, type, protocol)
int     domain;
int     type;
int     protocol;
{
    int     s;
    FILE   *fp;

    if ((s = socket(domain, type, protocol)) < 0) {
        return (0);
    } else {
        if ((fp = fdopen(s, "r+")) == 0) {
            close(s);
        }
        return (fp);
    }
}
 
static void timeout(sig)
int     sig;
{
    longjmp(timebuf, sig);
}

char   *rfcauth(rmt_sin, our_sin)
struct sockaddr_in *rmt_sin;
struct sockaddr_in *our_sin;
{
    unsigned rmt_port;
    unsigned our_port;
    struct sockaddr_in rmt_query_sin;
    struct sockaddr_in our_query_sin;
    static char user[256];
    char    buffer[512];
    char   *cp;
    char   *result = "unknown";
    FILE   *fp;
 
    if ((fp = fsocket(AF_INET, SOCK_STREAM, 0)) != 0) {
        setbuf(fp, (char *) 0);
 
        if (setjmp(timebuf) == 0) {
            signal(SIGALRM, timeout);
            alarm(rfc931_timeout);
            our_query_sin = *our_sin;
            our_query_sin.sin_port = htons(ANY_PORT);
            rmt_query_sin = *rmt_sin;
            rmt_query_sin.sin_port = htons(RFC931_PORT);

            if (bind_connect(fileno(fp),
                             (struct sockaddr *) & our_query_sin,
                             (struct sockaddr *) & rmt_query_sin,
                             sizeof(our_query_sin)) >= 0) {

                fprintf(fp, "%u,%u\r\n",
                        ntohs(rmt_sin->sin_port),
                        ntohs(our_sin->sin_port));
                fflush(fp);

                if (fgets(buffer, sizeof(buffer), fp) != 0
                    && ferror(fp) == 0 && feof(fp) == 0
                    && sscanf(buffer, "%u , %u : USERID :%*[^:]:%255s",
                              &rmt_port, &our_port, user) == 3
                    && ntohs(rmt_sin->sin_port) == rmt_port
                    && ntohs(our_sin->sin_port) == our_port) {

                    if ((cp = strchr(user, '\r')))
                        *cp = 0;
                    result = user;
                }
            }
            alarm(0);
        }
        fclose(fp);
    }
    return (result);
}
 
int     bind_connect( int s, struct sockaddr *local, struct sockaddr *remote, int length )
{
    if (bind(s, local, length) < 0) {
        return (-1);
    } else {
        return (connect(s, remote, length));
    }
}
 
int count_chars(char *txt, char character)
{
   int i, cnt = 0;
   
   for(i = 0; txt[i]; i++)
      if(txt[i] == character)
	 cnt++;
    
   return cnt;
}
