#ifndef __ActorManager_h__
#define __ActorManager_h__

/*
 *  ActorManager.h
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 20 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
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
 
 void updateActors();
 
 void drawActors(Screen *screen, uint16 x, uint16 y, uint16 width, uint16 height, uint8 level);
 
};

#endif /* __ActorManager_h__ */
