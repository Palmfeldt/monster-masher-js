/* Implementation of the ImageCache class.
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

#include "helpers.hpp"
#include "image-cache.hpp"


ImageCache &ImageCache::instance()
{
  static ImageCache c;
  return c;
}

ImageCache::ImageCache()
{
}

ImageCache::~ImageCache()
{
  for (cache_map::iterator i = cache.begin(), end = cache.end(); i != end; ++i)
    delete i->second;
}

const Images &ImageCache::get(const std::string &filename, int no)
{
  Images *data = cache[filename];
  
  if (data == 0) {
    data = new Images(filename, no);
    cache[filename] = data;
  }
  
  return *data;
}



Images::Images(const std::string &file, int no)
{
  Glib::RefPtr<Gdk::Pixbuf> row = load_pixbuf(file);

  images.reserve(no);
  
  if (no == 1)
    images.push_back(row);
  else {
    int h = row->get_height();
    int w = row->get_width() / no;

    // disassemble row
    for (int x = 0; x < no; ++x) {
      Glib::RefPtr<Gdk::Pixbuf> buf
	= Gdk::Pixbuf::create(row->get_colorspace(),
			      row->get_has_alpha(),
			      row->get_bits_per_sample(),
			      w, h);

      row->copy_area(x * w, 0, w, h, buf, 0, 0);
    
      images.push_back(buf);
    }
  }
}

Glib::RefPtr<Gdk::Pixbuf> Images::get(int no) const
{
  assert(no >= 0 && no < size());

  return images[no];
}

int Images::size() const
{
  return images.size();
}

