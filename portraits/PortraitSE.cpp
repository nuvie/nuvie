/*
 *  PortraitSE.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Dec 2 2013.
 *  Copyright (c) 2013. All rights reserved.
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
#include "Game.h"

#include "ActorManager.h"
#include "Actor.h"
#include "PortraitSE.h"
#include "U6misc.h"

bool PortraitSE::init()
{
 std::string filename;

 avatar_portrait_num = 0;

 width = 79;
 height = 85;

 config_get_path(config,"faces.lzc",filename);
 if(faces.open(filename,4) == false)
 {
   ConsoleAddError("Opening " + filename);
   return false;
 }

 return true;
}

bool PortraitSE::load(NuvieIO *objlist)
{
  // U6 only?
 objlist->seek(0x1c72);

 avatar_portrait_num = objlist->read1(); //read in the avatar portrait number from objlist.
 if(avatar_portrait_num > 0)
   avatar_portrait_num--;

 return true;
}

uint8 PortraitSE::get_portrait_num(Actor *actor)
{
	uint8 num;

	if(actor == NULL)
		return NO_PORTRAIT_FOUND;

	num = actor->get_actor_num();

	return num;
}

unsigned char *PortraitSE::get_portrait_data(Actor *actor)
{
  uint8 num = get_portrait_num(actor);
  if(num == NO_PORTRAIT_FOUND)
    return NULL;

  return get_wou_portrait_data(&faces, num);
}
