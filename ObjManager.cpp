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
}

ObjManager::~ObjManager()
{
}
 
bool ObjManager::loadObjs()
{
 std::string path;
 char *filename;
 char x,y;
 uint16 len;
 uint8 i;
 
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
     surface[i] = loadObjSuperChunk(filename);
     i++;
    }
  }
 
 loadBaseTile();
 
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

// x, y in world coords
Obj *ObjManager::get_base_obj(uint16 x, uint16 y, uint8 level)
{
 U6Link *link;
 Obj *obj;
 uint16 sx,sy; // note these values are not required if level > 0
 
 if(level == 0)
   {
    sx = x / 128;
    sy = y / 128;
    link = surface[sy * 8 + sx]->start();
   }
 else
   link = dungeon[level-1]->start();
 
 for(;link != NULL;link=link->next)
   {
    obj = (Obj *)link->data;
    if(obj->x == x && obj->y == y)
      return obj;
   }

 return NULL;
}
 
uint16 ObjManager::get_obj_tile_num(uint16 obj_num) //assume obj_num is < 1024 :)
{   
 return obj_to_tile[obj_num];
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
 
 if(file.open(filename,"rb") == false)
   return NULL;
 
 list = new U6LList();
 
 num_objs = file.read2();
 
 for(i=0;i<num_objs;i++)
  {
   obj = loadObj(&file);
   if(obj->status & OBJ_STATUS_IN_CONTAINER)
     {
      addObjToContainer(list,obj);
     }
   else      
      list->addAtPos(0,obj);
  }
   
 return list;
}

bool ObjManager::addObjToContainer(U6LList *list, Obj *obj)
{
 U6Link *link;
 Obj *c_obj; //container object
 
 return true;
 
 link = list->gotoPos(obj->x); //get the parent container object
 c_obj = (Obj *)link->data; //FIX from here.. <+===-
 
 if(c_obj->container == NULL)
   c_obj->container = new U6LList();
 
 c_obj->container->addAtPos(0,obj);
   
 return true;
}

Obj *ObjManager::loadObj(U6File *file)
{
 uint8 b1,b2;
 Obj *obj;
 
 obj = new Obj;
 obj->container = NULL;
 
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
