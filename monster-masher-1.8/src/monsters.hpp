/* The monster classes.
 *
 * Copyright (c) 2003 Ole Laursen.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 */

#ifndef MONSTERS_HPP
#define MONSTERS_HPP

#include <memory>

#include "movement.hpp"
#include "vector.hpp"
#include "thing.hpp"
#include "image-cache.hpp"
#include "animation.hpp"

class Player;

// base class for all monsters
class Monster: public Thing
{
public:
  Monster(const Images &move, const Images &gore,
	  int movement_steps, int kill_score, bool hard);
  ~Monster();
  
  void update();
  void splat(Vector<int> dir);
  void explode();
  
  int const kill_score;
  bool const hard;
  
protected:
  // interface for implementation of concrete monsters
  void head(Vector<int> dir);
  void set_thinking_time(int time);
  bool valid_tile(Vector<int> tile);
  Vector<int> get_random_direction();
  Vector<int> find_target();
  Vector<int> find_direction_dumbly(Vector<int> target_tile);
  Vector<int> find_direction_smartly(Vector<int> target_tile);

private:
  Movement movement;
  int thinking_time;
  Animation move_animation;
  const Images &gore;
  Player *prev_target;
  std::auto_ptr<Graphic> freeze_box;

  virtual void do_update() = 0;	// called whenever we aren't moving or thinking
  virtual void do_halfway(Vector<int> prev_tile) {}
};


// ordinary monster, no frills
class PlainMonster: public Monster
{
public:
  PlainMonster();
  
private:
  virtual void do_update();
};

// hard monster, must be mashed agains walls
class HardMonster: public Monster
{
public:
  HardMonster();

private:
  virtual void do_update();
};


// more intelligent monster, knows the shortest path (also around corners)
class SeekerMonster: public Monster
{
public:
  SeekerMonster();
  
private:
  int move_mode;
  
  virtual void do_update();
};

// lays egg, tries to keep away from the battle
class EggLayerMonster: public Monster
{
public:
  EggLayerMonster();
  
private:
  virtual void do_update();
  void do_halfway(Vector<int> prev_tile);

  bool hunting;			// whether we're hunting a hero
};


// egg base class
class Egg: public Thing {
public:
  Egg(Vector<int> pos, const Images &images, int anim_rate, const Images &gore);

  void update();
  bool hatch_ready();
  void hatch();

  void splat(Vector<int> dir);
  void explode();

  static int const kill_score = 1;
  
private:
  Animation anim;
  const Images &gore;

  virtual Monster *do_hatch() = 0;
};


// concrete eggs
class PlainEgg: public Egg
{
public:
  PlainEgg(Vector<int> pos);

private:
  virtual Monster *do_hatch();
};

class HardEgg: public Egg
{
public:
  HardEgg(Vector<int> pos);

private:
  virtual Monster *do_hatch();
};

class SeekerEgg: public Egg
{
public:
  SeekerEgg(Vector<int> pos);

private:
  virtual Monster *do_hatch();
};

class EggLayerEgg: public Egg
{
public:
  EggLayerEgg(Vector<int> pos);

private:
  virtual Monster *do_hatch();
};

#endif
