#ifndef __ActorManager_h__
#define __ActorManager_h__

/*
 *  ActorManager.h
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 20 2003.
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

#include <string>

#include "U6def.h"
#include "Configuration.h"
#include "Screen.h"
#include "Map.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Actor.h"

class ActorManager
{
 Configuration *config;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 Map *map;
 Actor *actors[256];
 
 public:
  
 ActorManager(Configuration *cfg, Map *m, TileManager *tm, ObjManager *om);
 ~ActorManager();
 
 bool loadActors();
 
 Actor *get_actor(uint8 actor_num);
 Actor *get_partyLeader();

 void updateActors();
 
 void drawActors(Screen *screen, uint16 x, uint16 y, uint16 width, uint16 height, uint8 level);
 
 protected:
 
 bool ActorManager::loadActorSchedules();
 
};

#endif /* __ActorManager_h__ */
