#ifndef __ObjManager_h__
#define __ObjManager_h__

/*
 *  ObjManager.h
 *  Nuvie
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

#include "iAVLTree.h"

#include "U6def.h"
#include "NuvieIOFile.h"
#include "U6LList.h"
#include "TileManager.h"
#include "Configuration.h"

class TileManager;
class UseCode;

// obj status bit flags
#define OBJ_STATUS_NO_OBJ       0
#define OBJ_STATUS_OK_TO_TAKE   1
#define OBJ_STATUS_IN_CONTAINER 2
#define OBJ_STATUS_IN_PARTY_INV 3
#define OBJ_STATUS_NOT_PASSABLE 4
#define OBJ_STATUS_PASSABLE     5

#define OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS true
#define OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS false

#define OBJ_ADD_TOP true

struct ObjTreeNode
{
 iAVLKey key;
 U6LList *obj_list;
};

struct Obj
{
 uint16 objblk_n;
 uint16 obj_n;
 uint8 frame_n;

 uint8 status;
 uint16 x;
 uint16 y;
 uint8 z;

 uint8 qty;
 uint8 quality;

 U6LList *container;
 Obj() {obj_n = 0; status = 0; frame_n = 0; qty = 0; quality = 0; container = NULL; }; 
};

Obj *new_obj(uint16 obj_n, uint8 frame_n, uint16 x, uint16 y, uint16 z);

class ObjManager
{
 Configuration *config;
 TileManager *tile_manager;
 iAVLTree *surface;
 iAVLTree *dungeon[5];

 uint16 obj_to_tile[1024]; //maps object number (index) to tile number.
 uint8 obj_weight[1024];
 U6LList *actor_inventories[256];

 UseCode *usecode;

 public:

 ObjManager(Configuration *cfg);
 ~ObjManager();

 bool loadObjs(TileManager *tm);

 void set_usecode(UseCode *uc) { usecode = uc; }
 UseCode *get_usecode()        { return(usecode); }

 //U6LList *get_obj_superchunk(uint16 x, uint16 y, uint8 level);
 bool is_boundary(uint16 x, uint16 y, uint8 level);
 //bool is_door(Obj * obj);

 uint8 is_passable(uint16 x, uint16 y, uint8 level);
 bool is_forced_passable(uint16 x, uint16 y, uint8 level);
 U6LList *get_obj_list(uint16 x, uint16 y, uint8 level);

 Tile *get_obj_tile(uint16 x, uint16 y, uint8 level, bool top_obj = true);
 Obj *get_obj(uint16 x, uint16 y, uint8 level, bool top_obj = true);
 Obj *get_obj_of_type_from_location(uint16 obj_n, uint16 x, uint16 y, uint8 z);
 Obj *get_objBasedAt(uint16 x, uint16 y, uint8 level, bool top_obj);

 uint16 get_obj_tile_num(uint16 obj_num);

 U6LList *get_actor_inventory(uint16 actor_num);
 bool actor_has_inventory(uint16 actor_num);
 
 Obj *find_next_obj(Obj *prev_obj);
 Obj *find_obj(uint16 obj_n, uint8 quality, uint8 level, Obj *prev_obj=NULL);

 bool move(Obj *obj, uint16 x, uint16 y, uint8 level);
 bool add_obj(Obj *obj, bool addOnTop=false);
 bool remove_obj(Obj *obj);
 bool remove_obj_type_from_location(uint16 obj_n, uint16 x, uint16 y, uint8 z);
 
 Obj *copy_obj(Obj *obj);
 const char *look_obj(Obj *obj, bool show_prefix = false);

 const char *get_obj_name(Obj *obj);
 float get_obj_weight(Obj *obj, bool include_container_items=OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS);
 uint8 get_obj_weight(uint16 obj_n) { return(obj_weight[obj_n]); }

 void animate_forwards(Obj *obj, uint32 loop_count = 1);
 void animate_backwards(Obj *obj, uint32 loop_count = 1);

 protected:

 bool loadBaseTile();
 bool loadWeightTable();

 bool loadObjSuperChunk(char *filename, uint8 level);
 bool add_obj(iAVLTree *obj_tree, Obj *obj, bool addOnTop=false);
 bool addObjToContainer(U6LList *list, Obj *obj);
 Obj *loadObj(NuvieIOFileRead *file, uint16 objblk_n);
 char *get_objblk_path(char *path);
 iAVLTree *get_obj_tree(uint8 level);

 iAVLKey get_obj_tree_key(Obj *obj);
 iAVLKey get_obj_tree_key(uint16 x, uint16 y, uint8 level);
 //inline U6LList *ObjManager::get_schunk_list(uint16 x, uint16 y, uint8 level);

 public:
 void print_object_list();
 void print_egg_list();
 void print_obj(Obj *obj, bool in_container, uint8 indent=0);
};

#endif /* __ObjManager_h__ */
