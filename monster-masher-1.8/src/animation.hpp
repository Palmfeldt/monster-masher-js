/* An animation which uses Images to change a Graphic.
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


#ifndef ANIMATION_HPP
#define ANIMATION_HPP

class Images;
class Graphic;

class Animation 
{
public:
  Animation(Graphic &graphic, const Images &images, int delay, int start = 0);

  // calling update will animate repeatedly
  void update();
  bool last_frame();
  int size();

private:
  Graphic &graphic;
  const Images &images;
  int delay;
  int count, current_image;
};

#endif
