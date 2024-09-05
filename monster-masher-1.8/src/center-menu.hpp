/* A menu that places itself in the middle of the canvas.
 *
 * Copyright (c) 2003, 2004 Ole Laursen.
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

#ifndef CENTER_MENU_HPP
#define CENTER_MENU_HPP

#include <memory>
#include <vector>

#include <sigc++/slot.h>
#include <glibmm/ustring.h>
#include <libgnomecanvasmm/group.h>
#include <libgnomecanvasmm/rect.h>
#include <libgnomecanvasmm/widget.h>

#include "vector.hpp"
#include "helpers.hpp"


class CenterMenu: noncopyable
{
public:
  struct Action
  {
    Glib::ustring label;
    sigc::slot<void> callback;
  };

  typedef std::vector<Action> action_sequence;

  CenterMenu(action_sequence actions, Gtk::Window &window);

private:
  std::auto_ptr<Gnome::Canvas::Widget> widget;
  std::auto_ptr<Gnome::Canvas::Rect> background;
};

#endif
