/* Implementation of class FadingDecoration.
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

#include "fading-decoration.hpp"

#include "arena.hpp"
#include "graphic.hpp"
#include "pixbuf-drawing.hpp"


FadingDecoration::FadingDecoration(Graphic *graph, int s, int d)
  : graphic(graph), original(graphic->get_pixbuf()), steps(s), delay(d)
{
  step_count = delay_count = 0;
  graphic->set_pixbuf(original->copy());
}

FadingDecoration::~FadingDecoration()
{
}

Graphic &FadingDecoration::get_graphic()
{
  return *graphic.get();
}

void FadingDecoration::update()
{
  assert(step_count < steps);
  
  if (delay_count < delay)
    ++delay_count;
  else {
    delay_count = 0;

    ++step_count;

    Glib::RefPtr<Gdk::Pixbuf> p = graphic->get_pixbuf();
    
    for (PixelIterator d = begin(p), s = begin(original), e = end(p);
	 d != e; ++d, ++s)
      d->alpha() = int(s->alpha()) * (steps - step_count) / steps;
    
    graphic->set_pixbuf(p);
  }
}

bool FadingDecoration::last_frame()
{
  return delay_count == delay - 1 && step_count == steps - 1;
}

