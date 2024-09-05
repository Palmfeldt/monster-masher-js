/* Implementation of Movement class.
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

#include <cassert>

#include "movement.hpp"

#include "arena.hpp"
#include "graphic.hpp"
#include "thing.hpp"


Movement::Movement(Thing &t, int updates_per_move)
  : steps(not_moving), max_steps(updates_per_move), thing(t)
{
}

void Movement::update()
{
  assert(is_moving());
  
  if (is_halfway()) {
    if (Arena::instance().occupier(thing.tile_pos) == &thing)
      Arena::instance().occupy(thing.tile_pos, 0);
    thing.tile_pos = target_tile;
    Arena::instance().occupy(thing.tile_pos, &thing);
  }
  
  thing.get_graphic().move(velocity);
  
  ++steps;

  if (steps >= max_steps) {
    // make sure we sit tight
    thing.occupy_tile(thing.tile_pos);
    steps = not_moving;
  }
}

void Movement::head(Vector<int> dir)
{
  assert(!is_moving());

  target_tile = thing.tile_pos + dir;
  
  velocity.x = double(dir.x) * Arena::instance().tile_size / max_steps;
  velocity.y = double(dir.y) * Arena::instance().tile_size / max_steps;

  steps = 0;
}

void Movement::turn_around()
{
  assert(is_moving() && !has_left_tile());

  target_tile = thing.tile_pos;
  velocity = -velocity;

  steps = max_steps - steps;
}

bool Movement::is_moving()
{
  return steps != not_moving;
}

bool Movement::has_left_tile()
{
  return steps > max_steps / 2;
}

bool Movement::is_halfway()
{
  return steps == max_steps / 2;  
}


Vector<int> Movement::get_target_tile()
{
  return target_tile;
}

