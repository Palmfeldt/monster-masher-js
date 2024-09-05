/* The Player class for keeping track of player info.
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

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>

#include <glibmm/ustring.h>

#include "helpers.hpp"


class Hero;

class Player: noncopyable
{
public:
  Player(int player_no);
  ~Player();

  void spawn_hero();
  void clear_hero();
  void scored(int score);
  void kill_hero();
  bool is_dead();
    
  void update();

  Hero *get_hero();
  int get_score();
  void set_score(int score);
  
  void key_pressed(unsigned int keyval);
  void key_released(unsigned int keyval);

  static int resurrections;

private:
  int player_no;
  int score;
  bool pull_pressed, dead;

  std::auto_ptr<Hero> hero;
  
  unsigned int lookup_key(const Glib::ustring &key);
};

#endif
