/* Implementation of class CenterMenu.
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

#include <gtkmm/accelgroup.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>
#include <libgnomecanvasmm/canvas.h>

#include "center-menu.hpp"
#include "canvas.hpp"
#include "ucompose.hpp"


CenterMenu::CenterMenu(action_sequence actions, Gtk::Window &window)
{
  Vector<int>
    size = Canvas::instance().get_size(),
    offset = Canvas::instance().get_offset();
  
  // center the menu
  Gtk::Alignment &alignment = *manage(new Gtk::Alignment(0.5, 0.5, 0, 0));
  Gtk::VBox &vbox = *manage(new Gtk::VBox(true, 6));
  alignment.add(vbox);

  // generate buttons
  for (action_sequence::iterator i = actions.begin(), end = actions.end();
       i != end; ++i) {
    Glib::ustring str = String::ucompose("%1", i->label);
    Gtk::Label &label = *manage(new Gtk::Label(str, 0.0, 0.5, true));
    label.set_use_markup();

    Gtk::Box &spacer = *manage(new Gtk::HBox);
    spacer.pack_start(label, Gtk::PACK_EXPAND_WIDGET, 12);
    
    Gtk::Button &button = *manage(new Gtk::Button);
    button.add(spacer);
    button.signal_clicked().connect(i->callback);
    button.add_accelerator("activate",
			   window.get_accel_group(),
			   label.get_mnemonic_keyval(),
			   Gdk::ModifierType(),
			   Gtk::ACCEL_VISIBLE);

    vbox.pack_start(button, Gtk::PACK_SHRINK);
  }

  // setup canvas widget
  widget.reset(new Gnome::Canvas::Widget(Canvas::instance().text_layer,
					 offset.x, offset.y, alignment));
  widget->property_width() = size.x;
  widget->property_height() = size.y;

  // FIXME: focusing the first widget works fine, but then it is not possible
  // to move the focus - so disable this for now
  //vbox.children().front().get_widget()->grab_focus();

  // and action
  alignment.show_all();
  widget->show();

  
  // setup background
  int width, height;
  vbox.get_size_request(width, height);

  background.reset(new Gnome::Canvas::Rect(Canvas::instance().text_layer));
  background->property_fill_color_rgba() = 0x20202070;

  Vector<int> midpoint = offset + size / 2;
  int const margin = 36;
  
  background->property_x1() = midpoint.x - (width / 2 + margin);
  background->property_y1() = midpoint.y - (height / 2 + margin);
  background->property_x2() = midpoint.x + (width / 2 + margin);
  background->property_y2() = midpoint.y + (height / 2 + margin);

  background->show();
}

