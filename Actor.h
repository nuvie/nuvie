#ifndef __Actor_h__
#define __Actor_h__

/*
 *  Actor.h
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 20 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "U6def.h"
#include "Map.h"

class Actor
{
 friend class ActorManager;
 
 Map *map;
 
 uint16 x;
 uint16 y;
 uint16 z;
 
 uint16 a_num;
 uint16 frame_n;
 
 bool alive;
 bool met_player;
 
 bool in_party;
 
 uint8 max_str;
 uint8 max_hp;
 
 char *name;
 
 public:
 
 Actor(Map *m);
 ~Actor();
 
 bool is_alive();
 
};

#endif /* __Actor_h__ */
