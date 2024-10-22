#ifndef _ANSI_
#define _ANSI_
/* ANSI control sequences, to be include */
#define NORMAL    "''"
#define ESCAPE      ""
#define ANSI_NORMAL "[0m"
#define ANSI_BOLD   "[1m"
#define ANSI_UNDERL "[4m"
#define ANSI_BLINK  "[5m"
#define ANSI_INVERS "[7m"

#define ANSI_BLACK  "[30m"
#define ANSI_RED    "[31m"
#define ANSI_GREEN  "[32m"
#define ANSI_YELLOW "[33m"
#define ANSI_BLUE   "[34m"
#define ANSI_PURPLE "[35m"
#define ANSI_CYAN   "[36m"
#define ANSI_WHITE  "[37m"
#define ANSI_BOLD_RED    "[1;31m"
#define ANSI_BOLD_GREEN  "[1;32m"
#define ANSI_BOLD_YELLOW "[1;33m"
#define ANSI_BOLD_BLUE   "[1;34m"
#define ANSI_BOLD_PURPLE "[1;35m"
#define ANSI_BOLD_CYAN   "[1;36m"
#define ANSI_BOLD_WHITE  "[1;37m"

#define ANSI_CLS    "[2J"
#define ANSI_HOME   "[1;1H"
#endif

char *color_table[]=
{
  "[0m[31m",/* 0x0 */ 	  /* normal red */
  "[0m[32m", /*0x1*/		  /* normal green */
  "[0m[33m", /*0x2*/		  /* normal yellow */
  "[0m[34m", /*0x3*/		  /* normal blue */
  "[0m[35m", /*0x4*/		  /* normal purple */
  "[0m[36m", /*0x5*/		  /* normal cyan */
  "[0m[37m", /*0x6*/  	  /* normal white (gray) */
  "[1m[30m", /*0x7*/		  /* bold black */
  "[1m[31m", /*0x8*/		  /* bold red */
  "[1m[32m", /*0x9*/		  /* bold green */
  "[1m[33m", /*0xa*/		  /* bold yellow */
  "[1m[34m", /*0xb*/	  	  /* bold blue */
  "[1m[35m", /*0xc*/		  /* bold purple (pink) */
  "[1m[36m", /*0xd*/		  /* bold cyan */
  "[1m[37m", /*0xe*/		  /* bold white */
  "[0m[37m", /*0x6*/		  /* normal white (gray) */
  0
  };
  