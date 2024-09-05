/* The Game class contains the objects that participate in the current game.
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

#ifndef GAME_HPP
#define GAME_HPP

#include <list>
#include <memory>
#include <utility>
#include <vector>

#include <sigc++/trackable.h>
#include <glibmm/ustring.h>

#include "helpers.hpp"
#include "vector.hpp"
#include "arena.hpp"

class Monster;
class Egg;
class Player;
class Graphic;
class Wall;
class MonsterWall;
class Block;
class PowerUp;
class FadingDecoration;

class Game: public sigc::trackable, noncopyable
{
public:
  Game(int start_level, Vector<int> size, int tile_size);
  ~Game();
  
  // Game is a half-way singleton
  static Game &instance();

  bool main_loop();
  void pause();
  void unpause();

  static int const iterations_per_sec = 50;
  
  void key_pressed(unsigned int keyval);
  void key_released(unsigned int keyval);

  int get_level();
  Vector<int> get_size();
  
  // assume ownership of decoration, possibly attached to block
  void add_decoration(FadingDecoration *deco, Block *block = 0);
  void add_monster(Monster *monster); // take over ownership of monster
  void remove_monster(Monster *monster);
  void generate_monster(Vector<int> pos); // generate and add a monster
  void generate_egg(Vector<int> pos); // generate and add an egg
  void remove_egg(Egg *egg);

  void power_up_eaten();
  void freeze_monsters();
  bool monsters_freezing();	// whether monsters are being freezed
  int calc_monster_freeze_time();
  
  typedef std::vector<Player *> player_sequence;
  player_sequence players;
   // the following sequences must be lists since we sometimes delete in the
   // middle of them while iterating through them
  typedef std::list<Monster *> monster_sequence;
  monster_sequence monsters;
  typedef std::list<Egg *> egg_sequence;
  egg_sequence eggs;
  
  static const int max_levels = 30;

private:
  Arena arena;
  
  enum {
    before_level, playing, passed_level, won_game, lost_game
  } state;
  int state_countdown;
  bool paused;

  int level;

  int plain_monsters, hard_monsters, seeker_monsters, egg_layer_monsters;

  void init_level();
  void create_outer_walls();
  void create_obstacles(int no_blocks, int no_walls);
  template <class MonsterType> void create_monsters(int no);
  void batch_create_monsters(int l, int plain, int hard, int seeker, int eggl);
  void process_round();
  bool players_alive();
  void clear_arena();
  void set_power_up_time();

  typedef std::vector<Wall *> wall_sequence;
  wall_sequence walls;
  typedef std::vector<MonsterWall *> monster_wall_sequence;
  monster_wall_sequence monster_walls;
  typedef std::vector<Block *> block_sequence;
  block_sequence blocks;
  typedef std::pair<FadingDecoration *, Block *> deco_pair;
  typedef std::list<deco_pair> deco_sequence;
  deco_sequence decos;

  std::auto_ptr<PowerUp> power_up;
  int power_up_counter;

  int freeze_counter;
  
  static Game *current_instance;  
};

#endif
