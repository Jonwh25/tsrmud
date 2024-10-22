From erwin@andreasen.org Thu Jul 25 16:20:14 1996
Date: Thu, 25 Jul 1996 16:20:12 +0200 (MET DST)
From: erwin@andreasen.org
X-Sender: erwin@shire.shire.dk
To: MERC/Envy Mailing List <merc-l@webnexus.com>
Subject: Re: Lagout bugs (Merc 2.2 +)
In-Reply-To: <9607241909.AA15457@quix.robins.af.mil>
Message-ID: <Pine.LNX.3.91.960725141310.158A-100000@shire.shire.dk>
MIME-Version: 1.0
Content-Type: TEXT/PLAIN; charset=US-ASCII
Status: O
X-Status: 

On Wed, 24 Jul 1996, Mr. Jolt Cola wrote:

> >      Also, I was wondering if anyone has written a "polling" process for
> > linux that determines if a mud is locked up, and kills the process.  Any
> > ideas along these lines would be appreciated :)
> 
> You don't need another process. Use what the kernel provides, an alarm.
> You can do something like this.
> 
> (Pseduoish code)

Worked great :)

> If you do this you have to beef up your system calls.
> You need to check things like select() to see if errno returns EINTR
> because SIGALRM can arrive at any time.

Here is what I have done:

I started with aborting after X real time seconds have passed. Later, I 
wanted to add something so that the server would abort if something 
hanged while loading the area etc. files. However, using real seconds was 
a bit risky there - what if there was a big load on the machine? Setting 
the threshold high would work, but that would also mean that it would 
take longer to abort if there was no loads.

So, instead I measure user CPU time used by the process, and abort if a 
certain number of user CPU seconds have passed without checkpointing.

While booting the database, this threshold value is set to something 
higher than usually.

While testing, it took loading of 10,000 mobs with a single command (FOR) 
before the server aborted. This was on a 486DX4-100.

To avoid having to rewrite those system calls to look for EINTR (though I 
have already done so, so that profiling would work correctly) I use the 
SA_RESTART flag.

I don't know about the compatbility of this code; according to Steven's 
book, sigaction is POSIX.1 defined, so this should work under those 
systems. The below code was tested on a Linux 2.0.8 machine.

I placed all my code in update.c. Before calling boot_db() in comm.c, 
call init_alarm_handler(). Also, add alarm_update() to one of the loops. 
I'm not sure how expensive a getrusage() call is - you might want to call 
alarm_update() less frequently, and increase the RUNNING_ABORT_THRESHOLD. 
Experiment :)

To find a good BOOT_DB_THRESHOLD, alarm_update() prints the number of CPU 
seconds the MUD has used so far (but only the first time it is called 
after a reboot); note that this value will not be correct if you exec() 
the MUD using e.g. the copyover/hot reboot code, as those values are not 
cleared when exec'ing. You might want to set it higher - it all depends 
on the speed of your machine.

If you have some command that you KNOW takes a lot of CPU time, and you 
don't want it aborted, set the global variable disable_timer_abort to 
TRUE before starting, and set it to FALSE when finished. Things that 
might require that are something like an ASAVE WORLD in OLC.

Oh, and I assume that getrusage *is* reentrant. It's not listen in 
Steven's book, in the table in 10.6 Reentrant functions. I don't know 
though if this is a system call, if this data is located in user space? 
Later I found out that the times function was one I could use better, but 
then it was too late :)



#define BOOT_DB_ABORT_THRESHOLD			  15
#define RUNNING_ABORT_THRESHOLD			   5
#define ALARM_FREQUENCY                           20


/* Include files necessary */

#include <sys/resource.h>
#include <signal.h>

/* Interval in pulses after which to abort */
int	abort_threshold = BOOT_DB_ABORT_THRESHOLD;
bool	disable_timer_abort = FALSE;
int	last_checkpoint;


/* find number of CPU seconds spent in user mode so far */
int get_user_seconds ()
{
	struct rusage rus;
	getrusage (RUSAGE_SELF, &rus);
	return rus.ru_utime.tv_sec;
}

/* Update the checkpoint */
void alarm_update ()
{
	last_checkpoint = get_user_seconds();
	if (abort_threshold == BOOT_DB_ABORT_THRESHOLD)
	{
		abort_threshold = RUNNING_ABORT_THRESHOLD;
		fprintf (stderr, "Used %d user CPU seconds.\n", last_checkpoint);
	}
}

/* Set the virtual (CPU time) timer to the standard setting, ALARM_FREQUENCY */
void reset_itimer ()
{
	struct itimerval itimer;
	itimer.it_interval.tv_usec = 0; /* miliseconds */
	itimer.it_interval.tv_sec  = ALARM_FREQUENCY;
	itimer.it_value.tv_usec = 0;
	itimer.it_value.tv_sec = ALARM_FREQUENCY;

	/* start the timer - in that many CPU seconds, alarm_handler will be called */	
	if (setitimer (ITIMER_VIRTUAL, &itimer, NULL) < 0)
	{
		perror ("reset_itimer:setitimer");
		exit (1);
	}
}

const char * szFrozenMessage = "Alarm_handler: Not checkpointed recently, aborting!\n";

/* Signal handler for alarm - suggested for use in MUDs by Fusion */
void alarm_handler (int signo)
{
	int usage_now = get_user_seconds();

	/* Has there gone abort_threshold CPU seconds without alarm_update? */
    if (!disable_timer_abort && (usage_now - last_checkpoint > abort_threshold ))
	{
		/* For the log file */
		write(STDERR_FILENO, szFrozenMessage, strlen(szFrozenMessage+1));

        signal (signo, SIG_DFL);
        abort();
	}
	
	/* The timer resets to the values specified in it_interval automatically */
}

/* Install signal alarm handler */
void init_alarm_handler()
{
	struct sigaction sa;
	
	sa.sa_handler = alarm_handler;
	sa.sa_flags = SA_RESTART; /* Restart interrupted system calls */
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGVTALRM, &sa, NULL) < 0) /* setup handler for virtual timer */
	{
		perror ("init_alarm_handler:sigaction");
		exit (1);
	}
	last_checkpoint = get_user_seconds();
	reset_itimer(); /* start timer */
}
