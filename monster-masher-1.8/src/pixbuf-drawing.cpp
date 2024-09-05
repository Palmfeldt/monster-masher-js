/* Pixbuf-drawing helpers.
 *
 * Copyright (c) 2002, 2003 Ole Laursen.
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

#include "pixbuf-drawing.hpp"

void scale_alpha(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf, int scale)
{
  for (PixelIterator i = begin(pixbuf), e = end(pixbuf); i != e; ++i)
    i->alpha() = i->alpha() * scale / 256;
}


// shift the color by the given hue shift amount
// which is in { 0, 1, ..., 6 * 256 - 1 }
inline void shift_hue(unsigned char &r, unsigned char &g, unsigned char &b,
		      int shift)
{
  //
  // To make any sense of the calculations, consider the algorithm for
  // transformations between the RGB and the HSV color space - since the
  // saturation and value are fixed, we only need to consider the hue. The hue
  // falls in one of the 6 cases:
  //
  //   0: red to yellow, 1: yellow to green, 2: green to cyan, 3: cyan to blue
  //   4: blue to violet, 5: violet to red
  //
  // In each case, one color is at the maximum value, one is at the minimum
  // value and the last one is either increasing or decreasing betweeen them,
  // e.g. in case 0, red is at top, blue at bottom and green is increasing
  // linearly from the left end of the hue interval towards the right end,
  // changing the color from red to yellow.
  //
  
  // determine current case and hue inside the case
  unsigned char min, max, delta;
  int n, hue;
  
  if (r >= g)			// high middle low
    if (r >= b)
      if (g >= b) {		// r g b
	max = r;
	min = b;
	delta = max - min;
	if (delta == 0)		// achromatic (black-grey-white)
	  return;
	hue = 255 * (g - b) / delta;
	n = 0;		
      }
      else {			// r b g
	max = r;
	min = g;
	delta = max - min;
	if (delta == 0)	
	  return;
	hue = 255 * (b - g) / delta;
	n = 5;
      }
    else {			// b r g
      max = b;
      min = g;
      delta = max - min;
      if (delta == 0)	
	return;
      hue = 255 * (r - g) / delta;
      n = 4;	
    }
  else
    if (g >= b)
      if (r >= b) {		// g r b
	max = g;
	min = b;
	delta = max - min;
	if (delta == 0)	
	  return;
	hue = 255 * (r - b) / delta;
	n = 1;	
      }
      else {			// g b r
	max = g;
	min = r;
	delta = max - min;
	if (delta == 0)
	  return;
	hue = 255 * (b - r) / delta;
	n = 2;	
      }
    else {			// b g r
      max = b;
      min = r;
      delta = max - min;
      if (delta == 0)	
	return;
      hue = 255 * (g - r) / delta;
      n = 3;	
    }
  
  // shift hue and case
  hue += shift;

  n += hue / 256;
  hue %= 256;
  
  if (n > 5)
    n -= 6;

  // determine new RGB values
  unsigned char
    inc = (255 * min + delta * hue) / 255,
    dec = (255 * max - delta * hue) / 255;

  switch (n) {
  case 0:
    r = max;
    g = inc;
    b = min;
    break;
    
  case 1:
    r = dec;
    g = max;
    b = min;
    break;

  case 2:
    r = min;
    g = max;
    b = inc;
    break;

  case 3:
    r = min;
    g = dec;
    b = max;
    break;

  case 4:
    r = inc;
    g = min;
    b = max;
    break;

  default:			// case 5:
    r = max;
    g = min;
    b = dec;
    break;
  }
}


// shift the color by the given hue shift amount
// which is in { 0, 1, ..., 6 * 256 - 1 }
void shift_hue(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf, int shift)
{
  for (PixelIterator i = begin(pixbuf), e = end(pixbuf); i != e; ++i)
    shift_hue(i->red(), i->green(), i->blue(), shift);
}
