/* Class NewGameWindow for starting a new game.
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

#ifndef NEW_GAME_WINDOW_HPP
#define NEW_GAME_WINDOW_HPP

#include <libglademm/xml.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/window.h>

#include "persistent.hpp"


class NewGameWindow: public sigc::trackable
{
public:
  NewGameWindow();
  
  void raise();
  void show();

private:
  Glib::RefPtr<Gnome::Glade::Xml> ui;

  Gtk::Window *window;
  Gtk::RadioButton *one_player_radiobutton, *two_players_radiobutton;
  Gtk::RadioButton *easy_radiobutton, *medium_radiobutton, *hard_radiobutton;
  Gtk::Frame *level_frame;
  Gtk::SpinButton *level_spinbutton;
  Gtk::Label *level_info_label;
  
  void on_cancel_pressed();
  void on_start_game_pressed();
  void update_level_frame();
  void on_level_changed();

  bool on_closed(GdkEventAny *);

  int get_player_no();
  difficulty get_difficulty_level();
};

#endif
