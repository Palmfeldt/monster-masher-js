/* CanvasMessage for displaying a sign in the canvas.
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

#ifndef CANVAS_MESSAGE_HPP
#define CANVAS_MESSAGE_HPP

#include <memory>

#include <libgnomecanvasmm/group.h>
#include <libgnomecanvasmm/rect.h>
#include <libgnomecanvasmm/text.h>


class CanvasMessage
{
public:
  CanvasMessage();

  void set_message(const Glib::ustring &msg, bool centered = true);
  void show();
  void hide();
  
private:
  std::auto_ptr<Gnome::Canvas::Rect> background;
  std::auto_ptr<Gnome::Canvas::Text> text;
};

#endif
