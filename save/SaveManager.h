#ifndef __SaveManager_h__
#define __SaveManager_h__

/*
 *  SaveManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Apr 28 2004.
 *  Copyright (c) 2004. All rights reserved.
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

class Configuration;
class ActorManager;
class Actor;

class GUI_Scroller;
class GUI_Dialog;

class SaveManager
{
 Configuration *config;
 ActorManager *actor_manager;
 ObjManager *obj_manager;
 
 // gui widgets;
 
 GUI_Scroller *scroller;
 GUI_Dialog *dialog;
 
 public:
  
 SaveManager(Configuration *cfg, ActorManager *am, ObjManager *om);
 ~SaveManager();
 
 protected:
 
 void create_dialog();
 
};

#endif /* __SaveManager_h__ */
