#ifndef __ObjManager_h__
#define __ObjManager_h__

/*
 *  ObjManager.h
 *  Nuive
 *
 *  Created by Eric Fry on Sat Mar 15 2003.
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

#include "TileManager.h"

// obj status bit flags
#define OBJ_STATUS_OK_TO_TAKE   0x1
#define OBJ_STATUS_IN_CONTAINER 0x8
#define OBJ_STATUS_IN_PARTY_INV 0x10

typedef struct
{
 uint16 obj_n;
 uint8 frame_n;
 
 uint8 status;
 uint16 x;
 uint16 y;
 uint8 z; // ??
 
 uint8 qty;
 uint8 quality;
 
 U6LList *container;
} Obj;

class ObjManager
{
 Configuration *config;
 TileManager *tile_manager;
 U6LList *surface[64];
 U6LList *dungeon[5];
 uint16 obj_to_tile[1024]; //maps object number (index) to tile number.
 
 public:
 
 ObjManager(Configuration *cfg);
 ~ObjManager();
 
 bool loadObjs(TileManager *tm);
 
 U6LList *get_obj_superchunk(uint16 x, uint16 y, uint8 level);
 Tile *ObjManager::get_obj_tile(uint16 x, uint16 y, uint8 level);
 
 uint16 get_obj_tile_num(uint16 obj_num);

 protected:
 
 bool loadBaseTile();

 U6LList *loadObjSuperChunk(char *filename);
 bool addObjToContainer(U6LList *list, Obj *obj);
 Obj *loadObj(U6File *file);
 char *get_objblk_path(char *path);
};

#endif /* __ObjManager_h__ */
