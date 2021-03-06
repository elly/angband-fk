/* externs.h: declarations for global variables and initialized data

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern int errno;

extern char *copyright[5];

extern int player_uid;
extern int NO_SAVE;

/* horrible hack: needed because compact_monster() can be called from deep
   within creatures() via place_monster() and summon_monster() */
extern int hack_monptr;

extern int16 log_index;		/* Index to log file. -CJS- */
extern vtype died_from;
extern vtype savefile;			/* The save file. -CJS- */

/* These are options, set with set_options command -CJS- */
extern int rogue_like_commands;
extern int find_cut;			/* Cut corners on a run */
extern int find_examine;		/* Check corners on a run */
extern int find_prself;			/* Print yourself on a run (slower) */
extern int find_bound;			/* Stop run when the map shifts */
extern int prompt_carry_flag;		/* Prompt to pick something up */
extern int show_weight_flag;		/* Display weights in inventory */
extern int highlight_seams;		/* Highlight magma and quartz */
extern int find_ignore_doors;		/* Run through open doors */

/* Unique artifact weapon flags */
extern int GROND, RINGIL, AEGLOS, ARUNRUTH, MORMEGIL, ANGRIST, GURTHANG,
  CALRIS, ANDURIL, STING, ORCRIST, GLAMDRING, DURIN, AULE, THUNDERFIST,
  BLOODSPIKE, DOOMCALLER, NARTHANC, NIMTHANC, DETHANC, GILETTAR, RILIA,
  BELANGIL, BALLI, LOTHARANG, FIRESTAR, ERIRIL, CUBRAGOL, BARD, COLLUIN,
  HOLCOLLETH, TOTILA, PAIN, ELVAGIL, AGLARANG, EORLINGAS, BARUKKHELED,
  WRATH, HARADEKKET, MUNDWINE, GONDRICAM, ZARCUTHRA, CARETH, FORASGIL,
  CRISDURIAN, COLANNON, HITHLOMIR, THALKETTOTH, ARVEDUI, THRANDUIL, THENGEL,
  HAMMERHAND, CELEFARN, THROR, MAEDHROS, OLORIN, ANGUIREL, OROME,
  EONWE, THEODEN, ULMO, OSONDIR, TURMIL, TIL, DEATHWREAKER, AVAVIR, TARATOL;

/* Unique artifact armour flags */
extern int DOR_LOMIN, NENYA, NARYA, VILYA, BELEGENNON, FEANOR, ISILDUR,
SOULKEEPER, FINGOLFIN, ANARION, POWER, PHIAL, BELEG, DAL, PAURHACH,
PAURNIMMEN, PAURAEGEN, PAURNEN, CAMMITHRIM, CAMBELEG, INGWE, CARLAMMAS,
HOLHENNETH, AEGLIN, CAMLOST, NIMLOTH, NAR, BERUTHIEL, GORLIM, ELENDIL,
THORIN, CELEBORN, THRAIN, GONDOR, THINGOL, THORONGIL, LUTHIEN, TUOR, ROHAN,
TULKAS, NECKLACE, BARAHIR, CASPANION, RAZORBACK, BLADETURNER;

/* Brand new extra effecient and kind way to add unique monsters... HOORAY!! */
extern struct unique_mon u_list[MAX_CREATURES];

extern int quests[MAX_QUESTS];


/* global flags */
extern int good_item_flag;      /* True if an artifact has been created... */
extern int LOAD;
extern int new_level_flag;	  /* Next level when true  */
extern int search_flag;	      /* Player is searching   */
extern int teleport_flag;	/* Handle teleport traps  */
extern int eof_flag;		/* Used to handle eof/HANGUP */
extern int player_light;      /* Player carrying light */
extern int find_flag;	/* Used in MORIA	      */
extern int free_turn_flag;	/* Used in MORIA	      */
extern int weapon_heavy;	/* Flag if the weapon too heavy -CJS- */
extern int pack_heavy;		/* Flag if the pack too heavy -CJS- */
extern char doing_inven;	/* Track inventory commands */
extern int screen_change;	/* Screen changes (used in inven_commands) */

extern int character_generated;	 /* don't save score until char gen finished */
extern int character_saved;	 /* prevents save on kill after save_char() */
extern int highscore_fd;	/* High score file descriptor */
extern int command_count;	/* Repetition of commands. -CJS- */
extern int default_dir;		/* Use last direction in repeated commands */
extern int16 noscore;		/* Don't score this game. -CJS- */
extern int32u randes_seed;    /* For encoding colors */
extern int32u town_seed;	    /* Seed for town genera*/
extern int16 dun_level;	/* Cur dungeon level   */
extern int16 missile_ctr;	/* Counter for missiles */
extern int msg_flag;	/* Set with first msg  */
extern vtype old_msg[MAX_SAVE_MSG];	/* Last messages -CJS- */
extern int16 last_msg;			/* Where in the array is the last */
extern int death;	/* True if died	      */
extern int32 turn;	/* Cur trun of game    */
extern int wizard;	/* Wizard flag	      */
extern int to_be_wizard;
extern int16 panic_save; /* this is true if playing from a panic save */

extern int wait_for_more;

extern char days[7][29];
extern int closing_flag;	/* Used for closing   */

extern int16 cur_height, cur_width;	/* Cur dungeon size    */
/*  Following are calculated from max dungeon sizes		*/
extern int16 max_panel_rows, max_panel_cols;
extern int panel_row, panel_col;
extern int panel_row_min, panel_row_max;
extern int panel_col_min, panel_col_max;
extern int panel_col_prt, panel_row_prt;

/*  Following are all floor definitions				*/
extern cave_type cave[MAX_HEIGHT][MAX_WIDTH];

/* Following are player variables				*/
extern player_type py;
extern char *player_title[MAX_CLASS][MAX_PLAYER_LEVEL];
extern race_type race[MAX_RACES];
extern background_type background[MAX_BACKGROUND];
extern int32u player_exp[MAX_PLAYER_LEVEL];
extern int16u player_hp[MAX_PLAYER_LEVEL];
extern int16 char_row;
extern int16 char_col;

extern char *dsp_race[MAX_RACES];	/* Short strings for races. -CJS- */
extern int8u rgold_adj[MAX_RACES][MAX_RACES];

extern class_type class[MAX_CLASS];
extern int16 class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

/* Warriors don't have spells, so there is no entry for them. */
extern spell_type magic_spell[MAX_CLASS-1][63];
extern char *spell_names[127];
extern int32u spell_learned;	/* Bit field for spells learnt -CJS- */
extern int32u spell_learned2;	/* Bit field for spells learnt -CJS- */
extern int32u spell_worked;	/* Bit field for spells tried -CJS- */
extern int32u spell_worked2;	/* Bit field for spells tried -CJS- */
extern int32u spell_forgotten;	/* Bit field for spells forgotten -JEW- */
extern int32u spell_forgotten2;	/* Bit field for spells forgotten -JEW- */
extern int8u spell_order[64];	/* remember order that spells are learned in */
extern int16u player_init[MAX_CLASS][5];
extern int16 total_winner;

/* Following are store definitions				*/
extern owner_type owners[MAX_OWNERS];
extern store_type store[MAX_STORES];
extern int16u store_choice[MAX_STORES][STORE_CHOICES];
extern int (*store_buy[MAX_STORES])();

/* Following are treasure arrays	and variables			*/
extern treasure_type object_list[MAX_OBJECTS];
extern int8u object_ident[OBJECT_IDENT_SIZE];
extern int16 t_level[MAX_OBJ_LEVEL+1];
extern inven_type t_list[MAX_TALLOC];
extern inven_type inventory[INVEN_ARRAY_SIZE];
extern char *special_names[SN_ARRAY_SIZE];
extern int16 sorted_objects[MAX_DUNGEON_OBJ];
extern int16 inven_ctr;		/* Total different obj's	*/
extern int16 inven_weight;	/* Cur carried weight	*/
extern int16 equip_ctr;	/* Cur equipment ctr	*/
extern int16 tcptr;	/* Cur treasure heap ptr	*/

/* Following are creature arrays and variables			*/
extern creature_type c_list[MAX_CREATURES];
extern describe_mon_type desc_list[MAX_CREATURES];
extern monster_type m_list[MAX_MALLOC];
extern int16 m_level[MAX_MONS_LEVEL+1];
extern m_attack_type monster_attacks[N_MONS_ATTS];
extern recall_type c_recall[MAX_CREATURES];	/* Monster memories. -CJS- */
extern monster_type blank_monster;	/* Blank monster values	*/
extern int16 mfptr;	/* Cur free monster ptr	*/
extern int16 mon_tot_mult;	/* # of repro's of creature	*/

/* Following are arrays for descriptive pieces			*/
extern char *colors[MAX_COLORS];
extern char *mushrooms[MAX_MUSH];
extern char *woods[MAX_WOODS];
extern char *metals[MAX_METALS];
extern char *rocks[MAX_ROCKS];
extern char *amulets[MAX_AMULETS];
extern char *syllables[MAX_SYLLABLES];

extern int8u blows_table[11][12];

extern int16u normal_table[NORMAL_TABLE_SIZE];

/* Initialized data which had to be moved from some other file */
/* Since these get modified, macrsrc.c must be able to access them */
/* Otherwise, game cannot be made restartable */
/* dungeon.c */
extern char last_command;  /* Memory of previous command. */
/* moria1.c */
/* Track if temporary light about player.  */
extern int light_flag;

/* function return values */
/* only extern functions declared here, static functions declared inside
   the file that defines them */

/* create.c */
void create_character(void);

/* creature.c */
void update_mon(int);
int movement_rate(int16);
int multiply_monster(int, int, int, int);
void creatures(int);

/* death.c */
void exit_game(void);

/* desc.c */
int is_a_vowel(char);
void magic_init(void);
void known1(inven_type *);
int known1_p(inven_type *);
void known2(inven_type *);
int known2_p(inven_type *);
void clear_known2(inven_type *);
void clear_empty(inven_type *);
void store_bought(inven_type *);
int store_bought_p(inven_type *);
void sample(struct inven_type *);
void identify(int *);
void unmagic_name(inven_type *);
void objdes(char *, struct inven_type *, int);
void scribe_object(void);
void add_inscribe(inven_type *, int8u);
void inscribe(inven_type *, char *);
void invcopy(inven_type *, int);
void desc_charges(int);
void desc_remain(int);

/* dungeon.c */
void dungeon(void);

/* eat.c */
void eat(void);

/* files.c */
void read_times(void);
void helpfile(char *);
void print_objects(void);
int file_character(char *);


/* generate.c */
void generate_cave(void);

/* help.c */
void ident_char(void);

/* io.c */
#ifdef SIGTSTP
void suspend(int sig);
#endif
void init_curses(void);
void moriaterm(void);
void put_buffer(char *, int, int);
void put_qio(void);
void restore_term(void);
void shell_out(void);
char inkey(void);
void flush(void);
void erase_line(int, int);
void clear_screen(void);
void clear_from(int);
void print(char, int, int);
void move_cursor_relative(int, int);
void count_msg_print(char *);
void prt(char *, int, int);
void move_cursor(int, int);
void msg_print(char *);
int get_check(char *);
int get_com(char *, char *);
int get_string(char *, int, int, int);
void pause_line(int);
void pause_exit(int, int);
void save_screen(void);
void restore_screen(void);
void bell(void);
void screen_map(void);

/* magic.c */
void cast(void);

/* main.c */
int main(int, char **);

/* misc1.c */
void init_seeds(int32u);
void set_seed(int32u);
void reset_seed(void);
int randint(int);
int randnor(int, int);
int bit_pos(int32u *);
int in_bounds(int, int);
void panel_bounds(void);
int get_panel(int, int, int);
int panel_contains(int, int);
int distance(int, int, int, int);
int next_to_wall(int, int);
int next_to_corr(int, int);
int damroll(int, int);
int pdamroll(int8u* array);
int los(int, int, int, int);
unsigned char loc_symbol(int, int);
int test_light(int, int);
void prt_map(void);
void add_food(int);
int popm(void);
int max_hp(int8u *array);
void place_monster(int, int, int, int);
void place_win_monster(void);
int get_mons_num(int);
void alloc_monster(int, int, int);
int summon_monster(int * ,int *, int);
int summon_undead(int *, int *);
int popt(void);
void pusht(int8u);
int magik(int);
int m_bonus(int, int, int);
void magic_treasure(int, int, int, int);
void set_options(void);

/* misc2.c */
void place_trap(int, int, int);
void place_rubble(int, int);
void place_gold(int, int);
int get_obj_num(int, int);
void place_object(int, int);
void alloc_object(int (*)(), int, int);
void random_object(int, int, int);
void cnv_stat(int16u, char *);
void prt_stat(int);
void prt_field(char *, int, int);
int stat_adj(int);
int chr_adj(void);
int con_adj(void);
char *title_string(void);
void prt_title(void);
void prt_level(void);
void prt_cmana(void);
void prt_mhp(void);
void prt_chp(void);
void prt_pac(void);
void prt_gold(void);
void prt_depth(void);
void prt_hunger(void);
void prt_blind(void);
void prt_confused(void);
void prt_afraid(void);
void prt_poisoned(void);
void prt_state(void);
void prt_speed(void);
void prt_study(void);
void prt_winner(void);
int16u modify_stat(int, int16);
void set_use_stat(int);
int inc_stat(int);
int dec_stat(int);
int res_stat(int);
void bst_stat(int, int);
int tohit_adj(void);
int toac_adj(void);
int todis_adj(void);
int todam_adj(void);
void prt_stat_block(void);
void draw_cave(void);
void put_character(void);
void put_stats(void);
char *likert(int, int);
void put_misc1(void);
void put_misc2(void);
void put_misc3(void);
void display_char(void);
void get_name(void);
void change_name(void);
void inven_destroy(int);
void take_one_item(struct inven_type *, struct inven_type *);
void inven_drop(int, int);
int inven_damage(int (*)(), int);
int weight_limit(void);
int inven_check_num(inven_type *);
int inven_check_weight(struct inven_type *);
void check_strength(void);
int inven_carry(struct inven_type *);
int spell_chance(int);
void print_spells(int *, int, int, int);
int get_spell(int *, int, int *, int *, char *, int);
void calc_spells(int);
void gain_spells(void);
void calc_mana(int);
void prt_experience(void);
void calc_hitpoints(void);
void insert_str(char *, char *, char *);
void insert_lnum(char *, char *, int32, int);
int enter_wiz_mode(void);
int attack_blows(int, int *);
int tot_dam(struct inven_type *, int, int);
int critical_blow(int, int, int, int);
int mmove(int, int *, int *);
int player_saves(void);
int find_range(int, int, int *, int *);
void teleport(int);
void check_view(void);

/* monsters.c */

/* moria1.c */
void change_speed(int);
void py_bonuses(struct inven_type *, int);
void calc_bonuses(void);
int show_inven(int, int, int, int, int (*)());
char *describe_use(int);
int show_equip(int, int);
void takeoff(int, int);
int verify(char *, int);
void inven_command(char);
int get_item(int *, char *, int, int, int (*)());
int no_light(void);
int get_dir(char *, int *);
int get_alldir(char *, int *);
void move_rec(int, int, int, int);
void light_room(int, int);
void lite_spot(int, int);
void move_light(int, int, int, int);
void disturb(int, int);
void search_on(void);
void search_off(void);
void rest(void);
void rest_off(void);
int test_hit(int, int, int, int, int);
void take_hit(int, char *);
void change_trap(int, int);
void search(int, int, int);
void find_init(int dir);
void find_run(void);
void end_find(void);
void area_affect(int, int, int);
int minus_ac(int32u);
void corrode_gas(char *);
void poison_gas(int, char *);
void fire_dam(int, char *);
void cold_dam(int, char *);
void light_dam(int, char *);
void acid_dam(int, char *);

/* moria2.c */
int cast_spell(char * ,int, int *, int *);
void delete_monster(int);
void fix1_delete_monster(int);
void fix2_delete_monster(int);
int delete_object(int, int);
int32u monster_death(int y, int x, int32u flags, int32u good, int32u win);
int mon_take_hit(int, int);
void move_char(int, int);
void openobject(void);
void closeobject(void);
int twall(int, int, int, int);
void tunnel(int);
void disarm_trap(void);
void look(void);
void throw_object(void);
void bash(void);

/* potions.c */
void quaff(void);

/* prayer.c */
void pray(void);

/* recall.c */
int bool_roff_recall(int);
int roff_recall(int);

/* rnd.c */
int32u get_rnd_seed(void);
void set_rnd_seed(int32u);
int32 rnd(void);

/* save.c */
int save_char(void);
int _save_char(char *);
int get_char(int *);

/* scrolls.c */
void read_scroll(void);

/* sets.c */
int set_room(int);
int set_corr(int);
int set_floor(int);
int set_corrodes(inven_type *);
int set_flammable(inven_type *);
int set_frost_destroy(inven_type *);
int set_acid_affect(inven_type *);
int set_lightning_destroy(inven_type *);
int set_null(inven_type *);
int set_acid_destroy(inven_type *);
int set_fire_destroy(inven_type *);
int general_store(int);
int armory(int);
int weaponsmith(int);
int temple(int);
int alchemist(int);
int magic_shop(int);

/* signals.c */
void nosignals(void);
void signals(void);
void init_signals(void);
void ignore_signals(void);
void default_signals(void);
void restore_signals(void);

/* spells.c */
void monster_name(char *, struct monster_type *, struct creature_type *);
void lower_monster_name(char *, struct monster_type *, struct creature_type *);
int sleep_monsters1(int, int);
int detect_treasure(void);
int detect_object(void);
int detect_trap(void);
int detect_sdoor(void);
int detect_invisible(void);
int light_area(int, int);
int unlight_area(int, int);
void map_area(void);
int ident_spell(void);
int aggravate_monster(int);
int trap_creation(void);
int door_creation(void);
int td_destroy(void);
int detect_monsters(void);
void light_line(int, int, int);
void starlite(int, int);
int disarm_all(int, int, int);
void get_flags(int, int32u *, int32u *, int (**)());
void fire_bolt(int, int, int, int, int, char *);
void fire_ball(int, int, int, int, int, char *);
void breath(int, int, int, int, char *, int);
int recharge(int);
int hp_monster(int, int, int, int);
int drain_life(int, int, int, int);
int speed_monster(int, int, int, int);
int confuse_monster(int, int, int);
int sleep_monster(int, int, int);
int wall_to_mud(int, int, int);
int td_destroy2(int, int, int);
int poly_monster(int, int, int);
int build_wall(int, int, int);
int clone_monster(int, int, int);
void teleport_away(int, int);
void teleport_to(int, int);
int teleport_monster(int, int, int);
int mass_genocide(int spell);
int genocide(int spell);
int speed_monsters(int);
int sleep_monsters2(void);
int mass_poly(void);
int detect_evil(void);
int hp_player(int);
int cure_confusion(void);
int cure_blindness(void);
int cure_poison(void);
int remove_fear(void);
void earthquake(void);
int protect_evil(void);
void create_food(void);
int dispel_creature(int, int);
int turn_undead(void);
void warding_glyph(void);
void lose_str(void);
void lose_int(void);
void lose_wis(void);
void lose_dex(void);
void lose_con(void);
void lose_chr(void);
void lose_exp(int32);
int slow_poison(void);
void bless(int);
void detect_inv2(int);
void destroy_area(int, int);
int enchant(int16 *);
int remove_curse(void);
int restore_level(void);

/* staffs.c */
void use(void);

/* store1.c */
int32 item_value(struct inven_type *);
int32 sell_price(int, int32 *, int32 *, struct inven_type *);
int store_check_num(inven_type *, int);
void store_carry(int, int *, struct inven_type *);
void store_destroy(int, int, int);
void store_init(void);
void store_maint(void);
int noneedtobargain(int, int32);
void updatebargain(int, int32, int32);

/* store2.c */
void enter_store(int);

/* treasur1.c */

/* treasur2.c */

#ifdef unix
/* unix.c */
int check_input(int);
void user_name(char *, int);
int tilde(char *, char *);
FILE *tfopen(char *, char *);
int topen(char *, int, int);
#endif

/* variable.c */

/* wands.c */
void aim(void);

/* wizard.c */
void wizard_light(int light);
void change_character(void);
void wizard_create(void);
void artifact_check(void);
