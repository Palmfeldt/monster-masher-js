/* Maintenance of persistent stuff (like levels).
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

#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

#include <vector>
#include <glibmm/ustring.h>


class Codec;

enum difficulty {
  difficulty_easy, difficulty_medium, difficulty_hard
};

class Persistent 
{
public:
  // singleton
  static Persistent &instance();
  ~Persistent();

  void save_state();

  struct LevelInfo
  {
    int resurrections;
    std::vector<int> scores;
  };
  
  struct StateInfo 
  {
    // levels["level" - 1] has data for passed level no. "level"
    std::vector<LevelInfo> levels;
    int last;			// last played level (not last completed)
  };
  
  // public state
  StateInfo state[2][3];	// two players, three difficulties
  int no_of_players;
  difficulty difficulty_level;

private:
  Persistent();

  void load_state();
  void load_player_info(std::istream &is, Codec &codec,
			StateInfo &info, int no_players, int max_level);
  void save_player_info(std::ostream &os, Codec &codec,
			StateInfo &info);
};

#endif
