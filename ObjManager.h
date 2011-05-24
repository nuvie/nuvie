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

#include <list>
#include <cstring>
#include "iAVLTree.h"
#include "TileManager.h"
#include "U6LList.h"

//class U6LList;
class Configuration;
class EggManager;
class UseCode;
class NuvieIO;
class MapCoord;
class Actor;

//is_passable return codes
#define OBJ_NO_OBJ       0
#define OBJ_NOT_PASSABLE 1
#define OBJ_PASSABLE     2

#define OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS true
#define OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS false

#define OBJ_WEIGHT_DO_SCALE true
#define OBJ_WEIGHT_DONT_SCALE false

#define OBJ_ADD_TOP true


#define OBJ_TEMP_INIT 255 // this is used to stop temporary objects from being cleaned upon startup.


#include "Obj.h"

struct ObjTreeNode
{
 iAVLKey key;
 U6LList *obj_list;
};

Obj *new_obj(uint16 obj_n, uint8 frame_n, uint16 x, uint16 y, uint16 z);
void delete_obj(Obj *obj);

void clean_obj_tree_node(void *node);

class ObjManager
{
 Configuration *config;
 int game_type;
 EggManager *egg_manager;
 TileManager *tile_manager;
 //chunk object trees.
 iAVLTree *surface[64];
 iAVLTree *dungeon[5];

 uint16 obj_to_tile[1024]; //maps object number (index) to tile number.
 uint8 obj_weight[1024];
 uint8 obj_stackable[1024];
 U6LList *actor_inventories[256];

 bool show_eggs;
 uint16 egg_tile_num;

 UseCode *usecode;

 std::list<Obj *> temp_obj_list;
 uint16 last_obj_blk_x, last_obj_blk_y;
 uint8 last_obj_blk_z;

 uint16 obj_save_count;

 public:

 ObjManager(Configuration *cfg, TileManager *tm, EggManager *em);
 ~ObjManager();

 void show_egg_objs(bool value);

 bool loadObjs();
 bool load_super_chunk(NuvieIO *chunk_buf, uint8 level, uint8 chunk_offset);
 void startObjs();
 void clean();
 void clean_actor_inventories();

 bool save_super_chunk(NuvieIO *save_buf, uint8 level, uint8 chunk_offset);
 bool save_eggs(NuvieIO *save_buf);
 bool save_inventories(NuvieIO *save_buf);
 bool save_obj(NuvieIO *save_buf, Obj *obj, uint16 parent_objblk_n);

 void set_usecode(UseCode *uc) { usecode = uc; }
 UseCode *get_usecode()        { return(usecode); }
 EggManager *get_egg_manager() { return(egg_manager); }

 //U6LList *get_obj_superchunk(uint16 x, uint16 y, uint8 level);
 bool is_boundary(uint16 x, uint16 y, uint8 level, uint8 boundary_type=TILEFLAG_BOUNDARY);
 //bool is_door(Obj * obj);
 bool is_damaging(uint16 x, uint16 y, uint8 level);
 uint8 is_passable(uint16 x, uint16 y, uint8 level);
 bool is_forced_passable(uint16 x, uint16 y, uint8 level);
 bool is_stackable(Obj *obj);
 bool is_breakable(Obj *obj);
 bool can_store_obj(Obj *target, Obj *src); // Bag, open chest, spellbook.
 bool can_get_obj(Obj *obj);
 bool has_reduced_weight(Obj *obj);
 bool has_toptile(Obj *obj);

 U6LList *get_obj_list(uint16 x, uint16 y, uint8 level);

 Tile *get_obj_tile(uint16 obj_n, uint8 frame_n);
 Tile *get_obj_tile(uint16 x, uint16 y, uint8 level, bool top_obj = true);
 Tile *get_obj_dmg_tile(uint16 x, uint16 y, uint8 level);
 Obj *get_obj(uint16 x, uint16 y, uint8 level, bool top_obj = true);
 Obj *get_obj_of_type_from_location(uint16 obj_n, uint16 x, uint16 y, uint8 z);
 Obj *get_obj_of_type_from_location(uint16 obj_n, sint16 quality, sint32 qty, uint16 x, uint16 y, uint8 z);
 Obj *get_objBasedAt(uint16 x, uint16 y, uint8 level, bool top_obj);

 uint16 get_obj_tile_num(uint16 obj_num);
 void set_obj_tile_num(uint16 obj_num, uint16 tile_num);

 U6LList *get_actor_inventory(uint16 actor_num);
 bool actor_has_inventory(uint16 actor_num);

 Obj *find_next_obj(Obj *prev_obj);
 Obj *find_obj(uint16 obj_n, uint8 quality, uint8 level, Obj *prev_obj=NULL);

 bool move(Obj *obj, uint16 x, uint16 y, uint8 level);
 bool add_obj(Obj *obj, bool addOnTop=false);
 bool remove_obj_from_map(Obj *obj);
 bool remove_obj_type_from_location(uint16 obj_n, uint16 x, uint16 y, uint8 z);

 
 Obj *copy_obj(Obj *obj);
 const char *look_obj(Obj *obj, bool show_prefix = false);
 Obj *get_obj_from_stack(Obj *obj, uint32 count);

 bool list_add_obj(U6LList *list, Obj *obj, bool stack_objects=true, uint32 pos=0);

 const char *get_obj_name(Obj *obj);
 const char *get_obj_name(uint16 obj_n);
 const char *get_obj_name(uint16 obj_n, uint8 frame_n);
 
 float get_obj_weight(Obj *obj, bool include_container_items=OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS, bool scale=true);
 uint8 get_obj_weight(uint16 obj_n) { return(obj_weight[obj_n]); }

 void animate_forwards(Obj *obj, uint32 loop_count = 1);
 void animate_backwards(Obj *obj, uint32 loop_count = 1);

 void update(uint16 x, uint16 y, uint8 z);

 bool unlink_from_engine(Obj *obj);
 
 bool moveto_map(Obj *obj, MapCoord location);
 bool moveto_inventory(Obj *obj, uint16 actor_num);
 bool moveto_inventory(Obj *obj, Actor *actor);
 bool moveto_container(Obj *obj, Obj *container_obj);
   
 protected:

 void remove_obj(Obj *obj);
 
 bool load_basetile();
 bool load_weight_table();


 bool addObjToContainer(U6LList *list, Obj *obj);
 Obj *loadObj(NuvieIO *buf);
 iAVLTree *get_obj_tree(uint16 x, uint16 y, uint8 level);

 iAVLKey get_obj_tree_key(Obj *obj);
 iAVLKey get_obj_tree_key(uint16 x, uint16 y, uint8 level);
 //inline U6LList *ObjManager::get_schunk_list(uint16 x, uint16 y, uint8 level);

 bool temp_obj_list_add(Obj *obj);
 bool temp_obj_list_remove(Obj *obj);
 void temp_obj_list_clean_level(uint8 z);
 void temp_obj_list_clean_area(uint16 x, uint16 y);

 inline Obj *find_obj_in_tree(uint16 obj_n, uint8 quality, Obj *prev_obj, iAVLTree *obj_tree);
 inline void start_obj_usecode(iAVLTree *obj_tree);
 inline void print_egg_tree(iAVLTree *obj_tree);

 public:
 void print_object_list();
 void print_egg_list();
 void print_obj(Obj *obj, bool in_container, uint8 indent=0);
};



#endif /* __ObjManager_h__ */
