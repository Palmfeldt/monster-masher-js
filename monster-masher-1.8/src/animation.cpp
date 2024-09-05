/* Implementation of Animation.
 *
 * Copyright (c) 2002, 2003 Ole Laursen.
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

#include "animation.hpp"
#include "graphic.hpp"
#include "image-cache.hpp"

Animation::Animation(Graphic &graph, const Images &im, int d, int start)
  : graphic(graph), images(im), delay(d), count(0), current_image(0)
{
  assert(delay >= 0);
  
  if (delay == 0)
    current_image = start % images.size();
  else {
    count = start % delay;
    current_image = (start / delay) % images.size();
  }
  
  graphic.set_pixbuf(images.get(current_image));
}

void Animation::update()
{
  if (count < delay)
    ++count;
  else {
    count = 0;

    ++current_image;
    if (current_image == images.size())
      current_image = 0;

    graphic.set_pixbuf(images.get(current_image));
  }
}

bool Animation::last_frame()
{
  bool last_delay_slot;
  
  if (delay == 0)
    last_delay_slot = true;
  else
    last_delay_slot = count == delay - 1;
  
  return last_delay_slot && current_image + 1 == images.size();
}

int Animation::size()
{
  return images.size();
}

