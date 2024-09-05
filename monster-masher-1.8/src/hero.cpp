/* Implementation of Hero class.
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

#include <cassert>

#include "hero.hpp"

#include "arena.hpp"
#include "canvas.hpp"
#include "game.hpp"
#include "graphic.hpp"
#include "monsters.hpp"
#include "obstacles.hpp"
#include "image-cache.hpp"
#include "player.hpp"
#include "splat-helpers.hpp"
#include "sound.hpp"

int const hero_move_rounds = 6;

Hero::Hero(Player &p)
  : Thing(new Graphic(Canvas::instance().creature_layer)),
    movement(*this, hero_move_rounds), player(p),
    queued_move_dir(0, 0), invisible(0)
{
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name("hero-right.png"))
			   .get());
}

Hero::~Hero()
{
  unlevitate_blocks();
}


void Hero::update()
{
  if (invisible > 0) {
    --invisible;
    
    if (invisible == 0)
      orient_hero();		// update appearance
  }

  
  if (monster_encountered()) {
    // place a gore spot on the floor
    const Images &gore = ImageCache::instance()
      .get(Arena::instance().tile_name("hero-gore.png"), 8);
    splat_on_floor(get_graphic().get_pos(),
		   gore.get(std::rand() % gore.size()));
    
    // FIXME: play sound

    player.kill_hero();

    return;
  }

  if (movement.is_moving()) {
    bool halfway = movement.is_halfway();
    
    if (!movement.has_left_tile()) {
      if (valid_dir(move_dir)) {
	levitate_blocks(move_dir);
	if (halfway)
	  cross_boundary(move_dir);
      }
      else {
	unlevitate_blocks();
	footsteps.turn_around();
	movement.turn_around();
      }
    }
    
    movement.update();
    footsteps.update(get_graphic().get_pos()
		     + Vector<double>(get_graphic().get_size() / 2));

    // first pull when we've moved
    if (halfway && pull && pullable_block(tile_pos - 2 * move_dir))
      pull_block(tile_pos - 2 * move_dir, move_dir);
  }
  else {
    // make sure we are not levitating blocks
    unlevitate_blocks();
    
    // check if a move has been ordered
    if (tile_pos + queued_move_dir != tile_pos) {
      move_dir = queued_move_dir;
      queued_move_dir = make_vector(0, 0);
      pull = queued_pull;

      orient_hero();

      if (valid_dir(move_dir))
	initiate_move();
    }
  }
}

void Hero::order_move(Vector<int> dir, bool d)
{
  queued_move_dir = dir;
  queued_pull = d;
}

void Hero::make_invisible()
{
  invisible = Game::iterations_per_sec * 20;
  orient_hero();		// update appearance
}

bool Hero::is_invisible()
{
  return invisible > 0;
}

bool Hero::valid_dir(Vector<int> dir)
{
  Vector<int> tile = tile_pos + dir;
  Thing *thing = Arena::instance().occupier(tile);

  if (!thing)
    return true;
  else if (dynamic_cast<PowerUp *>(thing))
    return true;
  else if (dynamic_cast<Block *>(thing) && !pull) {
    // consider whether we can push this block
    for (;;) {
      tile += dir;
      thing = Arena::instance().occupier(tile);
      
      if (!thing)
	return true;
      else if (dynamic_cast<Hero *>(thing))
	return false;
      else if (dynamic_cast<Wall *>(thing))
	return false;
      else if (dynamic_cast<MonsterWall *>(thing))
	return false;
      else if (dynamic_cast<PowerUp *>(thing))
	return false;
      else if (Monster *m = dynamic_cast<Monster *>(thing)) {
	// check whether next tile is good for splatting
	tile += dir;
	thing = Arena::instance().occupier(tile);
	
	if (dynamic_cast<Wall *>(thing) ||
	    dynamic_cast<Block *>(thing) && !m->hard)
	  return true;
	else
	  return false;
      }
      else if (dynamic_cast<Egg *>(thing))
	return true;		// we can always splat eggs directly
    }
  }
  else
    return false;
}

void Hero::cross_boundary(Vector<int> dir)
{
  Vector<int> tile = tile_pos + dir;
  Thing *next = Arena::instance().occupier(tile);
  Block *block = dynamic_cast<Block *>(next);

  // no blocks
  if (!block) {
    // but perhaps a power up
    if (PowerUp *power_up = dynamic_cast<PowerUp *>(next))
      power_up->eat(player);
    
    return;
  }

  // else move each encountered block
  do {
    tile += dir;
    next = Arena::instance().occupier(tile);
    block->move_block_to(tile);
    block = dynamic_cast<Block *>(next);
  } while (block);

  if (Monster *monster = dynamic_cast<Monster *>(next)) {
    // we splatted a monster
    monster->splat(dir);
    player.scored(monster->kill_score);

    Game::instance().remove_monster(monster);
  }
  else if (Egg *egg = dynamic_cast<Egg *>(next)) {
    // we splatted an egg
    egg->splat(dir);
    player.scored(Egg::kill_score);
    
    Game::instance().remove_egg(egg);
  }

  next = Arena::instance().occupier(tile + dir);
  if (dynamic_cast<Wall *>(next) || dynamic_cast<Block *>(next))
    Sound::instance().play("clinck.wav");
}

bool Hero::pullable_block(Vector<int> tile)
{
  Arena &arena = Arena::instance();
  
  // it needs to be a block
  if (!dynamic_cast<Block *>(arena.occupier(tile)))
    return false;

  // we don't allow monsters around the block, then it would be too easy to
  // splat them by repeatedly pulling and pushing
  if (dynamic_cast<Monster *>(arena.occupier(tile + make_vector(-1,-1))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector( 0,-1))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector( 1,-1))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector(-1, 0))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector( 1, 0))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector(-1, 1))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector( 0, 1))) ||
      dynamic_cast<Monster *>(arena.occupier(tile + make_vector( 1, 1))))
    return false;
  else
    return true;
}

void Hero::pull_block(Vector<int> tile, Vector<int> dir)
{
  Block *block = dynamic_cast<Block *>(Arena::instance().occupier(tile));
  assert(block != 0);

  Arena::instance().occupy(tile, 0);
  block->move_block_to(tile + dir);
}

Monster *Hero::monster_encountered()
{
  for (Game::monster_sequence::iterator i = Game::instance().monsters.begin(),
	 end = Game::instance().monsters.end(); i != end; ++i)
    // the collision detection is rather crude, simply distance
    if (dist((*i)->get_graphic().get_pos(), get_graphic().get_pos())
	< Arena::instance().tile_size / 2)
      return *i;

  return 0;
}

void Hero::orient_hero()
{
  // first check that the pull isn't completely bogus
  if (pull &&
      !dynamic_cast<Block *>(Arena::instance().occupier(tile_pos - move_dir)))
    pull = false;
  
  // determine direction - reverse it when attempting pull
  Vector<int> dir;
  if (pull)
    dir = -move_dir;
  else
    dir = move_dir;

  footsteps.orient(dir);

  // update pixbuf
  std::string im; 
  if (dir.x == 1 && dir.y == 0)
    im = "hero-right.png";
  else if (dir.x ==  1 && dir.y ==  1)
    im = "hero-right-down.png";
  else if (dir.x ==  0 && dir.y ==  1)
    im = "hero-down.png";
  else if (dir.x == -1 && dir.y ==  1)
    im = "hero-left-down.png";
  else if (dir.x == -1 && dir.y ==  0)
    im = "hero-left.png";
  else if (dir.x == -1 && dir.y == -1)
    im = "hero-left-up.png";
  else if (dir.x ==  0 && dir.y == -1)
    im = "hero-up.png";
  else
    im = "hero-right-up.png";

  if (is_invisible())
    im = "invisible-" + im;
  
  get_graphic().set_pixbuf(ImageCache::instance()
			   .get(Arena::instance().tile_name(im)).get());
}

void Hero::initiate_move()
{
  movement.head(move_dir);

  // abort pull if really not possible
  if (pull && !pullable_block(tile_pos - move_dir))
    pull = false;
}

void Hero::levitate_blocks(Vector<int> dir)
{
  if (pull) {
    Vector<int> tile = tile_pos - dir;
    Block *block = dynamic_cast<Block *>(Arena::instance().occupier(tile));

    if (block) {
      block->levitate(0);
      unlevitate_blocks();
      levitated_blocks.push_back(block);
    }
  }
  else {

    Vector<int> tile = tile_pos + dir;
    Thing *next = Arena::instance().occupier(tile);
    Block *block = dynamic_cast<Block *>(next);

    // no blocks
    if (!block)
      return;

    std::vector<Block *> blocks;

    int distance = 0;
  
    // build a new set of encountered block
    do {
      tile += dir;
      next = Arena::instance().occupier(tile);
    
      blocks.push_back(block);
      block->levitate(distance++);
    
      block = dynamic_cast<Block *>(next);
    } while (block);

    // unlevitate old ones
    unlevitate_blocks();
  
    levitated_blocks = blocks;
  }
}

void Hero::unlevitate_blocks()
{
  for (std::vector<Block *>::iterator i = levitated_blocks.begin(),
	 end = levitated_blocks.end(); i != end; ++i)
    (*i)->unlevitate();

  levitated_blocks.clear();
}
