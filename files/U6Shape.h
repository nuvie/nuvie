/*
 *  U6Shape.h
 *  Nuvie
 *
 *  Created by Markus Niemistö on Sun May 25 2003.
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
#include <SDL.h>
#include "U6def.h"


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
	int num_shapes;
	SDL_Surface **shape;
	unsigned char **raw;
	uint16 *hotx, *hoty;

public:
	U6Shape();
	~U6Shape();

	bool load(std::string file_name);
	int get_num_shapes();
	SDL_Surface *get_shape_surface(int index);
	unsigned char *get_shape_data(int index);
	bool get_hot_point(int index, uint16 *x, uint16 *y);
};


#endif
