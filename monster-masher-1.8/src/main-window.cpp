/* Implementation of MainWindow.
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

#include <config.h>

#include <cassert>

#include <gtkmm/dialog.h>
#include <gtkmm/main.h>
#include <gtkmm/table.h>
#include <gdkmm/general.h>
#include <gdk/gdkkeysyms.h>


#include "main-window.hpp"

#include "preferences-window.hpp"
#include "new-game-window.hpp"
#include "intro.hpp"
#include "game.hpp"
#include "canvas.hpp"
#include "canvas-message.hpp"
#include "center-menu.hpp"
#include "graphic.hpp"
#include "pixbuf-drawing.hpp"
#include "image-cache.hpp"
#include "i18n.hpp"
#include "gui-helpers.hpp"
#include "ucompose.hpp"

// forward declaration
Glib::RefPtr<Gdk::Pixbuf> generate_background_sand(Vector<int> size);


int const tiles_x = 30, tiles_y = 22;
const char splash_filename[] = "splash.jpg";


MainWindow &MainWindow::instance()
{
  static MainWindow w;
  return w;
}

MainWindow::MainWindow()
  : gconf_dir("/apps/monster-masher")
{
  // load icon
  std::string icon_name = GNOMEICONDIR "/monster-masher.png";
  try {
    icon = Gdk::Pixbuf::create_from_file(icon_name);
  }
  catch(...) {
    std::cerr <<
      String::ucompose(_("Monster Masher: cannot load the icon \"%1\".\n"),
		       icon_name);
    // it's a minor problem if we can't find the icon
    icon = Glib::RefPtr<Gdk::Pixbuf>();
  }

  // setup gconf
  gconf_client = Gnome::Conf::Client::get_default_client();
  gconf_client->add_dir(gconf_dir);

  // start constructing window
  ui = get_glade_xml("main_window");
  ui->get_widget("main_window", window);

  window->set_icon(icon);
  
  ui->get_widget("resurrections_label", resurrections_label);
  ui->get_widget("player1_status_label", player1_status_label);
  ui->get_widget("player2_status_label", player2_status_label);
  ui->get_widget("level_label", level_label);

  resurrections_label->set_text("");
  player1_status_label->set_text("");
  player2_status_label->set_text("");
  level_label->set_text("");
  
  // setup window
  fullscreen = gconf_client->get_bool(gconf_dir + "/fullscreen");
  if (fullscreen)
    window->fullscreen();
  
  calculate_tile_size();
  
  // setup canvas
  Gtk::Frame *canvas_frame;
  ui->get_widget("canvas_frame", canvas_frame);
  Gnome::Canvas::Canvas *canvas_ptr = manage(new Gnome::Canvas::CanvasAA);
  canvas_frame->add(*canvas_ptr);
  canvas.reset(new Canvas(*canvas_ptr));
  set_canvas_size(tile_size * make_vector(tiles_x, tiles_y), make_vector(0, 0));
  set_background(ImageCache::instance().get(splash_filename).get());
  generate_main_menu();

  // connect GConf
  gconf_client->notify_add(gconf_dir + "/fullscreen",
			   sigc::mem_fun(*this, &MainWindow::fullscreen_listener));

  // connect various
  window->signal_key_press_event()
    .connect_notify(sigc::mem_fun(*this, &MainWindow::on_key_press_event));
  window->signal_key_release_event()
    .connect_notify(sigc::mem_fun(*this, &MainWindow::on_key_release_event));

  canvas_ptr->add_events(Gdk::BUTTON_PRESS_MASK);
  canvas_ptr->signal_button_press_event()
    .connect_notify(sigc::mem_fun(*this, &MainWindow::on_button_press_event));

  // connect window closement
  window->signal_delete_event()
    .connect(sigc::mem_fun(*this, &MainWindow::on_closed));

  window->set_resizable(true);	// FIXME: work around for GTK+ fullscreen bug
  window->show_all();
}

MainWindow::~MainWindow()
{
  // make sure menu doesn't try to hide message when we are dead
  intro.reset();
  center_menu.reset();
}
  
Gtk::Window &MainWindow::get_window()
{
  return *window;
}

void MainWindow::set_canvas_size(Vector<int> size, Vector<int> offset)
{
  canvas->set_size(size, offset);
  // the window will actually be larger than this because of menus etc.
  window->resize(size.x, size.y);
}

void MainWindow::set_resurrections(int resurrections)
{
  resurrections_label->set_text(String::ucompose(_("Resurrections: %1"),
						 resurrections));
}

void MainWindow::set_player_info(int player_no, int score)
{
  Glib::ustring s = String::ucompose(_("Score: %1"), score);
  
  if (player_no == 1)
    player1_status_label->set_text(s);
  else
    player2_status_label->set_text(s);
}

void MainWindow::set_level_info(int level)
{
  level_label->set_text(String::ucompose(_("Level: %1"), level));
}

void MainWindow::display_message(const Glib::ustring &message, bool centered)
{
  if (!canvas_message.get())
    canvas_message.reset(new CanvasMessage);
  
  canvas_message->set_message(message, centered);
  canvas_message->show();
}

void MainWindow::remove_message()
{
  if (canvas_message.get())
    canvas_message->hide();
}

void MainWindow::start_new_game(int start_level)
{
  // make sure no old info is hanging around
  resurrections_label->set_text("");
  player1_status_label->set_text("");
  player2_status_label->set_text("");
  level_label->set_text("");

  Vector<int> size = tile_size * make_vector(tiles_x, tiles_y);
  center_menu.reset();
  game.reset(); 		// make sure old is gone before creating new

  // offset the region so that we get to see only half of each of the outer
  // wall boundary tiles
  Vector<int> offset(tile_size / 2, tile_size / 2);
  MainWindow::instance().set_canvas_size(size, offset);
  
  set_background(generate_background_sand(size));
  game.reset(new Game(start_level, make_vector(tiles_x - 1, tiles_y - 1),
		      tile_size));
}

void MainWindow::end_game()
{
  remove_message();
  game.reset();

  set_background(ImageCache::instance().get(splash_filename).get());
  generate_main_menu();
}

void MainWindow::intro_finished()
{
  intro.reset();
  generate_main_menu();
}

void MainWindow::generate_main_menu()
{
  CenterMenu::action_sequence actions;
  CenterMenu::Action a;

  a.label = _("_New game");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_new_game_activated);
  actions.push_back(a);
  a.label = _("_Introduction");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_introduction_activated);
  actions.push_back(a);
  a.label = _("_Preferences");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_preferences_activated);
  actions.push_back(a);
  a.label = _("_About");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_about_activated);
  actions.push_back(a);
  a.label = _("_Quit");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_quit_activated);
  actions.push_back(a);
  
  center_menu.reset(new CenterMenu(actions, get_window()));
}

void MainWindow::generate_game_menu()
{
  CenterMenu::action_sequence actions;
  CenterMenu::Action a;

  a.label = _("_Resume game");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_resume_game_activated);
  actions.push_back(a);
  a.label = _("Re_start level");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_restart_level_activated);
  actions.push_back(a);
  a.label = _("_End game");
  a.callback = sigc::mem_fun(*this, &MainWindow::on_end_game_activated);
  actions.push_back(a);
  
  center_menu.reset(new CenterMenu(actions, get_window()));
}

void MainWindow::set_background(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  Vector<int> extra_offset(0, 0);
  if (pixbuf->get_width() > tile_size * tiles_x)
    extra_offset.x -= (pixbuf->get_width() - tile_size * tiles_x) / 2;
  if (pixbuf->get_height() > tile_size * tiles_y)
    extra_offset.y -= (pixbuf->get_height() - tile_size * tiles_y) / 2;

  background.reset(new Graphic(canvas->background_layer));
  background->set_pixbuf(pixbuf);

  background->place_at(canvas->get_offset() + extra_offset);
}


void MainWindow::calculate_tile_size()
{
  // setup tile size
  tile_size = 24;
  int margin = 0;

  if (!fullscreen)
    margin = 64;	    // this should compensate for panel + window border
  
   // include border walls
  if (Gdk::screen_width() >= 32 * (tiles_x + 1) + margin &&
      Gdk::screen_height() >= 32 * (tiles_y + 1) + margin)
    tile_size = 32;
}


// callbacks

void MainWindow::fullscreen_listener(unsigned int,
				     Gnome::Conf::Entry entry)
{
  if (entry.get_value().get_type() != Gnome::Conf::VALUE_BOOL)
    return;

  bool b = entry.get_value().get_bool();

  if (b != fullscreen) {
    if (b)
      window->fullscreen();
    else
      window->unfullscreen();

    calculate_tile_size();

    fullscreen = b;

    set_canvas_size(tile_size *  make_vector(tiles_x, tiles_y),
		    make_vector(0, 0));

    set_background(ImageCache::instance().get(splash_filename).get());
  }

#if (0)
  Gtk::Table *outer_table;
  ui->get_widget("outer_table", outer_table);

  Gdk::Color clr;
  unsigned int color = 0x00000000;
  clr.set_rgb(((color >> 24) & 0xff) * 256,
	      ((color >> 16) & 0xff) * 256,
	      ((color >>  8) & 0xff) * 256);
  window->modify_bg(Gtk::STATE_NORMAL, clr);
  window->modify_bg(Gtk::STATE_ACTIVE, clr);
  window->modify_bg(Gtk::STATE_PRELIGHT, clr);
  window->modify_bg(Gtk::STATE_SELECTED, clr);
  window->modify_bg(Gtk::STATE_INSENSITIVE, clr);
#endif
}


void MainWindow::on_new_game_activated()
{
  if (new_game_window.get() == 0) {
    new_game_window.reset(new NewGameWindow);
    new_game_window->show();
  }
  else {
    new_game_window->show();
    new_game_window->raise();
  }
}

void MainWindow::on_resume_game_activated()
{
  game->unpause();
  center_menu.reset();
}

void MainWindow::on_restart_level_activated()
{
  center_menu.reset();
  start_new_game(game->get_level()/*, game->get_size()*/);
}

void MainWindow::on_end_game_activated()
{
  end_game();
}

void MainWindow::on_introduction_activated()
{
  center_menu.reset();
  intro.reset(new Intro);
}

void MainWindow::on_preferences_activated()
{
  if (preferences_window.get() == 0) {
    preferences_window.reset(new PreferencesWindow);
    preferences_window->show();
  }
  else {
    preferences_window->show();
    preferences_window->raise();
  }
}

void MainWindow::on_quit_activated()
{
  window->hide();
}

void MainWindow::on_about_activated()
{
  std::vector<Glib::ustring> authors;
  authors.push_back("Ole Laursen <olau@hardworking.dk>");
  
  std::vector<Glib::ustring> documenters;
  // add documenters here

  Glib::ustring description =
    _("Fast-paced monster mashing game for GNOME. Splat the monsters "
      "before they roll over you and take over the world of the gnomes!");
  
  if (about.get() == 0) {
    about.reset(new Gtk::AboutDialog());
    about->set_name(_("Monster Masher"));
    about->set_version(VERSION);
    // %1 is the copyright symbol
    about->set_copyright(String::ucompose(_("Copyright %1 2003 Ole Laursen"),
					  "\xc2\xa9"));
    about->set_authors(authors);
    if (!documenters.empty())
      about->set_documenters(documenters);
    about->set_comments(description);
    // note to translators: please fill in your names and email addresses
    about->set_translator_credits(_("translator-credits"));
    about->set_logo(icon);
    about->set_icon(icon);
    about->show();
  }
  else {
    about->show();
    about->raise();
  }
}

void MainWindow::on_key_press_event(GdkEventKey *event)
{
  if (intro.get())
    intro->key_pressed(event->keyval);
  else if (game.get()) {
    if (event->keyval == GDK_Escape)
      if (center_menu.get())
	on_resume_game_activated();
      else {
	game->pause();
	generate_game_menu();
      }
    
    game->key_pressed(event->keyval);
  }
}

void MainWindow::on_key_release_event(GdkEventKey *event)
{
  if (game.get())
    game->key_released(event->keyval);
}

void MainWindow::on_button_press_event(GdkEventButton* event)
{
  if (game.get())
    if (center_menu.get())
      on_resume_game_activated();
    else {
      game->pause();
      generate_game_menu();
    }
}

bool MainWindow::on_closed(GdkEventAny *)
{
  return false;
}


Glib::RefPtr<Gdk::Pixbuf> generate_background_sand(Vector<int> size)
{
  Glib::RefPtr<Gdk::Pixbuf> p
    = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, size.x, size.y);

  for (PixelIterator i = begin(p), e = end(p); i != e; ++i) {
    i->red() = 200 + std::rand() % 30 - 15;
    i->blue() = 200 + std::rand() % 30 - 15;
    i->green() = 200 + std::rand() % 30 - 15;
  }

  return p;
}

