#ifndef __Map_h__
#define __Map_h__

/*
 *  Map.h
 *  Nuive
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
 Screen *screen;
  
 uint8 *surface;
 uint8 **dungeons;
 
 uint16 cur_x, cur_y;
 
 uint16 win_width, win_height;
 uint8 cur_level;
 
 public:

 Map(Configuration *cfg);
 ~Map();
 
 bool loadMap(Screen *s, TileManager *tm, ObjManager *om);
 
 void set_windowSize(uint16 width, uint16 height);
 
 void moveWindowLevel(uint8 new_level);
 void moveWindow(uint16 new_x, uint16 new_y);
 void moveWindowRelative(sint16 rel_x, sint16 rel_y);
 
 
 void get_windowLevel(uint8 *level);
 void get_windowPos(uint16 *x, uint16 *y);
 
 void drawMap();


 //void get_schunk_coords(&uint16 schunk_num, &uint16 schunk_x, &uint16 schunk_y);
 
 protected:
 
 void drawObjs();
 void drawObjSuperBlock(U6LList *superblock);
 inline void drawObj(Obj *obj);
 inline void drawTile(uint16 tile_num, uint16 x, uint16 y);
 
 void insertSurfaceSuperChunk(unsigned char *schunk_ptr, unsigned char *chunk_data, uint8 schunk_num);
 void insertSurfaceChunk(unsigned char *chunk, uint16 x, uint16 y);
 
};

#endif /* __Map_h__ */

