/*
 *  Portrait.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue May 20 2003.
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

#include "nuvieDefs.h"

#include "Configuration.h"

#include "NuvieIOFile.h"

#include "Actor.h"
#include "Portrait.h"
#include "U6Lzw.h"

#include "U6objects.h"

//define tmporary portraits.

#define PORTRAIT_U6_GARGOYLE 194
#define PORTRAIT_U6_GUARD 193
#define PORTRAIT_U6_WISP 192
#define PORTRAIT_U6_EXODUS 191
#define PORTRAIT_U6_NOTHING 188

Portrait::Portrait(Configuration *cfg)
{
 config = cfg;
}

bool Portrait::init()
{
 std::string filename;
 NuvieIOFileRead objlist;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
 if(objlist.open(filename) == false)
   throw "Opening savegame/objlist";

 objlist.seek(0x1c72);
 
 avatar_portrait_num = objlist.read1(); //read in the avatar portrait number from objlist.
 if(avatar_portrait_num > 0)
   avatar_portrait_num--;
 
 config->pathFromValue("config/ultima6/gamedir","portrait.a",filename);
 if(portrait_a.open(filename,4) == false)
    throw "Opening portrait.a";
 
 config->pathFromValue("config/ultima6/gamedir","portrait.b",filename);
 if(portrait_b.open(filename,4) == false)
    throw "Opening portrait.b";
 
 config->pathFromValue("config/ultima6/gamedir","portrait.z",filename);
 if(portrait_z.open(filename,4) == false)
    throw "Opening portrait.z";
 
 return true;
}

unsigned char *Portrait::get_portrait_data(Actor *actor)
{
 uint8 num;
 U6Lzw lzw;
 U6Lib_n *portrait;
 unsigned char *lzw_data;
 uint32 new_length;
 unsigned char *new_portrait;
 Obj *actor_obj;
 
 if(actor == NULL)
   return NULL;

 num = actor->get_actor_num();

 if(num == 1) // avatar portrait
    {
     portrait = &portrait_z;
     num = avatar_portrait_num;
    }
 else
   {
    num -= 1;

    if(num == (188-1))
        num = PORTRAIT_U6_EXODUS-1; // Exodus
    else if(num >= (191-1) && num <= (198-1)) // Shrines, Temple of Singularity
        return(NULL);
    else if(num > 194) // there are 194 npc portraits
      {
#warning Where is the return from Actor::make_obj freed?
	   actor_obj = actor->make_obj(); //check for temporary actors with portraits. eg guards and wisps
	   switch(actor_obj->obj_n)
	    {
		 case OBJ_U6_GUARD : num = PORTRAIT_U6_GUARD-1; break;
		 case OBJ_U6_WISP : num = PORTRAIT_U6_WISP-1; break;
		 case OBJ_U6_GARGOYLE : num = PORTRAIT_U6_GARGOYLE-1; break;
		 default : return NULL;
		}
      }

    if(num < 98)
      portrait = &portrait_a;
    else
     {
      num -= 98;
      portrait = &portrait_b;
     }
   }

 lzw_data = portrait->get_item(num);
 new_portrait = lzw.decompress_buffer(lzw_data, portrait->get_item_size(num), new_length);
 free(lzw_data);

 return new_portrait;
}
