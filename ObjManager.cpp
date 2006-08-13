/*
 *  ObjManager.cpp
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

#include <cassert>
#include "nuvieDefs.h"

#include "Configuration.h"

#include "EggManager.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "UseCode.h"
#include "U6misc.h"
#include "U6LList.h"
#include "NuvieIOFile.h"

static const int obj_egg_table[4] = {0,   // NUVIE_GAME_NONE
                                     335, // NUVIE_GAME_U6
                                     0,   // NUVIE_GAME_MD
                                     230};  // NUVIE_GAME_SE

iAVLKey get_iAVLKey(const void *item)
{
 return ((ObjTreeNode *)item)->key;
}

ObjManager::ObjManager(Configuration *cfg, TileManager *tm, EggManager *em)
{
 uint8 i;
 std::string show_eggs_key;

 config = cfg;
 tile_manager = tm;
 egg_manager = em;

 load_basetile();
 load_weight_table();

 memset(actor_inventories,0,sizeof(actor_inventories));

 for(i=0;i<64;i++)
  {
   surface[i] = iAVLAllocTree(get_iAVLKey);
  }

 for(i=0;i<5;i++)
  {
   dungeon[i] = iAVLAllocTree(get_iAVLKey);
  }

 last_obj_blk_x = 0;
 last_obj_blk_y = 0;
 last_obj_blk_z = OBJ_TEMP_INIT;

 config->value("config/GameType",game_type);

 //save the egg tile_num incase we want to switch egg display on again.
 egg_tile_num = get_obj_tile_num(obj_egg_table[game_type]);

 show_eggs_key = config_get_game_key(config);
 show_eggs_key.append("/show_eggs");

 config->value(show_eggs_key, show_eggs);

 //if(!show_eggs)
 //  show_egg_objs(false);
}

ObjManager::~ObjManager()
{
 clean();

 unsigned int i;
 for(i=0;i<64;i++)
  iAVLFreeTree(surface[i], clean_obj_tree_node);

 for(i=0;i<5;i++)
  iAVLFreeTree(dungeon[i], clean_obj_tree_node);

}

bool ObjManager::load_basetile()
{
 std::string filename;
 NuvieIOFileRead basetile;
 uint16 i;

 config_get_path(config,"basetile",filename);

 if(basetile.open(filename) == false)
   return false;

 for(i=0;i<1024;i++)
   {
    obj_to_tile[i] = basetile.read2();
    obj_stackable[i] = (uint8)tile_manager->tile_is_stackable(obj_to_tile[i]);
   }

 return true;
}

bool ObjManager::load_weight_table()
{
 std::string filename;
 NuvieIOFileRead tileflag;

 config_get_path(config,"tileflag",filename);

 if(tileflag.open(filename) == false)
   return false;

 tileflag.seek(0x1000);

 tileflag.readToBuf(obj_weight,1024);

 return true;
}


bool ObjManager::load_super_chunk(NuvieIO *chunk_buf, uint8 level, uint8 chunk_offset)
{
 NuvieIOFileRead file;
 U6LList *list;
 uint16 num_objs;
 Obj *obj;
 uint16 i;
 U6LList *inventory_list;
 iAVLTree *obj_tree;

 if(level == 0)
   obj_tree = surface[chunk_offset];
 else
   obj_tree = dungeon[level - 1];

 list = new U6LList();

 num_objs = chunk_buf->read2();
 //printf("chunk %02d number of objects: %d\n", chunk_offset, num_objs);

 for(i=0;i<num_objs;i++)
  {
   obj = loadObj(chunk_buf,i);

   list->add(obj);

   if(obj->obj_n == obj_egg_table[game_type])
     {
      egg_manager->add_egg(obj);
      // unhide eggs
      if(show_eggs && obj->is_invisible())
         obj->status ^= OBJ_STATUS_INVISIBLE; // FIXME? don't directly manipulate
     }

   if(usecode->is_container(obj) && !obj->container) //object type is container, but may be empty
     {
      obj->container = new U6LList();
     }

   if(obj->is_in_inventory_new() || obj->is_readied()) //triggered when object in actor's inventory OR equipped
     {
      inventory_list = get_actor_inventory(obj->x);
      inventory_list->add(obj);

     }
   else
     {
      if(obj->is_in_container_new()) //object in container
        {
         addObjToContainer(list,obj);
        }
      else
        {
         add_obj(obj); // show remaining objects
/*         if(show_eggs || obj->obj_n != obj_egg_table[game_type]) // show remaining objects, hiding eggs if neccecary.
            {
             add_obj(obj);
            // print_obj(obj,false);
            }*/
        }

     }
   //print_obj(obj,false);
  }


 delete list;

 return true;
}

bool ObjManager::save_super_chunk(NuvieIO *save_buf, uint8 level, uint8 chunk_offset)
{
 iAVLTree *obj_tree;
 ObjTreeNode *item;
 U6Link *link;
 iAVLCursor node;
 uint32 start_pos;
 uint32 finish_pos;
 uint16 egg_type = obj_egg_table[game_type];

 if(level == 0)
   obj_tree = surface[chunk_offset];
 else
   obj_tree = dungeon[level-1];

 item = (ObjTreeNode *)iAVLFirst(&node, obj_tree);

 start_pos = save_buf->position();

 //skip the 2 bytes for number of objects.
 save_buf->write2(0); // we'll fill this in later on.

 obj_save_count = 0;

 for(;item;)
  {
   for(link = item->obj_list->end(); link != NULL; link=link->prev)
    {
     if(((Obj *)link->data)->obj_n != egg_type) // we don't save eggs here. They are saved in save_eggs()
       save_obj(save_buf, (Obj *)link->data, NULL);
    }

   item = (ObjTreeNode *)iAVLNext(&node);
  }

 finish_pos = save_buf->position();
 save_buf->seek(start_pos);

 save_buf->write2(obj_save_count);
 save_buf->seek(finish_pos);

 return true;
}

bool ObjManager::save_eggs(NuvieIO *save_buf)
{
 uint32 start_pos;
 uint32 finish_pos;
 std::list<Egg *> *egg_list;
 std::list<Egg *>::iterator egg;

 start_pos = save_buf->position();

 //skip number of objects we will fill that in at the end.
 save_buf->write2(0);

 egg_list = egg_manager->get_egg_list();

 obj_save_count = 0;

 for(egg = egg_list->begin(); egg != egg_list->end();egg++)
   save_obj(save_buf, (*egg)->obj, NULL);

 finish_pos = save_buf->position();
 save_buf->seek(start_pos);

 save_buf->write2(obj_save_count);
 save_buf->seek(finish_pos);

 printf("Eggs: %d\n", obj_save_count);

 return true;
}

bool ObjManager::save_inventories(NuvieIO *save_buf)
{
 uint32 start_pos;
 uint32 finish_pos;
 U6Link *link;
 uint16 i;

 start_pos = save_buf->position();

 save_buf->write2(0);

 obj_save_count = 0;

 for(i=0;i<256;i++)
   {
    if(actor_inventories[i] != NULL)
      {
       for(link=actor_inventories[i]->start(); link != NULL; link=link->next)
         {
          save_obj(save_buf, (Obj *)link->data, NULL);
         }
      }
   }

 printf("Actor Inventories: %d\n", obj_save_count);

 finish_pos = save_buf->position();
 save_buf->seek(start_pos);

 save_buf->write2(obj_save_count);
 save_buf->seek(finish_pos);

 return true;
}

bool ObjManager::save_obj(NuvieIO *save_buf, Obj *obj, Obj *parent)
{
 uint8 b;
 U6Link *link;
 
// if these warnings never pop-up, we can trust parent_obj and rely on it instead of even passing parent...
 if (obj->parent_obj && !parent) 
 {
   printf("WARNING non-NULL Obj *obj->parent_obj, but no Obj *parent set in call to save_obj\n");
   printf("setting parent to parent_obj\n");
   parent=obj->parent_obj;
 }
 if (obj->parent_obj != parent) 
 {
   printf("WARNING Obj *obj->parent_obj differs from non-NULL Obj *parent set in call to save_obj\n");
   printf("Trusting parent, but keeping parent_obj\n");
 }
   
 if(parent) //obj is in a container
  {
   obj->in_container(); // in container 
   obj->x = parent->objblk_n;
   obj->y &= (0xff ^ 0x1); //clean zeroth bit in y which is used for objblk_n > 1024
   if(obj->x >= 1024)
     {
      obj->y |= 0x1;
      obj->x -= 1024;
     }
  }
 else
  {
   if(!(obj->is_readied()))
   {
     obj->status &= (0xff ^ OBJ_STATUS_IN_CONTAINER);
   }
  }

 save_buf->write1(obj->status);
 save_buf->write1(obj->x & 0xff);
 b = obj->x >> 8;
 b += obj->y << 2;
 save_buf->write1(b);

 b = obj->y >> 6;
 b += obj->z << 4;

 save_buf->write1(b);

 save_buf->write1(obj->obj_n & 0xff);

 b = obj->obj_n >> 8;
 b += obj->frame_n << 2;

 save_buf->write1(b);

 save_buf->write1((uint8)(obj->qty & 0xff)); //only save the lower byte to disk.

 if(is_stackable(obj))
   save_buf->write1(obj->qty >> 8);
 else
   save_buf->write1(obj->quality);

 obj->objblk_n = obj_save_count;
 obj_save_count += 1;
  
 if(obj->container)
  {
   for(link = obj->container->end(); link != NULL; link=link->prev)
     save_obj(save_buf, (Obj *)link->data, obj);
  }

 return true;
}

void ObjManager::clean()
{
 uint8 i;

 egg_manager->clean(show_eggs); //show_eggs determines wether we delete the actual Objs from egg manager.

 for(i=0;i<64;i++)
  iAVLCleanTree(surface[i], clean_obj_tree_node);

 for(i=0;i<5;i++)
  iAVLCleanTree(dungeon[i], clean_obj_tree_node);

 clean_actor_inventories();

 // remove the temporary object list. The objects were deleted from the surface and dungeon trees.
 temp_obj_list.clear();

 return;
}

void ObjManager::clean_actor_inventories()
{
 U6Link *link;
 uint16 i;

 for(i=0; i < 256; i++)
  {
   if(actor_inventories[i])
     {
      for(link=actor_inventories[i]->start(); link != NULL; link=link->next)
        delete (Obj *)link->data;
      actor_inventories[i]->removeAll();
     }
  }

 return;
}

void ObjManager::show_egg_objs(bool value)
{
 if(value == true)
   set_obj_tile_num(obj_egg_table[game_type], egg_tile_num); // show egg tile.
 else
   set_obj_tile_num(obj_egg_table[game_type], 0); //nothing. we don't want to show eggs.

 return;
}

/*
U6LList *ObjManager::get_obj_superchunk(uint16 x, uint16 y, uint8 level)
{
 uint16 i;

 if(level == 0)
   {
    i = y * 8 + x;
    return surface[i];
   }

 return dungeon[level-1];
}
*/

bool ObjManager::is_boundary(uint16 x, uint16 y, uint8 level)
{
 U6Link *link;
 U6LList *obj_list;
 Obj *obj;
 Tile *tile, *tile1;
 uint16 tile_num;
 bool check_tile;
 uint16 i,j;
 uint16 next_x, next_y;
 
 next_x = WRAPPED_COORD(x+1,level);
 next_y = WRAPPED_COORD(y+1,level);
 
 for(j=y;j<=y+1;j++)
   {
    for(i=x;i<=x+1;i++)
      {
       obj_list = get_obj_list(WRAPPED_COORD(i,level), WRAPPED_COORD(j,level), level);

       if(obj_list != NULL)
         {
          link = obj_list->end();

          for(check_tile = false;link != NULL;link = link->prev)
            {
             obj = (Obj *)link->data;
             tile_num = get_obj_tile_num(obj->obj_n)+obj->frame_n;
             tile = tile_manager->get_original_tile(tile_num);

             if(obj->x == x && obj->y == y)
               { check_tile = true; }
             if(tile->dbl_width && obj->x == next_x && obj->y == y)
                { tile_num--; check_tile = true; }
             if(tile->dbl_height && obj->x == x && obj->y == next_y)
                { tile_num--; check_tile = true; }
             if(obj->x == next_x && obj->y == next_y && tile->dbl_width && tile->dbl_height)
                { tile_num -= 2; check_tile = true; }
             if(check_tile)
               {
                tile1 = tile_manager->get_tile(tile_num);
                if(tile1->flags2 & TILEFLAG_BOUNDARY)
                  return true;

                check_tile = false;
               }
            }
         }
      }
   }

 return false;
}

/* no longer needed.
//FIX this needs to be moved magicnumbers :(
bool ObjManager::is_door(Obj * obj)
{
 //for U6
 if((obj->obj_n >= 297 && obj->obj_n <= 300) || obj->obj_n == 334 || obj->obj_n == 213) //OBJ_U6_MOUSEHOLE)
   return true;

 return false;
}
*/

uint8 ObjManager::is_passable(uint16 x, uint16 y, uint8 level)
{
 U6Link *link;
 U6LList *obj_list;
 Obj *obj;
 Tile *tile, *tile1;
 uint16 tile_num;
 bool check_tile;
 bool object_at_location = false;
 uint16 i,j;
 
 uint16 x2 = WRAPPED_COORD((x+1),level); // wrap on map edge
 uint16 y2 = WRAPPED_COORD((y+1),level); 

 for(i=x;;i=x2) // only checks x and x2
   {
    for(j=y;;j=y2) // only checks y and y2
      {
       obj_list = get_obj_list(i,j,level);
       if(i == x && j == y && obj_list)
         {
          if(obj_list->end() != NULL)
            object_at_location = true;
         }
       if(obj_list != NULL)
         {
          link = obj_list->end();

          for(check_tile = false;link != NULL;link = link->prev)
            {
             obj = (Obj *)link->data;
             tile_num = get_obj_tile_num(obj->obj_n)+obj->frame_n;
             tile = tile_manager->get_original_tile(tile_num);

             if(obj->x == x && obj->y == y)
               { check_tile = true; }
             if(tile->dbl_width && obj->x == x2 && obj->y == y)
                { tile_num--; check_tile = true; }
             if(tile->dbl_height && obj->x == x && obj->y == y2)
                { tile_num--; check_tile = true; }
             if(obj->x == x2 && obj->y == y2 && tile->dbl_width && tile->dbl_height)
                { tile_num -= 3; check_tile = true; }
             if(check_tile)
               {
                tile1 = tile_manager->get_original_tile(tile_num);
                if(tile1->passable == false)
                  return OBJ_NOT_PASSABLE;
                check_tile = false;
               }
            }
         }
         if(j == y) j = y2; else break;
      }
    if(i == x) i = x2; else break;
   }

 if(object_at_location)
   return OBJ_PASSABLE;

 return OBJ_NO_OBJ;
}

bool ObjManager::is_forced_passable(uint16 x, uint16 y, uint8 level)
{
 U6LList *obj_list;
 U6Link *link;
 Obj *obj;
 Tile *tile;

 obj_list = get_obj_list(x,y,level);

 if(obj_list)
  {
   for(link=obj_list->start();link!=NULL;link=link->next)
     {
      obj = (Obj *)link->data;
      tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
      if(tile->flags3 & TILEFLAG_FORCED_PASSABLE)
        return true;
     }
  }

 return false;
}

bool ObjManager::is_damaging(uint16 x, uint16 y, uint8 level)
{
 U6LList *obj_list;
 U6Link *link;
 Obj *obj;
 Tile *tile;

 obj_list = get_obj_list(x,y,level);

 if(obj_list)
  {
   for(link=obj_list->start();link!=NULL;link=link->next)
     {
      obj = (Obj *)link->data;
      tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
      if(tile->flags1 & TILEFLAG_DAMAGING)
        return true;
     }
  }

 return false;
}

bool ObjManager::is_stackable(Obj *obj)
{
// Tile *tile;

 if(obj == NULL)
   return false;
 if(obj->is_readied()) // readied objects cannot be stacked --SB-X
   return false;
/*
 tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);

 if(tile_manager->tile_is_stackable(tile->tile_num))
   return true;

 return false;
*/
 return (bool)obj_stackable[obj->obj_n];
}

bool ObjManager::has_toptile(Obj *obj)
{
 Tile *tile;
 uint8 i = 1;
 
 tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
  
 if(tile->dbl_width)
   i++;
   
 if(tile->dbl_height)
   i++;
   
 if(tile->dbl_width && tile->dbl_height)
   i++;

  for(; i > 0; i--) 
   {
    if(tile->toptile)
      return true;

    if(i != 1)
      tile = tile_manager->get_tile(tile->tile_num - 1);
   }

 return false;
}

//gets the linked list of objects at a perticular location.

U6LList *ObjManager::get_obj_list(uint16 x, uint16 y, uint8 level)
{
 iAVLTree *obj_tree;
 iAVLKey key;
 ObjTreeNode *item;

 WRAP_COORD(x,level); // wrap on map edge
 WRAP_COORD(y,level);
 
 obj_tree = get_obj_tree(x,y,level);
 key = get_obj_tree_key(x,y,level);

 item = (ObjTreeNode *)iAVLSearch(obj_tree,key);
 if(item)
  return item->obj_list;

 return NULL;
}

Tile *ObjManager::get_obj_tile(uint16 obj_n, uint8 frame_n)
{
 return  tile_manager->get_tile(get_obj_tile_num(obj_n)+frame_n);
}

Tile *ObjManager::get_obj_tile(uint16 x, uint16 y, uint8 level, bool top_obj)
{
 Obj *obj;
 Tile *tile;
 uint16 tile_num;

 obj = get_obj(x,y,level, top_obj);
 if(obj == NULL)
   return NULL;

 tile_num = get_obj_tile_num(obj->obj_n)+obj->frame_n;
 tile = tile_manager->get_tile(tile_num);

 if(tile->dbl_width && obj->x == x+1 && obj->y == y)
   tile_num--;
 if(tile->dbl_height && obj->x == x && obj->y == y+1)
   tile_num--;
 if(obj->x == x+1 && obj->y == y+1 && tile->dbl_width && tile->dbl_height)
   tile_num -= 2;

 return tile_manager->get_original_tile(tile_num);
}

Obj *ObjManager::get_obj(uint16 x, uint16 y, uint8 level, bool top_obj)
{
 Obj *obj;
 Tile *tile;

 obj = get_objBasedAt(x,y,level,top_obj);
 if(obj != NULL)
   return obj;

 obj = get_objBasedAt(x+1,y+1,level,top_obj);
 if(obj != NULL)
  {
   tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
   if(tile->dbl_width && tile->dbl_height)
     return obj;
  }

 obj = get_objBasedAt(x,y+1,level,top_obj);
 if(obj != NULL)
  {
   tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
   if(tile->dbl_height)
     return obj;
  }

 obj = get_objBasedAt(x+1,y,level,top_obj);
 if(obj != NULL)
  {
   tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
   if(tile->dbl_width)
     return obj;
  }


 return NULL;
}

Obj *ObjManager::get_obj_of_type_from_location(uint16 obj_n, uint16 x, uint16 y, uint8 z)
{
 return get_obj_of_type_from_location(obj_n, -1, -1, x, y, z);
}

Obj *ObjManager::get_obj_of_type_from_location(uint16 obj_n, sint16 quality, sint32 qty, uint16 x, uint16 y, uint8 z)
{
 U6LList *obj_list;
 U6Link *link;
 Obj *obj;

 obj_list = get_obj_list(x,y,z);

 if(obj_list == NULL)
  return NULL;
 // start from the top of the stack
 for(link=obj_list->end();link != NULL; link=link->prev)
   {
    obj = (Obj *)link->data;
    if(obj->obj_n == obj_n)
     {
      if(quality != -1 && obj->quality != (uint8)quality)
        continue;
        
      if(qty != -1 && obj->qty != (uint16)qty)
        continue;
    
      return obj;
     }
   }

 return NULL;
}

// x, y in world coords
Obj *ObjManager::get_objBasedAt(uint16 x, uint16 y, uint8 level, bool top_obj)
{
 U6Link *link;
 U6LList *obj_list;
 Obj *obj;

 obj_list = get_obj_list(x,y,level);

 if(obj_list != NULL)
   {
    if(top_obj)
       link = obj_list->end();
    else
       link = obj_list->start();

    if(link != NULL)
       {
        obj = (Obj *)link->data;
        return obj;
       }
   }

 return NULL;
}

/*
bool ObjManager::add_obj(Obj *obj, bool addOnTop)
{
 return add_obj(get_obj_tree(obj->x,obj->y,obj->z), obj, addOnTop);
}
*/
bool ObjManager::remove_obj(Obj *obj)
{
 U6LList *obj_list;

 obj_list = get_obj_list(obj->x,obj->y,obj->z);

 if(obj_list != NULL)
   {
    obj_list->remove(obj);
   }

 if(obj->status & OBJ_STATUS_TEMPORARY)
   temp_obj_list_remove(obj);

 if(obj->obj_n == obj_egg_table[game_type])
   {
    egg_manager->remove_egg(obj);
   }

 return true;
}

// remove all objects of type obj_n from location (x,y,z)

bool ObjManager::remove_obj_type_from_location(uint16 obj_n, uint16 x, uint16 y, uint8 z)
{
 U6LList *obj_list;
 U6Link *link;
 Obj *obj;
 bool objects_deleted = false;

 obj_list = get_obj_list(x,y,z);

 if(obj_list != NULL)
   {
    for(link=obj_list->start();link != NULL; )
     {
      obj = (Obj *)link->data;
      link=link->next;

      if(obj->obj_n == obj_n)
        {
         obj_list->remove(obj);
         delete_obj(obj);
         objects_deleted = true;
        }
     }
   }

 return objects_deleted;
}

Obj *ObjManager::copy_obj(Obj *obj)
{
 Obj *new_obj;

 if(obj == NULL)
   return NULL;

 new_obj = new Obj(*obj);
/* changed to direct copy in case we add new members to Obj --SB-X
 new_obj->obj_n = obj->obj_n;
 new_obj->frame_n = obj->frame_n;

 new_obj->status = obj->status;
 new_obj->qty = obj->qty;
 new_obj->quality = obj->quality;

 new_obj->x = obj->x;
 new_obj->y = obj->y;
 new_obj->z = obj->z;*/

 // should we copy container???
 new_obj->container = 0;

 return new_obj;
}

bool ObjManager::move(Obj *obj, uint16 x, uint16 y, uint8 level)
{
 U6LList *list;

 list = get_obj_list(obj->x, obj->y, obj->z);

 if(list == NULL)
  return false;

 list->remove(obj);

 obj->x = x;
 obj->y = y;
 obj->z = level;

 add_obj(obj,true); // add the object on top of the stack

 return true;
}

/* Returns an objects look-string, its general description.
 */
const char *ObjManager::look_obj(Obj *obj, bool show_prefix)
{
 const char *desc;
 if(obj == NULL)
  return NULL;

 desc = tile_manager->lookAtTile(get_obj_tile_num(obj->obj_n)+obj->frame_n,obj->qty,show_prefix);

 return desc;
}

const char *ObjManager::get_obj_name(Obj *obj)
{
 return tile_manager->lookAtTile(get_obj_tile_num(obj->obj_n),0,false);
}

const char *ObjManager::get_obj_name(uint16 obj_n)
{
 return tile_manager->lookAtTile(get_obj_tile_num(obj_n),0,false);
}

float ObjManager::get_obj_weight(Obj *obj, bool include_container_items, bool scale)
{
 float weight;
 U6Link *link;

 weight = obj_weight[obj->obj_n];

 if(is_stackable(obj))
   { weight *= obj->qty; weight /= 10; }

 //weight /= 10;
 if(obj->container != NULL && include_container_items == true)
   {
    for(link=obj->container->start();link != NULL;link=link->next)
      weight += get_obj_weight(reinterpret_cast<Obj*>(link->data), false); //don't scale container objects yet.
   }

 if(scale)
   weight /= 10;

 return weight;
}

uint16 ObjManager::get_obj_tile_num(uint16 obj_num) //assume obj_num is < 1024 :)
{
 return obj_to_tile[obj_num];
}

void ObjManager::set_obj_tile_num(uint16 obj_num, uint16 tile_num)
{
 obj_to_tile[obj_num] = tile_num;
 return;
}


/* Animate all visible tiles of an object `loop_count' times.
 */
void ObjManager::animate_forwards(Obj *obj, uint32 loop_count)
{
    tile_manager->set_anim_loop(get_obj_tile_num(obj->obj_n), loop_count, 0);
}


/* Animate in reverse all visible tiles of an object `loop_count' times.
 */
void ObjManager::animate_backwards(Obj *obj, uint32 loop_count)
{
    tile_manager->set_anim_loop(get_obj_tile_num(obj->obj_n), loop_count, 1);
}


U6LList *ObjManager::get_actor_inventory(uint16 actor_num)
{
 if(actor_num >= 256)
   return NULL;

 if(actor_inventories[actor_num] == NULL)
   {
    actor_inventories[actor_num] = new U6LList();
   }

 return actor_inventories[actor_num];
}

bool ObjManager::actor_has_inventory(uint16 actor_num)
{
 if(actor_inventories[actor_num] != NULL)
  {
   if(actor_inventories[actor_num]->start() != NULL)
     return true;
  }

 return false;
}

Obj *ObjManager::find_next_obj(Obj *prev_obj)
{
 if(prev_obj == NULL)
   return NULL;

 return find_obj(prev_obj->obj_n, prev_obj->quality, prev_obj->z, prev_obj);
}

Obj *ObjManager::find_obj(uint16 obj_n, uint8 quality, uint8 level, Obj *prev_obj)
{
 uint8 i;
 Obj *new_obj;
 bool passed_prev_obj = false;

 if(prev_obj == NULL)
   passed_prev_obj = true;

 if(level == 0)
   {
    for(i=0;i<64;i++)
      {
       new_obj = find_obj_in_tree(obj_n, quality, prev_obj, surface[i], &passed_prev_obj);
       if(new_obj != NULL)
         return new_obj;
      }
   }
 else
   {
    new_obj = find_obj_in_tree(obj_n, quality, prev_obj, dungeon[level-1], &passed_prev_obj);
    if(new_obj != NULL)
      return new_obj;
   }

 return NULL;
}

inline Obj *ObjManager::find_obj_in_tree(uint16 obj_n, uint8 quality, Obj *prev_obj, iAVLTree *obj_tree, bool *passed_prev_obj)
{
 iAVLCursor cursor;
 ObjTreeNode *node;
 U6Link *link;
 Obj *new_obj;

 node = (ObjTreeNode *)iAVLFirst(&cursor,obj_tree);

 for(;node != NULL;)
  {
   link = ((U6LList *)(node->obj_list))->start();
   for(;link != NULL;link=link->next)
    {
     if( ((Obj *)(link->data))->obj_n == obj_n && ((Obj *)(link->data))->quality == quality )
       {
        new_obj = (Obj *)link->data;
        if(new_obj == prev_obj)
          *passed_prev_obj = true;
        else
          {
           if(*passed_prev_obj)
             return new_obj;
          }
       }
    }

   node = (ObjTreeNode *)iAVLNext(&cursor);
  }

 return NULL;
}


bool ObjManager::add_obj(Obj *obj, bool addOnTop)
{
 iAVLTree *obj_tree;
 ObjTreeNode *node;
 U6LList *obj_list;
 iAVLKey key;

 obj_tree = get_obj_tree(obj->x, obj->y, obj->z);
 key = get_obj_tree_key(obj);

 node = (ObjTreeNode *)iAVLSearch(obj_tree,key);

 if(node == NULL)
   {
    obj_list = new U6LList();

    node = new ObjTreeNode;
    node->key = key;
    node->obj_list = obj_list;

    iAVLInsert(obj_tree, node);
   }
 else
   {
    obj_list = node->obj_list;
   }

 if(addOnTop)
   obj_list->add(obj);
 else
   obj_list->addAtPos(0,obj);

 if(obj->status & OBJ_STATUS_TEMPORARY)
   temp_obj_list_add(obj);

 return true;
}
bool ObjManager::addObjToContainer(U6LList *llist, Obj *obj)
{
 U6Link *link;
 Obj *c_obj = NULL; //container object
 uint16 index;

 index = obj->x;

 if(obj->y & 0x1)
   index += 1024;

 link = llist->gotoPos(index);
 if(link != NULL)
	c_obj = (Obj *)link->data;

 if(c_obj) // we've found our container.
   {
    if(c_obj->container == NULL)
	  c_obj->container = new U6LList();
    c_obj->container->addAtPos(0,obj);
    obj->parent_obj=c_obj;

	//printf("Add to container %s", tile_manager->lookAtTile(get_obj_tile_num(obj->obj_n)+obj->frame_n,0,false));
	//printf(" -> %s (%x,%x,%x)\n", tile_manager->lookAtTile(get_obj_tile_num(c_obj->obj_n)+c_obj->frame_n,0,false),c_obj->x,c_obj->y,c_obj->z);

    return true;
   }

 return false;
}

Obj *ObjManager::loadObj(NuvieIO *buf, uint16 objblk_n)
{
 uint8 b1,b2;
 Obj *obj;

 obj = new Obj;
 obj->objblk_n = objblk_n;

 obj->status = buf->read1();

 obj->x = buf->read1(); // h
 b1 = buf->read1();
 obj->x += (b1 & 0x3) << 8;

 obj->y = (b1 & 0xfc) >> 2;
 b2 = buf->read1();
 obj->y += (b2 & 0xf) << 6;

 obj->z = (b2 & 0xf0) >> 4;

 b1 = buf->read1();
 b2 = buf->read1();
 obj->obj_n = b1;
 obj->obj_n += (b2 & 0x3) << 8;

 obj->frame_n = (b2 & 0xfc) >> 2;

 obj->qty = buf->read1();
 obj->quality = buf->read1();
 if(is_stackable(obj))
   obj->qty = (uint16)(obj->quality << 8) + obj->qty;

 if(obj->qty == 0)
   obj->qty = 1;

 return obj;
}


iAVLTree *ObjManager::get_obj_tree(uint16 x, uint16 y, uint8 level)
{
 if(level == 0)
  {
   x >>= 7; // x = floor(x / 128)   128 = superchunk width
   y >>= 7; // y = floor(y / 128)   128 = superchunk height

   return surface[x + y * 8];
  }

 if(level > 5)
   return NULL;

 return dungeon[level-1];
}

inline iAVLKey ObjManager::get_obj_tree_key(Obj *obj)
{
 return get_obj_tree_key(obj->x, obj->y, obj->z);
}

iAVLKey ObjManager::get_obj_tree_key(uint16 x, uint16 y, uint8 level)
{
 if(level == 0)
   return y * 1024 + x;
 else
   return y * 256 + x;
}

void ObjManager::update(uint16 x, uint16 y, uint8 z)
{
 uint16 cur_blk_x, cur_blk_y;

 cur_blk_x = x >> 5; // x / 32;
 cur_blk_y = y >> 5; // y / 32;

 // We're changing levels so clean out all temp objects on the current level.
 if(last_obj_blk_z != z)
   {
    if(last_obj_blk_z != OBJ_TEMP_INIT)
      temp_obj_list_clean_level(last_obj_blk_z);

    egg_manager->spawn_eggs(x, y, z);

    last_obj_blk_x = cur_blk_x;
    last_obj_blk_y = cur_blk_y;
    last_obj_blk_z = z;

    return;
   }

 //FIX for level change. we want to remove all temps on level change.
 if(cur_blk_x != last_obj_blk_x || cur_blk_y != last_obj_blk_y)
   {
    last_obj_blk_x = cur_blk_x;
    last_obj_blk_y = cur_blk_y;

    temp_obj_list_clean_area(x, y);
    egg_manager->spawn_eggs(x, y, z);

   }

 return;
}

bool ObjManager::temp_obj_list_add(Obj *obj)
{
 if(obj == NULL)
  return false;

 temp_obj_list.push_back(obj);

 return true;
}

bool ObjManager::temp_obj_list_remove(Obj *obj)
{
 temp_obj_list.remove(obj);
 return true;
}

// clean objects from a whole level.
void ObjManager::temp_obj_list_clean_level(uint8 z)
{
 std::list<Obj *>::iterator obj;
 std::list<Obj *>::iterator tmp_obj;

 for(obj = temp_obj_list.begin(); obj != temp_obj_list.end();)
   {
    if((*obj)->z == z)
      {
       printf("Removing obj %s.\n", tile_manager->lookAtTile(get_obj_tile_num((*obj)->obj_n)+(*obj)->frame_n,0,false));
       tmp_obj = obj;
       tmp_obj++;
       remove_obj(*obj); // this calls temp_obj_list_remove()
       delete *obj;
       obj = tmp_obj;
      }
    else
      obj++;
   }

 return;
}


// Clean objects more than 32 tiles from position
void ObjManager::temp_obj_list_clean_area(uint16 x, uint16 y)
{
 std::list<Obj *>::iterator obj;
 std::list<Obj *>::iterator tmp_obj;
 sint16 dist_x, dist_y;

 for(obj = temp_obj_list.begin(); obj != temp_obj_list.end();)
   {
    dist_x = abs((sint16)(*obj)->x - x);
    dist_y = abs((sint16)(*obj)->y - y);

    if(dist_x > 32 || dist_y > 32)
      {
       printf("Removing obj %s.\n", tile_manager->lookAtTile(get_obj_tile_num((*obj)->obj_n)+(*obj)->frame_n,0,false));
       tmp_obj = obj;
       tmp_obj++;
       remove_obj(*obj);
       delete *obj;
       obj = tmp_obj;
      }
    else
      obj++;
   }

 return;
}

/*
inline U6LList *ObjManager::get_schunk_list(uint16 x, uint16 y, uint8 level)
{
 uint16 sx, sy;

 if(level == 0)
   {
    sx = x / 128;
    sy = y / 128;
    return surface[sy * 8 + sx];
   }

 return dungeon[level-1];
}
*/
//prints a human readable list of object number / names.

void ObjManager::print_object_list()
{
 uint16 i;

 for(i=0;i<1024;i++)
  {
   printf("%04d: %s\n",i,tile_manager->lookAtTile(get_obj_tile_num(i),0,false));
  }

 return;
}

void ObjManager::print_egg_list()
{
 uint8 i;

 for(i=0;i < 64;i++)
   print_egg_tree(surface[i]);

 for(i=0;i < 5;i++)
   print_egg_tree(dungeon[i]);

 return;
}

inline void ObjManager::print_egg_tree(iAVLTree *obj_tree)
{
 ObjTreeNode *tree_node;
 iAVLCursor cursor;
 U6LList *obj_list;
 U6Link *link;
 Obj *obj;

  tree_node = (ObjTreeNode *)iAVLFirst(&cursor,obj_tree);

  for(;tree_node != NULL;tree_node = (ObjTreeNode *)iAVLNext(&cursor) )
   {
    obj_list = (U6LList *)tree_node->obj_list;
    for(link = obj_list->start(); link != NULL; link = link->next)
     {
      obj = (Obj *)link->data;
      if(obj->obj_n == 335)
       {
        print_obj(obj,false);
       }
     }
   }

 return;
}

void ObjManager::print_obj(Obj *obj, bool in_container, uint8 indent)
{
 U6Link *link;
 Obj *container_obj;

 printf("\n");
 print_indent(indent);
 printf("%s ",tile_manager->lookAtTile(get_obj_tile_num(obj->obj_n)+obj->frame_n,0,false));
 
 if(in_container == false)
   printf("at %x, %x, %x (%d,%d,%d)",obj->x, obj->y, obj->z, obj->x, obj->y, obj->z);
 printf("\n");
 
 print_indent(indent);
 printf("object (Obj *) %p\n", obj);
 print_indent(indent);
 printf("parent (Obj *) %p\n", obj->parent_obj);

 print_indent(indent);
 printf("objblk_n: %d\n", obj->objblk_n);

 print_indent(indent);
 printf("obj_n: %d\n",obj->obj_n);

 print_indent(indent);
 printf("frame_n: %d\n",obj->frame_n);

 print_indent(indent);
 printf("Tile: %d\n", get_obj_tile_num(obj->obj_n));

 print_indent(indent);
 printf("Status: ");
 print_b(obj->status);
 if(obj->status != 0)
  {
   printf(" ( ");
   if(obj->is_readied())
     printf("POS:Ready ");
   else if(obj->is_in_container_new())
     printf("POS:Cont ");
   else if(obj->is_in_inventory_new())
     printf("POS:Inv ");
   if(obj->is_ok_to_take())
     printf("OK ");
   if(obj->is_temporary())
     printf("TEMP ");
   if(obj->is_invisible())
     printf("INVIS ");
   if(obj->is_egg_active())
    {
     if(obj->obj_n < 256)
      printf("MUTANT ");
     else
      printf("BROKEN ");
    }

   printf(")");
  }

 printf("\n");

 if(in_container)
  {
   print_indent(indent);
   printf("parent_id = %d, y = %d, z = %d\n", obj->x, obj->y, obj->z);
  }

 print_indent(indent);
 printf("Quantity: %d\n",obj->qty);
 print_indent(indent);
 printf("Quality: %d\n",obj->quality);

 if(obj->container)
   {
    print_indent(indent);
    printf("Container\n");
    print_indent(indent);
    printf("---------");

    for(link = obj->container->start(); link != NULL; link = link->next)
     {
      container_obj = (Obj *)link->data;
      print_obj(container_obj, true, indent + 2);
     }

    print_indent(indent);
    printf("---------\n");
   }

 if(in_container == false)
   printf("\n");

 return;
}

Obj *new_obj(uint16 obj_n, uint8 frame_n, uint16 x, uint16 y, uint16 z)
{
 Obj *obj;

 obj = new Obj();

 obj->obj_n = obj_n;
 obj->frame_n = frame_n;

 obj->x = x;
 obj->y = y;
 obj->z = z;

 return obj;
}

void delete_obj(Obj *obj)
{
 U6Link *link;
 if(obj->container)
  {
   for(link=obj->container->start();link != NULL; link=link->next)
     delete_obj((Obj *)link->data);
  }

 delete obj->container;
 delete obj;

 return;
}

// add object to list, stacking with exisiting objects if possible
// This is used for adding objects to inventory OR a container.
// *It will stack onto the new object and delete the existing object!*
bool ObjManager::list_add_obj(U6LList *llist, Obj *obj, bool stack_objects, uint32 pos)
{
 Obj *stack_with;
 uint16 new_qty;
 U6Link *link;
 
 if(!llist || !obj)
   return false;
 
 clamp_min(pos,llist->count());
   
 if(stack_objects && is_stackable(obj))
  {
   for(link=llist->start();link != NULL; )
    {
     stack_with = (Obj *)link->data;
     link = link->next;

     if(stack_with->obj_n == obj->obj_n && stack_with->frame_n == obj->frame_n
        && stack_with->quality == obj->quality && is_stackable(stack_with))
       {
        assert(obj->qty != 0 && stack_with->qty != 0); // stackable objects must have a quantity
        new_qty = obj->qty + stack_with->qty;
        obj->qty = new_qty;
        llist->addAtPos(llist->findPos(stack_with), obj);

        llist->remove(stack_with);
        delete_obj(stack_with);

        return true;
       }
    }
  }

 llist->addAtPos(pos,obj);

 return true;
}
// wrapper to the above to work with an existing c_obj instead
bool ObjManager::obj_add_obj(Obj *c_obj, Obj *obj, bool stack_objects, uint32 pos)
{
  if(!c_obj) // || !obj is not needed as already checked in list_add_obj(U6LList *...) 
  {
   return false;
  }
  if (c_obj==obj) {
    printf("WARNING: Tried to put container into itself.\n");
    return false;
  }
  if (!c_obj->container)
  {
    c_obj->container = new U6LList();
  }
  
  remove_obj(obj); 
  // and add it.
  if (list_add_obj(c_obj->container, obj, stack_objects, pos))
  {
    obj->in_container();
    obj->parent_obj=c_obj;
    // FIXME? may have to set x etc?
    // but objblk_n is not guaranteed to be unique, eventually will be fixed at 
    // save-time
    return true;
  } 
  else 
  {
    return false;
  }
}


/* Call load usecode for all objects (after loading them). This should be in
 * loadObj() but that was crashing when usecode tried to use timers.
 */
void ObjManager::startObjs()
{
 uint8 i;

 //iterate through surface chunks.
 for(i = 0;i < 64; i++)
   start_obj_usecode(surface[i]);

 //iterate through dungeon chunks.
 for(i=0;i < 5;i++)
   start_obj_usecode(dungeon[i]);

}

inline void ObjManager::start_obj_usecode(iAVLTree *obj_tree)
{
 ObjTreeNode *tree_node;
 iAVLCursor cursor;
 U6LList *obj_list;
 U6Link *link;
 Obj *obj;

 tree_node = (ObjTreeNode *)iAVLFirst(&cursor,obj_tree);
 for(;tree_node != NULL;tree_node = (ObjTreeNode *)iAVLNext(&cursor) )
   {
    obj_list = (U6LList *)tree_node->obj_list;
    for(link = obj_list->start(); link != NULL; link = link->next)
      {
        obj = (Obj *)link->data;
        if(usecode->has_loadcode(obj))
           usecode->load_obj(obj);
      }
   }
}


/* Subtract an object stack with quantity set to `count' from original object
 * stack `obj'.
 * Returns a new object if a stack could be subtracted from the original,
 * leaving the original intact.
 * Returns the original if its quantity was smaller than the requested count or
 * it is not stackable.
 */
Obj *ObjManager::get_obj_from_stack(Obj *obj, uint32 count)
{
    if(count == 0 || obj->qty <= count || !is_stackable(obj))
        return(obj);
    // requested is over 0, original quantity is greater than requested, object
    //  is stackable
    Obj *new_obj = copy_obj(obj);
    new_obj->qty = count;
    obj->qty -= count; // remove requested from original
    return(new_obj);
}



/* Returns the object which contains another object.
 */
Obj *ObjManager::get_obj_container(Obj *obj)
{
    return(obj->parent_obj); // will be NULL if not a contained object.
}

void clean_obj_tree_node(void *node)
{
 U6Link *link;
 ObjTreeNode *obj_node = (ObjTreeNode *)node;

 for(link=obj_node->obj_list->start();link != NULL; link=link->next)
   delete_obj((Obj *)link->data);

 delete obj_node->obj_list;
 delete obj_node;

 return;
}


/* Returns true if an object is in an actor inventory, including containers and readied items. */
/* now that we have a working? parent_obj, moved to Obj itself */
bool ObjManager::is_held(Obj *obj) 
{
  return obj->is_held();
}
