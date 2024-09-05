/* Implementation of splatting helpers.
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

#include <cstdlib>		// for rand

#include "splat-helpers.hpp"

#include "arena.hpp"
#include "canvas.hpp"
#include "game.hpp"
#include "graphic.hpp"
#include "obstacles.hpp"
#include "fading-decoration.hpp"
#include "pixbuf-drawing.hpp"


Vector<int> find_splat_center(Vector<double> cpos, Vector<int> tile_pos,
			      Vector<int> size, Vector<int> dir)
{
  // "move" position to border of tile
  int ts = Arena::instance().tile_size;
  Vector<int> pos;

  if (dir.x == 0)
    pos.x = int(cpos.x + size.x / 2);
  else
    pos.x = tile_pos.x * ts + (1 + dir.x) * ts / 2;

  if (dir.y == 0)
    pos.y = int(cpos.y + size.y / 2);
  else
    pos.y = tile_pos.y * ts + (1 + dir.y) * ts / 2;
      
  return pos;
}

void create_gore(Vector<int> pos, Vector<int> tile,
		 Glib::RefPtr<Gdk::Pixbuf> source)
{
  Thing *thing = Arena::instance().occupier(tile);

  int ts = Arena::instance().tile_size;
  
  if (dynamic_cast<Wall *>(thing) || dynamic_cast<Block *>(thing)) {
    // determine clipping parameters
    Vector<int>
      t1 = tile * ts,
      t2 = t1 + make_vector(ts, ts);
    // sp1 is upper left corner on source, sp2 is lower right corner
    Vector<int>
      sp1 = clip(t1, pos, t2) - pos,
      sp2 = clip(t1, pos + make_vector(source->get_width(),
				      source->get_height()), t2) - pos,
      tp1 = clip(t1, pos, t2) - t1; // upper left on tile
    
    Vector<int> size = sp2 - sp1;
    
    // create pixbuf
    Glib::RefPtr<Gdk::Pixbuf> pixbuf
      	= Gdk::Pixbuf::create(source->get_colorspace(), source->get_has_alpha(),
			      source->get_bits_per_sample(), size.x, size.y);

    // copy using obstacle as mask
    PixelIterator dest_i = begin(pixbuf);
    Glib::RefPtr<Gdk::Pixbuf> obstacle = thing->get_graphic().get_pixbuf();
    for (int y = 0; y < size.y; ++y) {
      PixelPosition src_p = get_position(source, sp1.x, sp1.y + y);
      PixelPosition mask_p = get_position(obstacle, tp1.x, tp1.y + y);
      
      for (int x = 0; x < size.x; ++x, src_p.right(), mask_p.right()) {
	dest_i->red() = src_p.pixel().red();
	dest_i->green() = src_p.pixel().green();
	dest_i->blue() = src_p.pixel().blue();
	dest_i->alpha()
	  = int(src_p.pixel().alpha()) * mask_p.pixel().alpha() / 255;
	++dest_i;
      }
    }
    
    // create decoration
    Graphic *graph = new Graphic(Canvas::instance().block_layer);
    graph->set_pixbuf(pixbuf);
    graph->place_at(pos + sp1);
    
    FadingDecoration *deco
      = new FadingDecoration(graph, 20, 5 * Game::iterations_per_sec);
    
    if (Block *block = dynamic_cast<Block *>(thing)) {
      block->add_cargo(graph);
      Game::instance().add_decoration(deco, block);
    }
    else
      Game::instance().add_decoration(deco);
  }
}

void splat_on_obstacles(Vector<double> cpos, Glib::RefPtr<Gdk::Pixbuf> pixbuf,
			Vector<int> tile_pos, Vector<int> dir)
{
  int pw = pixbuf->get_width(), ph = pixbuf->get_height();
  int ts = Arena::instance().tile_size;
  
  Vector<int> pos = find_splat_center(cpos, tile_pos, make_vector(pw, ph), dir)
    + make_vector(std::rand() % 5 - 2, std::rand() % 5 - 2);

  pos -= make_vector(pw, ph) / 2;
    
  Vector<int>
    ul_tile(pos.x / ts, pos.y / ts),
    ur_tile((pos.x + pw - 1) / ts, pos.y / ts),
    ll_tile(pos.x / ts, (pos.y + ph - 1) / ts),
    lr_tile((pos.x + pw - 1) / ts, (pos.y + ph - 1) / ts);

  if (ul_tile == ur_tile && ul_tile == ll_tile)
    create_gore(pos, ul_tile, pixbuf);
  else if (ul_tile == ur_tile) {
    create_gore(pos, ul_tile, pixbuf);
    create_gore(pos, ll_tile, pixbuf);
  }
  else if (ul_tile == ll_tile) {
    create_gore(pos, ul_tile, pixbuf);
    create_gore(pos, ur_tile, pixbuf);
  }
  else {
    create_gore(pos, ul_tile, pixbuf);
    create_gore(pos, ur_tile, pixbuf);
    create_gore(pos, ll_tile, pixbuf);
    create_gore(pos, lr_tile, pixbuf);
  }
}


void splat_on_floor(Vector<double> pos, Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  Graphic *graph = new Graphic(Canvas::instance().background_layer);
  graph->set_pixbuf(pixbuf);
  graph->place_at(pos);
  
  FadingDecoration *deco
    = new FadingDecoration(graph, 100, Game::iterations_per_sec / 10);
  Game::instance().add_decoration(deco);
}

