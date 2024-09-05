/* Implementation of class Thing.
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

#include "thing.hpp"

#include "arena.hpp"
#include "graphic.hpp"


Thing::Thing(Graphic *graph)
  : graphic(graph)
{
}

Thing::~Thing()
{
  // default clean-up
  if (Arena::instance().occupier(tile_pos) == this)
    Arena::instance().occupy(tile_pos, 0);
}

void Thing::occupy_tile(Vector<int> pos)
{
  tile_pos = pos;
  graphic->place_at(tile_pos * Arena::instance().tile_size);

  Arena::instance().occupy(tile_pos, this);
}

Graphic &Thing::get_graphic()
{
  return *graphic.get();
}
