#ifndef __Player_h__
#define __Player_h__

/*
 *  player.h
 *  Nuive
 *
 *  Created by Eric Fry on Sun Mar 23 2003.
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
#include "Configuration.h"

#include "Actor.h"
#include "ActorManager.h"
#include "MapWindow.h"

class Player
{
 Configuration *config;
 
 bool party_mode;
 Actor *actor;
 ActorManager *actor_manager;
 
 MapWindow *map_window;
 
 public:
 
 Player(Configuration *cfg);
 
 bool init(Actor *a, ActorManager *am, MapWindow *mw);
 Actor *get_actor();

 void moveLeft();
 void moveRight();
 void moveUp();
 void moveDown();
 
};

#endif /* __Player_h__ */
