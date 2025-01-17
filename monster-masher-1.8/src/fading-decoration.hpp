/* The FadingDecoration class for transient decorations.
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

#ifndef FADING_DECORATION_HPP
#define FADING_DECORATION_HPP

#include <memory>

#include <gdkmm/pixbuf.h>

#include "vector.hpp"
#include "helpers.hpp"

class Graphic;

class FadingDecoration: noncopyable
{
public:
  // take over ownership of graphic
  FadingDecoration(Graphic *graphic, int steps, int delay);
  virtual ~FadingDecoration();

  Graphic &get_graphic();
  
  // calling update will fade out gradually
  void update();
  bool last_frame();
  
private:
  std::auto_ptr<Graphic> graphic;
  Glib::RefPtr<Gdk::Pixbuf> original;

  int const steps, delay;
  int step_count, delay_count;
};

#endif
