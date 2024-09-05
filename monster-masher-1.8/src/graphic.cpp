/* Implementation of class Graphic.
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

#include "graphic.hpp"
#include "helpers.hpp"

Graphic::Graphic(Gnome::Canvas::Group &group)
  : canvas_pixbuf(new Gnome::Canvas::Pixbuf(group))
{}

Graphic::~Graphic()
{}

void Graphic::set_pixbuf(const Glib::RefPtr<Gdk::Pixbuf> &p)
{
  canvas_pixbuf->property_pixbuf() = p;
}

Glib::RefPtr<Gdk::Pixbuf> Graphic::get_pixbuf()
{
  return canvas_pixbuf->property_pixbuf();
}

void Graphic::place_at(Vector<double> pos)
{
  move(pos - get_pos());
}

void Graphic::move(Vector<double> delta)
{
  canvas_pixbuf->move(delta.x, delta.y);
}

Vector<double> Graphic::get_pos()
{
  double x = 0, y = 0;
  canvas_pixbuf->i2w(x, y);
  return make_vector(x, y);
}

Vector<int> Graphic::get_size()
{
  Glib::RefPtr<Gdk::Pixbuf> p = canvas_pixbuf->property_pixbuf();

  return make_vector(p->get_width(), p->get_height());
}
