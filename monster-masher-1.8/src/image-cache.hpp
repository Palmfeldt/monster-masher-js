/* The ImageCache class which can be queried for images, loading them with lazy
 * initialization.
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

#ifndef IMAGE_CACHE_HPP
#define IMAGE_CACHE_HPP

#include <string>
#include <map>

#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>
#include "helpers.hpp"

class Images;

class ImageCache: noncopyable
{
public:
  // ImageCache is a singleton
  static ImageCache &instance();
  ~ImageCache();

  // get a number of concatenated images
  const Images &get(const std::string &filename, int no = 1);
  
private:
  ImageCache();
  
  typedef std::map<std::string, Images *> cache_map;
  cache_map cache;
};

// an image sequence
class Images: noncopyable
{
public:
  // load no. images from file in which they are arranged side-by-side
  Images(const std::string &file, int no);

  Glib::RefPtr<Gdk::Pixbuf> get(int no = 0) const;
  int size() const;
  
private:
  typedef std::vector<Glib::RefPtr<Gdk::Pixbuf> > image_sequence;
  image_sequence images;
};

#endif
