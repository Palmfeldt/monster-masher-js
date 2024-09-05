/* Implementation of persistent state.
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

#include <config.h>

#include <string>
#include <fstream>
#include <cassert>

#include <glibmm.h>
#include <gtkmm/messagedialog.h>

#include "persistent.hpp"
#include "ucompose.hpp"
#include "i18n.hpp"

// helper class for security through obscurity
class Codec 
{
public:
  Codec()
    : code("0NLYL4MERZCH33T")
  {
    element = code.begin();
  }

  int encode(int c)
  {
    int ret = c * *element;
    cycle_element();
    return ret;
  }

  int decode(int c)
  {
    int ret = c / *element;
    cycle_element();
    return ret;
  }

private:
  void cycle_element()
  {
    ++element;
    if (element == code.end())
      element = code.begin();
  }

  std::string code;
  std::string::iterator element;
};



Persistent &Persistent::instance()
{
  static Persistent p;
  return p;
}

Persistent::Persistent()
{
  // default values
  for (int i = 0; i < 2; ++i) {
    state[i][difficulty_easy].last = 1;
    state[i][difficulty_medium].last = 1;
    state[i][difficulty_hard].last = 1;
#if 0
    for (int j = 0; j < 29; ++j) {
      LevelInfo li;
      li.resurrections = 3;
      li.scores.push_back(0);
      if (i == 1)
	li.scores.push_back(0);
      state[i][difficulty_easy].levels.push_back(li);
      state[i][difficulty_medium].levels.push_back(li);
      state[i][difficulty_hard].levels.push_back(li);
    }
#endif
  }
  no_of_players = 1;
  difficulty_level = difficulty_easy;

  load_state();
}

Persistent::~Persistent()
{
  save_state();
}

void read_int(std::istream &is, Codec &codec, int &dest, int vstart, int vend)
{
  int tmp;
  
  if (!(is >> tmp))
    throw 0;
  else {
    tmp = codec.decode(tmp);
    if (tmp >= vstart && tmp <= vend)
      dest = tmp;
  }
}

void Persistent::load_state()
{
  // try to read as much as possible, else values are defaulted
  std::string filename
    = Glib::build_filename(Glib::get_home_dir(), ".monster-masher");

  if (!Glib::file_test(filename, Glib::FILE_TEST_IS_REGULAR))
    return;

  std::ifstream is(filename.c_str());

  try {
    if (!is)
      throw 0;

    int version;
    if (!(is >> version))
      throw 0;

    // version 1 is the only version right now
    if (version != 1)
      throw 0;
    
    Codec codec;

    read_int(is, codec, no_of_players, 1, 2);
    int tmp;
    read_int(is, codec, tmp, difficulty_easy, difficulty_hard);
    difficulty_level = static_cast<difficulty>(tmp);
    
    for (int i = 0; i < 2; ++i) {
      load_player_info(is, codec, state[i][difficulty_easy], i + 1, 30);
      load_player_info(is, codec, state[i][difficulty_medium], i + 1, 30);
      load_player_info(is, codec, state[i][difficulty_hard], i + 1, 30);
    }
  }
  catch(...) {
    Gtk::MessageDialog
      d(String::ucompose(_("An error occurred while reading the "
			   "file \"%1\". This may mean "
			   "that the recordings of what levels you have "
			   "passed are lost."),
			 filename),
	Gtk::MESSAGE_ERROR);

    d.set_title(_("Load error"));

    d.run();
  }
}

void Persistent::load_player_info(std::istream &is, Codec &codec,
				  StateInfo &info, int no_players,
				  int max_level)
{
  assert(info.levels.empty());
  
  int no_levels = 0;
  read_int(is, codec, no_levels, 1, max_level);
  info.levels.reserve(no_levels);
  for (int i = 0; i < no_levels; ++i) {
    LevelInfo li;
    
    read_int(is, codec, li.resurrections, 0, 10000);

    // read scores
    for (int j = 0; j < no_players; ++j) {
      int score = 0;
      li.scores.reserve(no_players);
      read_int(is, codec, score, 0, 1000000);
      li.scores.push_back(score);
    }
    
    info.levels.push_back(li);
  }
  
  read_int(is, codec, info.last, 1, no_levels + 1);
}


void write_int(std::ostream &os, Codec &codec, int src)
{
  if (!(os << codec.encode(src) << ' '))
    throw 0;
}

void Persistent::save_state()
{
  std::string filename
    = Glib::build_filename(Glib::get_home_dir(), ".monster-masher");

  std::ofstream os(filename.c_str());

  try {
    if (!os)
      throw 0;

    if (!(os << 1 << ' '))	// version 1
      throw 0;

    Codec codec;

    write_int(os, codec, no_of_players);
    write_int(os, codec, difficulty_level);
    
    for (int i = 0; i < 2; ++i) {
      save_player_info(os, codec, state[i][difficulty_easy]);
      save_player_info(os, codec, state[i][difficulty_medium]);
      save_player_info(os, codec, state[i][difficulty_hard]);
    }
  }
  catch(...) {
    Gtk::MessageDialog
      d(String::ucompose(_("An error occurred while saving the "
			   "file \"%1\". This may mean "
			   "that the recordings of what levels you have "
			   "passed are lost."),
			 filename),
	Gtk::MESSAGE_ERROR);

    d.set_title(_("Save error"));

    d.run();
  }
}

void Persistent::save_player_info(std::ostream &os, Codec &codec,
				       StateInfo &info)
{
  write_int(os, codec, info.levels.size());
    
  for (std::vector<LevelInfo>::iterator i = info.levels.begin(),
	 end = info.levels.end(); i != end; ++i) {
    write_int(os, codec, i->resurrections);

    for (std::vector<int>::iterator j = i->scores.begin(),
	   jend = i->scores.end(); jend != j; ++j)
      write_int(os, codec, *j);
  }

  write_int(os, codec, info.last);
}
