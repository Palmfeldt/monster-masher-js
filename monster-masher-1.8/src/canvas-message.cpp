/* Implementation of CanvasMessage class.
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

#include <libgnomecanvasmm/canvas.h>
#include <pangomm/font.h>

#include "canvas-message.hpp"

#include "canvas.hpp"


CanvasMessage::CanvasMessage()
{
  background.reset(new Gnome::Canvas::Rect(Canvas::instance().text_layer));
  background->property_fill_color_rgba() = 0x20202090;
    
  text.reset(new Gnome::Canvas::Text(Canvas::instance().text_layer));
  text->property_fill_color_rgba() = 0xFFFFFFB0;
}

void CanvasMessage::set_message(const Glib::ustring &msg, bool centered)
{
  double x1, y1, x2, y2;
  // FIXME: should probably use Canvas::get_size instead
  text->get_canvas()->get_scroll_region(x1, y1, x2, y2);

  text->property_text() = msg;
  
  double x, y;

  if (centered) {
    y = (y1 + y2) / 2;
    text->property_size_points() = 16;
    text->property_anchor() = Gtk::ANCHOR_CENTER;
    text->property_justification() = Gtk::JUSTIFY_CENTER;

    pango_layout_set_width(text->gobj()->layout, -1);
  }
  else {
    y = (y1 + y2 * 7) / 8;
    text->property_size_points() = 14;
    text->property_anchor() = Gtk::ANCHOR_SOUTH;
    text->property_justification() = Gtk::JUSTIFY_LEFT;
    pango_layout_set_width(text->gobj()->layout,
			   int((x2 - x1) * 3/4) * Pango::SCALE);
#if 0
    g_object_set(G_OBJECT(text->gobj()),
		 "text_width", double((x2 - x1) * 3/4) * 1024, 0);
    //text->property_text_width() = double((x2 - x1) * 3/4);
#endif
  }
  
  x = (x1 + x2) / 2;
  text->property_x() = x;
  text->property_y() = y;

  if (centered) {
    double
      xoff = text->property_text_width() / 2,
      yoff = text->property_text_height() / 2;

    double const margin = 18;
    background->property_x1() = x - xoff - margin;
    background->property_y1() = y - yoff - margin;
    background->property_x2() = x + xoff + margin;
    background->property_y2() = y + yoff + margin;
  }
  else {
    double
      xoff = text->property_text_width() / 2,
      yoff = text->property_text_height();

    double const margin = 16;
    background->property_x1() = x - xoff - margin;
    background->property_y1() = y - yoff - margin;
    background->property_x2() = x + xoff + margin;
    background->property_y2() = y + margin;
  }
}

void CanvasMessage::show()
{
  text->show();
  background->show();
}

void CanvasMessage::hide()
{
  text->hide();
  background->hide();
}

