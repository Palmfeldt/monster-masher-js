/* Implementation of obstacle classes.
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

#include <cstdlib>
#include <cassert>

#include "obstacles.hpp"

#include "image-cache.hpp"
#include "canvas.hpp"
#include "graphic.hpp"
#include "arena.hpp"
#include "game.hpp"
#include "player.hpp"
#include "hero.hpp"
#include "monsters.hpp"
#include "fading-decoration.hpp"


Block::Block()
  : Thing(new Graphic(Canvas::instance().block_layer)), levitated(0)
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name("block.png"))
			   .get());
}

void Block::move_block_to(Vector<int> pos)
{
  Vector<double> delta(pos.x * Arena::instance().tile_size,
		       pos.y * Arena::instance().tile_size);
  delta -= get_graphic().get_pos();

  occupy_tile(pos);

  for (std::list<Graphic *>::iterator i = cargo.begin(), e = cargo.end();
       i != e; ++i)
    (*i)->move(delta);
}

void Block::add_cargo(Graphic *graphic)
{
  cargo.push_back(graphic);
}

void Block::remove_cargo(Graphic *graphic)
{
  cargo.remove(graphic);
}

void Block::levitate(int distance)
{
  if (levitated == 0 && cargo.size() < 3) {
    Graphic *graph = new Graphic(Canvas::instance().block_layer);
    const Images &im = ImageCache::instance()
      .get(Arena::instance().tile_name("levitation.png"), 8);
    graph->set_pixbuf(im.get(std::rand() % im.size()));
    graph->place_at(get_graphic().get_pos());
    add_cargo(graph);
    
    FadingDecoration *fader
      = new FadingDecoration(graph, 10 + distance * 5,
			     Game::iterations_per_sec / 25);
    
    Game::instance().add_decoration(fader, this);
  }
  
  ++levitated;
}

void Block::unlevitate()
{
  --levitated;

  assert(levitated >= 0);
#if 0
  if (levitated == 0) {
    Game::instance().remove_decoration(fader);
    delete fader;
  }
#endif
}



Wall::Wall()
  : Thing(new Graphic(Canvas::instance().block_layer))
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name("wall.png")).get());
}


MonsterWall::MonsterWall()
  : Thing(new Graphic(Canvas::instance().block_layer)),
    counter(calc_pause_time())
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name("monster-wall.png"))
			   .get());
}

void MonsterWall::update()
{
  if (counter <= 0) {
    counter = calc_pause_time();

    Arena &arena = Arena::instance();
      
    // try to spawn a monster
    Vector<int> tile;
    if (!arena.occupier(tile = tile_pos      + make_vector(-1,-1)))
      Game::instance().generate_monster(tile);
    else if (!arena.occupier(tile = tile_pos + make_vector( 0,-1)))
      Game::instance().generate_monster(tile);
    else if (!arena.occupier(tile = tile_pos + make_vector( 1,-1)))
      Game::instance().generate_monster(tile);
    else if (!arena.occupier(tile = tile_pos + make_vector( 1, 0)))
      Game::instance().generate_monster(tile);
    else if (!arena.occupier(tile = tile_pos + make_vector(-1, 1)))
      Game::instance().generate_monster(tile);
    else if (!arena.occupier(tile = tile_pos + make_vector( 0, 1)))
      Game::instance().generate_monster(tile);
    else if (!arena.occupier(tile = tile_pos + make_vector( 1, 1)))
      Game::instance().generate_monster(tile);
    else
      counter = 0;
  }
  else
    --counter;
}

int MonsterWall::calc_pause_time()
{
  return std::rand() % (100 * Game::iterations_per_sec);
}


//
// power ups
//

PowerUp::PowerUp(Graphic *graphic)
  : Thing(graphic)
{
  occupy_tile(Arena::instance().random_tile());
}

void PowerUp::eat(Player &player)
{
  do_eat(player);
  Game::instance().power_up_eaten();
}


InvisibilityPowerUp::InvisibilityPowerUp()
  : PowerUp(new Graphic(Canvas::instance().block_layer))
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance()
				.tile_name("power-up-invisibility.png"))
			   .get());
}

void InvisibilityPowerUp::do_eat(Player &player)
{
  // FIXME: play sound
  
  for (Game::player_sequence::iterator i = Game::instance().players.begin(),
	 end = Game::instance().players.end(); i != end; ++i)
    if (!(*i)->is_dead())
      if (Hero *hero = (*i)->get_hero())
	hero->make_invisible();
}


FreezePowerUp::FreezePowerUp()
  : PowerUp(new Graphic(Canvas::instance().block_layer))
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name("power-up-freeze.png"))
			   .get());
}

void FreezePowerUp::do_eat(Player &player)
{
  // FIXME: play sound

  Game::instance().freeze_monsters();
}


ExplodePowerUp::ExplodePowerUp()
  : PowerUp(new Graphic(Canvas::instance().block_layer))
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name("power-up-explode.png"))
			   .get());
}

void ExplodePowerUp::do_eat(Player &player)
{
  int const size = 4 * Arena::instance().tile_size;
  // FIXME: play sound
  
  for (Game::monster_sequence::iterator i = Game::instance().monsters.begin(),
	 end = Game::instance().monsters.end(); i != end;) {
    Monster &m = **i++;
    if (dist(get_graphic().get_pos(), m.get_graphic().get_pos()) <= size) {
      m.explode();
      player.scored(m.kill_score);
      
      Game::instance().remove_monster(&m);
    }
  }

  for (Game::egg_sequence::iterator i = Game::instance().eggs.begin(),
	 end = Game::instance().eggs.end(); i != end;) {
    Egg &e = **i++;
    if (dist(get_graphic().get_pos(), e.get_graphic().get_pos()) <= size) {
      e.explode();
      player.scored(Egg::kill_score);
      
      Game::instance().remove_egg(&e);
    }
  }
}

