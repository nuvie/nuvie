#ifndef __ViewManager_h__
#define __ViewManager_h__

/*
 *  ViewManager.h
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

class Configuration;
class GUI;
class TileManager;
class ObjManager;
class Portrait;
class Party;
class Player;
class Text;
class Actor;

class View;
class PartyView;
class PortraitView;
class InventoryView;
class ActorView;

class ViewManager
{
 protected:
 
 Configuration *config;
 int game_type;
 GUI *gui;
 Text *text;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 Party *party;

 ActorView *actor_view;
 InventoryView *inventory_view;
 PortraitView *portrait_view;
 PartyView *party_view;
 
 View *current_view;
 
 public:
 
 ViewManager(Configuration *cfg);
 virtual ~ViewManager();
 
 bool init(GUI *g, Text *t, Party *p, Player *player, TileManager *tm, ObjManager *om, Portrait *portrait);

 void update();
 
 void set_portrait_mode(Actor *actor, char *name);
 void set_inventory_mode();
 void set_party_mode();
 void set_actor_mode();
  
 protected:
 
 bool set_current_view(View *view);
};

#endif /* __ViewManager_h__ */

