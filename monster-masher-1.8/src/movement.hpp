/* The Movement class for keeping track of a movement.
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

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "vector.hpp"

class Thing;

class Movement
{
public:
  Movement(Thing &thing, int updates_per_move);

  void update();
  
  void head(Vector<int> dir);	// start a move
  void turn_around();		// cancel a move, going back

  bool is_moving();		// whether we need update
  bool has_left_tile();		// whether we've moved to target tile
  bool is_halfway();	        // whether we are going to move upon next update
  
  Vector<int> get_target_tile();
  
private:
  Vector<int> target_tile;
  Vector<double> velocity;
  int steps;
  int const max_steps;
  Thing &thing;

  static int const not_moving = -1;
};

#endif
