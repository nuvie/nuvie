#ifndef __EggManager_h__
#define __EggManager_h__

/*
 *  EggManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Dec 07 2003.
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
#include <list>

#include "ObjManager.h"

struct Egg
{
 bool seen_egg;
 Obj *obj;
 Egg() { seen_egg = false; obj = NULL; }
};

class Configuration;
class ActorManager;
class Actor;
class Map;

class EggManager
{
 Configuration *config;
 Map *map;
 ActorManager *actor_manager;
 
 std::list<Egg *> egg_list;
 
 public:
  
 EggManager(Configuration *cfg, Map *m);
 ~EggManager();
 
 void set_actor_manager(ActorManager *am) { actor_manager = am; }
 
 void add_egg(Obj *egg);
 void remove_egg(Obj *egg);
 void spawn_eggs(uint16 x, uint16 y, uint8 z);
 
 protected:
 
};

#endif /* __EggManager_h__ */
