/* The obstacle classes - blocks, walls, power ups.
 *
 * Copyright (c) 2003, 2004 Ole Laursen.
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

#ifndef OBSTACLES_HPP
#define OBSTACLES_HPP

#include <list>

#include "thing.hpp"

class Player;


// blocks can be pushed around
class Block: public Thing
{
public:
  Block();

  void move_block_to(Vector<int> pos);
  void add_cargo(Graphic *graphic);
  void remove_cargo(Graphic *graphic);

  void levitate(int distance);	// in blocks from acting hero
  void unlevitate();

private:
  int levitated;		// number of levitations
  std::list<Graphic *> cargo;
};

// walls cannot be pushed, but are good for splatting hard monsters
class Wall: public Thing
{
public:
  Wall();
};


// monster walls cannot be pushed and spawn monsters
class MonsterWall: public Thing
{
public:
  MonsterWall();

  void update();

private:
  int counter;
  
  int calc_pause_time();
};


// powerups can be picked up
class PowerUp: public Thing
{
protected:
  PowerUp(Graphic *graphic);
  
public:
  void eat(Player &player);
  
private:
  virtual void do_eat(Player &player) = 0;
};

// make all players invisible for a period of time
class InvisibilityPowerUp: public PowerUp 
{
public:
  InvisibilityPowerUp();

private:
  virtual void do_eat(Player &player);
};

// freeze all monsters for a while
class FreezePowerUp: public PowerUp 
{
public:
  FreezePowerUp();

private:
  virtual void do_eat(Player &player);
};

// kill monsters and eggs within range
class ExplodePowerUp: public PowerUp 
{
public:
  ExplodePowerUp();

private:
  virtual void do_eat(Player &player);
};

#endif
