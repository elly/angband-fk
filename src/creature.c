/* creature.c: handle monster movement and attacks

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "monster.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#if defined(LINT_ARGS)
static void get_moves(int, int *);
static void make_attack(int);
static void make_move(int, int *, int32u *);
static void mon_cast_spell(int, int *);
static void mon_move(int, int32u *);
static int check_mon_lite(int, int);
#endif

static shatter_quake();
static br_wall();

/* Updates screen when monsters move about		-RAK-	*/
void update_mon(monptr)
int monptr;
{
  register int flag;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;

  m_ptr = &m_list[monptr];
  flag = FALSE;
  if ((m_ptr->cdis <= MAX_SIGHT) &&
      (!(py.flags.status & PY_BLIND) || py.flags.telepathy) &&
      (panel_contains((int)m_ptr->fy, (int)m_ptr->fx)))
    {
      /* Wizard sight.	     */
      if (wizard || py.flags.telepathy)
	flag = TRUE;
      /* Normal sight.	     */
      else if (los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	{
	  c_ptr = &cave[m_ptr->fy][m_ptr->fx];
	  r_ptr = &c_list[m_ptr->mptr];
          if ((py.flags.see_infra > 0) && /* moved here to allow infra to see invis -CFT */
                   (m_ptr->cdis <= py.flags.see_infra))
            {
             if (NO_INFRA & r_ptr->cdefense) /* changed to act sensibly -CFT */
                c_recall[m_ptr->mptr].r_cdefense |= NO_INFRA;
              else
                flag = TRUE; /* only can see if not NO_INFRA... */
            }
	  if (c_ptr->pl || c_ptr->tl ||
	      (find_flag && m_ptr->cdis < 2 && player_light))
	    {
	      if ((CM_INVISIBLE & r_ptr->cmove) == 0)
		flag = TRUE;
	      else if (py.flags.see_inv)
		{
		  flag = TRUE;
		  c_recall[m_ptr->mptr].r_cmove |= CM_INVISIBLE;
		}
	    }
	}
    }
  /* Light it up.	 */
  if (flag)
    {
      if (!m_ptr->ml)
	{
	  disturb (1, 0);
	  m_ptr->ml = TRUE;
	  lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
	  screen_change = TRUE; /* notify inven_command */
	}
    }
  /* Turn it off.	 */
  else if (m_ptr->ml)
    {
      m_ptr->ml = FALSE;
      lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
      screen_change = TRUE; /* notify inven_command */
    }
}


/* Given speed,	 returns number of moves this turn.	-RAK-	*/
/* NOTE: Player must always move at least once per iteration,	  */
/*	 a slowed player is handled by moving monsters faster	 */
int movement_rate(speed)
register int16 speed;
{
  if (speed > 0)
    {
      if (py.flags.rest > 0 || py.flags.rest==-1)
	return 1;
      else
	return speed;
    }
  else
    {
      /* speed must be negative here */
      return ((turn % (2 - speed)) == 0);
    }
}


/* Makes sure a new creature gets lit up.			-CJS- */
static int check_mon_lite(y, x)
int y, x;
{
  register int monptr;

  monptr = cave[y][x].cptr;
  if (monptr <= 1)
    return FALSE;
  else
    {
      update_mon(monptr);
      return m_list[monptr].ml;
    }
}


/* Choose correct directions for monster movement	-RAK-	*/
static void get_moves(monptr, mm)
int monptr;
register int *mm;
{
  int y, ay, x, ax, move_val;

  y = m_list[monptr].fy - char_row;
  x = m_list[monptr].fx - char_col;
  if (y < 0)
    {
      move_val = 8;
      ay = -y;
    }
  else
    {
      move_val = 0;
      ay = y;
    }
  if (x > 0)
    {
      move_val += 4;
      ax = x;
    }
  else
    ax = -x;
  /* this has the advantage of preventing the diamond maneuvre, also faster */
  if (ay > (ax << 1))
    move_val += 2;
  else if (ax > (ay << 1))
    move_val++;
  switch(move_val)
    {
    case 0:
      mm[0] = 9;
      if (ay > ax)
	{
	  mm[1] = 8;
	  mm[2] = 6;
	  mm[3] = 7;
	  mm[4] = 3;
	}
      else
	{
	  mm[1] = 6;
	  mm[2] = 8;
	  mm[3] = 3;
	  mm[4] = 7;
	}
      break;
    case 1: case 9:
      mm[0] = 6;
      if (y < 0)
	{
	  mm[1] = 3;
	  mm[2] = 9;
	  mm[3] = 2;
	  mm[4] = 8;
	}
      else
	{
	  mm[1] = 9;
	  mm[2] = 3;
	  mm[3] = 8;
	  mm[4] = 2;
	}
      break;
    case 2: case 6:
      mm[0] = 8;
      if (x < 0)
	{
	  mm[1] = 9;
	  mm[2] = 7;
	  mm[3] = 6;
	  mm[4] = 4;
	}
      else
	{
	  mm[1] = 7;
	  mm[2] = 9;
	  mm[3] = 4;
	  mm[4] = 6;
	}
      break;
    case 4:
      mm[0] = 7;
      if (ay > ax)
	{
	  mm[1] = 8;
	  mm[2] = 4;
	  mm[3] = 9;
	  mm[4] = 1;
	}
      else
	{
	  mm[1] = 4;
	  mm[2] = 8;
	  mm[3] = 1;
	  mm[4] = 9;
	}
      break;
    case 5: case 13:
      mm[0] = 4;
      if (y < 0)
	{
	  mm[1] = 1;
	  mm[2] = 7;
	  mm[3] = 2;
	  mm[4] = 8;
	}
      else
	{
	  mm[1] = 7;
	  mm[2] = 1;
	  mm[3] = 8;
	  mm[4] = 2;
	}
      break;
    case 8:
      mm[0] = 3;
      if (ay > ax)
	{
	  mm[1] = 2;
	  mm[2] = 6;
	  mm[3] = 1;
	  mm[4] = 9;
	}
      else
	{
	  mm[1] = 6;
	  mm[2] = 2;
	  mm[3] = 9;
	  mm[4] = 1;
	}
      break;
    case 10: case 14:
      mm[0] = 2;
      if (x < 0)
	{
	  mm[1] = 3;
	  mm[2] = 1;
	  mm[3] = 6;
	  mm[4] = 4;
	}
      else
	{
	  mm[1] = 1;
	  mm[2] = 3;
	  mm[3] = 4;
	  mm[4] = 6;
	}
      break;
    case 12:
      mm[0] = 1;
      if (ay > ax)
	{
	  mm[1] = 2;
	  mm[2] = 4;
	  mm[3] = 3;
	  mm[4] = 7;
	}
      else
	{
	  mm[1] = 4;
	  mm[2] = 2;
	  mm[3] = 7;
	  mm[4] = 3;
	}
      break;
    }
}

static int monster_critical(dice, sides, dam)
  int dice, sides, dam;
{
  int total=dice*sides;
  int max=0;

  if (dam==total && dam>20)
    max=1;
  if ((dam > (19*total)/20) && ((dam<20)? randint(20)==1: TRUE)) {
    if (dam>20)
      while (randint(50)==1) max++;
    if (dam>45) return 6+max;
    if (dam>33) return 5+max;
    if (dam>25) return 4+max;
    if (dam>18) return 3+max;
    if (dam>11) return 2+max;
    return 1+max;
  }
  return 0;
}

/* Make an attack on the player (chuckle.)		-RAK-	*/
static void make_attack(monptr)
int monptr;
{
  int attype, adesc, adice, asides;
  int i, j, damage, flag, attackn, notice, visible;
  int shatter = FALSE;
  int CUT=FALSE, STUN=FALSE;
  int32 gold;
  attid *attstr;
  vtype cdesc, tmp_str, ddesc;
  register creature_type *r_ptr;
  monster_type *m_ptr;
  register struct misc *p_ptr;
  register struct flags *f_ptr;
  register inven_type *i_ptr;
  int8u blinked = 0; /* flag to see if blinked away (after steal) -CFT */

  if (death)  /* don't beat a dead body! */
    return;

  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];

  if (r_ptr->cdefense & DESTRUCT)
    shatter= TRUE;

  if (!m_ptr->ml)
    (void) strcpy(cdesc, "It ");
  else {
    if (c_list[m_ptr->mptr].cdefense & UNIQUE)
      (void) sprintf(cdesc, "%s ", r_ptr->name);
    else
      (void) sprintf(cdesc, "The %s ", r_ptr->name);
  }

  /* For "DIED_FROM" string	   */
  if (r_ptr->cdefense & UNIQUE)
    (void) sprintf(ddesc, "%s", r_ptr->name);
  else if (is_a_vowel (r_ptr->name[0]))
    (void) sprintf(ddesc, "an %s", r_ptr->name);
  else
    (void) sprintf (ddesc, "a %s", r_ptr->name);
  /* End DIED_FROM		   */

  attackn = 0;
  attstr = r_ptr->damage;
  while ((*attstr != 0) && !death && !blinked)
    {
      attype = monster_attacks[*attstr].attack_type;
      adesc = monster_attacks[*attstr].attack_desc;
      adice = monster_attacks[*attstr].attack_dice;
      asides = monster_attacks[*attstr].attack_sides;
      attstr++;
      flag = FALSE;
      if (((py.flags.protevil > 0) && (r_ptr->cdefense & EVIL) &&
	((py.misc.lev + 1) > r_ptr->level)) && (randint(100)+(py.misc.lev)>50))
	/* Random (100) + level > 50 chance for stop any attack added */
	{
	  if (m_ptr->ml)
	    c_recall[m_ptr->mptr].r_cdefense |= EVIL;
	  attype = 99;
	  adesc = 99;
	}
      p_ptr = &py.misc;
      switch(attype)
	{
	case 1:	      /*Normal attack  */
	  if (test_hit(60, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 2:	      /*Lose Strength*/
	  if (test_hit(-3, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 3:	      /*Confusion attack*/
	  if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 4:	      /*Fear attack    */
	  if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 5:	      /*Fire attack    */
	  if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 6:	      /*Acid attack    */
	  if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 7:	      /*Cold attack    */
	  if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 8:	      /*Lightning attack*/
	  if (test_hit(10, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 9:	      /*Corrosion attack*/
	  if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 10:      /*Blindness attack*/
	  if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 11:      /*Paralysis attack*/
	  if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 12:      /*Steal Money    */
	  if ((test_hit(5, (int)r_ptr->level, 0, (int)py.misc.lev,
			CLA_MISC_HIT))
	      && (py.misc.au > 0))
	    flag = TRUE;
	  break;
	case 13:      /*Steal Object   */
	  if ((test_hit(2, (int)r_ptr->level, 0, (int)py.misc.lev,
			CLA_MISC_HIT))
	      && (inven_ctr > 0))
	    flag = TRUE;
	  break;
	case 14:      /*Poison	       */
	  if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 15:      /*Lose dexterity*/
	  if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 16:      /*Lose constitution*/
	  if (test_hit(0, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 17:      /*Lose intelligence*/
	  if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 18:      /*Lose wisdom*/
	  if (test_hit(2, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 19:      /*Lose experience*/
	  if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 20:      /*Aggravate monsters*/
	  flag = TRUE;
	  break;
	case 21:      /*Disenchant	  */
	  if (test_hit(20, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 22:      /*Eat food	  */
	  if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 23:      /*Eat light	  */
	  if (test_hit(5, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
		       CLA_MISC_HIT))
	    flag = TRUE;
	  break;
	case 24:      /*Eat charges	  */
	  if ((test_hit(15, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
			CLA_MISC_HIT)) &&
	      (inven_ctr > 0))	/* check to make sure an object exists */
	    flag = TRUE;
	  break;
        case 25:     /*Drain all stats   */
	  if ((test_hit(2, (int)r_ptr->level, 0, p_ptr->pac+p_ptr->ptoac,
			CLA_MISC_HIT)))
	    flag = TRUE;
	  break;
	case 99:
	  flag = TRUE;
	  break;
	default:
	  break;
	}
      if (flag)
	{
	  /* can not strcat to cdesc because the creature may have
	     multiple attacks */
	  disturb (1, 0);
	  (void) strcpy(tmp_str, cdesc);
	  CUT=STUN=FALSE;
	  switch(adesc)
	    {
	    case 1: msg_print(strcat(tmp_str, "hits you."));
	      CUT=TRUE;
	      STUN=TRUE;
	      break;
	    case 2:
	      msg_print(strcat(tmp_str, "bites you."));
	      CUT=TRUE;
	      break;
	    case 3:
	      msg_print(strcat(tmp_str, "claws you."));
	      CUT=TRUE;
	      break;
	    case 4: msg_print(strcat(tmp_str, "stings you.")); break;
	    case 5: msg_print(strcat(tmp_str, "touches you.")); break;
	    case 7: msg_print(strcat(tmp_str, "gazes at you.")); break;
	    case 8: msg_print(strcat(tmp_str, "breathes on you.")); break;
	    case 9: msg_print(strcat(tmp_str, "spits on you.")); break;
	    case 10: msg_print(strcat(tmp_str,"makes a horrible wail."));break;
	    case 12: msg_print(strcat(tmp_str, "crawls on you.")); break;
	    case 13:
	      msg_print(strcat(tmp_str, "releases a cloud of spores.")); break;
	    case 14: msg_print(strcat(tmp_str, "begs you for money.")); break;
	    case 15: msg_print("You've been slimed!"); break;
	    case 16: msg_print(strcat(tmp_str, "crushes you.")); break;
	    case 17:
	      msg_print(strcat(tmp_str, "tramples you."));
	      STUN=TRUE;
	      break;
	    case 18: msg_print(strcat(tmp_str, "drools on you.")); break;
	    case 19:
	      switch(randint(9))
		{
		case 1: msg_print(strcat(tmp_str, "insults you!")); break;
		case 2:
		  msg_print(strcat(tmp_str, "insults your mother!")); break;
		case 3:
		  msg_print(strcat(tmp_str, "gives you the finger!")); break;
		case 4: msg_print(strcat(tmp_str, "humiliates you!")); break;
		case 5: msg_print(strcat(tmp_str, "wets on your leg!")); break;
		case 6: msg_print(strcat(tmp_str, "defiles you!")); break;
		case 7: msg_print(strcat(tmp_str, "dances around you!"));break;
		case 8:
		  msg_print(strcat(tmp_str, "makes obscene gestures!")); break;
		case 9: msg_print(strcat(tmp_str, "moons you!!!")); break;
		}
	      break;
	    case 20:
	      msg_print(strcat(tmp_str, "butts you."));
	      STUN=TRUE;
	      break;
	    case 21:
	      msg_print(strcat(tmp_str, "charges you."));
	      STUN=TRUE;
	      break;
	    case 22: msg_print(strcat(tmp_str, "engulfs you.")); break;
	    case 23:
	      switch(randint(5))
		  {
		  case 1: msg_print(strcat(tmp_str, "wants his mushrooms back")); break;
		  case 2: msg_print(strcat(tmp_str, "tells you to get off his land")); break;
		  case 3: msg_print(strcat(tmp_str, "looks for his dogs")); break;
		  case 4: msg_print(strcat(tmp_str, "says 'Did you kill my Fang?'")); break;
		  case 5: msg_print(strcat(tmp_str, "asks 'Do you want to buy any mushrooms?'")); break;
		  }
		break;
	    case 99: msg_print(strcat(tmp_str, "is repelled.")); break;
	    default: break;
	    }

	  notice = TRUE;
	  /* always fail to notice attack if creature invisible, set notice
	     and visible here since creature may be visible when attacking
	     and then teleport afterwards (becoming effectively invisible) */
	  if (!m_ptr->ml)
	    {
	      visible = FALSE;
	      notice = FALSE;
	    }
	  else
	    visible = TRUE;

	  damage = damroll (adice, asides);
	  switch(attype)
	    {
	    case 1:    /*Normal attack	*/
	      /* round half-way case down */
	      damage -= ((((((p_ptr->pac+p_ptr->ptoac)>150)? 150 :
			   (p_ptr->pac+p_ptr->ptoac))*3)/4)*damage)/200;
	      take_hit(damage, ddesc);
	      if ((damage>23) && shatter)
                shatter_quake(m_ptr->fy, m_ptr->fx);
	      break;
	    case 2:    /*Lose Strength*/
	      take_hit(damage, ddesc);
	      if (py.flags.sustain_str)
		msg_print("You feel weaker for a moment, but it passes.");
	      else if (randint(2) == 1)
		{
		  msg_print("You feel weaker.");
		  (void) dec_stat (A_STR);
		}
	      else
		notice = FALSE;
	      break;
	    case 3:    /*Confusion attack*/
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if ((!py.flags.confusion_resist) && (!py.flags.chaos_resist))
	        {
	          if (randint(2) == 1)
		    {
		      if (f_ptr->confused < 1)
		        {
		          msg_print("You feel confused.");
		          f_ptr->confused += randint((int)r_ptr->level);
		        }
		      else
		        notice = FALSE;
		      f_ptr->confused += 3;
		    }
	          else
		    notice = FALSE;
		}
	      break;
	    case 4:    /*Fear attack	*/
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (player_saves() || (py.misc.pclass==1 && randint(3)==1))
		msg_print("You stand your ground!");
	      else if (f_ptr->afraid < 1)
		{
		  msg_print("You are suddenly afraid!");
		  f_ptr->afraid += 3 + randint((int)r_ptr->level);
		}
	      else
		{
		  f_ptr->afraid += 3;
		  notice = FALSE;
		}
	      break;
	    case 5:    /*Fire attack	*/
	      msg_print("You are enveloped in flames!");
	      fire_dam(damage, ddesc);
	      break;
	    case 6:    /*Acid attack	*/
	      msg_print("You are covered in acid!");
	      acid_dam(damage, ddesc);
	      break;
	    case 7:    /*Cold attack	*/
	      msg_print("You are covered with frost!");
	      cold_dam(damage, ddesc);
	      break;
	    case 8:    /*Lightning attack*/
	      msg_print("Lightning strikes you!");
	      light_dam(damage, ddesc);
	      break;
	    case 9:    /*Corrosion attack*/
	      msg_print("A stinging red gas swirls about you.");
	      corrode_gas(ddesc);
	      take_hit(damage, ddesc);
	      break;
	    case 10:	/*Blindness attack*/
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (!py.flags.blindness_resist) {
	        if (f_ptr->blind < 1)
		  {
		    f_ptr->blind += 10 + randint((int)r_ptr->level);
		    msg_print("Your eyes begin to sting.");
		  }
	        else
		  {
		    f_ptr->blind += 5;
		    notice = FALSE;
		  }
	      }
	      break;
	    case 11:	/*Paralysis attack*/
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (player_saves())
		msg_print("You resist the effects!");
	      else if (f_ptr->paralysis < 1)
		{
		  if (f_ptr->free_act)
		    msg_print("You are unaffected.");
		  else
		    {
		      f_ptr->paralysis = randint((int)r_ptr->level) + 3;
		      msg_print("You are paralysed.");
		    }
		}
	      else
		notice = FALSE;
	      break;
	    case 12:	/*Steal Money	  */
	      if ((py.flags.paralysis < 1) &&
		  (randint(168) < py.stats.use_stat[A_DEX]))
		  /* immune to steal at 18/150 */
		msg_print("You quickly protect your money pouch!");
	      else
		{
		  gold = (p_ptr->au/10) + randint(25);
		  if (gold > p_ptr->au)
		    p_ptr->au = 0;
		  else
		    p_ptr->au -= gold;
		  msg_print("Your purse feels lighter.");
		  prt_gold();
		}
	      if (randint(2) == 1)
		{
		  msg_print("There is a puff of smoke!");
		  blinked = 1; /* added -CFT */
		  teleport_away(monptr, MAX_SIGHT);
		}
	      break;
	    case 13:	/*Steal Object	 */
	      if ((py.flags.paralysis < 1) &&
		  (randint(168) < py.stats.use_stat[A_DEX]))
		  /* immune to steal at 18/150 change */
		msg_print("You grab hold of your backpack!");
	      else
		{
		  i = randint(inven_ctr) - 1;
                    if ((inventory[i].tval >= TV_MIN_WEAR) &&
                        (inventory[i].tval <= TV_MAX_WEAR) &&
                        (inventory[i].flags2 & TR_ARTIFACT))
                      break; /* don't destroy artifacts -CFT */
		  inven_destroy(i);
		  msg_print("Your backpack feels lighter.");
		}
	      if (randint(3) == 1)
		{
		  msg_print("There is a puff of smoke!");
		  blinked = 1; /* added -CFT */
		  teleport_away(monptr, MAX_SIGHT);
		}
	      break;
	    case 14:	/*Poison	 */
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
              if (!(f_ptr->poison_im || f_ptr->poison_resist ||
		    f_ptr->resist_poison)) {
		msg_print("You feel very sick.");
		f_ptr->poisoned += randint((int)r_ptr->level)+5;
	      } else {
		msg_print("The poison has no effect.");
	      }
	      break;
	    case 15:	/*Lose dexterity */
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (f_ptr->sustain_dex)
		msg_print("You feel clumsy for a moment, but it passes.");
	      else
		{
		  msg_print("You feel more clumsy.");
		  (void) dec_stat (A_DEX);
		}
	      break;
	    case 16:	/*Lose constitution */
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (f_ptr->sustain_con)
		msg_print("Your body resists the effects of the disease.");
	      else
		{
		  msg_print("Your health is damaged!");
		  (void) dec_stat (A_CON);
		}
	      break;
	    case 17:	/*Lose intelligence */
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      msg_print("You have trouble thinking clearly.");
	      if (f_ptr->sustain_int)
		msg_print("But your mind quickly clears.");
	      else
		(void) dec_stat (A_INT);
	      break;
	    case 18:	/*Lose wisdom	   */
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (f_ptr->sustain_wis)
		msg_print("Your wisdom is sustained.");
	      else
		{
		  msg_print("Your wisdom is drained.");
		  (void) dec_stat (A_WIS);
		}
	      break;
	    case 19:	/*Lose experience  */
	      f_ptr = &py.flags;
	      if (f_ptr->hold_life && randint(5)>1)
		msg_print("You keep hold of your life force!");
	      else
		{
		  if (f_ptr->hold_life) {
		    msg_print("You feel your life slipping away!");
		    lose_exp(damage + (py.misc.exp / 1000)*MON_DRAIN_LIFE);
		  } else {
		    msg_print("You feel your life draining away!");
		    lose_exp(damage + (py.misc.exp / 100)*MON_DRAIN_LIFE);
		  }
                }
	      break;
	    case 20:	/*Aggravate monster*/
	      (void) aggravate_monster(20);
	      break;
	    case 21:	/*Disenchant	   */
	      if (!py.flags.disenchant_resist) {
	        take_hit(damage, ddesc);
	        flag = FALSE;
	        switch(randint(7))
		  {
		  case 1: i = INVEN_WIELD; break;
		  case 2: i = INVEN_BODY;  break;
		  case 3: i = INVEN_ARM;   break;
		  case 4: i = INVEN_OUTER; break;
		  case 5: i = INVEN_HANDS; break;
		  case 6: i = INVEN_HEAD;  break;
		  case 7: i = INVEN_FEET;  break;
		  }
	        i_ptr = &inventory[i];
		  if (i_ptr->tohit > 0) {
		    i_ptr->tohit -= randint(2);
		    /* don't send it below zero */
		    if (i_ptr->tohit < 0)
		      i_ptr->tohit = 0;
		    flag = TRUE;
		  }
		  if (i_ptr->todam > 0) {
		    i_ptr->todam -= randint(2);
		    /* don't send it below zero */
		    if (i_ptr->todam < 0)
		      i_ptr->todam = 0;
		    flag = TRUE;
		  }
		  if (i_ptr->toac > 0) {
		    i_ptr->toac  -= randint(2);
		    /* don't send it below zero */
		    if (i_ptr->toac < 0)
		    i_ptr->toac = 0;
		    flag = TRUE;
		  }
	        if (flag)
		  {
		    msg_print("There is a static feeling in the air.");
		    calc_bonuses ();
		  }
	        else
		  notice = FALSE;
	      }
	      break;
	    case 22:	/*Eat food	   */
	      if (find_range(TV_FOOD, TV_NEVER, &i, &j))
		{
		  inven_destroy(i);
		  msg_print ("It got at your rations!");
		}
	      else
		notice = FALSE;
	      break;
	    case 23:	/*Eat light	   */
	      i_ptr = &inventory[INVEN_LIGHT];
	      if (i_ptr->p1 > 0)
		{
		  i_ptr->p1 -= (250 + randint(250));
		  if (i_ptr->p1 < 1)  i_ptr->p1 = 1;
		  if (py.flags.blind < 1)
		    msg_print("Your light dims.");
		  else
		    notice = FALSE;
		}
	      else
		notice = FALSE;
	      break;
	    case 24:	/*Eat charges	  */
	      i = randint(inven_ctr) - 1;
	      j = r_ptr->level;
	      i_ptr = &inventory[i];
	      if (((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND))
		  && (i_ptr->p1 > 0))
		{
		  m_ptr->hp += j*i_ptr->p1;
		  i_ptr->p1 = 0;
		  if (!known2_p(i_ptr))
		    add_inscribe(i_ptr, ID_EMPTY);
		  msg_print("Energy drains from your pack!");
		}
	      else
		notice = FALSE;
	      break;
	    case 25:   /* Drain all stats. Haha! SM */
	      f_ptr = &py.flags;
	      take_hit(damage, ddesc);
	      if (py.flags.sustain_str)
		msg_print("You feel weaker for a moment, but it passes.");
	      else
	      {
		msg_print("You feel weaker.");
	        (void) dec_stat (A_STR);
	      }
              if (f_ptr->sustain_dex)
                msg_print("You feel clumsy for a moment, but it passes.");
              else
	      {
                msg_print("You feel more clumsy.");
                (void) dec_stat (A_DEX);
	      }
              if (f_ptr->sustain_con)
                msg_print("Your body resists the effects of the disease.");
              else
	      {
                msg_print("Your health is damaged!");
                (void) dec_stat (A_CON);
	      }
              msg_print("You have trouble thinking clearly.");
              if (f_ptr->sustain_int)
                msg_print("But your mind quickly clears.");
              else
                (void) dec_stat (A_INT);
              if (f_ptr->sustain_wis)
                msg_print("Your wisdom is sustained.");
              else
	      {
                msg_print("Your wisdom is drained.");
                (void) dec_stat (A_WIS);
	      }
	      if (f_ptr->sustain_chr)
		msg_print("You keep your good looks.");
	      else
	      {
		msg_print("Your features are twisted.");
		(void) dec_stat (A_CHR);
	      }
	      break;
	    case 99:
	      notice = FALSE;
	      break;
	    default:
	      notice = FALSE;
	      break;
	    }

	  if (CUT && STUN) {
	    switch (randint(2)) {
	    case 1:
	      CUT=FALSE;
	      break;
	    case 2:
	      STUN=FALSE;
	      break;
	    }
	  }
	  switch (monster_critical(adice, asides, damage)) {
	  case 0:
	    break;
	  case 1:
	    if (CUT) cut_player(randint(5));
	    else if (STUN) stun_player(randint(5));
	    break;
	  case 2:
	    if (CUT) cut_player(randint(5)+5);
	    else if (STUN) stun_player(randint(5)+5);
	    break;
	  case 3:
	    if (CUT) cut_player(randint(30)+20);
	    else if (STUN) stun_player(randint(20)+10);
	    break;
	  case 4:
	    if (CUT) cut_player(randint(70)+30);
	    else if (STUN) stun_player(randint(40)+30);
	    break;
	  case 5:
	    if (CUT) cut_player(randint(250)+50);
	    else if (STUN) stun_player(randint(50)+40);
	    break;
	  case 6:
	    if (CUT) cut_player(300);
	    else if (STUN) stun_player(randint(60)+57);
	    break;
	  default:
	    if (CUT) cut_player(5000);
	    else if (STUN) stun_player(100+randint(10));
	    break;
	  }

	  /* moved here from mon_move, so that monster only confused if it
	     actually hits */
	  if (py.flags.confuse_monster && py.flags.protevil<=0)
	    {
	      msg_print("Your hands stop glowing.");
	      py.flags.confuse_monster = FALSE;
	      if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
		  (CHARM_SLEEP & r_ptr->cdefense))
		(void) sprintf(tmp_str, "%sis unaffected.", cdesc);
	      else
		{
		  (void) sprintf(tmp_str, "%sappears confused.", cdesc);
		  m_ptr->confused = TRUE;
		}
	      msg_print(tmp_str);
	      if (visible && !death && randint(4) == 1)
		c_recall[m_ptr->mptr].r_cdefense |= r_ptr->cdefense &
		  CHARM_SLEEP;
	    }

	  /* increase number of attacks if notice true, or if had
	     previously noticed the attack (in which case all this does is
	     help player learn damage), note that in the second case do
	     not increase attacks if creature repelled (no damage done) */
	  if ((notice ||
	       (c_recall[m_ptr->mptr].r_attacks[attackn] != 0 &&
		attype != 99))
	      && c_recall[m_ptr->mptr].r_attacks[attackn] < MAX_UCHAR)
	    c_recall[m_ptr->mptr].r_attacks[attackn]++;
	  if (visible && death && c_recall[m_ptr->mptr].r_deaths < MAX_SHORT)
	    c_recall[m_ptr->mptr].r_deaths++;
	}
      else
	{
	  if ((adesc >= 1 && adesc <= 3) || (adesc == 6))
	    {
	      disturb (1, 0);
	      (void) strcpy(tmp_str, cdesc);
	      msg_print(strcat(tmp_str, "misses you."));
	    }
	}
      if (attackn < MAX_MON_NATTACK-1)
	attackn++;
      else
	break;
    }
}


/* Make the move if possible, five choices		-RAK-	*/
static void make_move(monptr, mm, rcmove)
int monptr;
int *mm;
int32u *rcmove;
{
  int i, newy, newx, do_turn, do_move, stuck_door;
  int32u movebits;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register inven_type *t_ptr;

  i = 0;
  do_turn = FALSE;
  do_move = FALSE;
  m_ptr = &m_list[monptr];
  movebits = c_list[m_ptr->mptr].cmove;
  do
    {
      /* Get new position		*/
      newy = m_ptr->fy;
      newx = m_ptr->fx;
      (void) mmove(mm[i], &newy, &newx);
      c_ptr = &cave[newy][newx];
      if (c_ptr->fval != BOUNDARY_WALL)
	{
	  /* Floor is open?		   */
	  if (c_ptr->fval <= MAX_OPEN_SPACE)
	    do_move = TRUE;
	  /* Creature moves through walls? */
	  else if (movebits & CM_PHASE)
	    {
	      do_move = TRUE;
	      *rcmove |= CM_PHASE;
	    }
	  else if (c_list[m_ptr->mptr].cdefense & BREAK_WALL) {
	    /* Crunch up those Walls Morgoth and Umber Hulks!!!! */
	    t_ptr = &t_list[c_ptr->tptr];
	    do_move = TRUE;
	    c_recall[m_ptr->mptr].r_cdefense |= BREAK_WALL;
            if ((t_ptr->tval == TV_CLOSED_DOOR) ||
                (t_ptr->tval == TV_SECRET_DOOR)) { /* break the door -CFT */
              invcopy(t_ptr, OBJ_OPEN_DOOR);
              t_ptr->p1 = -1; /* make it broken, not just open */
              c_ptr->fval = CORR_FLOOR; /* change floor setting */
              lite_spot(newy, newx); /* show broken door */
              msg_print("You hear a door burst open!");
              disturb(1,0);
              }
            else /* otherwise, break those walls! -CFT */
	    (void) twall(newy, newx, 1, 0);
	  }
	  /* Creature can open doors?	   */
	  else if (c_ptr->tptr != 0)
	    {
	      t_ptr = &t_list[c_ptr->tptr];
	      if (movebits & CM_OPEN_DOOR)
		{     /* Creature can open doors.		     */
		  stuck_door = FALSE;
		  if (t_ptr->tval == TV_CLOSED_DOOR)
		    {
		      do_turn = TRUE;
		      if (t_ptr->p1 == 0)		/* Closed doors	 */
			do_move = TRUE;
		      else if (t_ptr->p1 > 0)	       /* Locked doors	*/
			{
			  if (randint((m_ptr->hp+1)*(50+t_ptr->p1)) <
			      40*(m_ptr->hp-10-t_ptr->p1))
			    t_ptr->p1 = 0;
			}
		      else if (t_ptr->p1 < 0)	       /* Stuck doors	*/
			{
			  if (randint((m_ptr->hp+1)*(50-t_ptr->p1)) <
			      40*(m_ptr->hp-10+t_ptr->p1))
			    {
			      msg_print("You hear a door burst open!");
			      disturb (1, 0);
			      stuck_door = TRUE;
			      do_move = TRUE;
			    }
			}
		    }
		  else if (t_ptr->tval == TV_SECRET_DOOR)
		    {
		      do_turn = TRUE;
		      do_move = TRUE;
		    }
		  if (do_move)
		    {
		      invcopy(t_ptr, OBJ_OPEN_DOOR);
		      if (stuck_door) /* 50% chance of breaking door */
			t_ptr->p1 = randint(2) - 1;
		      c_ptr->fval = CORR_FLOOR;
		      lite_spot(newy, newx);
		      *rcmove |= CM_OPEN_DOOR;
		      do_move = FALSE;
		    }
		}
	      else
		{     /* Creature can not open doors, must bash them   */
		  if (t_ptr->tval == TV_CLOSED_DOOR)
		    {
		      do_turn = TRUE;
		      if (randint((m_ptr->hp+1)*(80+abs(t_ptr->p1))) <
			  40*(m_ptr->hp-20-abs(t_ptr->p1)))
			{
			  invcopy(t_ptr, OBJ_OPEN_DOOR);
			  /* 50% chance of breaking door */
			  t_ptr->p1 = randint(2) - 1;
			  c_ptr->fval = CORR_FLOOR;
			  lite_spot(newy, newx);
			  msg_print ("You hear a door burst open!");
			  disturb (1, 0);
			}
		    }
		}
	    }
	  /* Glyph of warding present?	   */
	  if (do_move && (c_ptr->tptr != 0) &&
	      (t_list[c_ptr->tptr].tval == TV_VIS_TRAP) &&
	      (t_list[c_ptr->tptr].subval == 99))
	    {
	      if (randint(OBJ_RUNE_PROT) < c_list[m_ptr->mptr].level)
		{
		  if ((newy==char_row) && (newx==char_col))
		    msg_print("The rune of protection is broken!");
		  (void) delete_object(newy, newx);
		}
	      else
		{
		  do_move = FALSE;
		  /* If the creature moves only to attack, */
		  /* don't let it move if the glyph prevents */
		  /* it from attacking */
		  if (movebits & CM_ATTACK_ONLY)
		    do_turn = TRUE;
		}
	    }
	  /* Creature has attempted to move on player?	   */
	  if (do_move)
	    if (c_ptr->cptr == 1)
	      {
		/* if the monster is not lit, must call update_mon, it may
		   be faster than character, and hence could have just
		   moved next to character this same turn */
		if (!m_ptr->ml)
		  update_mon(monptr);
		make_attack(monptr);
		do_move = FALSE;
		do_turn = TRUE;
	      }
	  /* Creature is attempting to move on other creature?	   */
	    else if ((c_ptr->cptr > 1) &&
		     ((newy != m_ptr->fy) ||
		      (newx != m_ptr->fx)))
	      {
		/* Creature eats other creatures?	 */
		if ((movebits & CM_EATS_OTHER) &&
		    (c_list[m_ptr->mptr].mexp >
		     c_list[m_list[c_ptr->cptr].mptr].mexp))
		  {
		    if (m_list[c_ptr->cptr].ml)
		      *rcmove |= CM_EATS_OTHER;
		    /* It ate an already processed monster. Handle normally. */
		    if (monptr < c_ptr->cptr)
		      delete_monster((int) c_ptr->cptr);
		    /* If it eats this monster, an already processed monster
		       will take its place, causing all kinds of havoc.  Delay
		       the kill a bit. */
		    else
		      fix1_delete_monster((int) c_ptr->cptr);
		  }
		else
		  do_move = FALSE;
	      }
	  /* Creature has been allowed move.	 */
	  if (do_move)
	    {
	      /* Pick up or eat an object	       */
	      if (movebits & CM_PICKS_UP)
		{
		  c_ptr = &cave[newy][newx];

		  if ((c_ptr->tptr != 0)
		      && (t_list[c_ptr->tptr].tval <= TV_MAX_OBJECT)
                      && ((t_list[c_ptr->tptr].tval < TV_MIN_WEAR)
                        || (t_list[c_ptr->tptr].tval > TV_MAX_WEAR)
                        || !(t_list[c_ptr->tptr].flags2 & TR_ARTIFACT)))
		    {
		      *rcmove |= CM_PICKS_UP;
		      (void) delete_object(newy, newx);
		    }
		}
	      /* Move creature record		       */
	      move_rec((int)m_ptr->fy, (int)m_ptr->fx, newy, newx);
	      if (m_ptr->ml)
		{
		  m_ptr->ml = FALSE;
		  lite_spot ((int)m_ptr->fy, (int)m_ptr->fx);
		}
	      m_ptr->fy = newy;
	      m_ptr->fx = newx;
	      m_ptr->cdis = distance (char_row, char_col, newy, newx);
	      do_turn = TRUE;
	    }
	}
      i++;
      /* Up to 5 attempts at moving,   give up.	  */
    }
  while ((!do_turn) && (i < 5));
}


/* Creatures can cast spells too.  (Dragon Breath)	-RAK-	*/
/* cast_spell = true if creature changes position	*/
/* took_turn  = true if creature casts a spell		*/
static void mon_cast_spell(monptr, took_turn)
  int monptr;
  int *took_turn;
{
  int32u i;
  int y, x, chance, thrown_spell, r1;
  register int k, disenchant;
  int spell_choice[64], desperate=FALSE;
  vtype cdesc, outval, ddesc;
  register struct flags *f_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  register inven_type *i_ptr;

  if (death)
    return;

  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];
  /* 1 in x chance of casting spell		   */
  chance = (int)(r_ptr->spells & CS_FREQ);
  if (chance == 0) {
    msg_print("CHANCE == 0");
    msg_print("caused by ....");
    msg_print(r_ptr->name);
    *took_turn = FALSE;
  }
  else if (randint(chance) != 1)
    *took_turn	= FALSE;
  /* Must be within certain range		   */
  else if (m_ptr->cdis > MAX_SPELL_DIS)
    *took_turn	= FALSE;
  /* Must have unobstructed Line-Of-Sight	   */
  else if (!los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
    *took_turn	= FALSE;
  else	/* Creature is going to cast a spell	 */
    {
      *took_turn  = TRUE;
      /* Check to see if monster should be lit. */
      update_mon (monptr);
      /* Describe the attack			       */
      if (m_ptr->ml) {
	if (r_ptr->cdefense & UNIQUE)
	  (void) sprintf(cdesc, "%s ", r_ptr->name);
	else
	  (void) sprintf(cdesc, "The %s ", r_ptr->name);
      }
      else
	(void) strcpy(cdesc, "It ");
      /* For "DIED_FROM" string	 */
      if (UNIQUE & r_ptr->cdefense)
	(void) sprintf(ddesc, "%s", r_ptr->name);
      else if (is_a_vowel (r_ptr->name[0]))
	(void) sprintf (ddesc, "an %s", r_ptr->name);
      else
	(void) sprintf(ddesc, "a %s", r_ptr->name);
      /* End DIED_FROM		       */

      /* Extract all possible spells into spell_choice */
      if ((r_ptr->cdefense&INTELLIGENT) &&
	  (m_ptr->hp < ((r_ptr->hd[0]*r_ptr->hd[1])/10)) &&
	  (r_ptr->spells&CS_INT1 || r_ptr->spells2&CS_INT2 ||
	   r_ptr->spells3&CS_INT3) && randint(2)==1) {
	desperate=TRUE;
	c_recall[m_ptr->mptr].r_cdefense |= INTELLIGENT;
      }

      i = (r_ptr->spells & ~CS_FREQ);
      if (desperate) i&= CS_INT1;
      k = 0;
      while (i != 0) {
	spell_choice[k] = bit_pos(&i);
	k++;
      }
      i = r_ptr->spells2;
      if (desperate) i&= CS_INT2;
      while (i != 0) {
	spell_choice[k] = bit_pos(&i)+32;
	k++;
      }
      i = r_ptr->spells3;
      if (desperate) i&= CS_INT3;
      while (i != 0) {
	spell_choice[k] = bit_pos(&i)+64;
	k++;
      }
      /* Choose a spell to cast			       */
      if (!k)
	thrown_spell = 200;
      else
	thrown_spell = spell_choice[randint(k) - 1];
      thrown_spell++;
      /* all except teleport_away() and drain mana spells always disturb */
      if (thrown_spell > 6 && thrown_spell != 7)
	disturb (1, 0);
      /* Cast the spell.			     */
      switch(thrown_spell)
	{
	case 5:	 /*Teleport Short*/
	  (void) strcat(cdesc, "blinks away.");
	  msg_print(cdesc);
	  teleport_away(monptr, 5);
	  break;
	case 6:	 /*Teleport Long */
	  (void) strcat(cdesc, "teleports away.");
	  msg_print(cdesc);
	  teleport_away(monptr, MAX_SIGHT);
	  break;
	case 7:	 /*Teleport To	 */
	  (void) strcat(cdesc, "commands you to return!");
	  msg_print(cdesc);
	  teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
	  break;
	case 8:	 /*Light Wound	 */
	  (void) strcat(cdesc, "points at you and curses.");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You resist the effects of the spell.");
	  else
	    take_hit(damroll(3, 8), ddesc);
	  break;
	case 9:	 /*Serious Wound */
	  (void) strcat(cdesc, "points at you and curses horribly.");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You resist the effects of the spell.");
	  else
	    take_hit(damroll(8, 8), ddesc);
	  break;
	case 10:  /*Hold Person	  */
	  (void) strcat(cdesc, "gazes deep into your eyes!");
	  msg_print(cdesc);
	  if (py.flags.free_act)
	    msg_print("You are unaffected.");
	  else if (player_saves())
	    msg_print("You stare back unafraid!");
	  else if (py.flags.paralysis > 0)
	    py.flags.paralysis += 2;
	  else
	    py.flags.paralysis = randint(5)+4;
	  break;
	case 11:  /*Cause Blindness*/
	  (void) strcat(cdesc, "casts a spell, burning your eyes!");
	  msg_print(cdesc);
	  if ((player_saves()) || (py.flags.blindness_resist))
	    msg_print("You blink and your vision clears.");
	  else if (py.flags.blind > 0)
	    py.flags.blind += 6;
	  else
	    py.flags.blind += 12 + randint(3);
	  break;
	case 12:  /*Cause Confuse */
	  (void) strcat(cdesc, "creates a mesmerising illusion.");
	  msg_print(cdesc);
	  if ((player_saves()) || (py.flags.confusion_resist)
				|| (py.flags.chaos_resist))
	    msg_print("You disbelieve the feeble spell.");
	  else if (py.flags.confused > 0)
	    py.flags.confused += 2;
	  else
	    py.flags.confused = randint(5) + 3;
	  break;
	case 13:  /*Cause Fear	  */
	  (void) strcat(cdesc, "casts a fearful illusion.");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You refuse to be frightened.");
	  else if (py.flags.afraid > 0)
	    py.flags.afraid += 2;
	  else
	    py.flags.afraid = randint(5) + 3;
	  break;
	case 14:  /*Summon Monster*/
	  (void) strcat(cdesc, "magically summons help!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  hack_monptr = monptr;
	  (void) summon_monster(&y, &x, FALSE);
	  hack_monptr = -1;
	  update_mon ((int)cave[y][x].cptr);
	  break;
	case 15:  /*Summon Undead*/
	(void) strcat(cdesc, "magically summons help from beyond the grave!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  hack_monptr = monptr;
	  (void) summon_undead(&y, &x);
	  hack_monptr = -1;
	  update_mon ((int)cave[y][x].cptr);
	  break;
	case 16:  /*Slow Person	 */
	  (void) strcat(cdesc, "drains power from your muscles!");
	  msg_print(cdesc);
	  if (py.flags.free_act)
	    msg_print("You are unaffected.");
	  else if (player_saves())
	    msg_print("Your body resists the spell.");
	  else if (py.flags.slow > 0)
	    py.flags.slow += 2;
	  else
	    py.flags.slow = randint(5) + 3;
	  break;
	case 17:  /*Drain Mana	 */
	  if (py.misc.cmana > 0)
	    {
	      disturb (1, 0);
	      (void) sprintf(outval, "%sdraws psychic energy from you!",cdesc);
	      msg_print(outval);
	      if (m_ptr->ml)
		{
		  (void) sprintf(outval, "%sappears healthier.", cdesc);
		  msg_print(outval);
		}
	      if ((r_ptr->level>>1)==0) msg_print("case 17");
	      r1 = (randint((int)r_ptr->level) >> 1) + 1;
	      if (r1 > py.misc.cmana)
		{
		  r1 = py.misc.cmana;
		  py.misc.cmana = 0;
		  py.misc.cmana_frac = 0;
		}
	      else
		py.misc.cmana -= r1;
	      prt_cmana();
	      m_ptr->hp += 6*(r1);
	    }
	  break;
	case 18:  /*Summon Demon */
	  (void) strcat(cdesc, "summons a hellish adversary!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  hack_monptr = monptr;
	  (void) summon_demon(c_list[m_ptr->mptr].level, &y, &x);
	  hack_monptr = -1;
	  update_mon ((int)cave[y][x].cptr);
	  break;
	case 19:  /*Summon Dragon */
	  (void) strcat(cdesc, "magically summons a Dragon!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  hack_monptr = monptr;
	  (void) summon_dragon(&y, &x);
	  hack_monptr = -1;
	  update_mon ((int)cave[y][x].cptr);
	  break;
	case 20:  /*Breath Light */
	  (void) strcat(cdesc, "breathes lightning.");
	  msg_print(cdesc);
	  breath(GF_LIGHTNING, char_row, char_col,
		 ((m_ptr->hp / 3)>1600?1600:(m_ptr->hp / 3)), ddesc, monptr);
	  break;
	case 21:  /*Breath Gas	 */
	  (void) strcat(cdesc, "breathes gas.");
	  msg_print(cdesc);
	  breath(GF_POISON_GAS, char_row, char_col,
		 ((m_ptr->hp / 3)>800?800:(m_ptr->hp / 3)), ddesc, monptr);
	  break;
	case 22:  /*Breath Acid	 */
	  (void) strcat(cdesc, "breathes acid.");
	  msg_print(cdesc);
	  breath(GF_ACID, char_row, char_col,
		 ((m_ptr->hp / 3)>1600?1600:(m_ptr->hp / 3)), ddesc, monptr);
	  break;
	case 23:  /*Breath Frost */
	  (void) strcat(cdesc, "breathes frost.");
	  msg_print(cdesc);
	  breath(GF_FROST, char_row, char_col,
		 ((m_ptr->hp / 3)>1600?1600:(m_ptr->hp / 3)), ddesc, monptr);
	  break;
	case 24:  /*Breath Fire	 */
	  (void) strcat(cdesc, "breathes fire.");
	  msg_print(cdesc);
	  breath(GF_FIRE, char_row, char_col,
		 ((m_ptr->hp / 3)>1600?1600:(m_ptr->hp / 3)), ddesc, monptr);
	  break;
	case 25:  /* Fire Bolt */
	  (void) strcat(cdesc, "casts a Fire bolt.");
	  msg_print(cdesc);
	  bolt(GF_FIRE, char_row, char_col,
		 damroll(9,8)+(c_list[m_ptr->mptr].level/3)
		 , ddesc, m_ptr, monptr);
	  break;
	case 26:  /* Frost Bolt */
	  (void) strcat(cdesc, "casts a Frost bolt.");
	  msg_print(cdesc);
	  bolt(GF_FROST, char_row, char_col,
		 damroll(6,8)+(c_list[m_ptr->mptr].level/3)
		 , ddesc, m_ptr, monptr);
	  break;
	case 27:  /* Acid Bolt */
	  (void) strcat(cdesc, "casts a Acid bolt.");
	  msg_print(cdesc);
	  bolt(GF_ACID, char_row, char_col,
		 damroll(7,8)+(c_list[m_ptr->mptr].level/3)
		 , ddesc, m_ptr, monptr);
	  break;
	case 28:  /* Magic Missiles */
	  (void) strcat(cdesc, "casts a Magic missile.");
	  msg_print(cdesc);
	  bolt(GF_MAGIC_MISSILE, char_row, char_col,
		 damroll(2,6)+(c_list[m_ptr->mptr].level/3)
		 , ddesc, m_ptr, monptr);
	  break;
	case 29:	 /*Critical Wound	 */
	  (void) strcat(cdesc, "points at you, incanting terribly!");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You resist the effects of the spell.");
	  else
	    take_hit(damroll(10, 15), ddesc);
	  break;
	case 30:  /* Fire Ball */
	  (void) strcat(cdesc, "casts a Fire ball.");
	  msg_print(cdesc);
	  breath(GF_FIRE, char_row, char_col,
		 randint((c_list[m_ptr->mptr].level*7)/2)+10, ddesc, monptr);
	  break;
	case 31:  /* Frost Ball */
	  (void) strcat(cdesc, "casts a Frost ball.");
	  msg_print(cdesc);
	  breath(GF_FROST, char_row, char_col,
		 randint((c_list[m_ptr->mptr].level*3)/2)+10, ddesc, monptr);
	  break;
	case 32:  /* Mana Bolt */
	  (void) strcat(cdesc, "casts a Mana bolt.");
	  msg_print(cdesc);
	  bolt(GF_MAGIC_MISSILE, char_row, char_col,
	       randint((c_list[m_ptr->mptr].level*7)/2)+50, ddesc, m_ptr,
	       monptr);
	  break;
	case 33:
	  (void) strcat(cdesc, "breathes Chaos.");
	  msg_print(cdesc);
	  if (py.flags.chaos_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	 	  ((m_ptr->hp/6)>400?400:(m_ptr->hp/6)), ddesc, monptr);
	  else if (py.flags.nether_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		  ((m_ptr->hp/6)>450?450:(m_ptr->hp/6)), ddesc, monptr);
          else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	           ((m_ptr->hp/6)>600?600:(m_ptr->hp/6)), ddesc, monptr);
	    f_ptr = &py.flags;
	    if (f_ptr->hold_life && randint(3)>1)
	      msg_print("You keep hold of your life force!");
	    else {
	      msg_print("You feel your life draining away!");
	      lose_exp(5000+(py.misc.exp / 100)*MON_DRAIN_LIFE);
	    }
	    if ((!py.flags.confusion_resist) && (!py.flags.chaos_resist))
	      {
                if (py.flags.confused > 0)
	          py.flags.confused += 12;
	        else
	          py.flags.confused = randint(20) + 10;
	     }
	    py.flags.image += randint(10);
	    (void) chaos(m_ptr);
	  }
          break;
	case 34:
	  (void) strcat(cdesc, "breathes shards.");
	  msg_print(cdesc);
	  if (py.flags.shards_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	          ((m_ptr->hp/6)>267?267:(m_ptr->hp/6)), ddesc, monptr);
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
                  ((m_ptr->hp/6)>400?400:(m_ptr->hp/6)), ddesc, monptr);
            cut_player(m_ptr->hp/3);
	  }
	  break;
	case 35:
	  (void) strcat(cdesc, "breathes sound.");
	  msg_print(cdesc);
	  if (py.flags.sound_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	   	 ((m_ptr->hp / 6)>200?200:(m_ptr->hp / 6)), ddesc, monptr);
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp / 6)>400?400:(m_ptr->hp / 6)), ddesc, monptr);
           stun_player(randint((((m_ptr->hp/20)>30)? 35 : (m_ptr->hp/20))+5));
          }
          break;
	case 36:
	  (void) strcat(cdesc, "breathes confusion.");
	  msg_print(cdesc);
	  if ((py.flags.confusion_resist) || (py.flags.chaos_resist))
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp / 6)>200?200:(m_ptr->hp / 6)), ddesc, monptr);
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp / 6)>400?400:(m_ptr->hp / 6)), ddesc, monptr);
            if (py.flags.confused > 0)
	      py.flags.confused += 12;
	    else
	      py.flags.confused = randint(20) + 10;
          }
          break;
	case 37:
	  (void) strcat(cdesc, "breathes disenchantment.");
	  msg_print(cdesc);
	  if (py.flags.disenchant_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
                   ((m_ptr->hp / 6)>334?334:(m_ptr->hp / 6)), ddesc, monptr);
	  else {
            disenchant = FALSE;
	    switch(randint(7))
	      {
	      case 1: i = INVEN_BODY; break;
              case 2: i = INVEN_BODY;  break;
              case 3: i = INVEN_ARM;   break;
	      case 4: i = INVEN_OUTER; break;
	      case 5: i = INVEN_HANDS; break;
	      case 6: i = INVEN_HEAD;  break;
	      case 7: i = INVEN_FEET;  break;
	      }
	    i_ptr = &inventory[i];
	      if (i_ptr->tohit > 0) {
	        i_ptr->tohit -= randint(2);
	        /* don't send it below zero */
	        if (i_ptr->tohit < 0)
	          i_ptr->tohit = 0;
	        disenchant = TRUE;
	      }
              if (i_ptr->todam > 0) {
	        i_ptr->todam -= randint(2);
	        /* don't send it below zero */
	        if (i_ptr->todam < 0)
	          i_ptr->todam = 0;
	        disenchant = TRUE;
	      }
	      if (i_ptr->toac > 0) {
	        i_ptr->toac  -= randint(2);
	        /* don't send it below zero */
	        if (i_ptr->toac < 0)
		  i_ptr->toac = 0;
	        disenchant = TRUE;
	      }
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	           ((m_ptr->hp / 6)>500?500:(m_ptr->hp / 6)), ddesc, monptr);
	    if (disenchant)
	    {
	      msg_print("There is a static feeling in the air.");
	      calc_bonuses ();
	    }
	  }
         break;
	case 38:
	  (void) strcat(cdesc, "breathes nether.");
	  msg_print(cdesc);
	  if (py.flags.nether_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		  ((m_ptr->hp/6)>300?300:(m_ptr->hp/6)), ddesc, monptr);
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		   ((m_ptr->hp/6)>400?400:(m_ptr->hp/6)), ddesc, monptr);
	    f_ptr = &py.flags;
	    if (f_ptr->hold_life && randint(4)>1)
	      msg_print("You keep hold of your life force!");
	    else {
	      msg_print("You feel your life draining away!");
	      lose_exp((m_ptr->hp/3)+(py.misc.exp / 100)*MON_DRAIN_LIFE);
	    }
	  }
	  break;
	case 39:
	  (void) strcat(cdesc, "casts a Lightning bolt.");
	  msg_print(cdesc);
	  bolt(GF_LIGHTNING, char_row, char_col,
		 damroll(4,8)+(c_list[m_ptr->mptr].level/3)
		 , ddesc, m_ptr, monptr);
	  break;
	case 40:
	  (void) strcat(cdesc, "casts a Lightning ball.");
	  msg_print(cdesc);
	  breath(GF_LIGHTNING, char_row, char_col,
		 randint((c_list[m_ptr->mptr].level*3)/2)+8, ddesc, monptr);
	  break;
	case 41:
	  (void) strcat(cdesc, "casts an Acid ball.");
	  msg_print(cdesc);
	  breath(GF_ACID, char_row, char_col,
		 randint(c_list[m_ptr->mptr].level*3)+15, ddesc, monptr);
	  break;
	case 42:
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  (void) trap_creation();
	  break;
	case 43:
	  (void) strcat(cdesc, "points at you, screaming the word DIE!");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You laugh at the feeble spell.");
	  else {
	    msg_print("You start to bleed!");
            take_hit(damroll(15, 15), ddesc);
	    cut_player(m_ptr->hp);
	  }
	  break;
	case 44:
	  (void) strcat(cdesc, "stares at you.");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You resist the effects.");
	  else {
	    msg_print("Your mind is blasted by psionic energy.");
	    if ((!py.flags.confusion_resist) && (!py.flags.chaos_resist))
	      {
	        if (py.flags.confused > 0)
	          py.flags.confused += 2;
	        else
	          py.flags.confused = randint(5) + 3;
	      }
            take_hit(damroll(8, 8), ddesc);
	  }
	  break;
	case 45:
	  (void) strcat(cdesc, "teleports you away.");
	  msg_print(cdesc);
	  (void) teleport(100);
	  break;
	case 46:
	  (void) sprintf(outval,"%sconcentrates on its wounds.",cdesc);
	  msg_print(outval);
            if (m_ptr->maxhp == 0){ /* then we're just going to fix it! -CFT */
            if ((c_list[m_ptr->mptr].cdefense & MAX_HP) || be_nasty)
              m_ptr->maxhp = max_hp(c_list[m_ptr->mptr].hd);
            else
              m_ptr->maxhp = pdamroll(c_list[m_ptr->mptr].hd);
          }
          if (m_ptr->hp>=m_ptr->maxhp) { /* need >= because, if we recalc-ed
                                           maxhp, we might have gotten a low
                                          roll, which could be below hp -CFT */
	      (void) strcat(cdesc, "looks as healthy as can be.");
	      msg_print(cdesc);
	  } else {
	      m_ptr->hp += (c_list[m_ptr->mptr].level)*6;
	      if (m_ptr->hp > m_ptr->maxhp)
		  m_ptr->hp = m_ptr->maxhp;
	      if (m_ptr->hp==m_ptr->maxhp)
		  (void) strcat(cdesc, "looks REALLY healthy!");
	      else
		  (void) strcat(cdesc, "looks healthier.");
	      msg_print(cdesc);
	  }
          break;
	case 47:
	  (void) sprintf(outval,"%scasts a spell.",cdesc);
	  msg_print(outval);
	  if ((m_ptr->cspeed)<=(c_list[m_ptr->mptr].speed-10+py.flags.speed))
          {
	    if ((c_list[m_ptr->mptr].speed) <= 15)
	    {
	      (void) strcat(cdesc, "starts moving faster.");
	      msg_print(cdesc);
	      m_ptr->cspeed += 1;
	    }
	  }
	  break;
	case 48:
	  (void) strcat(cdesc, "fires missiles at you.");
	  msg_print(cdesc);
	  bolt(GF_MAGIC_MISSILE, char_row, char_col,
		 damroll(6,7), ddesc, m_ptr, monptr);
	  break;
	case 49:
	  (void) strcat(cdesc,"casts a Plasma bolt.");
	  msg_print(cdesc);
	  bolt(GF_MAGIC_MISSILE, char_row, char_col,
		 10+damroll(8,7)+(c_list[m_ptr->mptr].level)
		 , ddesc, m_ptr, monptr);
	  break;
	case 50:
	  (void) strcat(cdesc, "magically summons monsters!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
          for (k=0; k<8; k++) {
	    hack_monptr = monptr;
	    (void) summon_monster(&y, &x, FALSE);
	    hack_monptr = -1;
	    update_mon ((int)cave[y][x].cptr);
	  }
	  break;
        case 51:
	  (void) strcat(cdesc, "casts a Nether bolt.");
	  msg_print(cdesc);
	  if (py.flags.nether_resist)
	  bolt(GF_MAGIC_MISSILE, char_row, char_col,
	        20+damroll(5,3)+((c_list[m_ptr->mptr].level*8)/3),
	        ddesc, m_ptr, monptr);
	  else {
	    int player_hit; /* used to decide if should drain exps. Not
			       needed above, no need with neth res. -CFT */
	    player_hit = bolt(GF_MAGIC_MISSILE, char_row, char_col,
		  30+damroll(5,5)+(c_list[m_ptr->mptr].level/4),
		  ddesc, m_ptr, monptr);
	    if (player_hit) { /* only do 2ndary effects if hit player, not 
				 if hit other monster... -CFT */
	      f_ptr = &py.flags;
	      if (f_ptr->hold_life && randint(5)>1)
	        msg_print("You keep hold of your life force!");
	      else if (f_ptr->hold_life) {
	        msg_print("You feel your life slipping away!");
	        lose_exp(200+(py.misc.exp / 1000)*MON_DRAIN_LIFE);
	      } else {
	        msg_print("You feel your life draining away!");
	        lose_exp(200+(py.misc.exp / 100)*MON_DRAIN_LIFE);
	      }
	    }
	  }
	  break;
	case 52:
	  f_ptr=&py.flags;
	  (void) strcat(cdesc, "casts an Ice bolt.");
	  msg_print(cdesc);
	  { int player_hit; /* control 2ndary effects -CFT */
	    player_hit = bolt(GF_FROST, char_row, char_col,
		 damroll(6,6)+(c_list[m_ptr->mptr].level)
		 , ddesc, m_ptr, monptr);
	    if (player_hit) {
	      take_hit(damroll(8,10), ddesc);
	      if (!py.flags.sound_resist)
                stun_player(randint(25));
	      cut_player(randint(10));
	    }
	  }
	  break;
	case 53:
          (void) strcat(cdesc, "gestures in shadow.");
	  msg_print(cdesc);
	  (void) unlight_area(char_row,char_col);
	  break;
	case 54:
	  (void) strcat(cdesc, "tries to blank your mind.");
	  msg_print(cdesc);
	  if (player_saves() || randint(2)==1)
	    msg_print("You resist the spell.");
	  else if (lose_all_info())
	    msg_print("Your memories fade away.");
	  break;
	case 55:
	  (void) strcat(cdesc, "concentrates and its eyes glow red.");
	  msg_print(cdesc);
	  if (player_saves())
	    msg_print("You avert your gaze!");
	  else {
	    msg_print("Your mind is blasted by psionic energy.");
	    take_hit(damroll(12, 15), ddesc);
	    if ((!py.flags.confusion_resist) && (!py.flags.chaos_resist))
	      {
	        if (py.flags.confused > 0)
	          py.flags.confused += 2;
	        else
	          py.flags.confused = randint(5) + 3;
	      }
            if (!py.flags.free_act) {
	      if (py.flags.paralysis > 0)
	        py.flags.paralysis += 2;
	      else
	        py.flags.paralysis = randint(5)+4;
	      if (py.flags.slow > 0)
	        py.flags.slow += 2;
	      else
	        py.flags.slow = randint(5) + 3;
	    }
	    if (!py.flags.blindness_resist) {
	      if (py.flags.blind > 0)
	        py.flags.blind += 6;
	      else
	        py.flags.blind += 12 + randint(3);
	    }
	  }
	  break;
	case 56:
	  (void) strcat(cdesc, "casts a Stinking cloud.");
	  msg_print(cdesc);
	  breath(GF_POISON_GAS, char_row, char_col,
		 damroll(12,2), ddesc, monptr);
	  break;
	case 57:
	  (void) strcat(cdesc, "gestures at you.");
	  msg_print(cdesc);
	  if ((player_saves()) || (randint(3)!=1) || (py.flags.nexus_resist))
	    msg_print("You keep your feet firmly on the ground.");
	  else {
	    k=dun_level;
	    if (dun_level == Q_PLANE)
	      dun_level=0;
	    else if (is_quest(dun_level))
	      dun_level-=1;
	    else
	      dun_level+=(-3)+2*randint(2);
	    if (dun_level<0) dun_level=0;
	    if (k==Q_PLANE)
	      msg_print("You warp through a cross-dimension gate.");
	    else if (k<dun_level)
	      msg_print("You sink through the floor.");
	    else
	      msg_print("You rise up through the ceiling.");
	    new_level_flag=TRUE;
	  }
	  break;
	case 58:
	  f_ptr=&py.flags;
	  (void) strcat(cdesc, "casts a Water bolt.");
	  msg_print(cdesc);
	  { int player_hit; /* control 2ndary effect -CFT */
	  player_hit = bolt(GF_MAGIC_MISSILE, char_row, char_col,
		 damroll(10,10)+(c_list[m_ptr->mptr].level)
		 , ddesc, m_ptr, monptr);
	  if (player_hit) {
	    if (!py.flags.sound_resist)
              stun_player(randint(15));
	    }
	  }
	  break;
	case 59:
	  f_ptr = &py.flags;
	  (void) strcat(cdesc, "gestures fluidly.");
	  msg_print(cdesc);
	  msg_print("You are engulfed in a whirlpool.");
	  breath(GF_MAGIC_MISSILE, char_row, char_col,
		 randint((c_list[m_ptr->mptr].level*5)/2)+50, ddesc, monptr);
	  if (!py.flags.sound_resist)
	    stun_player(randint(55));
	  if ((!player_saves()) && (!py.flags.confusion_resist)
				&& (!py.flags.chaos_resist)) {
	    if (py.flags.confused > 0)
	      py.flags.confused += 6;
	    else
	      py.flags.confused = randint(8) + 6;
	  }
	  break;
	case 60:
	  (void) strcat(cdesc, "casts a Nether Ball.");
	  msg_print(cdesc);
	  if (py.flags.nether_resist)
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	((34+damroll(10,7)+(c_list[m_ptr->mptr].level)*2)/3), ddesc, monptr);
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	       (50+damroll(10,10)+(c_list[m_ptr->mptr].level)), ddesc, monptr);
	    f_ptr = &py.flags;
	    if (f_ptr->hold_life && randint(2)==1)
	      msg_print("You keep hold of your life force!");
	    else {
	      msg_print("You feel your life draining away!");
	      lose_exp(5000+(py.misc.exp / 100)*MON_DRAIN_LIFE);
	    }
	  }
	  break;
	case 61:
	  (void) strcat(cdesc, "summons an Angel.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  hack_monptr = monptr;
	  (void) summon_angel(&y, &x);
	  hack_monptr = -1;
	  update_mon ((int)cave[y][x].cptr);
	  break;
	case 62:
	  (void) strcat(cdesc, "magically summons Spiders.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  for (k=0; k<6; k++) {
	      hack_monptr = monptr;
	      (void) summon_spider(&y, &x);
	      hack_monptr = -1;
	      update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 63:
	  (void) strcat(cdesc, "magically summons Hounds.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called,it needs monptr */
	  for (k=0; k<8; k++) {
	      hack_monptr = monptr;
	      (void) summon_hound(&y, &x);
	      hack_monptr = -1;
	      update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 64:
	  (void) strcat(cdesc, "breathes Nexus.");
	  msg_print(cdesc);
	  if (py.flags.nexus_resist) {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		  ((m_ptr->hp/3)>167?167:(m_ptr->hp/3)), ddesc, monptr);
	  }
          else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	           ((m_ptr->hp/3)>250?250:(m_ptr->hp/3)), ddesc, monptr);
	    switch (randint(7)) {
	    case 1:
	    case 2:
	    case 3:
	      teleport(200);
	      break;
	    case 4:
	    case 5:
	      teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
	      break;
	    case 6:
	      if (player_saves())
	        msg_print("You resist the effects.");
	      else {
	        k=dun_level;
	        if (dun_level==Q_PLANE)
		  dun_level=0;
	        else if (is_quest(dun_level))
		  dun_level-=1;
	        else
		  dun_level+=(-3)+2*randint(2);
	        if (dun_level<0) dun_level=0;
	        if (k==Q_PLANE)
		  msg_print("You warp through a cross-dimension gate.");
	        else if (k<dun_level)
		  msg_print("You sink through the floor.");
	        else
		  msg_print("You rise up through the ceiling.");
	        new_level_flag=TRUE;
	      }
	      break;
	    case 7:
	      if (player_saves() && randint(2)==1)
	        msg_print("You resist the effects.");
	      else {
	        int max1, cur1, max2, cur2, i, j;
	        msg_print("Your body starts to scramble...");
	        i=randint(6)-1;
	        do {
		  j=randint(6)-1;
	        } while (i==j);
	        max1 = py.stats.max_stat[i];
	        cur1 = py.stats.cur_stat[i];
	        max2 = py.stats.max_stat[j];
	        cur2 = py.stats.cur_stat[j];
	        py.stats.max_stat[i] = max2;
	        py.stats.cur_stat[i] = cur2;
	        py.stats.max_stat[j] = max1;
	        py.stats.cur_stat[j] = cur1;
		set_use_stat(i);
		set_use_stat(j);
	        prt_stat(i);
	        prt_stat(j);
	      }
	    }
	  }
	  break;
	case 65:
	  f_ptr=&py.flags;
	  (void) strcat(cdesc, "breathes elemental force.");
	  msg_print(cdesc);
	  if (randint(10)==1)
	    br_wall(m_ptr->fy, m_ptr->fx);
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		   ((m_ptr->hp/6)>200?200:(m_ptr->hp/6)), ddesc, monptr);
	    if (!py.flags.sound_resist)
		stun_player(randint(20));
	  }
	  break;
	case 66:
	  (void) strcat(cdesc, "breathes inertia.");
	  msg_print(cdesc);
	  breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp/6)>200?200:(m_ptr->hp/6)), ddesc, monptr);
	  msg_print("You feel less able to move.");
	  py.flags.slow = randint(5) + 3;
	  break;
	case 67:
	  (void) strcat(cdesc, "breathes light.");
	  msg_print(cdesc);
 	  if (py.flags.light_resist) {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	           ((m_ptr->hp/6)>200?200:(m_ptr->hp/6)), ddesc, monptr);
	    light_area(char_row, char_col);
	  }
          else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	           ((m_ptr->hp/6)>400?400:(m_ptr->hp/6)), ddesc, monptr);
	    light_area(char_row, char_col);
	    if (!py.flags.blindness_resist) {
	      if (py.flags.blind<=0) {
	        msg_print("You are blinded by the flash!");
	        py.flags.blind+=randint(6)+3;
	      }
	    }
	  }
	  break;
	case 68:
	  (void) strcat(cdesc, "breathes time.");
	  msg_print(cdesc);
	  breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp/3)>150?150:(m_ptr->hp/3)), ddesc, monptr);
	  switch (randint(10)) {
	  case 1:
	  case 2:
	  case 3:
	  case 4:
	  case 5:
	    msg_print("You feel life has clocked back.");
	    lose_exp(m_ptr->hp+(py.misc.exp / 300)*MON_DRAIN_LIFE);
	    break;
	  case 6:
	  case 7:
	  case 8:
	  case 9:
	    switch (randint(6)) {
	    case 1:
	      msg_print("You're not as strong as you used to be...");
	      py.stats.cur_stat[A_STR]=(py.stats.cur_stat[A_STR]*3)/4;
	      if (py.stats.cur_stat[A_STR]<3) py.stats.cur_stat[A_STR]=3;
	      py.stats.use_stat[A_STR]=(py.stats.use_stat[A_STR]*3)/4;
	      if (py.stats.use_stat[A_STR]<3) py.stats.use_stat[A_STR]=3;
	      prt_stat(A_STR);
	      break;
	    case 2:
	      msg_print("You're not as bright as you used to be...");
	      py.stats.cur_stat[A_INT]=(py.stats.cur_stat[A_INT]*3)/4;
	      if (py.stats.cur_stat[A_INT]<3) py.stats.cur_stat[A_INT]=3;
	      py.stats.use_stat[A_INT]=(py.stats.use_stat[A_INT]*3)/4;
	      if (py.stats.use_stat[A_INT]<3) py.stats.use_stat[A_INT]=3;
	      prt_stat(A_INT);
	      break;
	    case 3:
	      msg_print("You're not as wise as you used to be...");
	      py.stats.cur_stat[A_WIS]=(py.stats.cur_stat[A_WIS]*3)/4;
	      if (py.stats.cur_stat[A_WIS]<3) py.stats.cur_stat[A_WIS]=3;
	      py.stats.use_stat[A_WIS]=(py.stats.use_stat[A_WIS]*3)/4;
	      if (py.stats.use_stat[A_WIS]<3) py.stats.use_stat[A_WIS]=3;
	      prt_stat(A_WIS);
	      break;
	    case 4:
	      msg_print("You're not as agile as you used to be...");
	      py.stats.cur_stat[A_DEX]=(py.stats.cur_stat[A_DEX]*3)/4;
	      if (py.stats.cur_stat[A_DEX]<3) py.stats.cur_stat[A_DEX]=3;
	      py.stats.use_stat[A_DEX]=(py.stats.use_stat[A_DEX]*3)/4;
	      if (py.stats.use_stat[A_DEX]<3) py.stats.use_stat[A_DEX]=3;
	      prt_stat(A_DEX);
	      break;
	    case 5:
	      msg_print("You're not as hale as you used to be...");
	      py.stats.cur_stat[A_CON]=(py.stats.cur_stat[A_CON]*3)/4;
	      if (py.stats.cur_stat[A_CON]<3) py.stats.cur_stat[A_CON]=3;
	      py.stats.use_stat[A_CON]=(py.stats.use_stat[A_CON]*3)/4;
	      if (py.stats.use_stat[A_CON]<3) py.stats.use_stat[A_CON]=3;
	      prt_stat(A_CON);
	      break;
	    case 6:
	      msg_print("You're not as beautiful as you used to be...");
	      py.stats.cur_stat[A_CHR]=(py.stats.cur_stat[A_CHR]*3)/4;
	      if (py.stats.cur_stat[A_CHR]<3) py.stats.cur_stat[A_CHR]=3;
	      py.stats.use_stat[A_CHR]=(py.stats.use_stat[A_CHR]*3)/4;
	      if (py.stats.use_stat[A_CHR]<3) py.stats.use_stat[A_CHR]=3;
	      prt_stat(A_CHR);
	      break;
	    }
	    break;
	  case 10:
	    msg_print("You're not as strong as you used to be...");
	    py.stats.cur_stat[A_STR]=(py.stats.cur_stat[A_STR]*3)/4;
	    if (py.stats.cur_stat[A_STR]<3) py.stats.cur_stat[A_STR]=3;
	    py.stats.use_stat[A_STR]=(py.stats.use_stat[A_STR]*3)/4;
	    if (py.stats.use_stat[A_STR]<3) py.stats.use_stat[A_STR]=3;
	    prt_stat(A_STR);
	    msg_print("You're not as bright as you used to be...");
	    py.stats.cur_stat[A_INT]=(py.stats.cur_stat[A_INT]*3)/4;
	    if (py.stats.cur_stat[A_INT]<3) py.stats.cur_stat[A_INT]=3;
	    py.stats.use_stat[A_INT]=(py.stats.use_stat[A_INT]*3)/4;
	    if (py.stats.use_stat[A_INT]<3) py.stats.use_stat[A_INT]=3;
	    prt_stat(A_INT);
	    msg_print("You're not as wise as you used to be...");
	    py.stats.cur_stat[A_WIS]=(py.stats.cur_stat[A_WIS]*3)/4;
	    if (py.stats.cur_stat[A_WIS]<3) py.stats.cur_stat[A_WIS]=3;
	    py.stats.use_stat[A_WIS]=(py.stats.use_stat[A_WIS]*3)/4;
	    if (py.stats.use_stat[A_WIS]<3) py.stats.use_stat[A_WIS]=3;
	    prt_stat(A_WIS);
	    msg_print("You're not as agile as you used to be...");
	    py.stats.cur_stat[A_DEX]=(py.stats.cur_stat[A_DEX]*3)/4;
	    if (py.stats.cur_stat[A_DEX]<3) py.stats.cur_stat[A_DEX]=3;
	    py.stats.use_stat[A_DEX]=(py.stats.use_stat[A_DEX]*3)/4;
	    if (py.stats.use_stat[A_DEX]<3) py.stats.use_stat[A_DEX]=3;
	    prt_stat(A_DEX);
	    msg_print("You're not as hale as you used to be...");
	    py.stats.cur_stat[A_CON]=(py.stats.cur_stat[A_CON]*3)/4;
	    if (py.stats.cur_stat[A_CON]<3) py.stats.cur_stat[A_CON]=3;
	    py.stats.use_stat[A_CON]=(py.stats.use_stat[A_CON]*3)/4;
	    if (py.stats.use_stat[A_CON]<3) py.stats.use_stat[A_CON]=3;
	    prt_stat(A_CON);
	    msg_print("You're not as beautiful as you used to be...");
	    py.stats.cur_stat[A_CHR]=(py.stats.cur_stat[A_CHR]*3)/4;
	    if (py.stats.cur_stat[A_CHR]<3) py.stats.cur_stat[A_CHR]=3;
	    py.stats.use_stat[A_CHR]=(py.stats.use_stat[A_CHR]*3)/4;
	    if (py.stats.use_stat[A_CHR]<3) py.stats.use_stat[A_CHR]=3;
	    prt_stat(A_CHR);
	    break;
	  }
	  break;
	case 69: /* gravity */
	  f_ptr=&py.flags;
	  (void) strcat(cdesc, "breathes gravity.");
	  msg_print(cdesc);
	  if (!py.flags.sound_resist)
	      stun_player(randint((m_ptr->hp/8)>35? 35 : ((m_ptr->hp/8)<=0)?
			      1 : (m_ptr->hp/8)));
	  breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp/3)>200?200:(m_ptr->hp/3)), ddesc, monptr);
	  py.flags.slow = randint(5) + 3;
	  msg_print("Gravity warps around you.");
	  teleport(5);
	  break;
	case 70: /* darkness */
	  (void) strcat(cdesc, "breathes darkness.");
	  msg_print(cdesc);
	  if (py.flags.dark_resist) {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		  ((m_ptr->hp/6)>200?200:(m_ptr->hp/6)), ddesc, monptr);
	    unlight_area(char_row, char_col);
	  }
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
		  ((m_ptr->hp/6)>400?400:(m_ptr->hp/6)), ddesc, monptr);
	    unlight_area(char_row, char_col);
	  }
          break;
	case 71: /* plasma */
	  f_ptr=&py.flags;
	  (void) strcat(cdesc, "breathes plasma.");
	  msg_print(cdesc);
	  breath(GF_MAGIC_MISSILE, char_row, char_col,
		 ((m_ptr->hp/6)>150?150:(m_ptr->hp/6)), ddesc, monptr);
	  if (!py.flags.sound_resist)
	      stun_player(randint((m_ptr->hp/8)>35? 35 : ((m_ptr->hp/8)<=0)?
			      1 : (m_ptr->hp/8)));
	  break;
	case 72:
	  (void) strcat(cdesc, "fires an arrow at you.");
	  msg_print(cdesc);
	  bolt(GF_MAGIC_MISSILE, char_row, char_col,
		 damroll(1,6), ddesc, m_ptr, monptr);
	  break;
	case 73:
  	  (void) strcat(cdesc, "magically summons mighty undead opponents.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called, it needs monptr */
	  for (k=0; k<10; k++) {
	      hack_monptr = monptr;
	      (void) summon_wraith(&y, &x);
	      hack_monptr = -1;
	      update_mon ((int)cave[y][x].cptr);
	  }
	  for (k=0; k<7; k++) {
	      hack_monptr = monptr;
	      (void) summon_gundead(&y, &x);
	      hack_monptr = -1;
	      update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 74: /* Big darkness storm */
	  (void) strcat(cdesc, "casts a darkness storm.");
	  msg_print(cdesc);
	  if (py.flags.dark_resist) {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
	          ((m_ptr->hp/6)>250?250:(m_ptr->hp/6)), ddesc, monptr);
	    unlight_area(char_row, char_col);
	  }
	  else {
	    breath(GF_MAGIC_MISSILE, char_row, char_col,
                  ((m_ptr->hp/6)>500?500:(m_ptr->hp/6)), ddesc, monptr);
            unlight_area(char_row, char_col);
          }
          break;
	case 75: /* Mana storm */
	  (void) strcat(cdesc, "invokes a Mana storm.");
	  msg_print(cdesc);
	  breath(GF_MAGIC_MISSILE, char_row, char_col,
	    	(c_list[m_ptr->mptr].level*5)+damroll(10,10),
		ddesc, monptr);
	  break;
	case 76: /* Summon reptiles */
	  (void) strcat(cdesc, "magically summons reptiles.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called, it needs monptr */
	  for (k=0; k<8; k++) {
	      hack_monptr = monptr;
	      (void) summon_reptile(&y, &x);
	      hack_monptr = -1;
	      update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 77: /* Summon ants */
	  (void) strcat(cdesc, "magically summons ants.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called, it needs monptr */
	  for (k=0; k<7; k++) {
	    hack_monptr = monptr;
	    (void) summon_ant(&y, &x);
	    hack_monptr = -1;
	    update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 78: /* Summon unique monsters */
	  (void) strcat(cdesc, "summons special opponents!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called, it needs monptr */
	  for (k=0; k<5; k++) {
	    hack_monptr = monptr;
	    (void) summon_unique(&y, &x);
	    hack_monptr = -1;
	    update_mon ((int)cave[y][x].cptr);
	  }
	  for (k=0; k<4; k++) {
	    hack_monptr = monptr;
	    (void) summon_jabberwock(&y, &x);
	    hack_monptr = -1;
	    update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 79: /* Summon greater undead */
	  (void) strcat(cdesc, "summons the DEAD!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called, it needs monptr */
	  for (k=0; k<8; k++) {
	    hack_monptr = monptr;
	    (void) summon_gundead(&y, &x);
	    hack_monptr = -1;
	    update_mon ((int)cave[y][x].cptr);
	  }
	  break;
	case 80: /* Summon ancient dragons */
	  (void) strcat(cdesc, "summons Ancient Dragons.");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  /* in case compact_monster() is called, it needs monptr */
	  for (k=0; k<5; k++) {
	    hack_monptr = monptr;
	    (void) summon_ancientd(&y, &x);
	    hack_monptr = -1;
	    update_mon ((int)cave[y][x].cptr);
	  }
	  break;
        default:
	  if (k!=200)
	    (void) strcat (cdesc, "casts a bugged spell.");
	  else
	    (void) strcat (cdesc, "had no spell to cast, tell someone NOW!");
	  msg_print(cdesc);
	}
      /* End of spells				       */
        if ((m_ptr->ml) /* this won't work if we've been moved, so... */
        || (thrown_spell == 45) /* add teleport away, */
        || (thrown_spell == 57) /* and teleport lv -CFT */
          ){
	if (thrown_spell<33)
	  c_recall[m_ptr->mptr].r_spells |= 1L << (thrown_spell-1);
	else if (thrown_spell<65)
          c_recall[m_ptr->mptr].r_spells2 |= 1L << (thrown_spell-33);
        else if (thrown_spell<97) /* in case of 200 for bugs... -CFT */
          c_recall[m_ptr->mptr].r_spells3 |= 1L << (thrown_spell-65);
	if ((c_recall[m_ptr->mptr].r_spells & CS_FREQ) != CS_FREQ)
	  c_recall[m_ptr->mptr].r_spells++;
	if (death && c_recall[m_ptr->mptr].r_deaths < MAX_SHORT)
	  c_recall[m_ptr->mptr].r_deaths++;
      }
    }
}


/* Places creature adjacent to given location		-RAK-	*/
/* Rats and Flys are fun!					 */
int multiply_monster(y, x, cr_index, monptr)
int y, x, cr_index;
int monptr;
{
  register int i, j, k;
  register cave_type *c_ptr;

  i = 0;
  do
    {
      j = y - 2 + randint(3);
      k = x - 2 + randint(3);
      /* don't create a new creature on top of the old one, that causes
	 invincible/invisible creatures to appear */
      if (in_bounds(j, k) && (j != y || k != x))
	{
	  c_ptr = &cave[j][k];
	  if ((c_ptr->fval <= MAX_OPEN_SPACE) && (c_ptr->tptr == 0) &&
	      (c_ptr->cptr != 1))
	    {
	      if (c_ptr->cptr > 1)     /* Creature there already?	*/
		{
		  /* Some critters are cannibalistic!	    */
		  if ((c_list[cr_index].cmove & CM_EATS_OTHER)
		      /* Check the experience level -CJS- */
		     && c_list[cr_index].mexp >=
		      c_list[m_list[c_ptr->cptr].mptr].mexp)
		    {
		      /* It ate an already processed monster.Handle normally.*/
		      if (monptr < c_ptr->cptr)
			delete_monster((int) c_ptr->cptr);
		      /* If it eats this monster, an already processed mosnter
			 will take its place, causing all kinds of havoc.
			 Delay the kill a bit. */
		      else
			fix1_delete_monster((int) c_ptr->cptr);

		      /* in case compact_monster() is called,it needs monptr */
		      hack_monptr = monptr;
		      place_monster(j, k, cr_index, FALSE);
		      hack_monptr = -1;
		      mon_tot_mult++;
		      return check_mon_lite(j, k);
		    }
		}
	      else
		/* All clear,  place a monster	  */
		{
		  /* in case compact_monster() is called,it needs monptr */
		  hack_monptr = monptr;
		  place_monster(j, k, cr_index, FALSE);
		  hack_monptr = -1;
		  mon_tot_mult++;
		  return check_mon_lite(j, k);
		}
	    }
	}
      i++;
    }
  while (i <= 18);
  return FALSE;
}


/* Move the critters about the dungeon			-RAK-	*/
static void mon_move(monptr, rcmove)
int monptr;
int32u *rcmove;
{
  register int i, j;
  int k, move_test, dir;
  register creature_type *r_ptr;
  register monster_type *m_ptr;
  int mm[9];

  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];
  /* Does the critter multiply?				   */
  if ((r_ptr->cmove & CM_MULTIPLY) && (MAX_MON_MULT >= mon_tot_mult) &&
      (((py.flags.rest!=-1) && ((py.flags.rest % MON_MULT_ADJ) == 0)) ||
	((py.flags.rest==-1) && (randint(MON_MULT_ADJ)==1))))
    {
      k = 0;
      for (i = m_ptr->fy-1; i <= m_ptr->fy+1; i++)
	for (j = m_ptr->fx-1; j <= m_ptr->fx+1; j++)
	  if (in_bounds(i, j) && (cave[i][j].cptr > 1))
	    k++;
      /* can't call randint with a value of zero, increment counter
	 to allow creature multiplication */
      if (k == 0)
	k++;
      if ((k < 4) && (randint(k*MON_MULT_ADJ) == 1))
	if (multiply_monster((int)m_ptr->fy, (int)m_ptr->fx,
			 (int)m_ptr->mptr, monptr))
	  *rcmove |= CM_MULTIPLY;
    }
  move_test = FALSE;

  /* if in wall, must immediately escape to a clear area */
  if (!(r_ptr->cmove & CM_PHASE) &&
      (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL))
    {
      k = 0;
      dir = 1;
      /* note direction of for loops matches direction of keypad from 1 to 9 */
      /* do not allow attack against the player */
      for (i = m_ptr->fy+1; i >= m_ptr->fy-1; i--)
	for (j = m_ptr->fx-1; j <= m_ptr->fx+1; j++)
	  {
	    if ((dir != 5) && (cave[i][j].fval <= MAX_OPEN_SPACE)
/**/		&& (cave[i][j].cptr != 1))
	      mm[k++] = dir;
	    dir++;
	  }
      if (k != 0)
	{
	  /* put a random direction first */
	  dir = randint (k) - 1;
	  i = mm[0];
	  mm[0] = mm[dir];
	  mm[dir] = i;
	  make_move (monptr, mm, rcmove);
	  /* this can only fail if mm[0] has a rune of protection */
	}
      /* if still in a wall, let it dig itself out, but also apply some
	 more damage */
      if (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL)
	{
	  /* in case the monster dies, may need to call fix1_delete_monster()
	     instead of delete_monsters() */
	  hack_monptr = monptr;
	  i = mon_take_hit(monptr, damroll (8, 8));
	  hack_monptr = -1;
	  if (i >= 0)
	    {
	      msg_print("You hear a scream muffled by rock!");
	      /* prt_experience(); */
	    }
	  else
	    {
	      msg_print ("A creature digs itself out from the rock!");
	      (void) twall ((int)m_ptr->fy, (int)m_ptr->fx, 1, 0);
	    }
	}
      return;  /* monster movement finished */
    }
  /* Creature is confused?  Chance it becomes un-confused  */
  else if (m_ptr->confused)
    {
      mm[0] = randint(9);
      mm[1] = randint(9);
      mm[2] = randint(9);
      mm[3] = randint(9);
      mm[4] = randint(9);
      /* don't move him if he is not supposed to move! */
      if (!(r_ptr->cmove & CM_ATTACK_ONLY))
	make_move(monptr, mm, rcmove);
      if (randint(8) == 1)
	m_ptr->confused = FALSE;
      move_test = TRUE;
    }
  /* Creature may cast a spell */
  else if (r_ptr->spells != 0) {
    mon_cast_spell(monptr, &move_test);
  }
  if (!move_test)
    {
      /* 75% random movement */
      if ((r_ptr->cmove & CM_75_RANDOM) && (randint(100) < 75))
	{
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  *rcmove |= CM_75_RANDOM;
	  make_move(monptr, mm, rcmove);
	}
      /* 40% random movement */
      else if ((r_ptr->cmove & CM_40_RANDOM) && (randint(100) < 40))
	{
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  *rcmove |= CM_40_RANDOM;
	  make_move(monptr, mm, rcmove);
	}
      /* 20% random movement */
      else if ((r_ptr->cmove & CM_20_RANDOM) && (randint(100) < 20))
	{
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  *rcmove |= CM_20_RANDOM;
	  make_move(monptr, mm, rcmove);
	}
      /* Normal movement */
      else if (r_ptr->cmove & CM_MOVE_NORMAL)
	{
	  if (randint(200) == 1)
	    {
	      mm[0] = randint(9);
	      mm[1] = randint(9);
	      mm[2] = randint(9);
	      mm[3] = randint(9);
	      mm[4] = randint(9);
	    }
	  else
	    get_moves(monptr, mm);
	  *rcmove |= CM_MOVE_NORMAL;
	  make_move(monptr, mm, rcmove);
	}
      /* Attack, but don't move */
      else if ((r_ptr->cmove & CM_ATTACK_ONLY) && (m_ptr->cdis < 2))
	{
	  get_moves(monptr, mm);
	  *rcmove |= CM_ATTACK_ONLY;
	  make_move(monptr, mm, rcmove);
	}
      else if ((r_ptr->cmove & CM_ALL_MV_FLAGS) == 0 && (m_ptr->cdis < 2))
	{
	  /* little hack for Quylthulgs, so that will eventually notice that
	     they have no physical attacks */
	  if (c_recall[m_ptr->mptr].r_attacks[0] < MAX_UCHAR)
	    c_recall[m_ptr->mptr].r_attacks[0]++;
	}
    }
}


/* Creatures movement and attacking are done from here	-RAK-	*/
void creatures(attack)
int attack;
{
  register int i, k;
  register monster_type *m_ptr;
  recall_type *r_ptr;
  int32u notice, rcmove;
  int wake, ignore;
  vtype cdesc;

  /* Process the monsters  */
  for (i = mfptr - 1; i >= MIN_MONIX && !death; i--)
    {
      m_ptr = &m_list[i];
      /* Get rid of an eaten/breathed on monster.  Note: Be sure not to
	 process this monster. This is necessary because we can't delete
	 monsters while scanning the m_list here. */
      if (m_ptr->hp < 0)
	{
          check_unique(m_ptr);
	  fix2_delete_monster(i);
	  continue;
	}

      m_ptr->cdis = distance(char_row, char_col,
			     (int)m_ptr->fy, (int)m_ptr->fx);
      if (attack)   /* Attack is argument passed to CREATURE*/
	{
	  k = movement_rate(m_ptr->cspeed);
	  if (k <= 0)
	    update_mon(i);
	  else
	    while (k > 0)
	      {
		k--;
		wake = FALSE;
		ignore = FALSE;
		rcmove = 0;
		if (m_ptr->ml || (m_ptr->cdis <= c_list[m_ptr->mptr].aaf))
		  {
		    if (m_ptr->csleep > 0)
		      if (py.flags.aggravate)
			m_ptr->csleep = 0;
		      else if ((py.flags.rest == 0 && py.flags.paralysis < 1)
			       || (randint(50) == 1))
			{
			  notice = randint(1024);
			  if ((notice*notice*notice) <= (1L << (29 - py.misc.stl)))
			    {
			      m_ptr->csleep -= (100 / m_ptr->cdis);
			      if (m_ptr->csleep > 0)
				ignore = TRUE;
			      else
				{
				  wake = TRUE;
				  /* force it to be exactly zero */
				  m_ptr->csleep = 0;
				}
			    }
			}
		    if (m_ptr->stunned != 0)
		      {
			/* NOTE: Balrog = 100*100 = 10000, it always
			   recovers instantly */
			if (randint(5000) < c_list[m_ptr->mptr].level
			    * c_list[m_ptr->mptr].level)
			  m_ptr->stunned = 0;
			else
			  m_ptr->stunned--;
			if (m_ptr->stunned == 0)
			  {
			    if (!m_ptr->ml)
			      (void) strcpy(cdesc, "It ");
			    else if (c_list[m_ptr->mptr].cdefense & UNIQUE)
			      (void) sprintf(cdesc, "%s ",
					     c_list[m_ptr->mptr].name);
                            else (void) sprintf(cdesc, "The %s ",
					     c_list[m_ptr->mptr].name);
			    msg_print(strcat(cdesc,
					     "recovers and glares at you."));
			  }
		      }
		    if ((m_ptr->csleep == 0) && (m_ptr->stunned == 0))
		      mon_move (i, &rcmove);
		  }
		update_mon(i);
		if (m_ptr->ml)
		  {
		    r_ptr = &c_recall[m_ptr->mptr];
		    if (wake)
		      {
			if (r_ptr->r_wake < MAX_UCHAR)
			  r_ptr->r_wake++;
		      }
		    else if (ignore)
		      {
			if (r_ptr->r_ignore < MAX_UCHAR)
			  r_ptr->r_ignore++;
		      }
		    r_ptr->r_cmove |= rcmove;
		  }
	      }
	}
      else
	update_mon(i);

      /* Get rid of an eaten/breathed on monster.  This is necessary because
	 we can't delete monsters while scanning the m_list here.  This
	 monster may have been killed during mon_move(). */
      if (m_ptr->hp < 0)
	{
          check_unique(m_ptr);
	  fix2_delete_monster(i);
	  continue;
	}
    }
  /* End processing monsters	   */
}

/* This is a fun one.  In a given block, pick some walls and	*/
/* turn them into open spots.  Pick some open spots and turn	 */
/* them into walls.  An "Earthquake" effect.	       -LVB-   */
static shatter_quake(mon_y, mon_x)
  int mon_y;
  int mon_x;
{
  register int i, j, k, l;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int kill, damage, tmp, y, x;
  vtype out_val, m_name;

  for (i = mon_y-8; i <= mon_y+8; i++)
    for (j = mon_x-8; j <= mon_x+8; j++)
      if (in_bounds(i, j) && (randint(8) == 1))
	{
	  if ((i==mon_y) && (j==mon_x)) continue;
	  c_ptr = &cave[i][j];
	  if (c_ptr->tptr != 0)
            if ((t_list[c_ptr->tptr].tval >= TV_MIN_WEAR) &&
                (t_list[c_ptr->tptr].tval <= TV_MAX_WEAR) &&
                (t_list[c_ptr->tptr].flags2 & TR_ARTIFACT))
              continue; /* don't kill artifacts... */
            else
	    (void) delete_object(i, j);
	  if (c_ptr->cptr>1) {
	    m_ptr = &m_list[c_ptr->cptr];
	    r_ptr = &c_list[m_ptr->mptr];

	    if (!(r_ptr->cmove&CM_PHASE) && !(r_ptr->cdefense&BREAK_WALL)) {
	      if ((movement_rate (m_ptr->cspeed) == 0) ||
		  (r_ptr->cmove & CM_ATTACK_ONLY))
		/* monster can not move to escape the wall */
		kill = TRUE;
	      else {
		/* only kill if there is nowhere for the monster to
		   escape to */
		kill = TRUE;
		for (y = i-1; y <= i+1; y++)
		  for (x = j-1; x <= j+1; x++)
		    if (cave[y][x].fval >= MIN_CLOSED_SPACE)
		      kill = FALSE;
	      }
	      if (kill)
		damage = 3000;  /* this will kill everything */
	      else
		damage = damroll (4, 8);
	      monster_name (m_name, m_ptr, r_ptr);
	      (void) sprintf (out_val, "%s wails out in pain!", m_name);
	      msg_print (out_val);
	      i = mon_take_hit((int)c_ptr->cptr, damage);
	      if (i >= 0) {
		(void) sprintf (out_val, "%s is embedded in the rock.",
				m_name);
		msg_print (out_val);
	      }
	    }
	  } else if (c_ptr->cptr == 1) { /* Kill the dumb player! */
	    kill = TRUE;
	    for (y = i-1; y <= i+1; y++) {
	      for (x = j-1; x <= j+1; x++) {
		if ((cave[y][x].fval>=MIN_CLOSED_SPACE) &&
		    (cave[y][x].cptr==0)) {
		  kill = FALSE;
		  break;
		}
	      }
	      if (!kill) break;
	    }

	    switch (randint(3)) {
	    case 1:
	      msg_print("The cave ceiling collapses!");
	      break;
	    case 2:
	      msg_print("The floor turns and crushes you!");
	      break;
	    case 3:
	      msg_print("You are pummeled with debris!");
	      break;
	    }
	    if (kill) {
	      msg_print("You are trapped, crushed and cannot move!");
	      damage = 320;
	    } else {
	      switch(randint(3)) {
	      case 1:
		msg_print("The rubble bashes you!");
		damage = damroll(10,4);
		stun_player(randint(50));
		break;
	      case 2:
		msg_print("But you nimbly dodge the blast!");
		damage = 0;
		break;
	      case 3:
		msg_print("The floor crushes you against the ceiling!");
		damage = damroll(10,4);
		stun_player(randint(50));
		break;
	      }
	      move_rec(char_row, char_col, y, x);
	      for (k = char_row-1; k <= char_row+1; k++)
		for (l = char_col-1; l <= char_col+1; l++) {
		  c_ptr = &cave[k][l];
		  c_ptr->tl = FALSE;
		  lite_spot(k, l);
		}
	      lite_spot(char_row, char_col);
	      char_row = y;
	      char_col = x;
	      check_view();
	      /* light creatures */
	      creatures(FALSE);
	    }
	    take_hit(damage,"an Earthquake");
	  }

	  if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL))            {
	    c_ptr->fval  = CORR_FLOOR;
	    c_ptr->pl = FALSE;
	    c_ptr->fm = FALSE;
	  }
	  else if (c_ptr->fval <= MAX_CAVE_FLOOR) {
	    tmp = randint(10);
	    if (tmp < 6)
	      c_ptr->fval  = QUARTZ_WALL;
	    else if (tmp < 9)
	      c_ptr->fval  = MAGMA_WALL;
	    else
	      c_ptr->fval  = GRANITE_WALL;

	    c_ptr->fm = FALSE;
	  }
	  lite_spot(i, j);
	}
}

/* This is a fun one.  In a given block, pick some walls and	*/
/* turn them into open spots.  Pick some open spots and turn	 */
/* them into walls.  An "Earthquake" effect.	       -LVB-   */
static br_wall(mon_y, mon_x)
  int mon_y;
  int mon_x;
{
  register int i, j, k, l;
  register cave_type *c_ptr;
  int kill, damage, tmp, y, x;
  vtype out_val;

  kill = TRUE;
  for (y = char_row-1; y <= char_row+1; y++) {
    for (x = char_col-1; x <= char_col+1; x++) {
      if ((cave[y][x].fval<=MAX_OPEN_SPACE) &&
	  (cave[y][x].cptr==0) && !(y==char_row && x==char_col)) {
	    kill = FALSE;
	    break;
	  }
    }
    if (!kill) break;
  }

  switch (randint(3)) {
  case 1:
    msg_print("The cave ceiling collapses!");
    break;
  case 2:
    msg_print("The floor turns and crushes you!");
    break;
  case 3:
    msg_print("You are pummeled with debris!");
    break;
  }
  if (kill) {
    msg_print("You are trapped, crushed and cannot move!");
    damage = 250;
  } else {
    switch(randint(3)) {
    case 1:
      msg_print("The rubble bashes you!");
      damage = damroll(10,4);
      stun_player(randint(50));
      break;
    case 2:
      msg_print("But you nimbly dodge the blast!");
      damage = 0;
      break;
    case 3:
      msg_print("The floor crushes you against the ceiling!");
      damage = damroll(10,4);
      stun_player(randint(50));
      break;
    }
  }
  c_ptr = &cave[char_row][char_col];
  move_rec(char_row, char_col, y, x);
  if (c_ptr->fval <= MAX_CAVE_FLOOR) {
    tmp = randint(10);
    if (tmp < 6)
      c_ptr->fval  = QUARTZ_WALL;
    else if (tmp < 9)
      c_ptr->fval  = MAGMA_WALL;
    else
      c_ptr->fval  = GRANITE_WALL;

    c_ptr->fm = FALSE;
  }
  for (k = char_row-1; k <= char_row+1; k++)
    for (l = char_col-1; l <= char_col+1; l++) {
      c_ptr = &cave[k][l];
      c_ptr->tl = FALSE;
      lite_spot(k, l);
    }
  lite_spot(char_row, char_col);
  char_row = y;
  char_col = x;
  check_view();
  /* light creatures */
  creatures(FALSE);
  lite_spot(char_row, char_col);
  take_hit(damage,"an Earthquake");
}
