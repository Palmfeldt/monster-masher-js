/* The Footsteps class for keeping track of a hero's footsteps.
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

#ifndef FOOTSTEPS_HPP
#define FOOTSTEPS_HPP

#include <string>

#include "vector.hpp"

class Footsteps
{
public:
  Footsteps();

  void update(Vector<double> hero_center);
  void orient(Vector<int> dir);
  void turn_around();

private:
  int counter;
  int const footprint_time;
  bool towards_left;
  Vector<int> direction;

  void place_footprint(Vector<double> pos);
  std::string image_name();
};

#endif
