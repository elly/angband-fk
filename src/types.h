/* types.h: global type declarations

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdint.h>

typedef uint32_t       int32u;
typedef int32_t        int32;
typedef uint16_t       int16u;
typedef int16_t        int16;
typedef uint8_t        int8u;
/* some machines will not accept 'signed char' as a type, and some accept it
   but still treat it like an unsigned character, let's just avoid it,
   any variable which can ever hold a negative value must be 16 or 32 bits */

#define VTYPESIZ	160
#define BIGVTYPESIZ	300
typedef char vtype[VTYPESIZ];
/* note that since its output can easily exceed 80 characters, objdes must
   always be called with a bigvtype as the first paramter */
typedef char bigvtype[BIGVTYPESIZ];
typedef char stat_type[7];

/* Many of the character fields used to be fixed length, which greatly
   increased the size of the executable.  I have replaced many fixed
   length fields with variable length ones. */

/* all fields are given the smallest possbile type, and all fields are
   aligned within the structure to their natural size boundary, so that
   the structures contain no padding and are minimum size */

/* bit fields are only used where they would cause a large reduction in
   data size, they should not be used otherwise because their use
   results in larger and slower code */

typedef int16u attid;

typedef struct creature_type
{
  char *name;		/* Descrip of creature	*/
  int32u cmove;		/* Bit field		*/
  int32u spells;	/* Creature spells	*/
  int32u cdefense;	/* Bit field		*/
  int32u spells2;	/* More creature spells */
  int32u spells3;	/* Yes! even More creature spells */
  int32u mexp;		/* Exp value for kill	*/
  int8u sleep;		/* Inactive counter/10	*/
  int8u aaf;		/* Area affect radius	*/
  int8u ac;		/* AC			*/
  int8u speed;		/* Movement speed+10	*/
  int8u cchar;		/* Character rep.	*/
  int8u hd[2];		/* Creatures hit die	*/
  attid damage[4];	/* Type attack and damage*/
  int16u level;		/* Level of creature	*/
  int8u rarity;		/* Rarity of creature	*/
} creature_type;

typedef struct m_attack_type	/* Monster attack and damage types */
  {
    int8u attack_type;
    int8u attack_desc;
    int8u attack_dice;
    int8u attack_sides;
  } m_attack_type;

typedef struct recall_type	/* Monster memories. -CJS- */
  {
    int32u r_cmove;
    int32u r_spells;
    int32u r_spells2;
    int32u r_spells3;
    int16u r_kills, r_deaths;
    int32u r_cdefense;
    int8u r_wake, r_ignore;
    int8u r_attacks[MAX_MON_NATTACK];
  } recall_type;

struct unique_mon {
  int exist;
  int dead;
};

typedef struct describe_mon_type {
  char *name;
  char *desc;
} describe_mon_type;

typedef struct monster_type
{
  int16 hp;		/* Hit points		*/
  int16 maxhp;		/* Max Hit points	*/
  int16 csleep;		/* Inactive counter	*/
  int16 cspeed;		/* Movement speed	*/
  int16u mptr;		/* Pointer into creature was int16u*/
  /* Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256 */
  int8u fy;		/* Y Pointer into map	*/
  int8u fx;		/* X Pointer into map	*/
  int8u cdis;		/* Cur dis from player	*/
  int8u ml;
  int8u stunned;
  int8u confused;
} monster_type;

typedef struct treasure_type
{
  char *name;		/* Object name		*/
  int32u flags;		/* Special flags	*/
  int8u tval;		/* Category number	*/
  int8u tchar;		/* Character representation*/
  int16 p1;

/* Misc. use variable	*/
  int32 cost;		/* Cost of item		*/
  int8u subval;		/* Sub-category number	*/
  int8u number;		/* Number of items	*/
  int16u weight;	/* Weight		*/
  int16 tohit;		/* Plusses to hit	*/
  int16 todam;		/* Plusses to damage	*/
  int16 ac;		/* Normal AC		*/
  int16 toac;		/* Plusses to AC	*/
  int8u damage[2];	/* Damage when hits	*/
  int8u level;		/* Level item first found */
  int8u rare;           /* True if Rare         */
  int32u flags2;        /* Yes! even more froggin' flags!*/
} treasure_type;

/* only damage, ac, and tchar are constant; level could possibly be made
   constant by changing index instead; all are used rarely */
/* extra fields x and y for location in dungeon would simplify pusht() */
/* making inscrip a pointer and mallocing space does not work, there are
   two many places where inven_types are copied, which results in dangling
   pointers, so we use a char array for them instead */
#define INSCRIP_SIZE 13  /* notice alignment, must be 4*x + 1 */
typedef struct inven_type
{
  int16u index;		/* Index to object_list */
  int8u name2;		/* Object special name  */
  char inscrip[INSCRIP_SIZE]; /* Object inscription   */
  int32u flags;		/* Special flags	*/
  int8u tval;		/* Category number	*/
  int8u tchar;		/* Character representation*/
  int16 p1;		/* Misc. use variable	*/
  int32 cost;		/* Cost of item		*/
  int8u subval;		/* Sub-category number	*/
  int8u number;		/* Number of items	*/
  int16u weight;	/* Weight		*/
  int16 tohit;		/* Plusses to hit	*/
  int16 todam;		/* Plusses to damage	*/
  int16 ac;		/* Normal AC		*/
  int16 toac;		/* Plusses to AC	*/
  int8u damage[2];	/* Damage when hits	*/
  int8u level;		/* Level item first found */
  int8u ident;		/* Identify information */
  int32u flags2;        /* Yes! even more froggin' flags!*/
  int16u timeout;        /* How long to wait before reactivating an Artifact */
} inven_type;


typedef struct player_type
{
  struct misc
    {
      char name[27];	/* Name of character	*/
      int8u male;	/* Sex of character	*/
      int32 au;		/* Gold			*/
      int32 max_exp;	/* Max experience	*/
      int32 exp;	/* Cur experience	*/
      int16u exp_frac;	/* Cur exp fraction * 2^16 */
      int16u age;	/* Characters age	*/
      int16u ht;	/* Height		*/
      int16u wt;	/* Weight		*/
      int16u lev;	/* Level		*/
      int16u max_dlv;	/* Max level explored	*/
      int16 srh;	/* Chance in search	*/
      int16 fos;	/* Frenq of search	*/
      int16 bth;	/* Base to hit		*/
      int16 bthb;	/* BTH with bows	*/
      int16 mana;	/* Mana points		*/
      int16 mhp;	/* Max hit pts		*/
      int16 ptohit;	/* Plusses to hit	*/
      int16 ptodam;	/* Plusses to dam	*/
      int16 pac;	/* Total AC		*/
      int16 ptoac;	/* Magical AC		*/
      int16 dis_th;	/* Display +ToHit	*/
      int16 dis_td;	/* Display +ToDam	*/
      int16 dis_ac;	/* Display +ToAC	*/
      int16 dis_tac;	/* Display +ToTAC	*/
      int16 disarm;	/* % to Disarm		*/
      int16 save;	/* Saving throw		*/
      int16 sc;		/* Social Class		*/
      int16 stl;	/* Stealth factor	*/
      int8u pclass;	/* # of class		*/
      int8u prace;	/* # of race		*/
      int8u hitdie;	/* Char hit die		*/
      int8u expfact;	/* Experience factor	*/
      int16 cmana;	/* Cur mana pts		*/
      int16u cmana_frac; /* Cur mana fraction * 2^16 */
      int16 chp;	/* Cur hit pts		*/
      int16u chp_frac;	/* Cur hit fraction * 2^16 */
      char history[4][60]; /* History record	*/
    } misc;
  /* Stats now kept in arrays, for more efficient access. -CJS- */
  struct stats
    {
      int16u max_stat[6];	/* What is restored */
      int8u cur_stat[6];	/* What is natural */
      int16 mod_stat[6];	/* What is modified, may be +/- */
      int16u use_stat[6];	/* What is used */
    } stats;
  struct flags
    {
      int32u status;		/* Status of player    */
      int16 rest;		/* Rest counter	       */
      int16 blind;		/* Blindness counter   */
      int16 paralysis;		/* Paralysis counter   */
      int16 confused;		/* Confusion counter   */
      int16 food;		/* Food counter	       */
      int16 food_digested;	/* Food per round      */
      int16 protection;		/* Protection fr. evil */
      int16 speed;		/* Cur speed adjust    */
      int16 fast;		/* Temp speed change   */
      int16 slow;		/* Temp speed change   */
      int16 afraid;		/* Fear		       */
      int16 cut;                /* Wounds              */
      int16 stun;               /* Stunned player      */
      int16 poisoned;		/* Poisoned	       */
      int16 image;		/* Hallucinate	       */
      int16 protevil;		/* Protect VS evil     */
      int16 invuln;		/* Increases AC	       */
      int16 hero;		/* Heroism	       */
      int16 shero;		/* Super Heroism       */
      int16 shield;		/* Shield Spell        */
      int16 blessed;		/* Blessed	       */
      int16 resist_heat;	/* Timed heat resist   */
      int16 resist_cold;	/* Timed cold resist   */
      int16 resist_acid;	/* Timed acid resist   */
      int16 resist_light;	/* Timed light resist  */
      int16 resist_poison;	/* Timed poison resist */
      int16 detect_inv;		/* Timed see invisible */
      int16 word_recall;	/* Timed teleport level*/
      int16 see_infra;		/* See warm creatures  */
      int16 tim_infra;		/* Timed infra vision  */
      int8u see_inv;		/* Can see invisible   */
      int8u teleport;		/* Random teleportation*/
      int8u free_act;		/* Never paralyzed     */
      int8u slow_digest;	/* Lower food needs    */
      int8u aggravate;		/* Aggravate monsters  */
      int8u fire_resist;	/* Resistance to fire  */
      int8u cold_resist;	/* Resistance to cold  */
      int8u acid_resist;	/* Resistance to acid  */
      int8u regenerate;		/* Regenerate hit pts  */
      int8u lght_resist;	/* Resistance to light */
      int8u ffall;		/* No damage falling   */
      int8u sustain_str;	/* Keep strength       */
      int8u sustain_int;	/* Keep intelligence   */
      int8u sustain_wis;	/* Keep wisdom	       */
      int8u sustain_con;	/* Keep constitution   */
      int8u sustain_dex;	/* Keep dexterity      */
      int8u sustain_chr;	/* Keep charisma       */
      int8u confuse_monster;	/* Glowing hands.      */
      int8u new_spells;		/* Number of spells can learn. */
      int8u poison_resist;	/* Resistance to poison*/
      int8u hold_life;	        /* Immune to life draining*/
      int8u telepathy;	        /* Gives telepathy     */
      int8u fire_im;	        /* Immune to fire      */
      int8u acid_im;	        /* Immune to acid      */
      int8u poison_im;	        /* Immune to poison    */
      int8u cold_im;	        /* Immune to cold      */
      int8u light_im;	        /* Immune to lightning */
      int8u light;	        /* Permanent light     */
      int8u confusion_resist;   /* Resist confusion    */
      int8u sound_resist;	/* Resist sound	       */
      int8u light_resist;       /* Resist light        */
      int8u dark_resist;	/* Resist darkness     */
      int8u chaos_resist;	/* Resist chaos	       */
      int8u disenchant_resist;	/* Resist disenchant   */
      int8u shards_resist;	/* Resist shards       */
      int8u nexus_resist;	/* Resist nexus	       */
      int8u blindness_resist;	/* Resist blindness    */
      int8u nether_resist;      /* Resist nether       */
    } flags;
} player_type;

typedef struct spell_type
{  /* spell name is stored in spell_names[] array at index i, +31 if priest */
  int8u slevel;
  int8u smana;
  int8u sfail;
  int8u sexp;	/* 1/4 of exp gained for learning spell */
} spell_type;

typedef struct race_type
{
  char	*trace;		/* Type of race			*/
  int16 str_adj;	/* adjustments			*/
  int16 int_adj;
  int16 wis_adj;
  int16 dex_adj;
  int16 con_adj;
  int16 chr_adj;
  int8u b_age;	       /* Base age of character		*/
  int8u m_age;	       /* Maximum age of character	*/
  int8u m_b_ht;	      /* base height for males		*/
  int8u m_m_ht;	      /* mod height for males		*/
  int8u m_b_wt;	      /* base weight for males		*/
  int8u m_m_wt;	      /* mod weight for males		*/
  int8u f_b_ht;	      /* base height females		*/
  int8u f_m_ht;	      /* mod height for females	*/
  int8u f_b_wt;	      /* base weight for female	*/
  int8u f_m_wt;	      /* mod weight for females	*/
  int16 b_dis;	       /* base chance to disarm		*/
  int16 srh;	       /* base chance for search	*/
  int16 stl;	       /* Stealth of character		*/
  int16 fos;	       /* frequency of auto search	*/
  int16 bth;	       /* adj base chance to hit	*/
  int16 bthb;	       /* adj base to hit with bows	*/
  int16 bsav;	       /* Race base for saving throw	*/
  int8u bhitdie;	       /* Base hit points for race	*/
  int8u infra;	       /* See infra-red			*/
  int8u b_exp;	       /* Base experience factor	*/
  int8u rtclass;       /* Bit field for class types	*/
} race_type;

typedef struct class_type
{
  char *title;		/* type of class		*/
  int8u adj_hd;		/* Adjust hit points		*/
  int8u mdis;		/* mod disarming traps		*/
  int8u msrh;		/* modifier to searching	*/
  int8u mstl;		/* modifier to stealth		*/
  int8u mfos;		/* modifier to freq-of-search	*/
  int8u mbth;		/* modifier to base to hit	*/
  int8u mbthb;		/* modifier to base to hit - bows*/
  int8u msav;		/* Class modifier to save	*/
  int16 madj_str;	/* Class modifier for strength	*/
  int16 madj_int;	/* Class modifier for intelligence*/
  int16 madj_wis;	/* Class modifier for wisdom	*/
  int16 madj_dex;	/* Class modifier for dexterity */
  int16 madj_con;	/* Class modifier for constitution*/
  int16 madj_chr;	/* Class modifier for charisma	*/
  int8u spell;		/* class use mage spells	*/
  int8u m_exp;		/* Class experience factor	*/
  int8u first_spell_lev;/* First level where class can use spells. */
} class_type;

typedef struct background_type
{
  char *info;		/* History information		*/
  int8u roll;		/* Die roll needed for history	*/
  int8u chart;		/* Table number			*/
  int8u next;		/* Pointer to next table	*/
  int8u bonus;		/* Bonus to the Social Class+50	*/
} background_type;

typedef struct cave_type
{
  int16u cptr;
  int16u tptr;
  int8u fval;
  unsigned int lr : 1;  /* room should be lit with perm light, walls with
			   this set should be perm lit after tunneled out */
  unsigned int fm : 1;	/* field mark, used for traps/doors/stairs, object is
			   hidden if fm is FALSE */
  unsigned int pl : 1;	/* permanent light, used for walls and lighted rooms*/
  unsigned int tl : 1;	/* temporary light, used for player's lamp light,etc.*/
} cave_type;

typedef struct owner_type
{
  char *owner_name;
  int16 max_cost;
  int8u max_inflate;
  int8u min_inflate;
  int8u haggle_per;
  int8u owner_race;
  int8u insult_max;
} owner_type;

typedef struct inven_record
{
  int32 scost;
  inven_type sitem;
} inven_record;

typedef struct store_type
{
  int32 store_open;
  int16 insult_cur;
  int8u owner;
  int8u store_ctr;
  int16u good_buy;
  int16u bad_buy;
  inven_record store_inven[STORE_INVEN_MAX];
} store_type;

typedef struct high_scores
{
  int32 points;
  int16u lev;
  int16u max_lev;
  int16 mhp;
  int16 chp;
  int16 uid;
  int16 dun_level;
  int8u sex;
  vtype name;
  vtype died_from;
  int8u pclass;
  int8u prace;
} high_scores;
