/* Helper functions.
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
#include <algorithm>

#include <config.h>

#include <gdkmm/pixbuf.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <cstdlib>

#include "ucompose.hpp"
#include "helpers.hpp"
#include "i18n.hpp"

void fatal_error(const Glib::ustring &msg)
{
  Gtk::MessageDialog d(msg, Gtk::MESSAGE_ERROR);

  d.set_modal();
  d.set_title(_("Fatal error"));

  d.run();
  
  exit(1);
}

Glib::RefPtr<Gdk::Pixbuf> load_pixbuf(const std::string &name)
{
  try {
    return Gdk::Pixbuf::create_from_file(MONSTER_MASHER_PIXMAP_DIR + name);
  }
  catch(Glib::Error &e) {
    fatal_error(String::ucompose(_("%1. "
				  "Check your installation or report "
				  "the problem to the distributor "
				  "if you installed from a package."),
				 e.what()));
    
    return Glib::RefPtr<Gdk::Pixbuf>();	// prevent compiler warnings
  }
}
