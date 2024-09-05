/* The intro class for showing the introduction.
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

#ifndef INTRO_HPP
#define INTRO_HPP

#include "helpers.hpp"

#include <sigc++/trackable.h>


class Intro: public sigc::trackable, noncopyable
{
public:
  Intro();
  ~Intro();

  void key_pressed(unsigned int keyval);

private:
  int state;

  bool main_loop();
  bool update();
  
  static int const iterations_per_sec = 1;
};

#endif
