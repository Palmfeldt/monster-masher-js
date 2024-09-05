/* Implementation of Intro class.
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
#include <gdk/gdkkeysyms.h>

#include "intro.hpp"

#include "main-window.hpp"
#include "i18n.hpp"

Intro::Intro()
  : state(0)
{
  Glib::signal_timeout()
    .connect(sigc::mem_fun(*this, &Intro::main_loop), 1000 / iterations_per_sec);

  update();			// setup first screen
}

Intro::~Intro()
{
  MainWindow::instance().remove_message();
}

void Intro::key_pressed(unsigned int keyval)
{
  switch (keyval) {
  case GDK_space:
  case GDK_Return:
  case GDK_KP_Enter:
    {
      bool changed;
      do {
	changed = update();
      } while (!changed);
    }
    break;

  case GDK_Escape:
    MainWindow::instance().intro_finished();
    break;
  }
}

bool Intro::main_loop()
{
  update();

  return true;
}

bool Intro::update()
{
  MainWindow &mw = MainWindow::instance();

  bool changed = true;
  
  int sum = 0;
  
  if (state == sum)
    mw.display_message(_("In the old days, before man entered the "
			 "world, the gnomes were abundant."), false);
  else if (state == (sum += 5))
    mw.display_message(_("Through centuries of hard labour, only "
			 "slightly eased by the levitational "
			 "powers provided to them by their god, "
			 "they bored out shafts and caves in the "
			 "mountains."), false);
  else if (state == (sum += 10))
    mw.display_message(_("Always seeking the precious stones and "
			 "valuable ore..."), false);
  else if (state == (sum += 5))
    mw.display_message(_("But one clan dug deeper than the "
			 "others. Only its wealth was growing "
			 "quicker than its greed."), false);
  else if (state == (sum += 7))
    mw.display_message(_("The years went by. The chief of the "
			 "clan became the king of the gnomes. "
			 "Thousands of men, women and children "
			 "were sent to work in the deepest, most "
			 "profitable mines."), false);
  else if (state == (sum += 10))
    mw.display_message(_("Then one day a large deposit of coal "
			 "was found. Or so it was thought..."), false);
  else if (state == (sum += 6))
    mw.display_message(_("From the black carbon, a strange "
			 "creature rose!"), false);
  else if (state == (sum += 5))
    mw.display_message(_("The first moments of surprise among "
			 "the workers were replaced with horror "
			 "as hundreds of other shapes broke out "
			 "of the vein, crushing everything and "
			 "everybody on their way."), false);
  else if (state == (sum += 10))
    mw.display_message(_("For months, the clan fought the "
			 "monsters. Only a handful of messengers "
			 "sent out to warn the other clans "
			 "survived."), false);
  else if (state == (sum += 7))
    mw.display_message(_("The world of the gnomes was "
			 "threatened..."), false);
  else if (state == (sum += 5))
    mw.display_message(_("By the black monsters made of carbon "
			 "harder than steel, mechanical in their "
			 "stupidity; but deadly."), false);
  else if (state == (sum += 8))
    mw.display_message(_("By the armoured monsters so "
			 "well-protected that only the bed-rock "
			 "was hard enough to slay them."), false);
  else if (state == (sum += 8))
    mw.display_message(_("By the seekers, sneaky and tricky, red "
			 "from the gnome blood that their spikes "
			 "had spilled."), false);
  else if (state == (sum += 8))
    mw.display_message(_("And by the blue egg layers that "
			 "multiplied their number by thousands."), false);
  else if (state == (sum += 7))
    mw.display_message(_("The only remaining hope of the gnomes "
			 "was a humble levitation worker. He had "
			 "found a way to mash the monsters "
			 "against the mountain rocks."), false);
  else if (state == (sum += 10))
    mw.display_message(_("That hope is you."), false);
  else if (state == (sum += 5))
    mw.display_message(_("You must battle your way deep into the "
			 "mountains, past each of the levels of "
			 "the clan of the kings and clear the way "
			 "for the rock summoners to block the "
			 "vein."), false);
  else if (state == (sum += 10))
    mw.display_message(_("Only then can the gnomes survive..."), false);
  else if (state == (sum += 12))
    ;				// do nothing, just wait
  else if (state >= (sum += 1))
    mw.intro_finished();
  else
    changed = false;
    
  ++state;
  
  return changed;
}
