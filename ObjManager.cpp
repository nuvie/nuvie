/*
 *  ObjManager.cpp
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

#include "ObjManager.h"

ObjManager::ObjManager(Configuration *cfg)
{
 config = cfg;
 
 memset(actor_inventories,0,sizeof(actor_inventories));
 
}

ObjManager::~ObjManager()
{
 //FIX ME. need to free objects.
}
 
bool ObjManager::loadObjs(TileManager *tm)
{
 std::string path;
 char *filename;
 char x,y;
 uint16 len;
 uint8 i;
 
 tile_manager = tm;
 
 config->value("config/ultima6/gamedir",path);
 
 filename = get_objblk_path((char *)path.c_str());
 
 len = strlen(filename);
 
 i = 0;
 
 for(y = 'a';y < 'i'; y++)
  {
   for(x = 'a';x < 'i'; x++)
    {
     filename[len-1] = y;
     filename[len-2] = x;
     //printf("Loading %s\n",filename);
     surface[i] = loadObjSuperChunk(filename);
     i++;
    }
  }
 
 filename[len-1] = 'i';
 
 for(i=0,x = 'a';x < 'f';x++,i++) //Load dungeons
  {
   filename[len-2] = x;
   //printf("Loading %s\n",filename);
   dungeon[i] = loadObjSuperChunk(filename);
  }
   
 loadBaseTile();
 
 delete filename;

 return true;
}
 
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

bool ObjManager::is_boundary(uint16 x, uint16 y, uint8 level)
{
 U6Link *link, *link1;
 ObjList *obj_list;
 Obj *obj;
 Tile *tile, *tile1;
 uint16 tile_num;
 uint16 sx,sy; // note these values are not required if level > 0
 bool check_tile;
 
 if(level == 0)
   {
    sx = x / 128;
    sy = y / 128;
    link = surface[sy * 8 + sx]->end();
   }
 else
   link = dungeon[level-1]->end();
 
 for(;link != NULL;link=link->prev)
   {
    obj_list = (ObjList *)link->data;
    
    if(obj_list->x == x || obj_list->x == x+1)
      {
       if(obj_list->y == y || obj_list->y == y+1)
         {
          link1 = obj_list->objs->end();
                    
          for(check_tile = false;link1 != NULL;link1 = link1->prev)
            {
             obj = (Obj *)link1->data;
             tile_num = get_obj_tile_num(obj->obj_n)+obj->frame_n;
             tile = tile_manager->get_original_tile(tile_num);

             if(obj->x == x && obj->y == y)
               { check_tile = true; }
             if(tile->dbl_width && obj->x == x+1 && obj->y == y)
                { tile_num--; check_tile = true; }
             if(tile->dbl_height && obj->x == x && obj->y == y+1)
                { tile_num--; check_tile = true; }
             if(obj->x == x+1 && obj->y == y+1 && tile->dbl_width && tile->dbl_height)
                { tile_num -= 2; check_tile = true; }
             if(check_tile)
               {
                if(is_door(obj))
                  {
                   tile1 = tile_manager->get_tile(tile_num);
                   if(tile1->boundary == true)
                      return true;
                   else
                      return false;
                  }
                check_tile = false;
               }
            }
         }
      }
   }

 return false;
}

bool ObjManager::is_door(Obj * obj)
{
 //for U6
 if((obj->obj_n >= 297 && obj->obj_n <= 300) || obj->obj_n == 334)
   return true;
 
 return false;
}

uint8 ObjManager::is_passable(uint16 x, uint16 y, uint8 level)
{
 U6Link *link, *link1;
 ObjList *obj_list;
 Obj *obj;
 Tile *tile, *tile1;
 uint16 tile_num;
 uint16 sx,sy; // note these values are not required if level > 0
 uint8 obj_status = OBJ_STATUS_NO_OBJ;
 bool check_tile;
 
 link1 = NULL;
 
 if(level == 0)
   {
    sx = x / 128;
    sy = y / 128;
    link = surface[sy * 8 + sx]->end();
   }
 else
   link = dungeon[level-1]->end();
 
 for(;link != NULL;link=link->prev)
   {
    obj_list = (ObjList *)link->data;
    
    if(obj_list->x == x || obj_list->x == x+1)
      {
       if(obj_list->y == y || obj_list->y == y+1)
         {
          link1 = obj_list->objs->end();

          obj_status = OBJ_STATUS_PASSABLE;
                    
          for(check_tile = false;link1 != NULL;link1 = link1->prev)
            {
             obj = (Obj *)link1->data;
             tile_num = get_obj_tile_num(obj->obj_n)+obj->frame_n;
             tile = tile_manager->get_original_tile(tile_num);
    
             if(obj->x == x && obj->y == y)
               { check_tile = true; }
             if(tile->dbl_width && obj->x == x+1 && obj->y == y)
                { tile_num--; check_tile = true; }
             if(tile->dbl_height && obj->x == x && obj->y == y+1)
                { tile_num--; check_tile = true; }
             if(obj->x == x+1 && obj->y == y+1 && tile->dbl_width && tile->dbl_height)
                { tile_num -= 2; check_tile = true; }
             if(check_tile)
               {
                tile1 = tile_manager->get_tile(tile_num);
                if(tile1->passable == false)
                  return OBJ_STATUS_NOT_PASSABLE;
                check_tile = false;
               }
            }
         }
      }
   }

 return obj_status;
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
 
 obj = get_objBasedAt(x+1,y,level,top_obj);
 if(obj != NULL)
  {
   tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
   if(tile->dbl_width)
     return obj;
  }

 obj = get_objBasedAt(x,y+1,level,top_obj);
 if(obj != NULL)
  {
   tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
   if(tile->dbl_height)
     return obj;
  }

 obj = get_objBasedAt(x+1,y+1,level,top_obj);
 if(obj != NULL)
  {
   tile = tile_manager->get_tile(get_obj_tile_num(obj->obj_n)+obj->frame_n);
   if(tile->dbl_width && tile->dbl_height)
     return obj;
  }

 return NULL;
}

// x, y in world coords
Obj *ObjManager::get_objBasedAt(uint16 x, uint16 y, uint8 level, bool top_obj)
{
 U6LList *list;
 U6Link *link, *link1;
 ObjList *obj_list;
 Obj *obj;
 
 link1 = NULL;
 
 list = get_schunk_list(x, y, level);
 link = list->end();
 
 for(;link != NULL;link=link->prev)
   {
    obj_list = (ObjList *)link->data;
    
    if(obj_list->x == x && obj_list->y == y)
      {
       if(top_obj)
          link1 = obj_list->objs->end();
       else
          link1 = obj_list->objs->start();
       
       if(link1 != NULL)
          {
           obj = (Obj *)link1->data;
           return obj;
          }
      }    
   }

 return NULL;
}

bool ObjManager::use_obj(Obj *obj)
{

 if(obj == NULL)
   return false;

 if(obj->obj_n >= 297 && obj->obj_n <= 300) // door objects
   {
    if(obj->frame_n <= 3)
      obj->frame_n += 4;
    else
      obj->frame_n -= 4;
   }
 
  switch(obj->obj_n)
   {
    case OBJ_U6_SECRET_DOOR :
    case OBJ_U6_CHEST :
    case OBJ_U6_CANDLE : 
                         if(obj->frame_n > 0)
                           obj->frame_n--;
                         else
                           obj->frame_n = 1;
                         break;
   }
 
  if(obj->obj_n == OBJ_U6_V_PASSTHROUGH)
 {
   if(obj->frame_n < 2)
    {
     move(obj,obj->x,obj->y-1,obj->z);
     obj->frame_n = 2;
    }
   else
    {
     move(obj,obj->x,obj->y+1,obj->z);
     obj->frame_n = 0;
    }
 }

  if(obj->obj_n == OBJ_U6_H_PASSTHROUGH)
 {
   if(obj->frame_n < 2)
    {
     move(obj,obj->x-1,obj->y,obj->z);
     obj->frame_n = 2;
    }
   else
    {
     move(obj,obj->x+1,obj->y,obj->z);
     obj->frame_n = 0;
    }
 }
   
 printf("Use Obj #%d Frame #%d\n",obj->obj_n, obj->frame_n);
 
 return true;
}

bool ObjManager::move(Obj *obj, uint16 x, uint16 y, uint8 level)
{
 U6LList *list;
 U6Link *link;
 ObjList *obj_list;
 
 list = get_schunk_list(obj->x, obj->y, obj->z);
 
 if(list == NULL)
  return false;
 
 for(link=list->start();link != NULL;link=link->next)
  {
   obj_list = (ObjList *)link->data;
   
   if(obj_list->x == obj->x && obj_list->y == obj->y)
    {
     obj_list->objs->remove(obj);
     break;
    }
  }

 list = get_schunk_list(x, y, level);
 
  for(link=list->start();link != NULL;link=link->next)
  {
   obj_list = (ObjList *)link->data;
   if(obj_list->x == x && obj_list->y == y)
    {
     break;
    }
  }

 if(link == NULL)
   {
    obj_list = new ObjList;
    list->add(obj_list);
   }

 obj_list->objs->add(obj);
 
 obj->x = x;
 obj->y = y;
 obj->z = level;
 
 return true; 
}

const char *ObjManager::look_obj(Obj *obj)
{
 const char *desc;
 if(obj == NULL)
  return NULL;
  
 desc = tile_manager->lookAtTile(get_obj_tile_num(obj->obj_n)+obj->frame_n,obj->qty,false);
 
 return desc;
}

uint16 ObjManager::get_obj_tile_num(uint16 obj_num) //assume obj_num is < 1024 :)
{   
 return obj_to_tile[obj_num];
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

bool ObjManager::loadBaseTile()
{
 std::string filename;
 U6File basetile;
 uint16 i;
 
 config->pathFromValue("config/ultima6/gamedir","basetile",filename);
 
 if(basetile.open(filename,"rb") == false)
   return false;
 
 for(i=0;i<1024;i++)
  obj_to_tile[i] = basetile.read2();
 
 return true;
}
 
U6LList *ObjManager::loadObjSuperChunk(char *filename)
{
 U6File file;
 U6LList *list;
 uint16 num_objs;
 Obj *obj;
 uint16 i;
 U6LList *inventory_list;
 
 if(file.open(filename,"rb") == false)
   return NULL;
 
 list = new U6LList();
 
 num_objs = file.read2();
 
 for(i=0;i<num_objs;i++)
  {
   obj = loadObj(&file,i);
  // addObj(list,obj);
     
   if(obj->status == 0x8) // FIX here might be & 0x8
     {
      addObjToContainer(list,obj);
     }
   else
    {
     if(obj->status & 0x10) //object in actor's inventory
       {
        //printf("%d: %d, %d, %d\n",obj->x, obj->status, obj->obj_n, obj->y);
        inventory_list = get_actor_inventory(obj->x);
        inventory_list->addAtPos(0,obj);
       }
     else
        addObj(list,obj);
    }

  }
   
 return list;
}

void ObjManager::addObj(U6LList *list, Obj *obj)
{
 U6Link *link;
 ObjList *obj_list;
  
 for(link = list->end();link != NULL;link = link->prev)
   {
    obj_list = (ObjList *)link->data;
    if(obj_list->x == obj->x && obj_list->y == obj->y)
      break;
   }
 
 if(link == NULL)
   {
    obj_list = new ObjList;
    obj_list->objs = new U6LList();
    
    obj_list->x = obj->x;
    obj_list->y = obj->y;
    
    list->addAtPos(0,obj_list);
   }
  
 obj_list->objs->addAtPos(0,obj);
}

bool ObjManager::addObjToContainer(U6LList *list, Obj *obj)
{
 U6Link *link, *link1;
 Obj *c_obj; //container object
 ObjList *obj_list;
  
 for(link = list->end();link != NULL;link = link->prev)
   {
    obj_list = (ObjList *)link->data;
    link1 = obj_list->objs->end();
    
    for(;link1!=NULL;link1=link1->prev)
      {
       c_obj = (Obj *)link1->data;
       if(c_obj->objblk_n == obj->x)
         {
          if(c_obj->container == NULL)
            c_obj->container = new U6LList();
          c_obj->container->addAtPos(0,obj);
          return true;
         }
      }
   }
   
 return false;
}

Obj *ObjManager::loadObj(U6File *file, uint16 objblk_n)
{
 uint8 b1,b2;
 Obj *obj;
 
 obj = new Obj;
 obj->objblk_n = objblk_n;
 
 obj->status = file->read1();
   
 obj->x = file->read1(); // h
 b1 = file->read1();
 obj->x += (b1 & 0x3) << 8; 
   
 obj->y = (b1 & 0xfc) >> 2;
 b2 = file->read1();
 obj->y += (b2 & 0xf) << 6;
   
 obj->z = (b2 & 0xf0) >> 4;
   
 b1 = file->read1();
 b2 = file->read1();
 obj->obj_n = b1;
 obj->obj_n += (b2 & 0x3) << 8;
   
 obj->frame_n = (b2 & 0xfc) >> 2;
   
 obj->qty = file->read1();
 obj->quality = file->read1();
   
 return obj;
}

char *ObjManager::get_objblk_path(char *path)
{
 char *filename;
 uint16 len;
 
 if(path == NULL)
   return NULL;
 
 len = strlen(path);

 if(len == 0)
   return NULL;
   
 filename = new char [len+19]; // + room for /savegame/objblkxx\0
 
 strcpy(filename,path);
 if(filename[len-1] != U6PATH_DELIMITER)
   {
    filename[len] = U6PATH_DELIMITER;
    filename[len+1] = '\0'; 
   }
   
 strcat(filename,"savegame/objblkxx");
 
 return filename;
}

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
