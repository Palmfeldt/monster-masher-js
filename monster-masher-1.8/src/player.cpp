/* Implementation of class Player.
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

#include <deque>
#include <vector>
#include <cassert>
#include <stdlib.h>
#include <gdk/gdkkeys.h>

#include "player.hpp"

#include "hero.hpp"
#include "main-window.hpp"
#include "vector.hpp"
#include "arena.hpp"
#include "game.hpp"
#include "monsters.hpp"
#include "ucompose.hpp"


int Player::resurrections;

Player::Player(int no)
  : player_no(no), score(0), pull_pressed(false), dead(false)
{
}

Player::~Player()
{
}

// helper for spawn_hero
void relax(Vector<int> p, Vector<int> size, int dist, std::vector<int> &field,
	   std::deque<Vector<int> > &active_points)
{
  if (p.x >= 0 && p.x < size.x && p.y >= 0 && p.y < size.y
      && field[p.y * size.x + p.x] > dist) {
    field[p.y * size.x + p.x] = dist;
    active_points.push_back(p);
  }
}


void Player::spawn_hero()
{
  hero.reset(new Hero(*this));

  // find the tile that is longest away from the closest monster/hero by flood
  // filling from the monsters/heroes
  Arena &arena = Arena::instance();
  Vector<int> size = arena.size;
  std::vector<int> field(size.x * size.y, INT_MAX);
  std::deque<Vector<int> > active_points;

  for (int y = 0; y < size.y; ++y)
    for (int x = 0; x < size.x; ++x) {
      Thing *t = arena.occupier(make_vector(x, y));
      if (t != 0)
	if (dynamic_cast<Monster *>(t) || dynamic_cast<Hero *>(t)) {
	  active_points.push_back(make_vector(x, y));
	  field[y * size.x + x] = 0;
	}
	else
	  field[y * size.x + x] = -1;
    }

  assert(!active_points.empty());
  
  Vector<int> cur;
  
  while (!active_points.empty()) {
    cur = active_points.front();
    int dist = field[cur.y * size.x + cur.x] + 1;
    
    relax(cur + make_vector(-1, -1), size, dist, field, active_points);
    relax(cur + make_vector(-1,  0), size, dist, field, active_points);
    relax(cur + make_vector(-1,  1), size, dist, field, active_points);
    relax(cur + make_vector( 0, -1), size, dist, field, active_points);
    relax(cur + make_vector( 0,  1), size, dist, field, active_points);
    relax(cur + make_vector( 1, -1), size, dist, field, active_points);
    relax(cur + make_vector( 1,  0), size, dist, field, active_points);
    relax(cur + make_vector( 1,  1), size, dist, field, active_points);
    
    active_points.pop_front();
  }

  // last one must be farthest away from closest monster
  hero->occupy_tile(cur);
}

void Player::clear_hero()
{
  hero.reset();
}

void Player::scored(int s)
{
  // extra life when crossing 50, 100, 150, ... boundaries
  if (score / 50 != (score + s) / 50)
    ++resurrections;		// FIXME: play tadaa sound

  score += s;
}

void Player::kill_hero()
{
  if (resurrections > 0) {
    --resurrections;
    spawn_hero();
  }
  else {
    hero.reset();
    dead = true;
  }
}

bool Player::is_dead()
{
  return dead;
}

void Player::update()
{
  if (hero.get())
    hero->update();
}

Hero *Player::get_hero()
{
  return hero.get();
}

int Player::get_score()
{
  return score;
}

void Player::set_score(int s)
{
  score = s;
}

void Player::key_pressed(unsigned int keyval)
{
  if (keyval == lookup_key("pull"))
    pull_pressed = true;

  if (!hero.get())
    return;
  
  if (keyval == lookup_key("left"))
    hero->order_move(make_vector(-1, 0), pull_pressed);
  if (keyval == lookup_key("left_up"))
    hero->order_move(make_vector(-1, -1), pull_pressed);
  if (keyval == lookup_key("up"))
    hero->order_move(make_vector(0, -1), pull_pressed);
  if (keyval == lookup_key("right_up"))
    hero->order_move(make_vector(1, -1), pull_pressed);
  if (keyval == lookup_key("right"))
    hero->order_move(make_vector(1, 0), pull_pressed);
  if (keyval == lookup_key("right_down"))
    hero->order_move(make_vector(1, 1), pull_pressed);
  if (keyval == lookup_key("down"))
    hero->order_move(make_vector(0, 1), pull_pressed);
  if (keyval == lookup_key("left_down"))
    hero->order_move(make_vector(-1, 1), pull_pressed);
}

void Player::key_released(unsigned int keyval)
{
  if (keyval == lookup_key("pull"))
    pull_pressed = false;
}

unsigned int Player::lookup_key(const Glib::ustring &key)
{
  Glib::RefPtr<Gnome::Conf::Client> &client
    = MainWindow::instance().gconf_client;
  Glib::ustring path = 
    String::ucompose("%1/player%2/%3_key",
		     MainWindow::instance().gconf_dir, player_no, key);

  return gdk_keyval_from_name(client->get_string(path).c_str());
}

