/* Implementation of main().
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

#include <config.h>

#include <cstdlib>
#include <ctime>

#include <libintl.h>

#include <libgnomecanvasmm/init.h>
#include <gconfmm/init.h>
#include <gtkmm/main.h>

#include <libgnome/gnome-program.h>
#include <libgnome/gnome-init.h>

#include <sigc++/slot.h>

#include "main-window.hpp"
#include "i18n.hpp"


int main(int argc, char *argv[])
{
  Gtk::Main main(argc, argv);
  Gnome::Conf::init();
  Gnome::Canvas::init();
  
  gnome_program_init(PACKAGE, VERSION, LIBGNOME_MODULE, argc, argv, NULL);

  try {
    // i18n
    bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    g_set_application_name(_("Monster Masher"));

    std::srand(std::time(0));

    main.run(MainWindow::instance().get_window());
  }
  catch(const Glib::Error &ex) {
    fatal_error(ex.what());
  }
}
