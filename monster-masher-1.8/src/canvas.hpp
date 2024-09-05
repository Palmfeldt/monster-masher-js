/* The Canvas class which represents the canvas in the main window.
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

#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <libgnomecanvasmm/canvas.h>
#include <libgnomecanvasmm/group.h>

#include "helpers.hpp"
#include "vector.hpp"


class MainWindow;

class Canvas: noncopyable
{
public:
  // Canvas is a half-way singleton
  static Canvas &instance();
  ~Canvas();
  
  Gnome::Canvas::Group &background_layer;
  Gnome::Canvas::Group &block_layer;
  Gnome::Canvas::Group &creature_layer;
  Gnome::Canvas::Group &text_layer;

  void set_size(Vector<int> size, Vector<int> offset);

  Vector<int> get_size();
  Vector<int> get_offset();

private:
  friend class MainWindow;
  Canvas(Gnome::Canvas::Canvas &canvas); // feed with reference to canvas
  
  static Canvas *current_instance;

  Gnome::Canvas::Canvas &canvas;
  Vector<int> size, offset;
};



#endif
