/* Implementation of PreferencesWindow.
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

#include <sigc++/bind.h>
#include <gdk/gdkkeys.h>
#include <gtk/gtkmain.h>

#include "preferences-window.hpp"
#include "main-window.hpp"

#include "i18n.hpp"
#include "gui-helpers.hpp"

PreferencesWindow::PreferencesWindow()
{
  ui = get_glade_xml("preferences_window");

  ui->get_widget("preferences_window", window);

  window->set_transient_for(MainWindow::instance().get_window());
  window->set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);

  // setup fullscreen check button
  ui->get_widget("fullscreen_checkbutton", fullscreen_checkbutton);
  
  Glib::RefPtr<Gnome::Conf::Client> &client
    = MainWindow::instance().gconf_client;
  Glib::ustring dir = MainWindow::instance().gconf_dir;
  
  fullscreen_listener(0, client->get_entry(dir + "/fullscreen"));

  fullscreen_checkbutton->signal_toggled()
    .connect(sigc::mem_fun(*this, &PreferencesWindow::on_fullscreen_toggled));
  client->notify_add(dir + "/fullscreen",
		     sigc::mem_fun(*this, &PreferencesWindow::fullscreen_listener));
  
  // setup player keys
  setup_button("player1_left", "/player1/left_key");
  setup_button("player1_left_up", "/player1/left_up_key");
  setup_button("player1_up", "/player1/up_key");
  setup_button("player1_right_up", "/player1/right_up_key");
  setup_button("player1_right", "/player1/right_key");
  setup_button("player1_right_down", "/player1/right_down_key");
  setup_button("player1_down", "/player1/down_key");
  setup_button("player1_left_down", "/player1/left_down_key");
  setup_button("player1_pull_block", "/player1/pull_key");

  setup_button("player2_left", "/player2/left_key");
  setup_button("player2_left_up", "/player2/left_up_key");
  setup_button("player2_up", "/player2/up_key");
  setup_button("player2_right_up", "/player2/right_up_key");
  setup_button("player2_right", "/player2/right_key");
  setup_button("player2_right_down", "/player2/right_down_key");
  setup_button("player2_down", "/player2/down_key");
  setup_button("player2_left_down", "/player2/left_down_key");
  setup_button("player2_pull_block", "/player2/pull_key");

  Gtk::Button *button;
  ui->get_widget("close_button", button);
  button->signal_clicked()
    .connect(sigc::mem_fun(*this, &PreferencesWindow::on_close_clicked));
  
  window->signal_delete_event()
    .connect(sigc::mem_fun(*this, &PreferencesWindow::on_closed));
}

PreferencesWindow::~PreferencesWindow()
{
}
  
void PreferencesWindow::show()
{
  window->show();
}

void PreferencesWindow::raise()
{
  window->deiconify();

  window->raise();
}

void PreferencesWindow::setup_button(const Glib::ustring &widget,
				     const Glib::ustring &key)
{
  Glib::RefPtr<Gnome::Conf::Client> &client
    = MainWindow::instance().gconf_client;
  Glib::ustring dir = MainWindow::instance().gconf_dir;

  Gtk::Button *button;
  
  ui->get_widget(widget, button);
  button->set_use_underline(false);
  button->set_label(client->get_string(dir + key));
  button->signal_clicked()
    .connect(sigc::bind(sigc::mem_fun(*this,
				   &PreferencesWindow::on_button_pressed),
			button, key));
  client->notify_add(dir + key,
		     sigc::bind(sigc::mem_fun(*this, &PreferencesWindow
					   ::button_listener),
				button));
}


bool PreferencesWindow::on_key_press_event(GdkEventKey* event,
					   Gtk::Button *button,
					   Glib::ustring key)
{
  Glib::RefPtr<Gnome::Conf::Client> &client
    = MainWindow::instance().gconf_client;
  Glib::ustring dir = MainWindow::instance().gconf_dir;

  grab_connection.disconnect();
  button->remove_modal_grab();

  Glib::ustring name = gdk_keyval_name(event->keyval);
  button->set_label(name);
  client->set(dir + key, name);

  return true;
}

void PreferencesWindow::on_button_pressed(Gtk::Button *button,
					  Glib::ustring key)
{
  button->set_label("   ");

  button->add_modal_grab();
  grab_connection = button->signal_key_press_event()
    .connect(sigc::bind(sigc::mem_fun(*this, &PreferencesWindow
				   ::on_key_press_event),
			button, key));
}

void PreferencesWindow::button_listener(unsigned int,
					Gnome::Conf::Entry conf_entry,
					Gtk::Button *button)
{
  button->set_label(conf_entry.get_value().get_string());
}


void PreferencesWindow::on_fullscreen_toggled()
{
  bool b = fullscreen_checkbutton->get_active();
  
  Glib::RefPtr<Gnome::Conf::Client> &client
    = MainWindow::instance().gconf_client;
  Glib::ustring dir = MainWindow::instance().gconf_dir;

  client->set(dir + "/fullscreen", b);
}

void PreferencesWindow::fullscreen_listener(unsigned int,
					    Gnome::Conf::Entry entry)
{
  if (entry.get_value().get_type() != Gnome::Conf::VALUE_BOOL)
    return;
  
  bool b = entry.get_value().get_bool();
  if (fullscreen_checkbutton->get_active() != b)
    fullscreen_checkbutton->set_active(b);
}

void PreferencesWindow::on_close_clicked()
{
  window->hide();
}

bool PreferencesWindow::on_closed(GdkEventAny *)
{
  window->hide();
  
  return true;
}

