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

// obj status bit flags
#define OBJ_STATUS_OK_TO_TAKE    0x1
//#define OBJ_STATUS_SEEN_EGG      0x2  // something to do with eggs <- not sure about this one.
#define OBJ_STATUS_INVISIBLE     0x2  // I think this is correct
#define OBJ_STATUS_CHARMED	     0x4 // objlist.txt says 'charmed'

// position: A 2 bit field, so can't use plain | to check / |= to set these. 
// FIXME: check to make sure we don't do this anywhere anymore
#define OBJ_STATUS_ON_MAP        0x0
#define OBJ_STATUS_IN_CONTAINER  0x8
#define OBJ_STATUS_IN_INVENTORY 0x10
#define OBJ_STATUS_READIED      0x18
#define OBJ_STATUS_MASK_GET     0x18
#define OBJ_STATUS_MASK_SET     0xE7

#define OBJ_STATUS_TEMPORARY    0x20
#define OBJ_STATUS_EGG_ACTIVE   0x40  // something to do with eggs
#define OBJ_STATUS_BROKEN       0x40  
#define OBJ_STATUS_MUTANT       0x40  
#define OBJ_STATUS_CURSED       0x40  
#define OBJ_STATUS_LIT          0x80


//first 3 bits of nuvie_status code object location
//in the nuvie engine.

//Nuvie engine obj locations
#define OBJ_LOC_NONE    0
#define OBJ_LOC_INV     1
#define OBJ_LOC_MAP     2
#define OBJ_LOC_READIED 3
#define OBJ_LOC_CONT    4

#define NUVIE_OBJ_STATUS_LOC_MASK_GET 0x7
#define NUVIE_OBJ_STATUS_LOC_MASK_SET 0xf8

#define NUVIE_OBJ_STATUS_SCRIPTING 0x8

//is_passable return codes
#define OBJ_NO_OBJ       0
#define OBJ_NOT_PASSABLE 1
#define OBJ_PASSABLE     2

#define OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS true
#define OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS false

#define OBJ_WEIGHT_DO_SCALE true
#define OBJ_WEIGHT_DONT_SCALE false

#define OBJ_ADD_TOP true
#define OBJ_MATCH_QUALITY true

#define OBJ_TEMP_INIT 255 // this is used to stop temporary objects from being cleaned upon startup.


//We use this in Obj::is_in_inventory()
#define OBJ_DONT_CHECK_PARENT false

struct ObjTreeNode
{
 iAVLKey key;
 U6LList *obj_list;
};

struct Obj
{
 //uint16 objblk_n;
 uint8 nuvie_status;
 uint16 obj_n;
 uint8 frame_n;

 uint8 status;
 uint16 x;
 uint16 y;
 uint8 z;

 uint16 qty;
 uint8 quality;
 void * parent; //either an Obj pointer or an Actor pointer depending on engine_loc.
 U6LList *container;
 Obj() {obj_n = 0; status = 0; nuvie_status = 0; frame_n = 0; qty = 0; quality = 0; parent = NULL; container = NULL; };
 Obj(Obj *sobj)
 {
   memcpy(this, sobj, sizeof(Obj));

   parent = NULL; container = NULL;
 };

 bool is_script_obj()    { return(nuvie_status & NUVIE_OBJ_STATUS_SCRIPTING); }
 
 bool is_ok_to_take()   { return(status & OBJ_STATUS_OK_TO_TAKE); }
 bool is_invisible()	  { return(status & OBJ_STATUS_INVISIBLE); } 
 bool is_charmed()	    { return(status & OBJ_STATUS_CHARMED); }
 bool is_temporary()    { return(status & OBJ_STATUS_TEMPORARY); }
 bool is_egg_active()   { return(status & OBJ_STATUS_EGG_ACTIVE); }
 bool is_broken()       { return(status & OBJ_STATUS_BROKEN); }
 bool is_mutant()       { return(status & OBJ_STATUS_MUTANT); }
 bool is_cursed()       { return(status & OBJ_STATUS_CURSED); }
 bool is_lit()          { return(status & OBJ_STATUS_LIT); }

 bool is_on_map()       { return((nuvie_status & OBJ_LOC_MAP) == OBJ_LOC_MAP); }
 /* old, until replaced everywhere properly */
// bool is_in_container_old() { fprintf(stderr,"Depricated is_in_container used\n"); return(status & OBJ_STATUS_IN_CONTAINER); }
// bool is_in_inventory_old() { fprintf(stderr,"Depricated is_in_container used\n"); return(status & OBJ_STATUS_IN_INVENTORY); }
 /* new, to replace the above two (redoing the logic) */
 bool is_in_container() { return((nuvie_status & NUVIE_OBJ_STATUS_LOC_MASK_GET) == OBJ_LOC_CONT); }
 
 /* links, disable to find instances while renaming, eventually swing over and change to _new when renaming back.. */
// bool is_in_container() { return is_in_container_old();}
// bool is_in_inventory() { return is_in_inventory_old();}
 bool is_readied()      { return((nuvie_status & NUVIE_OBJ_STATUS_LOC_MASK_GET) == OBJ_LOC_READIED); }

 
 void set_on_map(U6LList *map_list) { parent = map_list;
                                  nuvie_status &= NUVIE_OBJ_STATUS_LOC_MASK_SET; 
                                  nuvie_status |= OBJ_LOC_MAP; }
 
 Obj *get_container_obj() { return is_in_container() ? (Obj *)parent : NULL; }
 
 void set_in_container(Obj *container_obj)
 {
   parent = (void *)container_obj;
   nuvie_status &= NUVIE_OBJ_STATUS_LOC_MASK_SET; 
   nuvie_status |= OBJ_LOC_CONT;
 }

 void set_in_inventory() { nuvie_status &= NUVIE_OBJ_STATUS_LOC_MASK_SET; 
                       nuvie_status |= OBJ_LOC_INV; }
 void readied()      { nuvie_status &= NUVIE_OBJ_STATUS_LOC_MASK_SET; 
                       nuvie_status |= OBJ_LOC_READIED; }
 
 void set_noloc()   { parent = NULL; nuvie_status &= NUVIE_OBJ_STATUS_LOC_MASK_SET; } //clear location bits 0 = no loc
 
 void set_in_script() { nuvie_status |= NUVIE_OBJ_STATUS_SCRIPTING; }
 
 /* Returns true if an object is in an actor inventory, including containers and readied items. */

 bool is_in_inventory(bool check_parent=true)
 { 
   switch(get_engine_loc())
   {
     case OBJ_LOC_INV :
     case OBJ_LOC_READIED : return true;
     case OBJ_LOC_CONT : if(check_parent)
                           return ((Obj *)parent)->is_in_inventory(check_parent);
                         break;
     default : break;
   }
   
   return false;
 }
 
 uint8 get_engine_loc() { return (nuvie_status & NUVIE_OBJ_STATUS_LOC_MASK_GET); }
 
 Actor *get_actor_holding_obj()
 {
   switch(get_engine_loc())
   {
     case OBJ_LOC_INV :
     case OBJ_LOC_READIED : return (Actor *)this->parent;
       
     case OBJ_LOC_CONT : return ((Obj *)parent)->get_actor_holding_obj();
       
     default : break;
   }
   
   return NULL;
 }

 //Add child object into container
 bool add(Obj *obj)
 {
   if(container == NULL)
     container = new U6LList();
   
   container->add(obj);
   
   obj->set_in_container(this);
   
   return true;
 }

 //Remove child object from container.
 bool remove(Obj *obj)
 {
   if(container == NULL)
     return false;
   
   if(container->remove(obj) == false)
     return false;
   
   obj->x = 0;
   obj->y = 0;
   obj->z = 0;
   
   obj->set_noloc();
   
   return true;
 }
 
 Obj *find_in_container(uint16 obj_n, uint8 quality, bool match_quality=OBJ_MATCH_QUALITY, Obj *prev_obj=NULL)
 {
   U6Link *link;
   Obj *obj;

   if(container == NULL)
     return NULL;
   
   for(link = container->start();link != NULL;link=link->next)
   {
     obj = (Obj *)link->data;
     if(obj && obj->obj_n == obj_n && (match_quality == false || obj->quality == quality))
     {
       if(obj == prev_obj)
         prev_obj = NULL;
       else
       {
         if(prev_obj == NULL)
           return obj;
       }
     }
     
    if(obj->container)
    {
      obj = obj->find_in_container(obj_n, quality, match_quality, prev_obj);
      if(obj)
        return obj;
    }
   }

   return NULL;
 }
 
 uint32 get_total_qty(uint16 match_obj_n)
 {
   U6Link *link;
   Obj *obj;
   uint16 total_qty = 0;

   if(obj_n == match_obj_n)
     total_qty += qty;

   if(container != NULL)
   {
     for(link = container->start();link != NULL;link=link->next)
     {
       obj = (Obj *)link->data;
       if(obj)
       {
         if(obj->container)
           total_qty += obj->get_total_qty(match_obj_n);
         else if(obj->obj_n == match_obj_n)
           total_qty += obj->qty;
       }
     }
   }
   
   return total_qty;
 }
 
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
 bool is_boundary(uint16 x, uint16 y, uint8 level);
 //bool is_door(Obj * obj);
 bool is_damaging(uint16 x, uint16 y, uint8 level);
 uint8 is_passable(uint16 x, uint16 y, uint8 level);
 bool is_forced_passable(uint16 x, uint16 y, uint8 level);
 bool is_stackable(Obj *obj);
 bool has_reduced_weight(Obj *obj);
 bool has_toptile(Obj *obj);

 U6LList *get_obj_list(uint16 x, uint16 y, uint8 level);

 Tile *get_obj_tile(uint16 obj_n, uint8 frame_n);
 Tile *get_obj_tile(uint16 x, uint16 y, uint8 level, bool top_obj = true);
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
 bool obj_add_obj(Obj *c_obj, Obj *obj, bool stack_objects=true, uint32 pos=0); 

 const char *get_obj_name(Obj *obj);
 const char *get_obj_name(uint16 obj_n);
 
 float get_obj_weight(Obj *obj, bool include_container_items=OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS, bool scale=true);
 uint8 get_obj_weight(uint16 obj_n) { return(obj_weight[obj_n]); }

 void animate_forwards(Obj *obj, uint32 loop_count = 1);
 void animate_backwards(Obj *obj, uint32 loop_count = 1);

 void update(uint16 x, uint16 y, uint8 z);

 bool unlink_from_engine(Obj *obj);
 
 bool moveto_map(Obj *obj);
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
