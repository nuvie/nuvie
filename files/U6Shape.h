/*
 *  U6Shape.h
 *  Nuvie
 *
 *  Created by Markus Niemisto on Sun May 25 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef __U6Shape_h__
#define __U6Shape_h__


/*
 * ==========
 *  Includes
 * ==========
 */
#include <string>
#include "SDL.h"

class U6Lib_n;
class Configuration;

/*
 * ==================
 *  Class definition
 * ==================
 *
 * U6Shape can load Ultima VI shape files and return the shapes
 * stored into these files either as a SDL_Surface or as raw data.
 */
class U6Shape
{
private:
	uint16 hotx, hoty;

protected:
	unsigned char *raw;
  uint16 width, height;

public:
	U6Shape();
	virtual ~U6Shape();

	bool init(uint16 w, uint16 h, uint16 hx=0, uint16 hy=0);
	virtual bool load(std::string filename);
  bool load(U6Lib_n *file, uint32 index);
	virtual bool load(unsigned char *buf);
	bool load_from_lzc(std::string filename, uint32 idx, uint32 sub_idx);
	bool load_WoU_background(Configuration *config, nuvie_game_t game_type);

	unsigned char *get_data();
  SDL_Surface *get_shape_surface();
	bool get_hot_point(uint16 *x, uint16 *y);
	bool get_size(uint16 *w, uint16 *h);

	void draw_line(uint16 sx, uint16 sy, uint16 ex, uint16 ey, uint8 color);
	bool blit(U6Shape *shp, uint16 x, uint16 y);
	void fill(uint8 color);
};

#endif
