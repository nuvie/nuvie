/*
 *  U6Shape.cpp
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

/*
 * ==========
 *  Includes
 * ==========
 */
#include <string>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_endian.h>
#include "U6def.h"
#include "U6Lzw.h"
#include "U6Shape.h"


/*
 * Structure of shape file:
 * ========================
 *
 *      -> means input from file
 *      <- means storing pixel data
 *
 * .shp files are lzw compressed. After decompressing the file represents
 * following structure: -> file size (dword)
 *                      -> set of offsets (each word)
 *			-> set of shapes
 *
 * File size should be quite clear.
 *
 * Offsets are stored as unsigned words. The first offset in file is the
 * offset of the first object (simple, huh?). The number of offsets (objects)
 * in the file can be calculated as follows:
 *                      num_objects = (1st offset - 4) / 4.
 *
 * Frame structure:     -> num of pixels right from hot spot, X1 (word)
 *                      -> num of pixels left from hot spot, X2 (word)
 *                      -> num of pixels above hot spot, Y1 (word)
 *                      -> num of pixels below hot spot, Y2 (word)
 *                      -> set of pixel blocks
 *
 * The width of the shape can be calculated by adding X1 and X2 together and
 * height by adding Y1 and Y2 together. Coordinates for hot spot are X2 and Y1.
 *
 * Now the data it self is stored in pixel blocks which are quite complex:
 *                      -> number of pixels or repeats, num1 (word)
 *                      if (num1 and 1)
 *                         repeat num1 >> 1 times
 *                            -> temp value (unsigned byte)
 *                            if (temp value and 1)
 *                               -> pixel
 *                               <- store pixel temp value >> 1 times
 *                            else
 *                               <- read temp value >> 1 bytes
 *                         end
 *                     else
 *                         <- read num >> 1 bytes
 *
 * Color number 255 seems to be transperent.
 *
 * I hope this clears things up a bit.
 */


/*
 * =====================
 *  U6Shape::U6Shape();
 * =====================
 *
 * Just intializes all structures to 0.
 */
U6Shape::U6Shape()
{
	num_shapes = 0;
	shape = NULL;
	raw = NULL;
	hotx = hoty = NULL;
}


/*
 * ======================
 *  U6Shape::~U6Shape();
 * ======================
 *
 * Frees all memory allocated by this instance of U6Shape class.
 */
U6Shape::~U6Shape(void)
{
	if (num_shapes == 0)
		return;
		/* NOT REACHED */

	/* Free memory. */
	for (int i = 0; i < num_shapes; i++)
	{
		free(raw[i]);
		SDL_FreeSurface(shape[i]);
	}
	free(hotx);
	free(hoty);
	free(raw);
	free(shape);
}


/*
 * ===========================================
 *  bool U6Shape::load(std::string file_name);
 * ===========================================
 *
 * Loads all shapes from lzw-compressed file file_name.
 * Returns true if successful, else returns false.
 */
bool U6Shape::load(std::string file_name)
{
	U6Lzw *lzw;
	int encoded;
	uint32 file_size, file_size_lzw, temp;
	uint32 *shape_offset;
	unsigned char *data, *start;
	uint16 num_pixels, last_num_pixels = 0;
	sint16 xsize, ysize, xpos, ypos;

	/* A file already loaded. */
	if (num_shapes != 0)
		return false;
		/* NOT REACHED */

	/* Create a decompressor. */
	lzw = new U6Lzw();

	/* First decompress the file. */
	if ((data = start = lzw->decompress_file(file_name, file_size_lzw)) ==
	   NULL)
		return false;
		/* NOT REACHED */

	/* Decompressor is no longer needed. */
	delete lzw;

	/* Get file size. */
	file_size = SDL_SwapLE32(*(uint32*)data); data += 4;

	/* Test wheter file is corrupted. */
	if (file_size != file_size_lzw)
		return false;
		/* NOT RECHED */

	/* Calculate number of shapes. */
	temp = SDL_SwapLE32(*(uint32*)data);
	num_shapes = (temp - 4) / 4;

	/* Allocate memory. */
	shape_offset = (uint32*)malloc(sizeof(uint32) * num_shapes);
	shape = (SDL_Surface**)malloc(sizeof(SDL_Surface**) * num_shapes);
	raw = (unsigned char**)malloc(sizeof(unsigned char**) * num_shapes);
	hotx = (uint16*)malloc(sizeof(uint16) * num_shapes);
	hoty = (uint16*)malloc(sizeof(uint16) * num_shapes);

	/* Get shape offsets. */
	for (int i = 0; i < num_shapes; i++)
	{
		shape_offset[i] = SDL_SwapLE32(*(uint32*)data); data += 4;
	}

	/* Now we'll get the shapes themselves. */
	for (int i = 0; i < num_shapes; i++)
	{
		/* Find the start. */
		data = start + shape_offset[i];

		/* Size and hot point. */
		xsize = SDL_SwapLE16(*(uint16*)data); data += 2;
		xsize += hotx[i] = SDL_SwapLE16(*(uint16*)data); data += 2;

		ysize = hoty[i] = SDL_SwapLE16(*(uint16*)data); data += 2;
		ysize += SDL_SwapLE16(*(uint16*)data); data += 2;

		/* Allocate memory for shape and make it all transperent. */
		raw[i] = (unsigned char*)malloc(xsize * ysize);
		memset(raw[i], 255, xsize * ysize);

		/* Get the pixel data. */
		while ((num_pixels = SDL_SwapLE16(*(uint16*)data)) != 0)
		{
			data += 2;

			/* Coordinates relative to hot spot. */
			xpos = *(uint16*)data; data += 2;
			ypos = *(uint16*)data; data += 2;

			/*
			 * Test if this block of pixels is encoded
			 * (bit0 is set).
			 */
			encoded = num_pixels & 1;

			/* Divide it by 2. */
			num_pixels >>= 1;

			/* Normal pixel:
			 * =============
			 *
			 * Just fetch as many pixels as num_pixels suggests.
			 */
			if (!encoded)
			{
				memcpy(raw[i] + (hotx[i] + xpos) +
				   (hoty[i] + ypos) * xsize, data, num_pixels);
				data += num_pixels;

				continue;
				/* NOT REACHED */
			}

			/* Encoded pixel:
			 * ==============
			 *
			 * Do as many repeats as num_pixels suggests.
			 */
			for (int j = 0; j < num_pixels; )
			{
				unsigned char num_pixels2 = *data++;
				int repeat = num_pixels2 & 1;

				num_pixels2 >>= 1;

				/*
				 * Repeat pixel value (data + 1) num_pixels2
				 * times.
				 */
				if (repeat)
				{
					memset(raw[i] + (hotx[i] + xpos) +
					   (hoty[i] + ypos) * xsize + j,
					   *data++, num_pixels2);
				}


			 	/*
				 * Just fetch as many pixels as num_pixels2
				 * suggests.
				 */
				else
				{
					memcpy(raw[i] + (hotx[i] + xpos) +
					   (hoty[i] + ypos) * xsize + j, data,
					   num_pixels2);
					data += num_pixels2;
				}
				j += num_pixels2;
			}

		}

		/* Create a SDL_Surface. */
		shape[i] = SDL_CreateRGBSurfaceFrom(raw[i], xsize, ysize,
		   8, xsize, 0, 0, 0, 0);
	}

	/* Free temporarily allocated memory. */
	free(start);
	free(shape_offset);
	return true;
}


/*
 * ================================
 *  int U6Shape::get_num_shapes();
 * ================================
 *
 * Returns the number of shapes.
 */
int U6Shape::get_num_shapes()
{
	return num_shapes;
}


/*
 * =====================================================
 *  SDL_Surface *U6Shape::get_shape_surface(int index);
 * =====================================================
 *
 * Returns a SDL_Surface representing the shape number index
 * or NULL on failure.
 */
SDL_Surface *U6Shape::get_shape_surface(int index)
{
	if ((num_shapes == 0) || (index > num_shapes))
		return NULL;
		/* NOT REACHED */

	return shape[index];
}


/*
 * ====================================================
 *  unsigned char *U6Shape::get_shape_data(int index);
 * ====================================================
 *
 * Returns raw data representing the shape number index or NULL on failure.
 */
unsigned char *U6Shape::get_shape_data(int index)
{
	if ((num_shapes == 0) || (index > num_shapes))
		return NULL;
		/* NOT REACHED */

	return raw[index];
}


/*
 * ===============================================================
 *  bool U6Shape::get_hot_point(int index, uint16 *x, uint16 *y);
 * ===============================================================
 *
 * Puts the coordinates of the shape whose number is index in x and y and
 * returns true or on failure just returns false.
 */
bool U6Shape::get_hot_point(int index, uint16 *x, uint16 *y)
{
	if ((num_shapes == 0) || (index > num_shapes) ||
	   (x == NULL) || (y == NULL))
		return false;
		/* NOT REACHED */

	*x = hotx[index];
	*y = hoty[index];
	return true;
}
