/* Implementation of monster classes.
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

#include <cmath>		// for atan2 and cos/sin
#include <cstdlib>		// for rand
#include <queue>		// for priority_queue
#include <vector>

#include "monsters.hpp"

#include "arena.hpp"
#include "canvas.hpp"
#include "graphic.hpp"
#include "hero.hpp"
#include "player.hpp"
#include "game.hpp"
#include "obstacles.hpp"
#include "persistent.hpp"
#include "splat-helpers.hpp"
#include "sound.hpp"


//
// base Monster class 
//

int transform_movement_steps(int s)
{
  if (Persistent::instance().difficulty_level == difficulty_easy)
    return int(s * 1.20);
  else if (Persistent::instance().difficulty_level == difficulty_hard)
    return int(s * 0.80);
  else
    return s;
}


Monster::Monster(const Images &m, const Images &g,
		 int movement_steps, int ks, bool h)
  : Thing(new Graphic(Canvas::instance().creature_layer)),
    kill_score(ks), hard(h),
    movement(*this, transform_movement_steps(movement_steps)),
    thinking_time(std::rand() % (2 * Game::iterations_per_sec)),
    move_animation(get_graphic(), m, 1, rand() % (1 * m.size())), 
    gore(g),
    prev_target(0)
{
}

Monster::~Monster()
{
}

void Monster::update()
{
  if (movement.is_moving()) {
    if (!movement.has_left_tile() && !valid_tile(movement.get_target_tile())) {
      movement.turn_around();
      movement.update();
      move_animation.update();
    }
    else {
      bool halfway = movement.is_halfway();
      Vector<int> prev_tile = tile_pos;
    
      movement.update();
      move_animation.update();

      if (halfway)
	do_halfway(prev_tile);
    }
  }
  else if (freeze_box.get() != 0) {
    if (!Game::instance().monsters_freezing())
      // 10 % chance of thawing
      if (std::rand() < RAND_MAX / 10)
	freeze_box.reset();
  }
  else if (Game::instance().monsters_freezing()) {
    Graphic *g = new Graphic(Canvas::instance().creature_layer);
    g->set_pixbuf(ImageCache::instance()
		  .get(Arena::instance().tile_name("freeze-box.png")).get());
    g->place_at(get_graphic().get_pos());
    freeze_box.reset(g);
  }
  else if (thinking_time > 0)
    // stay ruminating over the joys of life here
    --thinking_time;
  else
    do_update();
}


void Monster::splat(Vector<int> dir)
{
  Sound::instance().play("splat.wav");
  
  splat_on_obstacles(get_graphic().get_pos(),
		     gore.get(std::rand() % gore.size()), tile_pos, dir);
}

void Monster::explode()
{
  // FIXME: play sound
  
  splat_on_floor(get_graphic().get_pos(), gore.get(std::rand() % gore.size()));
}

void Monster::head(Vector<int> dir)
{
  movement.head(dir);
}

void Monster::set_thinking_time(int time)
{
  thinking_time = time;

  if (Persistent::instance().difficulty_level == difficulty_easy)
    thinking_time = int(thinking_time * 1.50);
  else if (Persistent::instance().difficulty_level == difficulty_hard)
    thinking_time = int(thinking_time * 0.50);
}

bool Monster::valid_tile(Vector<int> tile)
{
  Thing *thing = Arena::instance().occupier(tile);

  return thing == 0 || dynamic_cast<Hero *>(thing);
}

Vector<int> Monster::find_target()
{
  int d = 100000;
  Player *target = 0;
  
  // find the closest hero
  for (Game::player_sequence::iterator i = Game::instance().players.begin(),
	 end = Game::instance().players.end(); i != end; ++i) {
    Player *p = *i;
    if (!p->get_hero() || p->get_hero()->is_invisible())
      continue;
    
    int hd = dist(tile_pos, p->get_hero()->tile_pos);
    if (hd < d) {
      d = hd;
      target = p;
    }
  }
  
  // consider staying with the old target 
  if (prev_target && prev_target != target && prev_target->get_hero()) {
    int prev_d = dist(tile_pos, prev_target->get_hero()->tile_pos);

    bool very_close = d < 2;
    bool far_away = d > 8;
    if (!(very_close || (far_away && prev_d < d * 3/2)))
      target = prev_target;
  }

  prev_target = target;

  if (target)
    return target->get_hero()->tile_pos;
  else {
    // just wander around
    Vector<int> random = tile_pos + get_random_direction();
    if (valid_tile(random))
      return random;
    else
      return tile_pos;
  }
}

Vector<int> Monster::get_random_direction()
{
  switch (std::rand() % 8) {
  case 0: return make_vector(-1, 0);
  case 1: return make_vector(-1,-1);
  case 2: return make_vector( 0,-1);
  case 3: return make_vector( 1,-1);
  case 4: return make_vector( 1, 0);
  case 5: return make_vector( 1, 1);
  case 6: return make_vector( 0, 1);
  default: return make_vector(-1, 1);
  }
}

Vector<int> Monster::find_direction_dumbly(Vector<int> target_tile)
{
  if (target_tile == tile_pos)
    return make_vector(0, 0);

  if (std::rand() < RAND_MAX / 8) // try randomly
    for (int i = 0; i < 16; ++i) {
      Vector<int> v = get_random_direction();
      if (valid_tile(tile_pos + v))
	return v;
    }
  
  float angle = std::atan2(float(target_tile.y - tile_pos.y),
			   target_tile.x - tile_pos.x);

  // convert angle to direction
  if (angle < 3.14159 * -7/8 || angle > 3.14159 * 7/8) {
    if (valid_tile(tile_pos + make_vector(-1, 0)))
      return make_vector(-1, 0);
    else if (valid_tile(tile_pos + make_vector(-1, -1)))
      return make_vector(-1, -1);
    else if (valid_tile(tile_pos + make_vector(-1, 1)))
      return make_vector(-1, 1);
  }
  else if (angle < 3.14159 * -5/8) {
    if (valid_tile(tile_pos + make_vector(-1, -1)))
      return make_vector(-1, -1);
    else if (valid_tile(tile_pos + make_vector(-1, 0)))
      return make_vector(-1, 0);
    else if (valid_tile(tile_pos + make_vector(0, -1)))
      return make_vector(0, -1);
  }
  else if (angle < 3.14159 * -3/8) {
    if (valid_tile(tile_pos + make_vector(0, -1)))
      return make_vector(0, -1);
    else if (valid_tile(tile_pos + make_vector(-1, -1)))
      return make_vector(-1, -1);
    else if (valid_tile(tile_pos + make_vector(1, -1)))
      return make_vector(1, -1);
  }
  else if (angle < 3.14159 * -1/8) {
    if (valid_tile(tile_pos + make_vector(1, -1)))
      return make_vector(1, -1);
    else if (valid_tile(tile_pos + make_vector(0, -1)))
      return make_vector(0, -1);
    else if (valid_tile(tile_pos + make_vector(1, 0)))
      return make_vector(1, 0);
  }
  else if (angle < 3.14159 * 1/8) {
    if (valid_tile(tile_pos + make_vector(1, 0)))
      return make_vector(1, 0);
    else if (valid_tile(tile_pos + make_vector(1, -1)))
      return make_vector(1, -1);
    else if (valid_tile(tile_pos + make_vector(1, 1)))
      return make_vector(1, 1);
  }
  else if (angle < 3.14159 * 3/8) {
    if (valid_tile(tile_pos + make_vector(1, 1)))
      return make_vector(1, 1);
    else if (valid_tile(tile_pos + make_vector(1, 0)))
      return make_vector(1, 0);
    else if (valid_tile(tile_pos + make_vector(0, 1)))
      return make_vector(0, 1);
  }
  else if (angle < 3.14159 * 5/8) {
    if (valid_tile(tile_pos + make_vector(0, 1)))
      return make_vector(0, 1);
    else if (valid_tile(tile_pos + make_vector(1, 1)))
      return make_vector(1, 1);
    else if (valid_tile(tile_pos + make_vector(-1, 1)))
      return make_vector(-1, 1);
  }
  else if (angle < 3.14159 * 7/8) {
    if (valid_tile(tile_pos + make_vector(-1, 1)))
      return make_vector(-1, 1);
    else if (valid_tile(tile_pos + make_vector(0, 1)))
      return make_vector(0, 1);
    else if (valid_tile(tile_pos + make_vector(-1, 0)))
      return make_vector(-1, 0);
  }

  // else try randomly
  for (int i = 0; i < 16; ++i) {
    Vector<int> v = get_random_direction();
    if (valid_tile(tile_pos + v))
      return v;
  }
  
  // give up
  return make_vector(0, 0);
}



// helpers for path-finding algorithm
struct PathNode 
{
  enum { unknown, active, inactive, blocked } status;
  int home_dist, target_dist;
  Vector<int> p_offset;		// offset to go in to get to parent
};

typedef std::vector<PathNode> PathField;

struct cost_lt
{
  bool operator()(PathNode const *lhs, PathNode const *rhs)
  {
    return
      lhs->home_dist + lhs->target_dist >= rhs->home_dist + rhs->target_dist;
  }
};

typedef std::priority_queue<PathNode *, std::vector<PathNode *>,
			    cost_lt> PathQueue;

inline Vector<int> node_to_pos(const PathField &field, PathNode *n) 
{
  int delta = n - &field[0], width = Arena::instance().size.x;
  return make_vector(delta % width, delta / width);
}

inline
void visit_pathnode(PathNode &n, PathNode &oldn, Vector<int> p_offset,
		    PathField &field, PathQueue &active, 
		    Vector<int> target_tile)
{
  // the offset distance is always 1
  int newdist = oldn.home_dist + 1;
  
  switch (n.status) {
  case PathNode::active:
  case PathNode::inactive:
    if (newdist >= n.home_dist)
      return;			// not worth going with new
    break;
    
  case PathNode::unknown:
    {
      Vector<int> pos = node_to_pos(field, &n);

      Thing *t = Arena::instance().occupier(pos);
      if (t && !dynamic_cast<Hero *>(t)) {
	n.status = PathNode::blocked;
	return;			// tile occupied, discard
      }
      else
	// compute heuristic  
	n.target_dist = std::max(std::abs(pos.x - target_tile.x),
				 std::abs(pos.y - target_tile.y));
    }
    break;

  case PathNode::blocked:
    return;
  }
  
  n.status = PathNode::active;
  n.home_dist = newdist;
  n.p_offset = p_offset;
  active.push(&n);
}

Vector<int> Monster::find_direction_smartly(Vector<int> target_tile)
{
  Vector<int> size = Arena::instance().size;
  
  PathNode n;
  n.status = PathNode::unknown;
  PathField field(size.x * size.y, n);
  
  PathQueue active;

  // some helper values
  PathNode *target_address = &field[target_tile.y * size.x + target_tile.x];
  
  // fill in first element
  PathNode &home = field[tile_pos.y * size.x + tile_pos.x];
  home.status = PathNode::active;
  home.home_dist = 0;
  home.target_dist = 2 * std::max(std::abs(tile_pos.x - target_tile.x),
				  std::abs(tile_pos.y - target_tile.y));

  active.push(&home);

  PathNode *closest = &home;

  // use A* algorithm to find the shortest (or closest to target) path
  do {
    PathNode &n = *active.top();
    if (&n == target_address)
      break;

    active.pop();

    if (n.status != PathNode::active) // has been invalidated
      continue;

    n.status = PathNode::inactive;
    
    visit_pathnode(*(&n + 1 + size.x), n, make_vector(-1, -1),
		   field, active, target_tile);
    visit_pathnode(*(&n + 1), n, make_vector(-1,  0),
		   field, active, target_tile);
    visit_pathnode(*(&n + 1 - size.x), n, make_vector(-1,  1),
		   field, active, target_tile);
    visit_pathnode(*(&n + size.x), n, make_vector( 0, -1),
		   field, active, target_tile);
    visit_pathnode(*(&n - size.x), n, make_vector( 0,  1),
		   field, active, target_tile);
    visit_pathnode(*(&n - 1 + size.x), n, make_vector( 1, -1),
		   field, active, target_tile);
    visit_pathnode(*(&n - 1), n, make_vector( 1,  0),
		   field, active, target_tile);
    visit_pathnode(*(&n - 1 - size.x), n, make_vector( 1,  1),
		   field, active, target_tile);

    if (n.target_dist < closest->target_dist)
      closest = &n;
    
  } while (!active.empty());


  Vector<int> p, off(0, 0);
  if (active.empty())
    p = node_to_pos(field, closest);
  else
    p = target_tile;
  
  while (p != tile_pos) {
    off = field[p.y * size.x + p.x].p_offset;
    p += off;
  }
  
  return -off;
}



//
// PlainMonster class
//

int const plain_monster_kill_score = 1;

PlainMonster::PlainMonster()
  : Monster(ImageCache::instance()
	    .get(Arena::instance().tile_name("plain-monster.png"), 32),
	    ImageCache::instance()
	    .get(Arena::instance().tile_name("plain-gore.png"), 8),
	    16, plain_monster_kill_score, false)
{
}

void PlainMonster::do_update()
{
  // go get some food!
  Vector<int> target_tile = find_target();
  Vector<int> dir = find_direction_dumbly(target_tile);
      
  if (!(dir.x == 0 && dir.y == 0)) {
    head(dir);
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 8/5));
  }
  else				// wait so we don't waste CPU resources
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 1/5));
}


//
// HardMonster class
//

int const hard_monster_kill_score = 3;

HardMonster::HardMonster()
  : Monster(ImageCache::instance()
	    .get(Arena::instance().tile_name("hard-monster.png"), 48),
	    ImageCache::instance()
	    .get(Arena::instance().tile_name("hard-gore.png"), 8),
	    26, hard_monster_kill_score, true)
{
}

void HardMonster::do_update()
{
  // go get some food!
  Vector<int> target_tile = find_target();
  Vector<int> dir = find_direction_dumbly(target_tile);
      
  if (!(dir.x == 0 && dir.y == 0)) {
    head(dir);
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 2/3));
  }
  else				// wait so we don't waste CPU resources
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 1/5));
}


//
// SeekerMonster class
//

int const seeker_monster_kill_score = 2;

SeekerMonster::SeekerMonster()
  : Monster(ImageCache::instance()
	    .get(Arena::instance().tile_name("seeker-monster.png"), 32),
	    ImageCache::instance()
	    .get(Arena::instance().tile_name("seeker-gore.png"), 8),
	    14, seeker_monster_kill_score, false),
    move_mode(std::rand() % 4)
{
}

void SeekerMonster::do_update()
{
  // go get some food!
  Vector<int> target_tile = find_target();
  Vector<int> dir = find_direction_smartly(target_tile);
      
  if (!(dir.x == 0 && dir.y == 0)) {
    head(dir);
    if (--move_mode < 0) {
      set_thinking_time(std::rand() % (Game::iterations_per_sec * 3));
      move_mode = std::rand() % 4;
    }
  }
  else				// wait so we don't waste CPU resources
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 1/5));
}


//
// EggLayerMonster class
//

int const egg_layer_monster_kill_score = 4;

EggLayerMonster::EggLayerMonster()
  : Monster(ImageCache::instance()
	    .get(Arena::instance().tile_name("egg-layer-monster.png"), 48),
	    ImageCache::instance()
	    .get(Arena::instance().tile_name("egg-layer-gore.png"), 8),
	    24, egg_layer_monster_kill_score, false), hunting(true)
{
}

void EggLayerMonster::do_update()
{
  // go get some food!
  Vector<int> target_tile = find_target();
  Vector<int> dir(0, 0);
  
  if (std::max(std::abs(target_tile.x - tile_pos.x),
	       std::abs(target_tile.y - tile_pos.y)) > 5) {
    // if far away, just walk around laying eggs
    hunting = false;
    for (int i = 0; i < 16; ++i) { 
      Vector<int> v = get_random_direction();
      if (valid_tile(tile_pos + v)) {
	dir = v;
	break;
      }
    }
  }
  else {
    // if close, better go for the target
    hunting = true;
    dir = find_direction_dumbly(target_tile);
  }
  
  if (!(dir.x == 0 && dir.y == 0)) {
    head(dir);
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 2));
  }
  else				// wait so we don't waste CPU resources
    set_thinking_time(std::rand() % (Game::iterations_per_sec * 1/5));
}

void EggLayerMonster::do_halfway(Vector<int> prev_tile)
{
  if (hunting)			// don't lay egg when hunting
    return;

  Vector<int> size = Arena::instance().size;
  
  if (int(Game::instance().monsters.size()) < size.x * size.y / 6
      && Game::instance().eggs.size() < 15)
    if (std::rand() < RAND_MAX / 10) // lay an egg
      Game::instance().generate_egg(prev_tile);
}


//
// Egg base class
//

Egg::Egg(Vector<int> pos, const Images &images, int anim_rate, const Images &g)
  : Thing(new Graphic(Canvas::instance().creature_layer)),
    anim(get_graphic(), images, anim_rate),
    gore(g)
{
  occupy_tile(pos);
}

void Egg::update()
{
  anim.update();
}

bool Egg::hatch_ready()
{
  return anim.last_frame();
}

void Egg::hatch()
{
  // FIXME: play sound
  splat_on_floor(get_graphic().get_pos(), gore.get(std::rand() % gore.size()));
  
  Monster *m = do_hatch();
  m->occupy_tile(tile_pos);
  Game::instance().add_monster(m);
}

void Egg::splat(Vector<int> dir)
{
  Sound::instance().play("splat.wav");
  
  splat_on_obstacles(get_graphic().get_pos(),
		     gore.get(std::rand() % gore.size()), tile_pos, dir);
}

void Egg::explode()
{
  // FIXME: play sound
  splat_on_floor(get_graphic().get_pos(), gore.get(std::rand() % gore.size()));
}


//
// concrete Egg classes
//

PlainEgg::PlainEgg(Vector<int> pos)
  : Egg(pos, ImageCache::instance()
	.get(Arena::instance().tile_name("plain-egg.png"), 4),
	Game::iterations_per_sec * 4,
	ImageCache::instance()
	.get(Arena::instance().tile_name("plain-gore.png"), 8))
{
}

Monster *PlainEgg::do_hatch()
{
  return new PlainMonster;
}

HardEgg::HardEgg(Vector<int> pos)
  : Egg(pos, ImageCache::instance()
	.get(Arena::instance().tile_name("hard-egg.png"), 4),
	Game::iterations_per_sec * 4 * 3/2,
	ImageCache::instance()
	.get(Arena::instance().tile_name("hard-gore.png"), 8))
{
}

Monster *HardEgg::do_hatch()
{
  return new HardMonster;
}

SeekerEgg::SeekerEgg(Vector<int> pos)
  : Egg(pos, ImageCache::instance()
	.get(Arena::instance().tile_name("seeker-egg.png"), 4),
	Game::iterations_per_sec * 4 * 4/2,
	ImageCache::instance()
	.get(Arena::instance().tile_name("seeker-gore.png"), 8))
{
}

Monster *SeekerEgg::do_hatch()
{
  return new SeekerMonster;
}

EggLayerEgg::EggLayerEgg(Vector<int> pos)
  : Egg(pos, ImageCache::instance()
	.get(Arena::instance().tile_name("egg-layer-egg.png"), 4),
	Game::iterations_per_sec * 4 * 5/2,
	ImageCache::instance()
	.get(Arena::instance().tile_name("egg-layer-gore.png"), 8))
{
}

Monster *EggLayerEgg::do_hatch()
{
  return new EggLayerMonster;
}
