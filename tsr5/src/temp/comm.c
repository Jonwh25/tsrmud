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

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#include "merc.h"
#include "interp.h"


/* Global Defines */
int     AUTHORIZE_LOGINS = FALSE;
void reset_builder_levels ( );



/*
 * Malloc debugging stuff.
 */
#if defined( sun )
#undef MALLOC_DEBUG
#endif

#if defined( MALLOC_DEBUG )
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute( atomic ) in signal.h,
 *   I dance around it.
 */
#if defined( apollo )
#define __attribute( x )
#endif

#if defined( unix )
#include <signal.h>
#endif

#if defined( apollo )
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if	defined( macintosh ) || defined( MSDOS )
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
#endif

#if	defined( unix )
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
#endif



/*
 * OS-dependent declarations.
 */
#if	defined( _AIX )
#include <sys/select.h>
/* int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
*/void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			       int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if     defined( irix )
void	bzero		args( ( char *b, int length ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
int	close		args( ( int fd ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#endif

#if	defined( apollo )
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined( __hpux )
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

#if     defined( interactive )
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined( linux )
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
*/int	close		args( ( int fd ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
/*int	read		args( ( int fd, char *buf, int nbyte ) );*/
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
/*int	write		args( ( int fd, char *buf, int nbyte ) );*/
bool    check_ban         args( ( struct descriptor_data *dnew, bool loggedin ) );
bool    check_newban      args( ( struct descriptor_data *dnew, bool loggedin ) );
#endif

#if	defined( macintosh )
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined( isascii )
#define	isascii( c )		( ( c ) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined( MIPS_OS )
extern	int		errno;
#endif

#if	defined( MSDOS )
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) ); /* JWD */
/* int	gettimeofday	args( ( struct timeval *tp, void *tzp ) ); */
int	kbhit		args( ( void ) );
#endif

#if	defined( NeXT )
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined( htons )
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined( ntohl )
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			       fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined( sequent )
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined( htons )
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined( ntohl )
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

/*
 * This includes Solaris SYSV as well
 */

#if defined( sun )
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
#if defined( SYSV )
int     setsockopt      args( ( int s, int level, int optname,
			       const char *optval, int optlen ) );
#else
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			       int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined( ultrix )
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
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    merc_down;		/* Shutdown                     */
bool		    wizlock;		/* Game is wizlocked		*/
int                 numlock = 0;        /* Game is numlocked at <level> */
char		    str_boot_time [ MAX_INPUT_LENGTH ];
time_t		    current_time;	/* Time of this pulse		*/
time_t		    exe_comp_time;	/* Time the executable was compiled */
char *		    exe_file;		/* Name of the executable	*/
int		    cocount;		/* Copyover Timer Var		*/
CHAR_DATA* 	    coowner;		/* Copyover Owner		*/


/*
 * OS-dependent local functions.
 */
#if defined( macintosh ) || defined( MSDOS )
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined( unix )
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
void    bust_a_prompt           args( ( DESCRIPTOR_DATA *d ) );


/* Semi-local functions that arent OS dependant.. :)..
   -- Altrag */
int port, control;
int maxdesc;

#define TIMEOFS     (5739 * 86400) + (7 * 3600) + (24 * 60)



int main( int argc, char **argv )
{
    struct timeval now_time;
    bool fCopyOver = FALSE;

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    exe_file = str_dup( argv[0] );
    {
      struct stat statis;
      if ( !stat( exe_file, &statis ) )
         exe_comp_time = statis.st_mtime;
    }
    gettimeofday( &now_time, NULL );
    current_time 	= (time_t) now_time.tv_sec;
    if ( current_time < TIMEOFS ) current_time += TIMEOFS;
    strcpy( str_boot_time, ctime( &current_time ) );

#ifdef THREADED_DNS
     rdns_cache_set_ttl(12*60*60);       /* twelve hours is moooore than enough */
#endif

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
	
	/* Are we recovering from a copyover? */
	if (argv[2] && argv[2][0])
 	{
 		fCopyOver = TRUE;
 		control = atoi(argv[3]);
 	}
 	else
 		fCopyOver = FALSE;
	
    }

    /* Checks what port -- note Angi */
    if ( port == 8888 ) 
       reset_builder_levels();

    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db( );
    log_string( "EnvyMud is ready to rock.", CHANNEL_NONE, -1 );
    game_loop_mac_msdos( );
#endif

#if defined(unix)
	if (!fCopyOver)
	    control = init_socket( port );
	    
    boot_db();
    sprintf( log_buf, "TSR is ready to rock on port %d.", port );
    log_string( log_buf, CHANNEL_NONE, -1 );
    
    if (fCopyOver)
    	copyover_recover();
    
    game_loop_unix( control );
    close (control);
#endif

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game.", CHANNEL_NONE, -1 );
    exit( 0 );
    return 0;
}


#if defined( unix )
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
           struct sockaddr_in sa;
                  int         x        = 1; 
                  int         fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	return -1;
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof( x ) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close( fd );
	return -1;
    }

/*
#if defined( SO_DONTLINGER ) && !defined( SYSV )
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof( ld ) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close( fd );
	    return -1;
	}
    }
#endif
*/

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof( sa ) ) < 0 )
    {
	if ( errno != EADDRINUSE )
	    perror( "Init_socket: bind" );
	close( fd );
	return (errno == EADDRINUSE ? -2 : -1);
    }

    if ( listen( fd, 3 ) < 0 )
    {
	perror( "Init_socket: listen" );
	close( fd );
	return -1;
    }

    return fd;
}
#endif



#if defined( macintosh ) || defined( MSDOS )
void game_loop_mac_msdos( void )
{
    static        DESCRIPTOR_DATA dcon;
           struct timeval         last_time;
           struct timeval         now_time;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;
    if ( current_time < TIMEOFS ) current_time += TIMEOFS;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.character      = NULL;
    dcon.connected	= CON_GET_NAME;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 1;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.showstr_head   = str_dup( "" );
    dcon.showstr_point  = 0;
    dcon.pEdit		= NULL;			/* OLC */
    dcon.pString	= NULL;			/* OLC */
    dcon.editor		= 0;			/* OLC */
    dcon.next		= descriptor_list;
    descriptor_list	= &dcon;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( &dcon, help_greeting+1, 0 );
	else
	    write_to_buffer( &dcon, help_greeting  , 0 );
    	write_to_buffer( &dcon, "We are moving to a different server soon.\n\r", 0);
    	write_to_buffer( &dcon, "Please standby for a new address popping up.\n\r", 0);
    	write_to_buffer( &dcon, "If you cannot find us, email to\n\r", 0);
    	write_to_buffer( &dcon, "SoD_Hunter@roprosoft.demon.nl\n\r", 0);
    	write_to_buffer( &dcon, "\n\rPlease enter thy name: ",0);
    }

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

#if defined( MSDOS )
	    if ( kbhit( ) )
#endif
	    {
		if ( d->character )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character )
			save_char_obj( d->character, FALSE );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }
	    if ( d->character && d->character->race_wait > 0 )
		--d->character->race_wait;
	    if ( d->character && d->character->wait > 0 )
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
		if ( d->showstr_point )
		    show_string( d, d->incomm );
		else
		if ( d->pString )
		    string_add( d->character, d->incomm );
		else
		    if ( d->connected == CON_PLAYING )
		    {
			if ( !run_olc_editor( d ) )
			    interpret( d->character, d->incomm ); 
		    }
		    else
			nanny( d, d->incomm );
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
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character )
			save_char_obj( d->character, FALSE );
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

#if defined( MSDOS )
	    if ( kbhit( ) )
#endif
	    {
		if ( dcon.character )
		    dcon.character->timer = 0;
		if ( !read_from_descriptor( &dcon ) )
		{
		    if ( dcon.character )
			save_char_obj( d->character, FALSE );
		    dcon.outtop	= 0;
		    close_socket( &dcon );
		}
#if defined( MSDOS )
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
	if ( current_time < TIMEOFS ) current_time += TIMEOFS;
    }

    return;
}
#endif



#if defined( unix )
void game_loop_unix( int control )
{
    static struct timeval null_time;
           struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;
    if ( current_time < TIMEOFS ) current_time += TIMEOFS;

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;
	fd_set           in_set;
	fd_set           out_set;
	fd_set           exc_set;
/*	int              maxdesc;*/

#if defined( MALLOC_DEBUG )
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
	    exit( 0 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    init_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character, FALSE);
		d->outtop	= 0;
		close_socket( d );
                continue;
	    }
	}
	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character )
			save_char_obj( d->character, FALSE );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }
	    if ( d->character && d->character->race_wait > 0 )
		--d->character->race_wait;

	    if ( d->character && d->character->wait > 0 )
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
		if ( d->showstr_point )
		    show_string( d, d->incomm );
		else
		if ( d->pString )
		    string_add( d->character, d->incomm );
		else
		{
		    char buf[MAX_INPUT_LENGTH];
		    char buf2[MAX_STRING_LENGTH];
		    FILE *fp;

		    sprintf( buf2, "%s: %s: %s\n", ctime( &current_time ),
			     d->character ? d->character->name : "(Unknown)",
			     d->incomm ); 
		    fclose(fpReserve);
		    if ( (fp = fopen( buf, "a" ) ) )
		    {
		      fprintf( fp, "%s: %s: %s\n", ctime( &current_time ),
			       d->character ? d->character->name : "(Unknown)",
			       d->incomm );
		      fclose(fp);
		    }
		    fpReserve = fopen( NULL_FILE, "r" );
		
		    if ( d->connected == CON_PLAYING )
		    {
			if ( !run_olc_editor( d ) )
			    interpret( d->character, d->incomm ); 
		    }
		    else if ( d->connected == CON_CHATTING )
		    {
		      chat_interp( d->character, d->incomm );
		    }
		    else
			nanny( d, d->incomm );
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
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
		&& FD_ISSET( d->descriptor, &out_set ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character )
			save_char_obj( d->character, FALSE );
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
	    usecDelta	= ( (int) last_time.tv_usec )
	                - ( (int)  now_time.tv_usec )
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ( (int) last_time.tv_sec )
	                - ( (int)  now_time.tv_sec );
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
		    exit( 0 );
		}
	    }
	}
	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
	if ( current_time < TIMEOFS ) current_time += TIMEOFS;
    }

    return;
}
#endif



#if defined(unix)
void init_descriptor( int control )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    int size;

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

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
    dnew = new_descriptor(); /* new_descriptor now also allocates things */
    dnew->descriptor    = desc;
    dnew->pEdit		= NULL;			/* OLC */
    dnew->pString	= NULL;			/* OLC */
    dnew->editor	= 0;			/* OLC */

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup("(unknown)");
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
	sprintf( log_buf, "Sock.sinaddr:  %s", buf );
	log_string( log_buf, CHANNEL_GOD, -1 );
	from = gethostbyaddr( (char *) &sock.sin_addr,
	       sizeof(sock.sin_addr), AF_INET );
	free_string(dnew->host);
	dnew->host = str_dup( from ? from->h_name : buf );
        sprintf( buf2, "Connection from: %s", dnew->host);
        wiznet(buf2,dnew->character,NULL,WIZ_SECURE, 0, 0);


     }
	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if ( check_ban( dnew, FALSE ) ) 
    {
	write_to_descriptor( desc,
	    "Your site has been banned from this mud.\n\r", 0 );
	close( desc );
	free_descriptor(dnew);
	return;
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
	extern char * help_greeting;
	extern char * help_greeting1;
	extern char * help_greeting2;
	extern char * help_greeting3;

        if ( help_greeting[0] == '.' )
                 switch (number_range(0,3))
                 {
	             default: write_to_buffer(dnew, help_greeting+1,0);
                     break;

	             case  0: write_to_buffer(dnew, help_greeting+1,0);
                     break;

	             case  1: write_to_buffer(dnew, help_greeting1+1,0);
                     break;

	             case  2: write_to_buffer(dnew, help_greeting2+1,0);
                     break;

	             case  3: write_to_buffer(dnew, help_greeting3+1,0);
                     break;
                 }
     	      else	    
                 switch (number_range(0,3))
                 {
	             default: write_to_buffer(dnew, help_greeting,0);
                     break;

	             case  0: write_to_buffer(dnew, help_greeting,0);
                     break;

	             case  1: write_to_buffer(dnew, help_greeting1,0);
                     break;

	             case  2: write_to_buffer(dnew, help_greeting2,0);
                     break;

	             case  3: write_to_buffer(dnew, help_greeting3,0);
                     break;
                 }
    }

    return;
}
#endif


void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->original ) )
    {
      if ( !dclose->character )
      {
	sprintf( log_buf, "Close_socket: original without character: %s",
		 (dclose->original->name ? dclose->original->name
		  : "(unknown)" ) );
	bug( log_buf, 0);
	dclose->original->desc = NULL;
      }
      else
      {
	if ( IS_NPC(dclose->character) )
	  do_return( dclose->character, "" );
	else
	  dclose->original->desc = NULL;
      }
    }

    if ( ( ch = dclose->character ) )
    {
	sprintf( log_buf, "Closing link to %s.", (ch->name ? ch->name : "(unknown)") );
	log_string( log_buf, CHANNEL_LOG, -1 );

	/* If ch is writing note or playing, just lose link otherwise clear char */
	if ((dclose->connected == CON_PLAYING && !merc_down)
	    ||((dclose->connected >= CON_NOTE_TO)
	    && (dclose->connected <= CON_NOTE_FINISH)))
	{
          if ( ch->name )
            act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
          else
	    act( "Someone has lost their link.", ch, NULL, NULL, TO_ROOM );
	  wiznet( "Netdeath has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0 );
	  if ( ch->in_room->vnum == ROOM_VNUM_SMITHY )
		{
		char_from_room( ch );
		char_to_room( ch, get_room_index( 768 ) );
		}
	  if (    !IS_SET( ch->act, PLR_WIZINVIS ) 
               && !IS_SET( ch->act, PLR_CLOAKED ) )
	       info( "The link between %s and the storm has been torn.", 
		     (int)(ch->name), 0 );
/*	  {
	    CHAR_DATA *PeT;
	    for ( PeT = ch->in_room->people; PeT; PeT = PeT->next_in_room )
	      {
	      if ( IS_NPC( PeT ) )
		if ( IS_SET( PeT->act, ACT_PET ) && PeT->master == ch )
		   {
		
		   REMOVE_BIT( PeT->act, ACT_PET );  
		   break;
		
		   }
	      } 
	  }*/
	  ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->character );
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
	if ( d )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    close( dclose->descriptor );
    free_string( dclose->host );
    free_string( dclose->user );
    free_string( dclose->showstr_head );
    /* RT socket leak fix */
    free_mem( dclose->outbuf, dclose->outsize );

/*    dclose->next	= descriptor_free;
    descriptor_free	= dclose;*/
    free_mem( dclose, sizeof( *dclose ) );
#if defined( MSDOS ) || defined( macintosh )
    exit( 0 );
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
    iStart = strlen( d->inbuf );
    if ( iStart >= sizeof( d->inbuf ) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf, CHANNEL_GOD, -1 );
        wiznet("Spam spam spam $N spam spam spam spam spam!",
               d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
        write_to_descriptor( d->descriptor,
                    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined( macintosh )
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
	if ( iStart > sizeof( d->inbuf ) - 10 )
	    break;
    }
#endif

#if defined( MSDOS ) || defined( unix )
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof( d->inbuf ) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read.", CHANNEL_GOD, -1 );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK || errno == EAGAIN )
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
    int i;
    int j;
    int k;

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
	else if ( isascii( d->inbuf[i] ) && isprint( d->inbuf[i] ) )
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
	    if ( ++d->repeat >= 20 )
	    {
		sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf, CHANNEL_GOD, -1 );
                wiznet("Spam spam spam $N spam spam spam spam spam!",
                       d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
                write_to_descriptor( d->descriptor,
                    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
/*                if ( d->descriptor ) */
		   strcpy( d->incomm, "quit" );


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
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !merc_down && d->connected == CON_PLAYING )	/* OLC */
        if ( d->showstr_point )
	    write_to_buffer( d,
  "[Please type (c)ontinue, (r)efresh, (b)ack, (h)elp, (q)uit, or RETURN]:  ",
			    0 );
	else
	if ( d->pString )
	    write_to_buffer( d, "> ", 2 );
	else
	{
	    CHAR_DATA *ch;
	    CHAR_DATA *victim;

/* XOR */
        /* battle prompt from Rom2 */
	ch = d->character;

        if ((victim = ch->fighting) != NULL)
        {
	  int percent;
/*	  int iHealth;
	  char buf2[MAX_STRING_LENGTH];
	  char health[20];*/
	  char wound[100];
	  char buf[MAX_STRING_LENGTH];
 
	  if (MAX_HIT(victim) > 0)
	    percent = victim->hit * 100 / MAX_HIT(victim);
	  else
	    percent = -1;
	  if (percent >= 100)
	    sprintf(wound,"is in excellent condition.");
	  else if (percent >= 90)
	    sprintf(wound,"has a few scratches.");
	  else if (percent >= 75)
	    sprintf(wound,"has some small wounds and bruises.");
	  else if (percent >= 50)
	    sprintf(wound,"has quite a few wounds.");
	  else if (percent >= 30)
	    sprintf(wound,"has some big nasty wounds and scratches.");
	  else if (percent >= 15)
	    sprintf(wound,"looks pretty hurt.");
	  else if (percent >= 0)
	    sprintf(wound,"is in awful condition.");
	  else
	    sprintf(wound,"is bleeding to death.");
/*
  	  strcpy( buf2, "[" );
	  if ( percent >= 66 )
	    strcat( buf2, "" );
	  else if( percent >= 33 )
	    strcat( buf2, "" );
	  else
	    strcat( buf2, "" );
	  health[0] = '+';
	  for( iHealth = 1; iHealth < 20; iHealth++ )
	    if ( percent >= iHealth * 5 )
	      health[iHealth] = '+';
	    else
	      health[iHealth] = ' ';
	  health[iHealth] = '\0';
	  strcat( buf2, health );
	  strcat( buf2, "]" );
	  sprintf( wound, "%s", buf2 );
*/
	  sprintf(buf,"\n\r%s %s",
		  IS_NPC(victim) ? victim->short_descr : victim->name,wound);
	  buf[0] = UPPER(buf[0]);

	  write_to_buffer( d, buf, 0);
        }
	else if ( (victim = ch->hunting) && IS_NPC(victim) )
	{
	  hunt_victim( ch ); 
	}

	    ch = d->original ? d->original : d->character;
	    if ( IS_SET( ch->act, PLR_BLANK     ) )
	        write_to_buffer( d, "\n\r", 2 );

	    if ( IS_SET( ch->act, PLR_PROMPT    ) )
	           bust_a_prompt( d );

	    if ( IS_SET( ch->act, PLR_TELNET_GA ) )
	        write_to_buffer( d, go_ahead_str, 0 );
	}
    else if ( d->connected == CON_CHATTING )
      write_to_buffer( d, ": ", 0);
    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor(d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	free_mem(d->outbuf, d->outsize);
	d->outsize = 1;
	d->outbuf = alloc_mem(d->outsize );
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	free_mem(d->outbuf, d->outsize);
	d->outsize = 1;
	d->outbuf = alloc_mem(d->outsize);
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
         CHAR_DATA *ch;
   const char      *str;
   const char      *i;
         char      *point;
         char      *pbuff;
         char       buffer[ MAX_STRING_LENGTH ];
         char       buf  [ MAX_STRING_LENGTH  ];
         char       buf2 [ MAX_STRING_LENGTH  ];

   ch = d->character;
   if( !ch->prompt || ch->prompt[0] == '\0' )
   {
      send_to_char_bw( "\n\r\n\r", ch );
      return;
   }
   i = 0;
   point = buf;
   str = d->original ? d->original->prompt : d->character->prompt;
   send_to_char( "", ch);
   while( *str != '\0' )
   {
      if( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
            i = " "; break;
         case 'h' :
            sprintf( buf2, "%d", ch->hit                               );
            i = buf2; break;
         case 'H' :
            sprintf( buf2, "%d", MAX_HIT(ch) );
            i = buf2; break;
         case 'm' :
	    sprintf( buf2, "%d", MT( ch ) );
/*
            if ( !is_class( ch, CLASS_VAMPIRE ) )
               sprintf( buf2, "%d", ch->mana );
            else
               sprintf( buf2, "%d", ch->bp   );
*/
            i = buf2; break;
         case 'M' :
	    sprintf( buf2, "%d", MT_MAX( ch ) );
/*
            if ( !is_class( ch, CLASS_VAMPIRE ) )
               sprintf( buf2, "%d", MAX_MANA(ch));
            else
               sprintf( buf2, "%d", MAX_BP(ch)  );
*/
            i = buf2; break;
         case 'v' :
            sprintf( buf2, "%d", ch->move                              ); 
            i = buf2; break;
         case 'V' :
            sprintf( buf2, "%d", MAX_MOVE(ch));
            i = buf2; break;
         case 'x' :
            sprintf( buf2, "%d", ch->exp                               );
            i = buf2; break;
         case 'X' :
	    sprintf( buf2, "%d", xp_tolvl( ch ) - ch->exp             );
	    i = buf2; break;
#ifdef NEW_MONEY
	 case 'g' :
	    sprintf( buf2, "%d", ch->money.gold			       );
#else
         case 'g' :
            sprintf( buf2, "%d", ch->gold                              );
#endif
	    i = buf2; break;
         case 'a' :
            if( ch->level >= 5 )
               sprintf( buf2, "%d", ch->alignment                      );
            else
               sprintf( buf2, "%s", IS_GOOD( ch ) ? "good"
		                  : IS_EVIL( ch ) ? "evil" : "neutral" );
            i = buf2; break;
         case 'r' :
            if( ch->in_room )
               sprintf( buf2, "%s", ch->in_room->name                  );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case 'R' :
            if( IS_IMMORTAL( ch ) && ch->in_room )
               sprintf( buf2, "%d", ch->in_room->vnum                  );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case 'z' :
            if( IS_IMMORTAL( ch ) && ch->in_room )
               sprintf( buf2, "%s", ch->in_room->area->name            );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case '%' :
            sprintf( buf2, "%%"                                        );
            i = buf2; break;
         case 'c' :		/* OLC */
	    i = olc_ed_name( ch );
	    break;
         case 'C' :		/* OLC */
	    i = olc_ed_vnum( ch );
	    break;
         case 'p' :
	   {
	     if ( IS_IMMORTAL( ch ) )
	     {
	       extern int sAllocPerm;

	       sprintf( buf2, "%d", (sAllocPerm * 10) / (1024 * 1024) ); 
             }
	     else
    	 	sprintf( buf2, " " ); 
	     i = buf2; break;
	   }
         case 'l' :
            if ( IS_SET( ch->act, PLR_WIZINVIS ) )
                sprintf( buf2, "Invisible" );
            else
                sprintf( buf2, "Visible" );
            i = buf2;break;
         case 'd' :
            if ( IS_SET( ch->act, PLR_CLOAKED ) )
                sprintf( buf2, "Cloaked" );
            else
                sprintf( buf2, "Visible" );
            i = buf2;break;                              
      } 
      ++str;
      while( ( *point = *i ) != '\0' )
         ++point, ++i;      
   }
   *point = '\0';
   pbuff	= buffer;
   colourconv( pbuff, buf, ch );
   write_to_buffer( d, buffer, 0 );
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
/*  strcpy( d->outbuf + d->outtop, txt ); */
    strncpy( d->outbuf + d->outtop, txt, length );
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

#if defined( macintosh )
    if ( desc == 0 )
	desc = 1;
#endif

    if ( length <= 0 )
	length = strlen( txt );

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 2048 );
#if !defined( WIN32 )
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
#else
	if ( ( nWrite = send( desc, txt + iStart, nBlock , 0) ) < 0 )
#endif
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *ch;
    char      *pwdnew;
    char      *p;
    char       buf [ MAX_STRING_LENGTH ];
    char       buf2[ MAX_STRING_LENGTH ];
    int        iClass;
    int	       iCount;
    bool       fOld;
#ifdef NEW_MONEY
    MONEY_DATA amt;
#endif

    /* Delete leading spaces UNLESS character is writing a note */
    if (d->connected != CON_NOTE_TEXT)
    {
       while ( isspace( *argument ) )
       argument++;
    }

    /* This is here so we wont get warnings.  ch = NULL anyways - Kahn */
    ch          = d->character;

    switch ( d->connected )
    {

    default:
    bug( "Nanny: bad d->connected %d.", d->connected );
    close_socket( d );
    return;

    case CON_GET_NAME:
    if ( argument[0] == '\0' )
    {
        close_socket( d );
        return;
    }

    argument[0] = UPPER( argument[0] );
        log_string( argument, CHANNEL_NONE , -1 );
    fOld = load_char_obj( d, argument );
    ch   = d->character;

    if ( !check_parse_name( argument ) && strcmp(argument,"Hunter"))
    {
        if ( !fOld )
        {
            write_to_buffer( d,
                "Illegal name, try another.\n\rName: ", 0 );
        if ( d->character )
          free_char( d->character );
        d->character = NULL;
        return;
       }
       else
       {
       sprintf( buf, "Illegal name:  %s", argument );
           bug( buf, 0 );
       }
   }

   if ( IS_SET( ch->act, PLR_DENY ) )
   {
       sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
       log_string( log_buf, CHANNEL_GOD , -1 );
       write_to_buffer( d, "You are denied access.\n\r", 0 );
       close_socket( d );
       return;
   }

   if ( check_reconnect( d, argument, FALSE ) )
   {
       fOld = TRUE;
   }
   else
   {
       /* Must be immortal with wizbit in order to get in */
       if ( wizlock && !IS_HERO( ch ) && !ch->wizbit )
       {
       write_to_buffer( d, "The game is open for Imms only right now. Come back later..\n\r", 0 );
       close_socket( d );
       return;
       }
       if ( ch->level <= numlock && !ch->wizbit && numlock != 0 )
       {
       write_to_buffer( d,
           "The game is locked to your level character.\n\r\n\r",
               0 );
       if ( ch->level == 0 )
       {
           write_to_buffer( d,
           "New characters are now temporarily in email ",
                   0 );
           write_to_buffer( d, "registration mode.\n\r\n\r", 0 );
           write_to_buffer( d,
           "Please email storm@netway.com to ", 0 );
           write_to_buffer( d, "register your character.\n\r\n\r",
                   0 );
            write_to_buffer( d,
            "One email address per character please.\n\r", 0 );
            write_to_buffer( d, "Thank you, EnvyMud Staff.\n\r\n\r",
                    0 );
        }
        close_socket( d ) ;
        return;
        }
    }

    if ( fOld )
    {
        /* Old player */
        write_to_buffer( d, "Password: ", 0 );
        write_to_buffer( d, echo_off_str, 0 );
        d->connected = CON_GET_OLD_PASSWORD;
        return;
    }
    else
    {
        /* New player */
        if ( check_playing( d, ch->name ) )
        return;
        if ( check_newban( d, FALSE ) )
        {
        close_socket( d );
        return;
        }
        sprintf( buf, 
		"Did I get that right, %s (Y/N)? ",
		argument );
        write_to_buffer( d, buf, 0 );
        d->connected = CON_CONFIRM_NEW_NAME;
        return;
    }
    break;

    case CON_GET_OLD_PASSWORD:
#if defined( unix )
    write_to_buffer( d, "\n\r", 2 );
#endif

    if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
        write_to_buffer( d, "Wrong password.\n\r", 0 );
        close_socket( d );
        return;
   }

   write_to_buffer( d, echo_on_str, 0 );

   if ( check_playing( d, ch->name ) )
       return;

   if ( check_reconnect( d, ch->name, TRUE ) )
       return;

   if ( ch->level != L_IMP )
     {
      sprintf( log_buf, "%s!%s@%s has connected.", 
		ch->name, d->user, d->host );
          wiznet(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch));
     }
   if ( IS_HERO( ch ) )
        do_help( ch, "imotd" );
   do_help( ch, "motd" );
   d->connected = CON_READ_MOTD;
   break;

   case CON_CONFIRM_NEW_NAME:
   switch ( *argument )
   {
   case 'y': case 'Y':
       sprintf( buf, 
		"New character.\n\rGive me a password for %s: %s",
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
       write_to_buffer( d, "Please type Yes or No: ", 0 );
       break;
   }
   break;

   case CON_GET_NEW_PASSWORD:
#if defined( unix )
    write_to_buffer( d, "\n\r", 2 );
#endif

    if ( strlen( argument ) < 5 )
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
    ch->pcdata->pwd = str_dup( pwdnew );
    write_to_buffer( d, "Please retype password: ", 0 );
    d->connected = CON_CONFIRM_NEW_PASSWORD;
    break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined( unix )
    write_to_buffer( d, "\n\r", 2 );
#endif

    if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
        write_to_buffer( d, 
	"Passwords don't match.\n\rRetype password: ",
        0 );
        d->connected = CON_GET_NEW_PASSWORD;
        return;
    }

    write_to_buffer( d, echo_on_str, 0 );
    write_to_buffer( d, "What is your sex (M/F/N)? ", 0 );
    d->connected = CON_GET_NEW_SEX;
    break;

    case CON_GET_NEW_SEX:
    switch ( argument[0] )
    {
    case 'm': case 'M': ch->sex = SEX_MALE;    break;
    case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
    case 'n': case 'N': ch->sex = SEX_NEUTRAL; break;
    default:
        write_to_buffer( d, 
	"That's not a sex.\n\rWhat IS your sex? ", 0 );
        return;
    }
    buf2[0] = '\0';
    strcpy( buf2, 
"\n\r                  -= Select a race from the list below =-\n\r\n\r" );
    for ( iClass = 0; iClass < MAX_RACE; iClass++ )
      {
      sprintf( buf, "[%-12s]%s", 
	       (get_race_data(iClass))->race_full, 
	      (( iClass+1) % 5) == 0 ? "\n\r" : "" );
      strcat( buf2, buf );
      }
    if ( iClass % 5 != 0 )
	strcat( buf2, "\n\r" );
    strcat( buf2, "\n\rRACE -> " );
    write_to_buffer( d, buf2, 0 );
    wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
    d->connected = CON_GET_NEW_RACE;
    break;
    case CON_GET_NEW_RACE:
    for ( iClass = 0; iClass < MAX_RACE; iClass++ )
    {
        if ( ( !str_prefix( argument, (get_race_data(iClass))->race_name ) )
        || ( !str_cmp( argument, (get_race_data(iClass))->race_full ) ) )
        {
        ch->race = iClass;
        break;
        }
    }

    if ( iClass == MAX_RACE )
    {
        write_to_buffer( d,
        "That's not a race.\n\rWhat IS your race? ", 0 );
        return;
    }
	sprintf( buf, "%s %s", (get_race_data(iClass))->race_full,
		 (get_race_data(iClass))->race_name );
	do_help( ch, buf );
        write_to_buffer( d, "Is this the race you desire? ", 0 );
        d->connected = CON_CONFIRM_RACE;
        break;

    case CON_CONFIRM_RACE:
    switch ( *argument )
    {
    case 'y': case 'Y':
    buf2[0]='\0';
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
    iCount = 0;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
	if ( class_table[iClass].races[ch->race] )
	  {
	  iCount++;
	  sprintf( buf, "[%-12s]%s", 
		class_table[iClass].who_long,
		iCount % 5 == 0 ? "\n\r" : "" );
	  strcat( buf2, buf );
	  }
    }
    if ( iCount % 5 != 0 )
      strcat( buf2, "\n\r" );
    strcat( buf2, "\n\r1ST CLASS -> " );
    write_to_buffer( d, buf2, 0 );
    d->connected = CON_GET_NEW_CLASS;
    break;
    case 'n': case 'N':
    strcpy( buf2, 
"\n\r                  -= Select a race from the list below =-\n\r\n\r" );
    for ( iClass = 0; iClass < MAX_RACE; iClass++ )
      {
      sprintf( buf, "[%-12s]%s", 
	       (get_race_data(iClass))->race_full, 
	      (( iClass+1) % 5) == 0 ? "\n\r" : "" );
      strcat( buf2, buf );
      }
    if ( iClass % 5 != 0 )
	strcat( buf2, "\n\r" );
    strcat( buf2, "\n\rRACE -> " );
    write_to_buffer( d, buf2, 0 );
    d->connected = CON_GET_NEW_RACE;
    break;

    default:
        write_to_buffer( d, 
	"Please type Yes or No: ", 0 );
        break;
    }
    break;

    case CON_GET_NEW_CLASS:
    if ( !str_cmp( argument, "back" ) )
	{
    	strcpy( buf2, 
"\n\r                  -= Select a race from the list below =-\n\r\n\r" );
	for ( iClass = 0; iClass < MAX_RACE; iClass++ )
	  {
	  sprintf( buf, "[%-12s]%s", 
	       (get_race_data(iClass))->race_full, 
	      (( iClass+1) % 5) == 0 ? "\n\r" : "" );
          strcat( buf2, buf );
          }
        if ( iClass % 5 != 0 )
	  strcat( buf2, "\n\r" );
        strcat( buf2, "\n\rRACE -> " );
        write_to_buffer( d, buf2, 0 );
        d->connected = CON_GET_NEW_RACE;
	return;
	}
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        if ( !str_prefix( argument, class_table[iClass].who_long )
        && class_table[iClass].races[ch->race] )
        {
        ch->class[0] = iClass;
        break;
        }
    }

    if ( iClass == MAX_CLASS )
    {
        write_to_buffer( d,
        "That's not a class.\n\rWhat IS your class? ", 0 );
        return;
    }

    do_help( ch, (char *)class_table[iClass].who_long );
    write_to_buffer( d, "Is this the class you desire? ", 0 );
    d->connected = CON_CONFIRM_CLASS;
        break;

    case CON_CONFIRM_CLASS:
    switch ( *argument )
    {
    case 'y': case 'Y':
    argument[0] = '\0';
    do_help( ch, "MULTICLASS" );
    sprintf( buf, "Do you wish to be multi-classed? " );
    write_to_buffer( d, buf, 0 );
    d->connected = CON_WANT_MULTI;
    break;

    case 'n': case 'N':
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
    iCount = 0;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
	if ( class_table[iClass].races[ch->race] )
	  {
	  iCount++;
	  sprintf( buf, "[%-12s]%s", 
		class_table[iClass].who_long,
		iCount % 5 == 0 ? "\n\r" : "" );
	  strcat( buf2, buf );
	  }
    }
    if ( iCount % 5 != 0 )
      strcat( buf2, "\n\r" );
    strcat( buf2, "\n\r1ST CLASS -> " );
    write_to_buffer( d, buf2, 0 );
    d->connected = CON_GET_NEW_CLASS;
    }
    break;
    case CON_WANT_MULTI:
	switch ( *argument )
	  {
	  case 'y': case 'Y':
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
	    iCount = 0;
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	      {
	      if ( class_table[iClass].races[ch->race] 
	      && prime_class( ch ) != iClass
	      && class_table[prime_class(ch)].multi[iClass] )
		{
		iCount++;
		sprintf( buf, "[%-12s]%s", 
			class_table[iClass].who_long,
			iCount % 5 == 0 ? "\n\r" : "" );
		strcat( buf2, buf );
		}
	      }
	    if ( iCount % 5 != 0 )
	      strcat( buf2, "\n\r" );
            strcat( buf2, "\n\r2ND CLASS -> " );
	    write_to_buffer( d, buf2, 0 );
	    d->connected = CON_GET_2ND_CLASS;
	    break;
	  case 'n': case 'N':
            ch->class[1] = -1;

            if (AUTHORIZE_LOGINS == TRUE) 
            {
               d->connected = CON_AUTHORIZE_NAME;
               sprintf(buf, "%s@%s needs to be AUTHORIZED!", 
	              ch->name, d->host);
               wiznet( buf, d->character, NULL, WIZ_LOGINS, 0, 0 );
               write_to_buffer(d, 
	         "Now, please wait for an immortal to authorize you.\n\r", 0);
            } 
            else
            {
	       write_to_buffer(d,"\n\r",2);
               do_help( ch, "motd" );
 	       d->connected = CON_READ_MOTD;
            }
	    break;
	  default:
            write_to_buffer( d, "Please type Yes or No: ", 0 );
            break;
	   }
	break;

    case CON_GET_2ND_CLASS:
    if ( !str_cmp( argument, "back" ) )
	{
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
	iCount = 0;
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	  {
	  if ( class_table[iClass].races[ch->race] )
	    {
	    iCount++;
	    sprintf( buf, "[%-12s]%s", 
		class_table[iClass].who_long,
		iCount % 5 == 0 ? "\n\r" : "" );
	    strcat( buf2, buf );
	    }
	  }
	if ( iCount % 5 != 0 )
	  strcat( buf2, "\n\r" );
        strcat( buf2, "\n\r1ST CLASS -> " );
	write_to_buffer( d, buf2, 0 );
        d->connected = CON_GET_NEW_CLASS;
	return;
	}
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        if ( !str_prefix( argument, class_table[iClass].who_long )
        && class_table[iClass].races[ch->race]
	&& prime_class( ch ) != iClass
        && class_table[prime_class(ch)].multi[iClass] )
        {
        ch->class[1] = iClass;
        break;
        }
    }

    if ( iClass == MAX_CLASS )
    {
        write_to_buffer( d,
        "That's not a class.\n\rWhat IS your class? ", 0 );
        return;
    }

    do_help( ch, (char *)class_table[iClass].who_long );
    write_to_buffer( d, "Is this the class you desire? ", 0 );
    d->connected = CON_CONFIRM_2ND_CLASS;
        break;

    case CON_CONFIRM_2ND_CLASS:
    switch ( *argument )
    {
    case 'y': case 'Y':
      ch->class[2] = -1;

            if (AUTHORIZE_LOGINS == TRUE) 
            {
               d->connected = CON_AUTHORIZE_NAME;
               sprintf(buf, "%s@%s needs to be AUTHORIZED!", 
	              ch->name, d->host);
               wiznet( buf, d->character, NULL, WIZ_LOGINS, 0, 0 );
               write_to_buffer(d, 
	         "Now, please wait for an immortal to authorize you.\n\r", 0);
            } 
            else
            {
	       write_to_buffer(d,"\n\r",2);
               do_help( ch, "motd" );
 	       d->connected = CON_READ_MOTD;
            }

/* For Third Class comment above and uncomment below */
/*
    argument[0] = '\0';
    sprintf( buf, "Do you wish a 3rd class? " );
    write_to_buffer( d, buf, 0 );
    d->connected = CON_WANT_MULTI_2;
*/
    break;

    case 'n': case 'N':
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
	    iCount = 0;
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	      {
	      if ( class_table[iClass].races[ch->race] 
	      && prime_class( ch ) != iClass		
	      && class_table[prime_class(ch)].multi[iClass] )
	    	{
		iCount++;
		sprintf( buf, "[%-12s]%s", 
			class_table[iClass].who_long,
			iCount % 5 == 0 ? "\n\r" : "" );
		strcat( buf2, buf );
		}
	      }
	    if ( iCount % 5 != 0 )
	      strcat( buf2, "\n\r" );
	    strcat( buf2, "\n\r2ND CLASS -> " );
	    write_to_buffer( d, buf2, 0 );
    d->connected = CON_GET_2ND_CLASS;
    }
    break;
   
    case CON_WANT_MULTI_2:
	switch ( *argument )
	  {
	  case 'y': case 'Y':
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
	    iCount = 0;
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	      {
	      if ( class_table[iClass].races[ch->race] 
		&& prime_class( ch ) != iClass
		&& ch->class[1] != iClass
	        && class_table[prime_class(ch)].multi[iClass]
		&& class_table[ch->class[1]].multi[iClass] )
		{
		iCount++;
		sprintf( buf, "[%-12s]%s", 
			class_table[iClass].who_long,
			iCount % 5 == 0 ? "\n\r" : "" );
		strcat( buf2, buf );
		}
	      }
	    if ( iCount % 5 != 0 )
	      strcat( buf2, "\n\r" );
            strcat( buf2, "\n\r3RD CLASS -> " );
	    write_to_buffer( d, buf2, 0 );
	    d->connected = CON_GET_3RD_CLASS;
	    break;
	  case 'n': case 'N':
            ch->class[2] = -1;
            if (AUTHORIZE_LOGINS == TRUE) 
            {
               d->connected = CON_AUTHORIZE_NAME;
               sprintf(buf, "%s@%s needs to be AUTHORIZED!", 
	              ch->name, d->host);
               wiznet( buf, d->character, NULL, WIZ_LOGINS, 0, 0 );
               write_to_buffer(d, 
	         "Now, please wait for an immortal to authorize you.\n\r", 0);
            } 
            else
            {
	       write_to_buffer(d,"\n\r",2);
               do_help( ch, "motd" );
 	       d->connected = CON_READ_MOTD;
            }
	    break;
	  default:
            write_to_buffer( d, "Please type Yes or No: ", 0 );
            break;
	  }
	break;
    case CON_GET_3RD_CLASS:
    if ( !str_cmp( argument, "back" ) )
	{
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
	iCount = 0;
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	  {
	  if ( class_table[iClass].races[ch->race] 
	  && prime_class( ch ) != iClass
          && class_table[prime_class(ch)].multi[iClass] )
	    {
	    iCount++;
	    sprintf( buf, "[%-12s]%s", 
			class_table[iClass].who_long,
			iCount % 5 == 0 ? "\n\r" : "" );
	    strcat( buf2, buf );
	    }
	}
	if ( iCount % 5 != 0 )
	   strcat( buf2, "\n\r" );
	strcat( buf2, "\n\r2ND CLASS -> " );
	write_to_buffer( d, buf2, 0 );
        d->connected = CON_GET_2ND_CLASS;
	return;
	}
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
    {
        if ( !str_prefix( argument, class_table[iClass].who_long )
        && class_table[iClass].races[ch->race]
	&& prime_class( ch ) != iClass 
	&& ch->class[1] != iClass
        && class_table[prime_class(ch)].multi[iClass]
	&& class_table[ch->class[1]].multi[iClass] )
        {
        ch->class[2] = iClass;
        break;
        }
    }

    if ( iClass == MAX_CLASS )
    {
        write_to_buffer( d,
        "That's not a class.\n\rWhat IS your class? ", 0 );
        return;
    }

    do_help( ch, (char *)class_table[iClass].who_long );
    write_to_buffer( d, "Is this the class you desire? ", 0 );
    d->connected = CON_CONFIRM_3RD_CLASS;
        break;

    case CON_CONFIRM_3RD_CLASS:
    switch ( *argument )
    {
    case 'y': case 'Y':
       ch->class[3] = -1;
            if (AUTHORIZE_LOGINS == TRUE) 
            {
               d->connected = CON_AUTHORIZE_NAME;
               sprintf(buf, "%s@%s needs to be AUTHORIZED!", 
	              ch->name, d->host);
               wiznet( buf, d->character, NULL, WIZ_LOGINS, 0, 0 );
               write_to_buffer(d, 
	         "Now, please wait for an immortal to authorize you.\n\r", 0);
            } 
            else
            {
	       write_to_buffer(d,"\n\r",2);
               do_help( ch, "motd" );
 	       d->connected = CON_READ_MOTD;
            }
    break;

    case 'n': case 'N':
    strcpy( buf2, 
"\n\r                 -= Select a class from the list below =-"
"\n\r           You may type BACK to return to previous selection.\n\r\n\r"
);
	    iCount = 0;
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	      {
	      if ( class_table[iClass].races[ch->race] 
		&& prime_class( ch ) != iClass
		&& ch->class[1] != iClass
	        && class_table[prime_class(ch)].multi[iClass]
		&& class_table[ch->class[1]].multi[iClass] )
		{
		iCount++;
		sprintf( buf, "[%-12s]%s", 
			class_table[iClass].who_long,
			iCount % 5 == 0 ? "\n\r" : "" );
		strcat( buf2, buf );
		}
	      }
	    if ( iCount % 5 != 0 )
	      strcat( buf2, "\n\r" );
            strcat( buf2, "\n\r3RD CLASS -> " );
	    write_to_buffer( d, buf2, 0 );
    d->connected = CON_GET_3RD_CLASS;
    break;

    default:
        write_to_buffer( d, "Please type Yes or No: ", 0 );
        break;
    }
    break;

    case CON_AUTHORIZE_NAME:
    case CON_AUTHORIZE_NAME1:
    case CON_AUTHORIZE_NAME2:
    case CON_AUTHORIZE_NAME3:
        write_to_buffer(d,"Please wait for an Immortal to authorize you.", 0);
        write_to_buffer(d, "\n\r", 0);
        sprintf(log_buf, "%s@%s needs to be AUTHORIZED!", ch->name, d->host ); 
        d->connected++;
           
        if(d->connected == CON_AUTHORIZE_LOGOUT)
        {
            write_to_buffer( d, "Auto exit to prevent spam.\n\r", 0 );
            sprintf(log_buf, "%s@%s auto logged off.", ch->name, d->host );
            close_socket( d );
            return;
        }
     	break;


    case CON_READ_MOTD:
    ch->next    = char_list;
    char_list   = ch;
    d->connected    = CON_PLAYING;

/*    send_to_char(
    "\n\rWelcome to The Eye of the Storm."
    "  May your visit here be tempest tossed.\n\r",
        ch ); */

    if ( ch->level == 0 )
    {
        RACE_DATA *pRace;
        switch ( class_table[prime_class( ch )].attr_prime )
        {
        case APPLY_STR: ch->pcdata->perm_str = 16; break;
        case APPLY_INT: ch->pcdata->perm_int = 16; break;
        case APPLY_WIS: ch->pcdata->perm_wis = 16; break;
        case APPLY_DEX: ch->pcdata->perm_dex = 16; break;
        case APPLY_CON: ch->pcdata->perm_con = 16; break;
        }

        pRace = get_race_data(ch->race);
        ch->pcdata->mod_str += pRace->mstr;
        ch->pcdata->mod_int += pRace->mint;
        ch->pcdata->mod_wis += pRace->mwis;
        ch->pcdata->mod_dex += pRace->mdex;
        ch->pcdata->mod_con += pRace->mcon;
	SET_BIT( ch->act, PLR_AUTOEXIT + PLR_AUTOCOINS );
	ch->level   = 1;
        ch->exp = number_classes( ch ) == 1 ? 1000
		: number_classes( ch ) * 1500;
        ch->hit     = MAX_HIT(ch);
        ch->mana    = MAX_MOVE(ch);
        ch->bp      = MAX_BP(ch);
        ch->move    = MAX_MOVE(ch);
/* charisma */
	if ( ( ch->race == RACE_GHOUL ) ||
	     ( ch->race == RACE_OGRE )  ||
	     ( ch->race == RACE_DEMON ) ||
	     ( ch->race == RACE_TROLL ) )
	  ch->charisma = number_range( 10, 14 );
	if ( ( ch->race == RACE_SHADOW )    ||
	     ( ch->race == RACE_MINOTAUR )  ||
	     ( ch->race == RACE_LIZARDMAN ) ||
	     ( ch->race == RACE_ILLITHID ) )
	  ch->charisma = number_range( 15, 19 );
	if ( ( ch->race == RACE_HUMAN ) ||
	     ( ch->race == RACE_DROW )  ||
	     ( ch->race == RACE_DWARF ) ||
	     ( ch->race == RACE_ELDER ) )
	  ch->charisma = number_range( 20, 24 );
	if ( ( ch->race == RACE_PIXIE )  ||
	     ( ch->race == RACE_ELF )    ||
	     ( ch->race == RACE_TABAXI ) ||
	     ( ch->race == RACE_HALFLING ) )
	  ch->charisma = number_range( 25, 30 );

	obj_to_char(create_object(get_obj_index(LOCKER_VNUM),0),ch);

        sprintf( buf, "the %s %s", 
            pRace->race_full,
	    class_table[prime_class(ch)].who_long );
        set_title( ch, buf );
        if ( !is_class( ch, CLASS_VAMPIRE ) )
           ch->prompt = str_dup( daPrompt );
        else
           ch->prompt = str_dup( "{G<{m%h{ghp {r%mbp {m%v{gmv{G>{x " );
        if ( ch->start_align == 'E' )
          char_to_room( ch, get_room_index( ROOM_VNUM_RW_SCHOOL ) );
        else
          char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
    }
    else if ( ch->in_room )
    {
        char_to_room( ch, ch->in_room );
    }
    else if ( IS_IMMORTAL( ch ) )
    {
        char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
    }
    else
    {
        char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
    }

    /*
     * New code to see where you last logged in from 
     * Thanks to Vorlin - Added 3/13/01 by Beowolf
     */
    if (ch->pcdata->host == NULL
	|| sizeof(ch->pcdata->host) == 0
	|| !str_cmp(ch->pcdata->host, "none")) {
	    sprintf(buf, "{RUpdating site login info:{Y %s{x\n\r", d->host);
	    send_to_char(buf, ch);
	} else if (!str_cmp(ch->pcdata->host, d->host)) {
	    sprintf(buf, "{CUsual login: %s{x\n\r\n\r", ch->pcdata->host);
	    send_to_char(buf, ch);
	} else {
	    sprintf(buf, "{RLast login: {C%s{x\n\r", ch->pcdata->host);
	    send_to_char(buf, ch);
	    sprintf(buf, "{CCurrent login: %s{x\n\r\n\r", d->host);
	    send_to_char(buf, ch);
	}

       /* Update with str_dup, not = ! --Vorlin */
	ch->pcdata->host = str_dup(d->host);


    if ( !IS_SET( ch->act, PLR_WIZINVIS )
        && !IS_AFFECTED( ch, AFF_INVISIBLE ) )
        act( "$n has returned to the storm.", ch, NULL, NULL, TO_ROOM);
        else
        {
           CHAR_DATA *gch;
           for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
           {
               if ( ch != gch )
               if ( get_trust(gch) >= ch->wizinvis )
               {
                  sprintf(buf,"%s slightly phased has returned to the storm.\n\r",ch->name);
                  send_to_char (  buf, gch );
               }
           }
        }


    if (!IS_IMMORTAL( ch ) )
        info( "%s has returned to the storm.", (int)(ch->name), 0 );


    if ( ch->pcdata->corpses >= 2 )
        {
        char strfng [ MAX_INPUT_LENGTH ];
        ch->pcdata->corpses = 0;
    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strfng, "%s%c/%s.cps", PLAYER_DIR, LOWER( ch->name[0] ),
         capitalize( ch->name ) );
#else
   sprintf( strfng, "%s%s.cps", PLAYER_DIR, capitalize( ch->name );
#endif
        if ( remove( strfng ) != 0 )
        perror( strfng );
        }
    sprintf(log_buf,"%s!%s@%s has connected.", ch->name, d->user,
d->host);
    log_string(log_buf, (ch->level == L_IMP ? 1 :
                CHANNEL_LOG), ch->level - 1 );

    if ( !IS_NPC( ch ) && ch->pcdata->storage )
    {
      OBJ_DATA *obj;
      OBJ_DATA *obj_next;
      int count = 1;
      char buf[MAX_STRING_LENGTH];

      for ( obj = ch->pcdata->storage; obj; obj = obj_next, count++ )
      {
        obj_next = obj->next_content;
#ifdef NEW_MONEY
	if ( (ch->pcdata->bankaccount.gold +
	     (ch->pcdata->bankaccount.silver/S_PER_G) +
	     (ch->pcdata->bankaccount.copper/C_PER_G) ) < count * 1000 )
#else
        if ( ch->pcdata->bankaccount < count * 1000 )
#endif
        {
          sprintf( buf,
              "The bank has repossessed %s from your storage.\n\r",
              obj->short_descr );
          send_to_char(  buf, ch );
          obj_from_storage( obj );
          extract_obj( obj );
        }
      }

      sprintf( buf,
    "The bank deducts %dgp from your account for the storage of %d items.\n\r",
          ch->pcdata->storcount * 1000, ch->pcdata->storcount );
      send_to_char(  buf, ch );
#ifdef NEW_MONEY
      amt.silver = amt.copper = 0;
      amt.gold = ch->pcdata->storcount * 1000;
      spend_money( &ch->pcdata->bankaccount, &amt );
#else
      ch->pcdata->bankaccount -= ch->pcdata->storcount * 1000;
#endif
    }

    do_look( ch, "auto" );
    do_board (ch, ""); /* Show board status */

    /* Here to make sure ch isn't switched --Angi */
    ch->pcdata->switched = FALSE;

    break;

        case CON_NOTE_TO:
                handle_con_note_to (d, argument);
                break;

        case CON_NOTE_SUBJECT:
                handle_con_note_subject (d, argument);
                break; /* subject */

        case CON_NOTE_EXPIRE:
                handle_con_note_expire (d, argument);
                break;

        case CON_NOTE_TEXT:
                handle_con_note_text (d, argument);
                break;

        case CON_NOTE_FINISH:
                handle_con_note_finish (d, argument);
                break;
    }
    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    char 	buf[ MAX_STRING_LENGTH*4 ];

    /*
     * Reserved words.	OLC 1.1b
     */
    if ( is_exact_name( name, "all auto immortal self someone none" ) )
	return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen( name ) <  3 )
	return FALSE;

    /* Check for bad names - Jubelo */
    sprintf(buf, "grep -ix %s bad.words > /dev/null",name);

    if (0==system(buf))   
        return FALSE;

#if defined( MSDOS )
    if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined( macintosh ) || defined( unix )
    if ( strlen( name ) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha( *pc ) )
		return FALSE;
	    if ( LOWER( *pc ) != 'i' && LOWER( *pc ) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    if (!strcmp(name,"Hunter")) return TRUE;
    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash [ MAX_KEY_HASH ];
	       MOB_INDEX_DATA *pMobIndex;
	       int             iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_exact_name( name, pMobIndex->player_name ) )
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
    OBJ_DATA  *obj;
    CHAR_DATA *ch;

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( !IS_NPC( ch ) && !ch->deleted
	    && ( !fConn || !ch->desc )
	    && !str_cmp( d->character->name, ch->name ) )
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
		ch->desc     = d;
		ch->timer    = 0;
		send_to_char( "Reconnecting.\n\r", ch );
		act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s!%s@%s reconnected.", ch->name, d->user, d->host );
                wiznet( log_buf,ch,NULL,WIZ_LINKS,0,0);
		if ( !IS_SET( ch->act, PLR_WIZINVIS ) 
                      && !IS_SET( ch->act, PLR_CLOAKED ) )
		    info( "%s has re-established a link to the storm.",
		    (int)(ch->name), 0 );
		if ( IS_AFFECTED2( ch, AFF_PLOADED ) )
		    REMOVE_BIT( ch->affected_by2, AFF_PLOADED );
		d->connected = CON_PLAYING;

               /* Inform the character of a note in progress 
                * and the possbility of continuation!
                */
               if (ch->pcdata->in_progress)
                  stc("You have a note in progress. Type NWRITE to continue.\n\r",ch);

		/*
		 * Contributed by Gene Choi
		 */
		if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
		    && obj->item_type == ITEM_LIGHT
		    && obj->value[2] != 0
		    && ch->in_room )
		    ++ch->in_room->light;
 	   
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
	    && dold->character
	    && dold->connected != CON_GET_NAME
	    && dold->connected != CON_GET_OLD_PASSWORD
	    && !str_cmp( name, dold->original
			? dold->original->name : dold->character->name ) )
	{
	    if ( dold->connected == CON_PLAYING )
	    {
	      free_char(d->character);
	      d->character = (dold->original ? dold->original :
	                dold->character );
	      if ( dold->original )
	        dold->original->desc = NULL;
	      dold->character->desc = NULL;
	      d->character->desc = d;
	      dold->original = NULL;
	      dold->character = NULL;
	      write_to_buffer(dold, "Kicking off old link.\n\r", 0);
	      close_socket(dold);
              
	      if ( !IS_SET( d->character->act, PLR_WIZINVIS ) )
   	         act(  "A ghostly aura briefly embodies $n.",
	           d->character, NULL, NULL, TO_ROOM );
	      send_to_char(  "You arise from netdeath and continue"
	                    " playing.\n\r", d->character );
	      sprintf(log_buf, "%s connects, kicking off old link.",
	              d->character->name);
	      log_string(log_buf, CHANNEL_LOG, d->character->level);
	      wiznet(log_buf, NULL, NULL, WIZ_LINKS, 0, 0);
	      if ( !IS_SET( d->character->act, PLR_WIZINVIS ) 
                    && !IS_SET( d->character->act, PLR_CLOAKED) )
		      info(log_buf, 0, 0);
	      d->connected = CON_PLAYING;
	      return TRUE;
	    }
	    write_to_buffer( d, "Already playing.\n\r Name: ", 0 );
	    d->connected = CON_GET_NAME;
	    if ( d->character )
	    {
		free_char( d->character );
		d->character = NULL;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if (   !ch
	|| !ch->desc
	||  ch->desc->connected != CON_PLAYING
	|| !ch->was_in_room
	||  ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}



/*
 * Write to all in the room.
 */
void send_to_room( const char *txt, ROOM_INDEX_DATA *room )
{
    DESCRIPTOR_DATA *d;
    
    for ( d = descriptor_list; d; d = d->next )
        if ( d->character != NULL )
	    if ( d->character->in_room == room )
	        act( txt, d->character, NULL, NULL, TO_CHAR );
}

/*
 * Write to all characters.
 */
void send_to_all_char( const char *text )
{
    DESCRIPTOR_DATA *d;

    if ( !text )
        return;
    for ( d = descriptor_list; d; d = d->next )
        if ( d->connected == CON_PLAYING )
	    send_to_char( text, d->character );

    return;
}

/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( !txt || !ch->desc )
        return;

    free_string( ch->desc->showstr_head );
    ch->desc->showstr_head  = str_dup( txt );
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string( ch->desc, "" );

    return;
}


/*
 * Send to one char, new colour version, by Lope.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char 	*point;
    		char 	*point2;
    		char 	buf[ MAX_STRING_LENGTH*4 ];
		int	skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if ( txt && ch->desc )
	{
	    if ( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			skip = colour( *point, ch, point2 );
			while( skip-- > 0 )
			    ++point2;
			continue;
		    }

		    *point2 = *point;		    *++point2 = '\0';
		}			
		*point2 = '\0';
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			if( *point == '{' )
			{
			    *point2 = *point;
			    *++point2 = '\0';
			}
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	}
    return;
}



 /* The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom
    for porting this SillyMud code for MERC 2.0 and laying down the groundwork.
    Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which
    the improvements to the pager was modeled from.  - Kahn */
 /* 12/1/94 Fixed bounds and overflow bugs in pager thanks to BoneCrusher
    of EnvyMud Staff - Kahn */

void show_string( struct descriptor_data *d, char *input )
{
    register char *scan;
             char  buffer[ MAX_STRING_LENGTH*6 ];
             char  buf   [ MAX_INPUT_LENGTH    ];
             int   line      = 0;
             int   toggle    = 0;
             int   pagelines = 20;

    one_argument( input, buf );

    switch( UPPER( buf[0] ) )
    {
    case '\0':
    case 'C': /* show next page of text */
	break;

    case 'R': /* refresh current page of text */
	toggle = 1;
	break;

    case 'B': /* scroll back a page of text */
	toggle = 2;
	break;

    default: /*otherwise, stop the text viewing */
	if ( d->showstr_head )
	{
	    free_string( d->showstr_head );
	    d->showstr_head = str_dup( "" );
	}
	d->showstr_point = 0;
	return;

    }

    if ( d->original )
        pagelines = d->original->pcdata->pagelen;
    else
        pagelines = d->character->pcdata->pagelen;

    if ( toggle )
    {
	if ( d->showstr_point == d->showstr_head )
	    return;
	if ( toggle == 1 )
	    line = -1;
	do
	{
	    if ( *d->showstr_point == '\n' )
	      if ( ( line++ ) == ( pagelines * toggle ) )
		break;
	    d->showstr_point--;
	} while( d->showstr_point != d->showstr_head );
    }
    
    line    = 0;
    *buffer = 0;
    scan    = buffer;
    if ( *d->showstr_point )
    {
	do
	{
	    *scan = *d->showstr_point;
	    if ( *scan == '\n' )
	      if ( ( line++ ) == pagelines )
		{
		  scan++;
		  break;
		}
	    scan++;
	    d->showstr_point++;
	    if( *d->showstr_point == 0 )
	      break;
	} while( 1 );
    }

    *scan = 0;

    write_to_buffer( d, buffer, strlen( buffer ) );
    if ( *d->showstr_point == 0 )
    {
      free_string( d->showstr_head );
      d->showstr_head  = str_dup( "" );
      d->showstr_point = 0;
    }

    return;
}

/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1,
	 const void *arg2, int type )
{
           OBJ_DATA        *obj1        = (OBJ_DATA  *) arg1;
	   OBJ_DATA        *obj2        = (OBJ_DATA  *) arg2;
	   CHAR_DATA       *to;
	   CHAR_DATA       *vch         = (CHAR_DATA *) arg2;
    static char *    const  he_she  [ ] = { "it",  "he",  "she" };
    static char *    const  him_her [ ] = { "it",  "him", "her" };
    static char *    const  his_her [ ] = { "its", "his", "her" };
    const  char            *str;
    const  char            *i;
           char            *point;
           char            *pbuff;
           char             buf     [ MAX_STRING_LENGTH ];
           char             buf1    [ MAX_STRING_LENGTH ];
           char             buffer  [ MAX_STRING_LENGTH*2 ];
           char             fname   [ MAX_INPUT_LENGTH  ];

    /*
     * Discard null and zero-length messages.
     */
    if ( !format || format[0] == '\0' )
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
	if ( !vch )
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    sprintf( buf1, "Bad act string:  %s", format );
	    bug( buf1, 0 );
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to; to = to->next_in_room )
    {
	if ( ( to->deleted )
	    || ( !to->desc && IS_NPC( to ) )
	    || !IS_AWAKE( to ) )
	    continue;

	if ( type == TO_CHAR    && to != ch )
	    continue;
	if ( type == TO_VICT    && ( to != vch || to == ch ) )
	    continue;
	if ( type == TO_ROOM    && to == ch )
	    continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
	    continue;

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( !arg2 && *str >= 'A' && *str <= 'Z' )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		sprintf( buf1, "Bad act string:  %s", format );
		bug( buf1, 0 );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
		          sprintf( buf1, "Bad act string:  %s", format );
		          bug( buf1, 0 );
			  i = " <@@@> ";				break;
		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;          			break;
		case 'n': i = PERS( ch,  to  );				break;
		case 'N': i = PERS( vch, to  );				break;
		case 'e': i = he_she  [URANGE( 0, ch  ->sex, 2 )];	break;
		case 'E': i = he_she  [URANGE( 0, vch ->sex, 2 )];	break;
		case 'm': i = him_her [URANGE( 0, ch  ->sex, 2 )];	break;
		case 'M': i = him_her [URANGE( 0, vch ->sex, 2 )];	break;
		case 's': i = his_her [URANGE( 0, ch  ->sex, 2 )];	break;
		case 'S': i = his_her [URANGE( 0, vch ->sex, 2 )];	break;

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
		    if ( !arg2 || ( (char *) arg2 )[0] == '\0' )
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

        *point++	= '\n';
        *point++	= '\r';
        *point		= '\0';
	buf[0]		= UPPER( buf[0] );
	pbuff		= buffer;
	colourconv( pbuff, buf, to );
        if (to->desc && (to->desc->connected == CON_PLAYING))
	    write_to_buffer( to->desc, buffer, 0 );
    }
    return;
}



/*
 * Macintosh support functions.
 */
#if defined( macintosh )
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

/*
 * Windows 95 and Windows NT support functions
 */
#if defined( WIN32 )
void gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
  }
#endif



/*
 * Macintosh support functions.
 */
#if defined( macintosh )
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif


void send_to_area( AREA_DATA *pArea, char *txt )
{
  DESCRIPTOR_DATA *d;
  
  for ( d = descriptor_list; d; d = d->next )
  {
    if ( d->connected != CON_PLAYING )
      continue;
    if ( d->character )
    {
    if ( !( d->character->in_room ) || ( d->character->deleted ) 
      || !IS_OUTSIDE( d->character ) || !IS_AWAKE( d->character ) )
      continue;
    if ( d->character->in_room->area == pArea )
      send_to_char(  txt, d->character );
    }
  }
  return;
}

bool check_ban( DESCRIPTOR_DATA *dnew, bool loggedin )
{
  BAN_DATA *pban;
  DESCRIPTOR_DATA *dremove;
  for ( pban = ban_list; pban; pban = pban->next )
  {
    if ( pban->type == 'N' )
	continue;

    if ( ( !str_suffix( pban->name, dnew->host ) && !pban->user )

        || ( pban->user && !str_cmp( pban->user, dnew->user ) 
            && !str_suffix( pban->name, dnew->host ) )
 )
    {
      if ( pban->user && !str_cmp( pban->user, dnew->user ) )
      if ( !str_suffix( pban->name, dnew->host ) )
      {
        write_to_descriptor( dnew->descriptor,
          "You have been banned from Eye of the Storm.\n\r", 0 );
      }
      if ( !str_suffix( pban->name, dnew->host ) && 
         !pban->user )
      {
        write_to_descriptor( dnew->descriptor,
          "Your site has been banned from Eye of the Storm.\n\r", 0 );
      }
      sprintf( log_buf, "%s!%s@%s being denied access.",
          dnew->character ? dnew->character->name : "(unknown)",
          dnew->user,
          dnew->host );
      bug( log_buf, 0 );
      close( dnew->descriptor );
      if ( loggedin )
      {
        for ( dremove = descriptor_list; dremove; dremove = dremove->next )
          if ( dremove->next == dnew ) break;
          
        dremove->next = dnew->next;
      }
      if ( dnew->character )
      {
        extract_char( dnew->character, TRUE );
      }
      free_string( dnew->host );
      free_string( dnew->user );
      free_string( dnew->showstr_head );
      free_mem( dnew->outbuf, dnew->outsize );
      free_mem( dnew, sizeof( *dnew ) );
      return TRUE;
    }
  }
  return FALSE;
}

bool check_newban( DESCRIPTOR_DATA *dnew, bool loggedin )
{
  BAN_DATA *pban;
  
  for ( pban = ban_list; pban; pban = pban->next )
  {
    if ( pban->type != 'N' )
	continue;
    if ( ( !str_suffix( pban->name, dnew->host ) && !pban->user )
        || ( pban->user && !str_cmp( pban->user, dnew->user ) 
            && !str_suffix( pban->name, dnew->host ) ) )
    {
      if ( pban->user && !str_cmp( pban->user, dnew->user ) )
      if ( !str_suffix( pban->name, dnew->host ) )
      {
        write_to_descriptor( dnew->descriptor,
"You have been banned from making new characters on Eye of the Storm.\n\r", 0 );
      }
      if ( !str_suffix( pban->name, dnew->host ) && 
         !pban->user )
      {
        write_to_descriptor( dnew->descriptor,
"Your site has been banned from making new characters on Eye of the Storm.\n\r", 0);
      }
      sprintf( log_buf, "%s!%s@%s being denied access.",
          dnew->character ? dnew->character->name : "(unknown)",
          dnew->user,
          dnew->host );
      bug( log_buf, 0 );
      return TRUE;
    }
  }
  return FALSE;
}


void reset_builder_levels ( )
{
  int cmd;

  for ( cmd = 0; cmd_table[cmd].name != '\0'; cmd++ )
  {
    if ( !str_prefix( "redit", cmd_table[cmd].name ) )
      cmd_table[cmd].level = L_JUN;
    else if ( !str_prefix( "medit", cmd_table[cmd].name ) )
      cmd_table[cmd].level = L_DEM;
    else if ( !str_prefix( "oedit", cmd_table[cmd].name ) )
      cmd_table[cmd].level = L_SEN;
    else if ( !str_prefix( "resets", cmd_table[cmd].name ) )
      cmd_table[cmd].level = L_SEN;
    else if ( !str_prefix( "oload", cmd_table[cmd].name ) )
      cmd_table[cmd].level = L_SEN;
  }
  return;
}


/* Color by Lope! */
int colour( char type, CHAR_DATA *ch, char *string )
{
    char	code[ 20 ];
    char	*p = '\0';

    if( IS_NPC( ch ) )
	return( 0 );

    switch( type )
    {
	default:
	    sprintf( code, CLEAR );
	    break;
	case 'x':
	    sprintf( code, CLEAR );
	    break;
	case 'b':
	    sprintf( code, C_BLUE );
	    break;
	case 'c':
	    sprintf( code, C_CYAN );
	    break;
	case 'g':
	    sprintf( code, C_GREEN );
	    break;
	case 'm':
	    sprintf( code, C_MAGENTA );
	    break;
	case 'r':
	    sprintf( code, C_RED );
	    break;
	case 'w':
	    sprintf( code, C_WHITE );
	    break;
	case 'y':
	    sprintf( code, C_YELLOW );
	    break;
	case 'B':
	    sprintf( code, C_B_BLUE );
	    break;
	case 'C':
	    sprintf( code, C_B_CYAN );
	    break;
	case 'G':
	    sprintf( code, C_B_GREEN );
	    break;
	case 'M':
	    sprintf( code, C_B_MAGENTA );
	    break;
	case 'R':
	    sprintf( code, C_B_RED );
	    break;
	case 'W':
	    sprintf( code, C_B_WHITE );
	    break;
	case 'Y':
	    sprintf( code, C_B_YELLOW );
	    break;
	case 'D':
	    sprintf( code, C_D_GREY );
	    break;
	case 'K':
	    sprintf( code, C_BLACK );
	    break;
	case '*':
	    sprintf( code, "%c", 007 );
	    break;
	case '/':
	    sprintf( code, "%c", 012 );
	    break;
	case '{':
	    sprintf( code, "%c", '{' );
	    break;
    }

    p = code;
    while( *p != '\0' )
    {
	*string = *p++;
	*++string = '\0';
    }

    return( strlen( code ) );
}

void colourconv( char *buffer, const char *txt , CHAR_DATA *ch )
{
    const	char	*point;
		int	skip = 0;

    if( ch->desc && txt )
    {
	if( IS_SET( ch->act, PLR_COLOUR ) )
	{
	    for( point = txt ; *point ; point++ )
	    {
		if( *point == '{' )
		{
		    point++;
		    skip = colour( *point, ch, buffer );
		    while( skip-- > 0 )
			++buffer;
		    continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	    }			
	    *buffer = '\0';
	}
	else
	{
	    for( point = txt ; *point ; point++ )
	    {
		if( *point == '{' )
		{
		    point++;
		    continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	    }
	    *buffer = '\0';
	}
     }
    return;
}


/* 
 * Combined the desc_check and desc_clean into
 * one command. No need to clutter up code and 
 * the wizhelp all at once.
 * Done on 3/6/2001 by Beowolf
 */
void do_ds( CHAR_DATA *ch, char *argument )
{
   char arg [MAX_INPUT_LENGTH];

   argument = one_argument(argument,arg);

   if (arg[0] == '\0' )
   {
      send_to_char("Syntax: ds check or ds clean\n\r",ch);
      return;
   }
   
   if (!str_prefix(arg,"check"))
   {
      int desc;
      struct stat bstat;
      char buf[256];
  
      for ( desc = 0; desc < 100; desc++ )
      {
         if ( fstat(desc, &bstat) < 0 )
            sprintf(buf, "%3d:  no   ", desc);
         else
            sprintf(buf, "%3d: yes   ", desc);

         send_to_char( buf, ch);

         if ( desc % 7 == 6 )
            send_to_char( "\n\r", ch);
      }
      if ( desc % 7 != 6 )
         send_to_char( "\n\r", ch);
      return;
   }

   if (!str_prefix(arg,"clean"))
   {
      DESCRIPTOR_DATA *d, *d_next;
      int number = 0;
    
      for ( d = descriptor_list; d != NULL; d = d_next ) 
      {
         d_next = d->next;
	
	 /*took out NULL forseen probs with hanging descriptors*/
	
         if( (d->connected == CON_GET_NAME) )
         {
            close_socket(d);
            number++;
         }
      }

      if (number > 0)
         send_to_char( "Descriptors cleaned!\n\r", ch );
      else
	 send_to_char( "Descriptors OK.\n\r", ch );
      return;
   }

   /* echo syntax */
   do_ds(ch,"");
}

 
/* 
 * (c) 1996 Erwin S. Andreasen <erwin@andreasen.org>
 * Those 3 functions are IMHO very handy: you can avoid having to create a
 * temporary buffer, sprintf() to it, then finally send_to_char.
 *
 * Example usage:
 * Instead:
 * {
 *       char buf[MSL];
 *       sprintf (buf, "Bla bla has %d and %s", num, string);
 *       send_to_char (buf, ch);
 * }
 *
 * just:
 * printf_to_char (ch, "Bla bla has %d and %s", num, string);
 *
 * Similarly, with bugf/logf:
 * logf ("%s got %d gold", ch->name, amount);
 * bugf ("Undefined %s at %d:%d", some_string, line, col);
 *
 * Added for Copyover on 3-10-2001 by Beowolf
 */
void logf (char * fmt, ...)
{
	char buf [2*MSL];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	
	log_string (buf, CHANNEL_NONE, -1);
}

/* source: EOD, by John Booth <???> */
void printf_to_char (CHAR_DATA *ch, char *fmt, ...)
{
        char buf [MAX_STRING_LENGTH];
        va_list args;
        va_start (args, fmt);
        vsprintf (buf, fmt, args);
        va_end (args);

        send_to_char (buf, ch);
}

void bugf (char * fmt, ...)
{
	char buf [MAX_STRING_LENGTH];
	va_list args;
	va_start (args, fmt);
	vsnprintf (buf, MSL, fmt, args);
	va_end (args);
	
	bug (buf, 0);
}


/* Whole inti_desc for Copyover stuff*/
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *new_descriptor(void)
{
    static DESCRIPTOR_DATA d_zero;
    DESCRIPTOR_DATA *d;

    if (descriptor_free == NULL)
	d = alloc_perm(sizeof(*d));
    else
    {
	d = descriptor_free;
	descriptor_free = descriptor_free->next;
    }
	
    *d = d_zero;
    d->connected	= CON_GET_NAME;
    d->showstr_head	= NULL;
    d->showstr_point = NULL;
    d->outsize	= 2000;
    d->outbuf	= alloc_mem( d->outsize );
    
    return d;
}


void free_descriptor(DESCRIPTOR_DATA *d)
{
    free_string( d->host );
    free_mem( d->outbuf, d->outsize );
    d->next = descriptor_free;
    descriptor_free = d;
}


/* 
 * Authorize Code - Beowolf
 * Ripped out of TSR. 
 * Added by Beowolf 3/22/01
 */
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
  for(d = descriptor_list;d;d = d->next)
  {
    if(!d->character) continue;
    if(!d->character->name) continue;
    if(mode == 0)
    {
      if(d->connected >= CON_AUTHORIZE_NAME
        && d->connected <= CON_AUTHORIZE_LOGOUT)
      {
        sprintf(buf, "  %s@%s\n\r", d->character->name, d->host);
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
        do_help(d->character, "motd" );
        sprintf(buf, "%s@%s AUTHORIZED by %s.",
         d->character->name, d->host, ch->name);
        log_string( buf, CHANNEL_NONE, -1 );
        wiznet( buf, ch, NULL, WIZ_LOGINS, 0, 0 );
        return;
      }
      else
      if(mode == 2)
      {
        send_to_char( "Character denied.\n\r", ch);
        write_to_buffer(d, "Please choose a more medieval name.\n\r", 0 );
        sprintf(buf, "%s@%s was DENIED (DOH!) by %s.",
         d->character->name, d->host, ch->name);
        log_string( buf, CHANNEL_NONE, -1 );
        wiznet( buf, ch, NULL, WIZ_LOGINS, 0, 0 );
        addbadname(ch,d->character->name);
        close_socket(d);
        return;
      }
      else
      {
        send_to_char( "Ok.\n\r", ch);
        sprintf(buf, "%s %s.\n\r", arg2, argument);
        write_to_buffer(d, buf, 0);
        sprintf(buf, "%s@%s was DENIED (DOH!) by %s.",
         d->character->name, d->host, ch->name);
        wiznet( buf, ch, NULL, WIZ_LOGINS, 0, 0 );
        close_socket(d);
        addbadname(ch,d->character->name);
        return;
      }
    }
  }
  if(mode)
    send_to_char( "No such unauthorized person.\n\r", ch);
  return;
}


/* 
 * Syntax: aset <on/off>
 * Command will display status of auth or turn it on/off
 * Created on 2/15/2000 by Beowolf
 */
void do_aset( CHAR_DATA *ch, char *argument )
{
   char arg [MAX_INPUT_LENGTH];

   argument = one_argument(argument,arg);

   if (arg[0] == '\0')
   {
      send_to_char("Syntax: aset <on/off/show>\n\r",ch);
      return;
   }
   
   if (!str_prefix(arg,"on"))
   {
      AUTHORIZE_LOGINS = TRUE;
      send_to_char("Authorization Has been Turned On.\n\r", ch);
      return;
   }

   if (!str_prefix(arg,"off"))
   {
      AUTHORIZE_LOGINS = FALSE;
      send_to_char("Authorization Has been Turned OFF!\n\r", ch);
      return;
   }

   if (!str_prefix(arg,"show"))
   {
      if ( AUTHORIZE_LOGINS == FALSE ) 
         send_to_char("The current state of Authorize: {ROFF{x.\n\r",ch);
      else
         send_to_char("The current state of Authorize: {GON{x.\n\r",ch);
      return;
   }

   /* echo syntax */
   do_aset(ch,"");
}
