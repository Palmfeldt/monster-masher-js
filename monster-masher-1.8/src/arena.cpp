/* Implementation of class Arena.
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
#include <cstdlib>
#include <sstream>

#include "arena.hpp"

#include "thing.hpp"


Arena *Arena::current_instance = 0;

Arena::Arena(Vector<int> s, int ts)
  : size(s.x + 2, s.y + 2), tile_size(ts), tiles(size.x * size.y)
{
  assert(current_instance == 0);
  current_instance = this;
}

Arena::~Arena()
{
  current_instance = 0;
}

Arena &Arena::instance()
{
  assert(current_instance != 0);

  return *current_instance;
}

Thing *Arena::occupier(Vector<int> pos)
{
  assert(pos.x >= 0 && pos.y >= 0 && pos.x < size.x && pos.y < size.y);
  
  return tiles[pos.y * size.x + pos.x];
}

void Arena::occupy(Vector<int> pos, Thing *thing)
{
  assert(pos.x >= 0 && pos.y >= 0 && pos.x < size.x && pos.y < size.y);
  
  tiles[pos.y * size.x + pos.x] = thing;
}

Vector<int> Arena::random_tile()
{
  Vector<int> t;

  do {
    t.x = std::rand() % (size.x - 2) + 1;
    t.y = std::rand() % (size.y - 2) + 1;
  } while (occupier(t) != 0);

  return t;
}

std::string Arena::tile_name(const std::string &image)
{
  std::string::size_type i = image.find('.');

  std::ostringstream os;
  os << image.substr(0, i) << '-' << tile_size
     << image.substr(i, std::string::npos);

  return os.str();
}

