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
#include "Console.h"
#include "NuvieIOFile.h"
#include "U6Shape.h"
#include "Dither.h"
#include "Game.h"

#include "Actor.h"
#include "Portrait.h"
#include "U6Lzw.h"
#include "U6misc.h"

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

 avatar_portrait_num = 0;
 // U6, uses portrait.a,.b,.z SE uses faces.lzc, MD uses mdfaces.lzc
 // need to know which game type we are playing.
 config->value("config/GameType",gametype);

 if (gametype == NUVIE_GAME_U6) {
  config_get_path(config,"portrait.a",filename);
  if(portrait_a.open(filename,4) == false)
  {
   ConsoleAddError("Opening " + filename);
   return false;
  }
  config_get_path(config,"portrait.b",filename);
  if(portrait_b.open(filename,4) == false)
  {
   ConsoleAddError("Opening " + filename);
   return false;
  }
  config_get_path(config,"portrait.z",filename);
  if(portrait_z.open(filename,4) == false)
  {
   ConsoleAddError("Opening " + filename);
   return false;
  }
 } 
 else
 {
  if (gametype == NUVIE_GAME_MD) 
  {
   config_get_path(config,"mdfaces.lzc",filename);
   //config->pathFromValue("config/martian/gamedir","mdfaces.lzc",filename);
  }
  if (gametype == NUVIE_GAME_SE)
  {
   config_get_path(config,"faces.lzc",filename);
   //config->pathFromValue("config/savage/gamedir","faces.lzc",filename);
  }
  if(faces.open(filename,4) == false)
  {
   ConsoleAddError("Opening " + filename);
   return false;
  }
 }
  return true;
}

bool Portrait::load(NuvieIO *objlist)
{
  // U6 only?
 objlist->seek(0x1c72);

 avatar_portrait_num = objlist->read1(); //read in the avatar portrait number from objlist.
 if(avatar_portrait_num > 0)
   avatar_portrait_num--;

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

 if(actor == NULL)
   return NULL;

 num = actor->get_actor_num();
 if (gametype==NUVIE_GAME_U6) {
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
   else if(num >= (192-1) && num <= (200-1)) // Shrines, Temple of Singularity
    return(NULL);
   else if(num > 194) // there are 194 npc portraits
   {
    switch(actor->get_obj_n()) //check for temporary actors with portraits. eg guards and wisps
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
  Game::get_game()->get_dither()->dither_bitmap(new_portrait,PORTRAIT_WIDTH,PORTRAIT_HEIGHT,true);

  return new_portrait;
 }
 // MD/SE
 if (gametype==NUVIE_GAME_MD) {
 // FIXME select right avatar portrait, correct offset for MD (SE only has male)
   num++;
 }
  U6Shape * shp;
  unsigned char *shp_data;
  NuvieIOBuffer shp_buf;
  U6Lib_n shp_lib;

  shp = new U6Shape();
  shp_data = faces.get_item(num,NULL);
  shp_buf.open(shp_data, faces.get_item_size(num), NUVIE_BUF_NOCOPY);
  shp_lib.open(&shp_buf, 4, gametype);
  shp->load(&shp_lib, 0);
  new_portrait=shp->get_data(); // probably need to copy here
  // FIXME shp deletion here is wrong, corrupts things, but if not deleted here
  // would never be deleted.
  //delete shp;
  shp_lib.close();
  free(shp_data);

  return new_portrait;
}
