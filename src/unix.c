/* unix.c: UNIX dependent code.					-CJS-

   Copyright (c) 1989 James E. Wilson, Christopher J. Stuart

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef unix

/* defines NULL */
#include <stdio.h>
/* defines CTRL */
#include <sys/ioctl.h>
/* defines TRUE and FALSE */
#include <curses.h>
#include <pwd.h>
#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#include <signal.h>

#include <string.h>
#include <termio.h>
#include <fcntl.h>

#include <pwd.h>
#include <sys/errno.h>

struct passwd *getpwuid();
struct passwd *getpwnam();

/* Provides for a timeout on input. Does a non-blocking read, consuming the
   data if any, and then returns 1 if data was read, zero otherwise.

   Porting:

   In systems without the select call, but with a sleep for
   fractional numbers of seconds, one could sleep for the time
   and then check for input.

   In systems which can only sleep for whole number of seconds,
   you might sleep by writing a lot of nulls to the terminal, and
   waiting for them to drain, or you might hack a static
   accumulation of times to wait. When the accumulation reaches a
   certain point, sleep for a second. There would need to be a
   way of resetting the count, with a call made for commands like
   run or rest. */
int check_input(microsec)
int microsec;
{
  int arg, result;

  /* Return true if a read on descriptor 1 will not block. */
  if (microsec != 0 && (turn & 0x7F) == 0)
    (void) sleep (1); /* mod 128, sleep one sec every 128 turns */
  /* Can't check for input, but can do non-blocking read, so... */
  /* Ugh! */
  arg = 0;
  arg = fcntl(0, F_GETFL, arg);
  arg |= O_NDELAY;
  (void) fcntl(0, F_SETFL, arg);

  result = getch();

  arg = 0;
  arg = fcntl(0, F_GETFL, arg);
  arg &= ~O_NDELAY;
  (void) fcntl(0, F_SETFL, arg);
  if (result == -1)
    return 0;
  else
    return 1;
}

/* Find a default user name from the system. */
void user_name(buf, id)
  char *buf;
  int id;
{
  struct passwd *pwd;
  pwd = getpwuid(id);
  (void) strcpy(buf, pwd->pw_name);
  if (*buf>='a' && *buf<='z') *buf=(*buf-'a')+'A';
}

/* expands a tilde at the beginning of a file name to a users home directory */
int tilde(file, exp)
char *file, *exp;
{
  *exp = '\0';
  if (file)
    {
      if (*file == '~')
	{
	  char user[128];
	  struct passwd *pw = NULL;
	  int i = 0;

	  user[0] = '\0';
	  file++;
	  while (*file != '/' && i < sizeof(user))
	    user[i++] = *file++;
	  user[i] = '\0';
	  if (i == 0)
	    {
	      char *login = (char *) getlogin();

	      if (login != NULL)
		(void) strcpy (user, login);
	      else if ((pw = getpwuid(getuid())) == NULL)
		return 0;
	    }
	  if (pw == NULL && (pw = getpwnam(user)) == NULL)
	    return 0;
	  (void) strcpy (exp, pw->pw_dir);
	}
      (void) strcat(exp, file);
      return 1;
    }
  return 0;
}

/* undefine these so that tfopen and topen will work */
#undef fopen
#undef open

/* open a file just as does fopen, but allow a leading ~ to specify a home
   directory */
FILE *tfopen(file, mode)
char *file;
char *mode;
{
  char buf[1024];
  extern int errno;

  if (tilde(file, buf))
    return (fopen(buf, mode));
  errno = ENOENT;
  return NULL;
}

/* open a file just as does open, but expand a leading ~ into a home directory
   name */
int topen(file, flags, mode)
char *file;
int flags, mode;
{
  char buf[1024];
  extern int errno;

  if (tilde(file, buf))
    return (open(buf, flags, mode));
  errno = ENOENT;
  return -1;
}
#endif
