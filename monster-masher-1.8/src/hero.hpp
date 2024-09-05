/* The Hero class for representing a good guy in the arena.
 *
 * Copyright (c) 2003, 04 Ole Laursen.
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

#ifndef HERO_HPP
#define HERO_HPP

#include <memory>
#include <vector>

#include "footsteps.hpp"
#include "movement.hpp"
#include "vector.hpp"
#include "thing.hpp"

class Player;
class Monster;
class Animation;
class Block;


class Hero: public Thing
{
public:
  Hero(Player &p);
  ~Hero();
  
  void update();
  void order_move(Vector<int> dir, bool pull);

  void make_invisible();
  bool is_invisible();
  
private:
  Movement movement;
  Footsteps footsteps;
  Player &player;

  std::vector<Block *> levitated_blocks;
  
  Vector<int> move_dir;
  bool pull;

  // queued order
  Vector<int> queued_move_dir;
  bool queued_pull;

  int invisible;
  
  // check whether it is possible to go in dir, perhaps pushing blocks
  bool valid_dir(Vector<int> dir);
  // possibly move a row of blocks or eat power up in direction
  void cross_boundary(Vector<int> dir);
  // whether there is a block we can pull on the tile
  bool pullable_block(Vector<int> tile);
  void pull_block(Vector<int> tile, Vector<int> dir);
  // check whether we encountered a monster, return it if we did
  Monster *monster_encountered();
  void orient_hero();
  void initiate_move();
  void levitate_blocks(Vector<int> dir); // requires dir to be a valid_dir
  void unlevitate_blocks();
};


#endif
