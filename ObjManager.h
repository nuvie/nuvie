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
#define OBJ_STATUS_NO_OBJ       0
#define OBJ_STATUS_OK_TO_TAKE   1
#define OBJ_STATUS_IN_CONTAINER 2
#define OBJ_STATUS_IN_PARTY_INV 3
#define OBJ_STATUS_NOT_PASSABLE 4
#define OBJ_STATUS_PASSABLE     5

//object numbers

#define OBJ_U6_CHEST           98
#define OBJ_U6_CANDLE         122
#define OBJ_U6_BOOK           151
#define OBJ_U6_SCROLL         152
#define OBJ_U6_V_PASSTHROUGH  278
#define OBJ_U6_H_PASSTHROUGH  280
#define OBJ_U6_LADDER         305
#define OBJ_U6_SIGN           332
#define OBJ_U6_SECRET_DOOR    334

typedef struct
{
 uint16 x;
 uint16 y;
 U6LList *objs;
} ObjList;

 
typedef struct
{
 uint16 objblk_n;
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
 bool is_boundary(uint16 x, uint16 y, uint8 level);
 uint8 is_passable(uint16 x, uint16 y, uint8 level);
 Tile *get_obj_tile(uint16 x, uint16 y, uint8 level, bool top_obj = true);
 Obj *get_obj(uint16 x, uint16 y, uint8 level, bool top_obj = true);
 Obj *get_objBasedAt(uint16 x, uint16 y, uint8 level, bool top_obj);
 
 uint16 get_obj_tile_num(uint16 obj_num);

 bool use_obj(Obj *obj);
 bool move(Obj *obj, uint16 x, uint16 y, uint8 level);
 char *look_obj(Obj *obj);
 
 protected:
 
 bool loadBaseTile();

 U6LList *loadObjSuperChunk(char *filename);
 void addObj(U6LList *list, Obj *obj);
 bool addObjToContainer(U6LList *list, Obj *obj);
 Obj *loadObj(U6File *file, uint16 objblk_n);
 char *get_objblk_path(char *path);
 
 inline U6LList *ObjManager::get_schunk_list(uint16 x, uint16 y, uint8 level);
};

#endif /* __ObjManager_h__ */
