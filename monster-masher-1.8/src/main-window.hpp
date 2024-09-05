/* Main window class.
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

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <memory>

#include <libglademm/xml.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>
#include <gtkmm/aboutdialog.h>

#include <sigc++/trackable.h>

#include <gconfmm/client.h>
#include <gconfmm/entry.h>

#include "helpers.hpp"
#include "vector.hpp"


class Canvas;
class PreferencesWindow;
class NewGameWindow;
class CanvasMessage;
class Graphic;
class Intro;
class Game;
class CenterMenu;

class MainWindow: public sigc::trackable, noncopyable
{
public:
  // MainWindow is a singleton
  static MainWindow &instance();
  ~MainWindow();
  
  void set_canvas_size(Vector<int> size, Vector<int> offset);
  void set_resurrections(int resurrections);
  void set_player_info(int player_no, int score);
  void set_level_info(int level);

  void display_message(const Glib::ustring &message, bool centered = true);
  void remove_message();
  
  void start_new_game(int start_level);
  void end_game();
  void intro_finished();
  
  Gtk::Window &get_window();
  
  Glib::RefPtr<Gnome::Conf::Client> gconf_client;
  const Glib::ustring gconf_dir;

private:
  MainWindow();

  Glib::RefPtr<Gnome::Glade::Xml> ui;

  Gtk::Window *window;
  Gtk::Label *resurrections_label;
  Gtk::Label *player1_status_label;
  Gtk::Label *player2_status_label;
  Gtk::Label *level_label;

  Glib::RefPtr<Gdk::Pixbuf> icon;
  
  // helpers
  void generate_main_menu();
  void generate_game_menu();
  void set_background(Glib::RefPtr<Gdk::Pixbuf> pixbuf);
  void calculate_tile_size();

  // GConf callbacks
  void fullscreen_listener(unsigned int, Gnome::Conf::Entry entry);
  
  // callbacks for main menu
  void on_new_game_activated();
  void on_introduction_activated();
  void on_preferences_activated();
  void on_about_activated();
  void on_quit_activated();

  // callbacks for game menu
  void on_resume_game_activated();
  void on_restart_level_activated();
  void on_end_game_activated();

  // general callbacks
  void on_key_press_event(GdkEventKey* event);
  void on_key_release_event(GdkEventKey* event);
  void on_button_press_event(GdkEventButton* event);
  
  bool on_closed(GdkEventAny *);

  std::auto_ptr<Canvas> canvas;
  std::auto_ptr<Graphic> background;
  std::auto_ptr<Gtk::AboutDialog> about;
  std::auto_ptr<PreferencesWindow> preferences_window;
  std::auto_ptr<NewGameWindow> new_game_window;
  std::auto_ptr<CenterMenu> center_menu;
  std::auto_ptr<CanvasMessage> canvas_message;
  std::auto_ptr<Intro> intro;
  std::auto_ptr<Game> game;

  int tile_size;
  bool fullscreen;
};

#endif
