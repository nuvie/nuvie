#ifndef __Map_h__
#define __Map_h__

/*
 *  Map.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
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

#include "U6def.h"
#include "U6File.h"
#include "U6LList.h"

#include "Configuration.h"
#include "Screen.h"
#include "TileManager.h"
#include "ObjManager.h"

class Map
{
 Configuration *config;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 
 uint8 *surface;
 uint8 *dungeons[5];
  
 public:

 Map(Configuration *cfg);
 ~Map();
 
 bool loadMap(TileManager *tm, ObjManager *om);
 unsigned char *get_map_data(uint8 level);
 uint16 get_width(uint8 level);
 bool is_passable(uint16 x, uint16 y, uint8 level);
 bool is_boundary(uint16 x, uint16 y, uint8 level);
 
 const char *look(uint16 x, uint16 y, uint8 level);
 
 protected:
  
 void insertSurfaceSuperChunk(unsigned char *schunk_ptr, unsigned char *chunk_data, uint8 schunk_num);
 void insertSurfaceChunk(unsigned char *chunk, uint16 x, uint16 y);
 
 void insertDungeonSuperChunk(unsigned char *schunk_ptr, unsigned char *chunk_data, uint8 level);
 void insertDungeonChunk(unsigned char *chunk, uint16 x, uint16 y, uint8 level); 
};

#endif /* __Map_h__ */

