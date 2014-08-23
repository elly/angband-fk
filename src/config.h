/* config.h: configuration definitions

   Copyright (c) 1989 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* files used by moria, set these to valid pathnames */

/* probably unix */
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

/* this sets the default user interface */
/* to use the original key bindings (keypad for movement) set ROGUE_LIKE
   to FALSE, to use the rogue-like key bindings (vi style movement)
   set ROGUE_LIKE to TRUE */
/* if you change this, you only need to recompile main.c */
#define ROGUE_LIKE TRUE
