/* Preferences window class.
 *
 * Copyright (c) 2003, 04 Ole Laursen.
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

#ifndef PREFERENCES_WINDOW_HPP
#define PREFERENCES_WINDOW_HPP

#include <libglademm/xml.h>

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/window.h>

#include <gconfmm/entry.h>

#include <sigc++/trackable.h>
#include <sigc++/connection.h>

#include "helpers.hpp"


class PreferencesWindow: public sigc::trackable, noncopyable
{
public:
  PreferencesWindow();
  ~PreferencesWindow();
  
  void raise();
  void show();

private:
  Glib::RefPtr<Gnome::Glade::Xml> ui;

  Gtk::Window *window;
  Gtk::CheckButton *fullscreen_checkbutton;

  sigc::connection grab_connection;

  bool on_key_press_event(GdkEventKey* event, Gtk::Button *button,
			  Glib::ustring key);
  
  void setup_button(const Glib::ustring &widget, const Glib::ustring &key);
  void on_button_pressed(Gtk::Button *button, Glib::ustring key);
  void button_listener(unsigned int, Gnome::Conf::Entry conf_entry,
		       Gtk::Button *button);
  void on_fullscreen_toggled();
  void fullscreen_listener(unsigned int, Gnome::Conf::Entry entry);

  void on_close_clicked();
  bool on_closed(GdkEventAny *);
};

#endif
