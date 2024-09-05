/* Implementation of class Game.
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
#include <algorithm>
#include <cassert>
#include <cmath>

#include "game.hpp"

#include "main-window.hpp"
#include "image-cache.hpp"
#include "player.hpp"
#include "hero.hpp"
#include "monsters.hpp"
#include "obstacles.hpp"
#include "fading-decoration.hpp"
#include "persistent.hpp"

#include "i18n.hpp"
#include "ucompose.hpp"


Game *Game::current_instance = 0;

Game::Game(int start_level, Vector<int> size, int tile_size)
  : arena(size, tile_size), paused(false), level(start_level)
{
  current_instance = this;

  Persistent &per = Persistent::instance();
    
  // setup players
  for (int i = 1; i <= per.no_of_players; ++i) {
    Player *p = new Player(i);
    players.push_back(p);
  }

  if (level > 1) {
    Persistent::LevelInfo &li
      = per.state[per.no_of_players-1][per.difficulty_level].levels[level - 2];
    
    Player::resurrections = li.resurrections;

    for (int i = 0; i < per.no_of_players; ++i)
      players[i]->set_score(li.scores[i]); 
  }
  else if (level == 1)
    Player::resurrections = 3 * per.no_of_players;
  
  init_level();

  // and action!
  Glib::signal_timeout()
    .connect(sigc::mem_fun(*this, &Game::main_loop), 1000 / iterations_per_sec);
}

Game::~Game()
{
  clear_arena();
  
  for (player_sequence::iterator i = players.begin(),
	 end = players.end(); i != end; ++i)
    delete *i;
}

Game &Game::instance()
{
  assert(current_instance != 0);

  return *current_instance;
}

bool Game::main_loop()
{
  if (paused)
    return true;
  
  switch (state) {
  case before_level:
    if (--state_countdown < 0) {
      MainWindow::instance().remove_message();
      state = playing;
    }
    break;
    
  case playing:
    process_round();
    if (monsters.empty() && eggs.empty()) {
      MainWindow::instance()
	.display_message(String::ucompose(_("Level %1 cleared for monsters!"),
					  level));
      
      // update state
      Persistent::LevelInfo li;
      li.resurrections = Player::resurrections;
      li.scores.reserve(players.size());
      
      for (player_sequence::size_type i = 0; i < players.size(); ++i)
	li.scores.push_back(players[i]->get_score());

      Persistent &p = Persistent::instance();
      Persistent::StateInfo &pi
	= p.state[p.no_of_players-1][p.difficulty_level];
	
      if (int(pi.levels.size()) < level)
	pi.levels.push_back(li);
      else
	pi.levels[level - 1] = li;
	
      if (level + 1 <= max_levels)
	pi.last = level + 1;
      else
	pi.last = max_levels;
      
      Persistent::instance().save_state();
	
      state_countdown = iterations_per_sec;
      state = passed_level;
    }
    else if (!players_alive()) {
      MainWindow::instance()
	.display_message(_("And the last hero disappeared..."));
      state_countdown = iterations_per_sec * 2; 	
      state = lost_game;
    }
    
    break;
    
  case passed_level:
    if (--state_countdown < 0) {
      if (++level == max_levels + 1) {
	MainWindow::instance()
	  .display_message(_("All levels liberated!"));
	state_countdown = iterations_per_sec * 10;
	state = won_game;
      }
      else
	init_level();
    }
    else
      process_round();
    break;

  case won_game:
    if (--state_countdown < 0)
      MainWindow::instance().end_game();
    else
      process_round();
    break;

  case lost_game:
    if (--state_countdown < 0)
      MainWindow::instance().end_game();
    else
      process_round();
    break;
    
  default:
    break;
  }
  
  return true;			// always continue
}

void Game::key_pressed(unsigned int keyval)
{
  if (!paused)
    for (std::vector<Player *>::iterator i = players.begin(),
	   end = players.end(); i != end; ++i)
      (*i)->key_pressed(keyval);
}

void Game::key_released(unsigned int keyval)
{
  if (!paused)
    for (std::vector<Player *>::iterator i = players.begin(),
	   end = players.end(); i != end; ++i)
      (*i)->key_released(keyval);
}

int Game::get_level()
{
  return level;
}

Vector<int> Game::get_size()
{
  return arena.size - make_vector(2, 2);
}

void Game::add_decoration(FadingDecoration *deco, Block *block)
{
  decos.push_back(std::make_pair(deco, block));
}

void Game::add_monster(Monster *m)
{
  monsters.push_back(m);
}
  
void Game::remove_monster(Monster *monster)
{
  monster_sequence::iterator i
    = find(monsters.begin(), monsters.end(), monster);

  assert(i != monsters.end());

  monsters.erase(i);

  delete monster;
}

void Game::generate_monster(Vector<int> pos)
{
  Monster *monster;
  int sum = plain_monsters + hard_monsters
    + seeker_monsters + egg_layer_monsters;
  int no = std::rand() % sum;

  if (no < plain_monsters)
    monster = new PlainMonster;
  else if (no < plain_monsters + hard_monsters)
    monster = new HardMonster;
  else if (no < plain_monsters + hard_monsters + seeker_monsters)
    monster = new SeekerMonster;
  else
    monster = new EggLayerMonster;

  monster->occupy_tile(pos);
  monsters.push_back(monster);
}
  
void Game::generate_egg(Vector<int> pos)
{
  Egg *egg;
  int sum = plain_monsters + hard_monsters
    + seeker_monsters + egg_layer_monsters;
  int no = std::rand() % sum;

  if (no < plain_monsters)
    egg = new PlainEgg(pos);
  else if (no < plain_monsters + hard_monsters)
    egg = new HardEgg(pos);
  else if (no < plain_monsters + hard_monsters + seeker_monsters)
    egg = new SeekerEgg(pos);
  else
    egg = new EggLayerEgg(pos);

  eggs.push_back(egg);
}
  
void Game::remove_egg(Egg *egg)
{
  egg_sequence::iterator i = find(eggs.begin(), eggs.end(), egg);

  assert(i != eggs.end());

  eggs.erase(i);

  delete egg;
}

void Game::power_up_eaten()
{
  power_up.reset();
  set_power_up_time();
}

void Game::pause()
{
  paused = true;
}

void Game::unpause()
{
  paused = false;
}

void Game::freeze_monsters()
{
  freeze_counter += calc_monster_freeze_time() - 1;
}

bool Game::monsters_freezing()
{
  return freeze_counter > 0;
}

int Game::calc_monster_freeze_time()
{
  return (std::rand() % 5 + 12) * iterations_per_sec;
}


void Game::init_level()
{
  clear_arena();

  // reward players
  if (level > 1)
    for (player_sequence::iterator i = players.begin(),
	   end = players.end(); i != end; ++i)
      if ((*i)->get_hero())
	(*i)->scored(12);

  Vector<int> size = arena.size;
  int tiles = size.x * size.y;
  
  create_outer_walls();

  if (level == 30) {
    int const mwidth = 6, mheight = 2;
    for (int y = size.y/2 - mheight/2; y < size.y/2 + mheight/2; ++y)
      for (int x = size.x/2 - mwidth/2; x < size.x/2 + mwidth/2; ++x) {
	MonsterWall *w = new MonsterWall;
	w->occupy_tile(make_vector(x, y));
	monster_walls.push_back(w);
      }
  }
  
  create_obstacles(int(0.025 * tiles), int(0.35 * tiles));

  //                  lvl pln hrd sek egg-layer
  batch_create_monsters(1, 12, 0, 0, 0);
  batch_create_monsters(2, 16, 0, 0, 0);
  batch_create_monsters(3, 20, 0, 0, 0);
  batch_create_monsters(4, 24, 0, 0, 0);
  batch_create_monsters(5, 28, 1, 0, 0);
  batch_create_monsters(6, 15, 2, 0, 0);
  batch_create_monsters(7, 15, 4, 0, 0);
  batch_create_monsters(8, 15, 6, 0, 0);
  batch_create_monsters(9, 15, 8, 0, 0);
  batch_create_monsters(10, 20, 0, 1, 0);
  batch_create_monsters(11, 20, 0, 3, 0);
  batch_create_monsters(12, 20, 1, 5, 0);
  batch_create_monsters(13, 20, 2, 6, 0);
  batch_create_monsters(14, 20, 2, 8, 0);
  batch_create_monsters(15, 20, 0, 2, 1);
  batch_create_monsters(16, 20, 0, 4, 2);
  batch_create_monsters(17, 20, 0, 4, 3);
  batch_create_monsters(18, 20, 0, 4, 4);
  batch_create_monsters(19, 20, 0, 4, 5);
  batch_create_monsters(20, 20, 1, 4, 4);
  batch_create_monsters(21, 25, 1, 4, 4);
  batch_create_monsters(22, 30, 1, 4, 4);
  batch_create_monsters(23, 35, 1, 4, 4);
  batch_create_monsters(24, 25, 3, 4, 4);
  batch_create_monsters(25, 20, 5, 4, 4);
  batch_create_monsters(26, 20, 8, 4, 5);
  batch_create_monsters(27, 20, 8, 6, 6);
  batch_create_monsters(28, 20, 8, 8, 7);
  batch_create_monsters(29, 20, 8, 10, 8);
  batch_create_monsters(30, 10, 4, 6, 6);

  for (player_sequence::iterator i = players.begin(),
	 end = players.end(); i != end; ++i)
    if (!(*i)->is_dead())
      (*i)->spawn_hero();

  set_power_up_time();
  freeze_counter = 0;
  
  MainWindow::instance().set_level_info(level);
  MainWindow::instance()
    .display_message(String::ucompose(_("Entering level %1..."), level));

  state_countdown = iterations_per_sec;
  state = before_level;
}

void Game::create_outer_walls()
{
  Vector<int> size = arena.size;
  
  Wall *w = 0;

  // build the walls around the arena
  for (int x = 1; x < size.x - 1; ++x) {
    w = new Wall;
    w->occupy_tile(make_vector(x, 0));
    walls.push_back(w);

    w = new Wall;
    w->occupy_tile(make_vector(x, size.y - 1));
    walls.push_back(w);
  }

  for (int y = 0; y < size.y; ++y) {
    w = new Wall;
    w->occupy_tile(make_vector(0, y));
    walls.push_back(w);

    w = new Wall;
    w->occupy_tile(make_vector(size.x - 1, y));
    walls.push_back(w);
  }
}

void Game::create_obstacles(int no_walls, int no_blocks)
{
  walls.reserve(walls.size() + no_walls);
  blocks.reserve(blocks.size() + no_blocks);
  
  // place random walls
  for (int i = 0; i < no_walls; ++i) {
    Wall *w = new Wall;
    w->occupy_tile(arena.random_tile());
    walls.push_back(w);
  }

  // and random blocks
  for (int i = 0; i < no_blocks; ++i) {
    Block *b = new Block;
    b->occupy_tile(arena.random_tile());
    blocks.push_back(b);
  }
}


template <class MonsterType> void Game::create_monsters(int no)
{
  for (int i = 0; i < no; ++i) {
    Monster *m = new MonsterType;
    m->occupy_tile(arena.random_tile());
    add_monster(m);
  }
}

void Game::batch_create_monsters(int l, int plain, int hard, int seeker,
				 int egg_layer)
{
  if (level == l) {
    plain_monsters = plain;
    hard_monsters = hard;
    seeker_monsters = seeker;
    egg_layer_monsters = egg_layer;
    
    if (Persistent::instance().difficulty_level == difficulty_easy) {
      // reduce easy-kill monsters by 20%, although round up to avoid 1
      // becoming 0
      plain_monsters = int(std::ceil(plain_monsters * 0.8));
      hard_monsters = int(std::ceil(hard_monsters * 0.8));
    }
    else if (Persistent::instance().difficulty_level == difficulty_hard) {
      // increase easy-kill monsters by 20%
      plain_monsters = int(plain_monsters * 1.2);
      hard_monsters = int(hard_monsters * 1.2);
    }
    
    create_monsters<PlainMonster>(plain_monsters);
    create_monsters<HardMonster>(hard_monsters);
    create_monsters<SeekerMonster>(seeker_monsters);
    create_monsters<EggLayerMonster>(egg_layer_monsters);
  }
}

void Game::process_round()
{
  // service monsters
  for (monster_sequence::iterator i = monsters.begin(),
	 end = monsters.end(); i != end; ++i)
    (*i)->update();

  // service eggs - be careful, we might have to remove one
  for (egg_sequence::iterator i = eggs.begin(),
	 end = eggs.end(); i != end; ) {
    Egg *egg = *(i++);
    if (egg->hatch_ready()) {
      egg->hatch();
      remove_egg(egg);
    }
    else
      egg->update();
  }
  
  // service heros
  MainWindow::instance().set_resurrections(Player::resurrections);
  int no = 1;
  for (player_sequence::iterator i = players.begin(),
	 end = players.end(); i != end; ++i) {
    Player *p = *i;
    p->update();
    MainWindow::instance().set_player_info(no++, p->get_score());
  }

  // service monster walls
  for (monster_wall_sequence::iterator i = monster_walls.begin(),
	 end = monster_walls.end(); i != end; ++i)
    (*i)->update();

  // service power up
  if (power_up_counter > 0)
    --power_up_counter;
  else {
    if (power_up.get()) {
      power_up.reset();
      set_power_up_time();
    }
    else {
      // prevent diamond harvesting when few monsters
      if (monsters.size() > 15) {
	int i = std::rand() % 3;
	if (i == 0)
	  power_up.reset(new InvisibilityPowerUp);
	else if (i == 1)
	  power_up.reset(new FreezePowerUp);
	else if (i == 2)
	  power_up.reset(new ExplodePowerUp);
      }
      
      set_power_up_time();
    }
  }
  
  // service decorations
  for (deco_sequence::iterator i = decos.begin(),
	 end = decos.end(); i != end;) {
    FadingDecoration *d = i->first;

    d->update();

    if (d->last_frame()) {
      if (Block *block = i->second)
	block->remove_cargo(&d->get_graphic());

      delete d;
      decos.erase(i++);
    }
    else
      ++i;
  }

  if (freeze_counter > 0)
    --freeze_counter;
}

bool Game::players_alive()
{
  for (player_sequence::iterator i = players.begin(),
	 end = players.end(); i != end; ++i)
    if (!(*i)->is_dead())
      return true;

  return false;
}

void Game::clear_arena()
{
  for (deco_sequence::iterator i = decos.begin(),
	 end = decos.end(); i != end; ++i) {
    FadingDecoration *d = i->first;
    if (Block *block = i->second)
      block->remove_cargo(&d->get_graphic());
    delete d;
  }
  decos.clear();
  
  for (player_sequence::iterator i = players.begin(),
	 end = players.end(); i != end; ++i)
    (*i)->clear_hero();
  
  for (monster_sequence::iterator i = monsters.begin(),
	 end = monsters.end(); i != end; ++i)
    delete *i;
  monsters.clear();

  for (egg_sequence::iterator i = eggs.begin(),
	 end = eggs.end(); i != end; ++i)
    delete *i;
  eggs.clear();
  
  for (wall_sequence::iterator i = walls.begin(),
	 end = walls.end(); i != end; ++i)
    delete *i;
  walls.clear();

  for (monster_wall_sequence::iterator i = monster_walls.begin(),
	 end = monster_walls.end(); i != end; ++i)
    delete *i;
  monster_walls.clear();

  power_up.reset();
  
  for (block_sequence::iterator i = blocks.begin(),
	 end = blocks.end(); i != end; ++i)
    delete *i;
  blocks.clear();
}

void Game::set_power_up_time()
{
  if (power_up.get())
    power_up_counter = (10 + std::rand() % 20) * iterations_per_sec;
  else
    power_up_counter = (10 + std::rand() % 10) * iterations_per_sec;
}

