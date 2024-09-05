/* Implementation of Footsteps class.
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

#include <cstdlib>

#include "footsteps.hpp"

#include "fading-decoration.hpp"
#include "graphic.hpp"
#include "game.hpp"
#include "canvas.hpp"
#include "image-cache.hpp"
#include "pixbuf-drawing.hpp"


Footsteps::Footsteps()
  : counter(0), footprint_time(2), towards_left(false), direction(1, 0)
{
}

void Footsteps::update(Vector<double> hero_center)
{
  // The footprints are placed a few pixels away from the center on a
  // line that is orthogonal with the direction of the hero. The
  // rotational linear transformation matrix
  //
  //   R(a) = [ cos(a) -sin(a)]
  //          [ sin(a)  cos(a)]
  //
  // is used to compute an orthogonal vector. Note that the angle is
  // reversed because of the reversed y-axis.
  
  if (towards_left) {
    --counter;
    
    if (counter <= -footprint_time) {
      // R(-pi/2) * (x, y) = [ 0  1 ][ -1 0 ] * (x, y) = (y, -x)
      Vector<double> pos
	= 3.0 * make_vector<double>(direction.y, -direction.x) + hero_center;
      place_footprint(pos);
      towards_left = false;
    }
  }
  else {
    ++counter;
    
    if (counter >= footprint_time) {
      // R(pi/2) * (x, y) = [ 0 -1 ][ 1  0 ] * (x, y) = (-y, x)
      Vector<double> pos
	= 3.0 * make_vector<double>(-direction.y, direction.x) + hero_center;
      place_footprint(pos);
      towards_left = true;
    }
  }
}

void Footsteps::orient(Vector<int> dir)
{
  direction = dir;
}

void Footsteps::turn_around()
{
  towards_left = !towards_left;
}

void Footsteps::place_footprint(Vector<double> pos)
{
  // generate pixbuf
  Glib::RefPtr<Gdk::Pixbuf> pixbuf
    = ImageCache::instance().get(image_name()).get()->copy();

  scale_alpha(pixbuf, 256 * (std::rand() % 20 + 40) / 100);

  // randomise position a little
  pos += make_vector(std::rand() % 5 - 2.0, std::rand() % 5 - 2.0);

  // setup decoration
  Graphic *graph = new Graphic(Canvas::instance().background_layer);
  graph->set_pixbuf(pixbuf);
  pos -= graph->get_size() / 2;
  graph->place_at(pos);
  
  FadingDecoration *deco
    = new FadingDecoration(graph, 20, Game::iterations_per_sec / 3);
  Game::instance().add_decoration(deco);
}

std::string Footsteps::image_name()
{
  std::string name;
  
  if (direction.x == 1 && direction.y == 0)
    name = "footprint-right";
  else if (direction.x ==  1 && direction.y ==  1)
    name = "footprint-right-down";
  else if (direction.x ==  0 && direction.y ==  1)
    name = "footprint-down";
  else if (direction.x == -1 && direction.y ==  1)
    name = "footprint-left-down";
  else if (direction.x == -1 && direction.y ==  0)
    name = "footprint-left";
  else if (direction.x == -1 && direction.y == -1)
    name = "footprint-left-up";
  else if (direction.x ==  0 && direction.y == -1)
    name = "footprint-up";
  else
    name = "footprint-right-up";

  if (towards_left)
    name += "-leftmost.png";
  else 
    name += "-rightmost.png";
  
  return name;
}

