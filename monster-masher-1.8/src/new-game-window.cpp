/* Implementation of the NewGameWindow class.
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

#include <config.h>

#include "new-game-window.hpp"
#include "main-window.hpp"
#include "gui-helpers.hpp"
#include "vector.hpp"
#include "persistent.hpp"
#include "game.hpp"

#include "ucompose.hpp"
#include "i18n.hpp"

NewGameWindow::NewGameWindow()
{
  ui = get_glade_xml("new_game_window");

  ui->get_widget("new_game_window", window);
  window->set_transient_for(MainWindow::instance().get_window());
  window->set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
  
  ui->get_widget("one_player_radiobutton", one_player_radiobutton);
  one_player_radiobutton->signal_toggled()
    .connect(sigc::mem_fun(*this, &NewGameWindow::update_level_frame));
  ui->get_widget("two_players_radiobutton", two_players_radiobutton);
  two_players_radiobutton->signal_toggled()
    .connect(sigc::mem_fun(*this, &NewGameWindow::update_level_frame));

  ui->get_widget("easy_radiobutton", easy_radiobutton);
  easy_radiobutton->signal_toggled()
    .connect(sigc::mem_fun(*this, &NewGameWindow::update_level_frame));
  ui->get_widget("medium_radiobutton", medium_radiobutton);
  medium_radiobutton->signal_toggled()
    .connect(sigc::mem_fun(*this, &NewGameWindow::update_level_frame));
  ui->get_widget("hard_radiobutton", hard_radiobutton);
  hard_radiobutton->signal_toggled()
    .connect(sigc::mem_fun(*this, &NewGameWindow::update_level_frame));

  ui->get_widget("level_frame", level_frame);
  ui->get_widget("level_spinbutton", level_spinbutton);
  level_spinbutton->signal_changed()
    .connect(sigc::mem_fun(*this, &NewGameWindow::on_level_changed));
  ui->get_widget("level_info_label", level_info_label);

  
  Gtk::Button *button;
  
  ui->get_widget("cancel_button", button);
  button->signal_clicked()
    .connect(sigc::mem_fun(*this, &NewGameWindow::on_cancel_pressed));
  
  ui->get_widget("start_game_button", button);
  button->signal_clicked()
    .connect(sigc::mem_fun(*this, &NewGameWindow::on_start_game_pressed));
  
  window->signal_delete_event()
    .connect(sigc::mem_fun(*this, &NewGameWindow::on_closed));

  // fill in saved values
  switch (Persistent::instance().no_of_players) {
  case 2:
    two_players_radiobutton->set_active();
    break;

  case 1:
  default:
    one_player_radiobutton->set_active();
    break;
  }

  switch (Persistent::instance().difficulty_level) {
  case difficulty_medium:
    medium_radiobutton->set_active();
    break;

  case difficulty_hard:
    hard_radiobutton->set_active();
    break;

  case difficulty_easy:
  default:
    easy_radiobutton->set_active();
    break;
  }
}

void NewGameWindow::show()
{
  window->show();

  update_level_frame();
}

void NewGameWindow::raise()
{
  window->deiconify();

  window->raise();
}

void NewGameWindow::on_cancel_pressed()
{
  window->hide();
}

void NewGameWindow::on_start_game_pressed()
{
  window->hide();

  int level = level_spinbutton->get_value_as_int();
  int player_no = get_player_no();
  difficulty diff = get_difficulty_level();
  
  Persistent::instance().no_of_players = player_no;
  Persistent::instance().difficulty_level = diff;
  Persistent::instance().state[player_no - 1][diff].last = level;
  Persistent::instance().save_state();
  
  MainWindow::instance().start_new_game(level);
}

void NewGameWindow::update_level_frame()
{
  Persistent::StateInfo &p =
    Persistent::instance()
    .state[get_player_no() - 1][get_difficulty_level()];

  int top = p.levels.size();
  if (top < Game::max_levels)
    ++top;
  
  level_spinbutton->set_range(1, top);
  
  level_spinbutton->set_value(p.last);
  level_frame->set_sensitive(p.levels.size() + 1 != 1);
}

void NewGameWindow::on_level_changed()
{
  int l = level_spinbutton->get_value_as_int();
  
  Glib::ustring s;
  if (l > 1) {
    Persistent::LevelInfo &info =
      Persistent::instance()
      .state[get_player_no() - 1][get_difficulty_level()].levels[l - 2];

    // note to translators: resur. stands for resurrections; resurrections
    // occur when the player is given life again after having died
    // (the translation should preferably be short)
    s = String::ucompose(_("(resur.: %1)"), info.resurrections);
  }
  
  level_info_label->set_text(s);
}

bool NewGameWindow::on_closed(GdkEventAny *)
{
  window->hide();
  
  return true;
}

int NewGameWindow::get_player_no()
{
  int n = 1;
  
  if (two_players_radiobutton->get_active())
    n = 2;

  return n;
}

difficulty NewGameWindow::get_difficulty_level()
{
  difficulty d = difficulty_easy;

  if (medium_radiobutton->get_active())
    d = difficulty_medium;
  else if (hard_radiobutton->get_active())
    d = difficulty_hard;

  return d;
}


