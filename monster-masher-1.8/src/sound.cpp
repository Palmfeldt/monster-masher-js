/* Implementation of class Sound.
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

#include <libgnome/gnome-sound.h>
#include <esd.h>

#include "sound.hpp"


Sound &Sound::instance()
{
  static Sound s;
  return s;
}

Sound::Sound()
{
}

Sound::~Sound()
{
}

void Sound::play(const std::string &name)
{
  int id;
  
  cache_map::iterator i = cache.find(name);
  if (i != cache.end())
    id = i->second;
  else {
    id = gnome_sound_sample_load(name.c_str(),
			    (MONSTER_MASHER_SOUND_DIR + name).c_str());
    cache.insert(make_pair(name, id));
  }

  esd_sample_play(gnome_sound_connection_get(), id);
}
