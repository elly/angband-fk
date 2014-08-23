/* config.h: configuration definitions

   Copyright (c) 1989 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* Person to bother if something goes wrong. */
/* Recompile files.c and misc2.c if these change. */
#define WIZARD	"Sean"
/* wizard password and wizard uid no longer used */


/* files used by moria, set these to valid pathnames */

/* probably unix */
#define ANGBAND_TST       LIBDIR"/test"
#define ANGBAND_HOU       LIBDIR"/files/hours"
#define ANGBAND_MOR       LIBDIR"/files/news"
#define ANGBAND_TOP       LIBDIR"/files/newscores"
#define ANGBAND_BONES     LIBDIR"/bones/"
#define ANGBAND_HELP      LIBDIR"/files/roglcmds.hlp"
#define ANGBAND_ORIG_HELP LIBDIR"/files/origcmds.hlp"
#define ANGBAND_WIZ_HELP  LIBDIR"/files/rwizcmds.hlp"
#define ANGBAND_OWIZ_HELP LIBDIR"/files/owizcmds.hlp"
#define ANGBAND_WELCOME   LIBDIR"/files/welcome.hlp"
#define ANGBAND_LOG       LIBDIR"/files/ANGBAND.log"
#define ANGBAND_VER       LIBDIR"/files/version.hlp"
#define ANGBAND_LOAD      LIBDIR"/files/loadcheck"
#define ANGBAND_WIZ       LIBDIR"/files/wizards"
#define ANGBAND_SAV       LIBDIR"/save"

/*#define ANNOY 142*/
/* #define SET_UID */

/* this sets the default user interface */
/* to use the original key bindings (keypad for movement) set ROGUE_LIKE
   to FALSE, to use the rogue-like key bindings (vi style movement)
   set ROGUE_LIKE to TRUE */
/* if you change this, you only need to recompile main.c */
#define ROGUE_LIKE TRUE


/* if you are compiling on a SYS V version of UNIX, define this */
#define SYS_V

/* if you are compiling on an ATARI ST with Mark Williams C, define this */
/* #define ATARIST_MWC */

/* if you are compiling on a Macintosh with MPW C 3.0, define this */
/* #define MAC */


/* system dependent defines follow, you should not need to change anything
   below */

/* substitute strchr for index on USG versions of UNIX */
#if defined(SYS_V) || defined(MSDOS) || defined(MAC)
#define index strchr
#endif

#if defined(SYS_V) || defined(MSDOS) || defined(MAC)
#ifndef USG
#define USG
#endif
#endif

#ifdef ATARIST_MWC
#define USG
#endif

#ifdef MSDOS
#define register      /* MSC 4.0 still has a problem with register bugs ... */
#endif

#ifdef MAC
#ifdef RSRC
#define MACRSRC		/* i.e., we're building the resources */
#else
#define MACGAME		/* i.e., we're building the game */
#endif
#endif

#ifdef MAC
/* Screen dimensions */
#define SCRN_ROWS	24
#define SCRN_COLS	80
#endif

#if vms
#define getch _getch
#define unlink delete
#define index strchr
#define lstat stat
#define exit uexit
#endif

#if defined(SYS_V) && defined(lint)
/* to prevent <string.h> from including <NLchar.h>, this prevents a bunch
   of lint errors. */
#define RTPC_NO_NLS
#endif

