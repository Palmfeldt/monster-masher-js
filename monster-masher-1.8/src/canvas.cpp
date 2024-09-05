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

#include <gtkmm/widget.h>

#include "canvas.hpp"


Canvas *Canvas::current_instance = 0;

Canvas::Canvas(Gnome::Canvas::Canvas &c)
  : background_layer(*manage(new Gnome::Canvas::Group(*c.root()))),
    block_layer(*manage(new Gnome::Canvas::Group(*c.root()))),
    creature_layer(*manage(new Gnome::Canvas::Group(*c.root()))),
    text_layer(*manage(new Gnome::Canvas::Group(*c.root()))),
    canvas(c)
{
  assert(current_instance == 0);
  current_instance = this;
}

Canvas::~Canvas()
{
  current_instance = 0;
}

Canvas &Canvas::instance()
{
  assert(current_instance != 0);

  return *current_instance;
}

void Canvas::set_size(Vector<int> s, Vector<int> o)
{
  size = s;
  offset = o;
  canvas.set_scroll_region(o.x, o.y, s.x + o.x, s.y + o.y);
  canvas.set_size_request(s.x, s.y);
}

Vector<int> Canvas::get_size()
{
  return size;
}

Vector<int> Canvas::get_offset()
{
  return offset;
}
