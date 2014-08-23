/* signals.c: signal handlers

   Copyright (c) 1989 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* This signal package was brought to you by		-JEW-  */
/* Completely rewritten by				-CJS- */

#include <stdio.h>

/* to get the SYS_V def if needed */
#include "config.h"

#if defined(SYS_V) && defined(lint)
/* for AIX, prevent hundreds of unnecessary lint errors, define before
   signal.h is included */
#define _h_IEEETRAP
typedef struct { int stuff; } fpvmach;
#endif

/* must include before externs.h, because that uses SIGTSTP */
#include <signal.h>

#include "constant.h"
#include "types.h"
#include "externs.h"

#ifndef USG
/* only needed for Berkeley UNIX */
#include <sys/types.h>
#include <sys/param.h>
#endif

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef USG
void exit();
#ifdef __TURBOC__
void sleep();
#else
unsigned sleep();
#endif
#endif

static int error_sig = -1;
static int signal_count = 0;

/*ARGSUSED*/
#ifndef USG
static int signal_handler(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  int smask;

  smask = sigsetmask(0) | (1 << sig);
#else
#ifdef __TURBOC__
static void signal_handler(sig)
#else
static void signal_handler(sig)
#endif
int sig;
{

#endif
  if(error_sig >= 0)	/* Ignore all second signals. */
    {
      if(++signal_count > 10)	/* Be safe. We will die if persistent enough. */
	(void) signal(sig, SIG_DFL);
      return;
    }
  error_sig = sig;

  /* Allow player to think twice. Wizard may force a core dump. */
  if (sig == SIGINT
      || sig == SIGQUIT
      )
    {
      if (death)
	(void) signal(sig, SIG_IGN);		/* Can't quit after death. */
      else if (!character_saved && character_generated)
	{
	  if ((!total_winner)?(!get_check("Really commit *Suicide*?"))
                          :(!get_check("Do you want to retire?")))
	    {
	      if (turn > 0)
		disturb(1, 0);
	      erase_line(0, 0);
	      put_qio();
	      error_sig = -1;
#ifdef USG
	      (void) signal(sig, signal_handler);/* Have to restore handler. */
#else
	      (void) sigsetmask(smask);
#endif
	      /* in case control-c typed during msg_print */
	      if (wait_for_more)
		put_buffer(" -more-", MSG_LINE, 0);
	      put_qio();
	      return;		/* OK. We don't quit. */
	    }
	  (void) strcpy(died_from, "Interrupting");
	}
      else
	(void) strcpy(died_from, "Abortion");
      prt("Interrupt!", 0, 0);
      death = TRUE;
      exit_game();
    }
  /* Die. */
  prt(
"OH NO!!!!!!  A gruesome software bug LEAPS out at you. There is NO defense!",
      23, 0);
  if (!death && !character_saved && character_generated)
    {
      panic_save = 1;
      prt("Your guardian angel is trying to save you.", 0, 0);
      (void) sprintf(died_from,"(panic save %d)",sig);
      if (!save_char())
	{
	  (void) strcpy(died_from, "software bug");
	  death = TRUE;
	  turn = -1;
	}
    }
  else
    {
      death = TRUE;
      (void) _save_char(savefile);	/* Quietly save the memory anyway. */
    }
  restore_term();
  /* always generate a core dump */
  (void) signal(sig, SIG_DFL);
  (void) kill(getpid(), sig);
  (void) sleep(5);
  exit(1);
}

#ifndef USG
static int mask;
#endif

void nosignals()
{
#ifdef SIGTSTP
  (void) signal(SIGTSTP, SIG_IGN);
#ifndef USG
  mask = sigsetmask(0);
#endif
#endif
  if (error_sig < 0)
    error_sig = 0;
}

void signals()
{
#ifdef SIGTSTP
  (void) signal(SIGTSTP, suspend);
#ifndef USG
  (void) sigsetmask(mask);
#endif
#endif
  if (error_sig == 0)
    error_sig = -1;
}


void init_signals()
{
  (void) signal(SIGINT, signal_handler);
  (void) signal(SIGFPE, signal_handler);
  /* Ignore HANGUP, and let the EOF code take care of this case. */
  (void) signal(SIGHUP, SIG_IGN);
  (void) signal(SIGQUIT, signal_handler);
  (void) signal(SIGILL, signal_handler);
  (void) signal(SIGTRAP, signal_handler);
  (void) signal(SIGIOT, signal_handler);
#ifdef SIGEMT  /* in BSD systems */
  (void) signal(SIGEMT, signal_handler);
#endif
#ifdef SIGDANGER /* in SYSV systems */
  (void) signal(SIGDANGER, signal_handler);
#endif
  (void) signal(SIGKILL, signal_handler);
  (void) signal(SIGBUS, signal_handler);
  (void) signal(SIGSEGV, signal_handler);
  (void) signal(SIGSYS, signal_handler);
  (void) signal(SIGTERM, signal_handler);
  (void) signal(SIGPIPE, signal_handler);
#ifdef SIGXCPU	/* BSD */
  (void) signal(SIGXCPU, signal_handler);
#endif
#ifdef SIGPWR /* SYSV */
  (void) signal(SIGPWR, signal_handler);
#endif
}

void ignore_signals()
{
  (void) signal(SIGINT, SIG_IGN);
#ifdef SIGQUIT
  (void) signal(SIGQUIT, SIG_IGN);
#endif
}

void default_signals()
{
  (void) signal(SIGINT, SIG_DFL);
#ifdef SIGQUIT
  (void) signal(SIGQUIT, SIG_DFL);
#endif
}

void restore_signals()
{
  (void) signal(SIGINT, signal_handler);
#ifdef SIGQUIT
  (void) signal(SIGQUIT, signal_handler);
#endif
}
