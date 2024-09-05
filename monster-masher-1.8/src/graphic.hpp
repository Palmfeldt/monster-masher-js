/* The Graphic class which maintains (x, y) position together with an image on
 * canvas.
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

#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <memory>

#include <libgnomecanvasmm/pixbuf.h>

#include "vector.hpp"
#include "helpers.hpp"

class Graphic: noncopyable
{
public:
  Graphic(Gnome::Canvas::Group &group);
  virtual ~Graphic();
  
  void set_pixbuf(const Glib::RefPtr<Gdk::Pixbuf> &p);
  Glib::RefPtr<Gdk::Pixbuf> get_pixbuf();
  void place_at(Vector<double> pos);
  void move(Vector<double> delta);

  Vector<double> get_pos();
  Vector<int> get_size();
  
private:
  std::auto_ptr<Gnome::Canvas::Pixbuf> canvas_pixbuf;
};

#endif
