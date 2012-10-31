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

#include <list>

class Configuration;
class GUI;
class TileManager;
class ObjManager;
class Portrait;
class Party;
class Player;
class Text;
class Actor;
class Obj;

class View;
class PartyView;
class PortraitView;
class InventoryView;
class ActorView;
class SpellView;
class ContainerViewGump;
class DraggableView;

using std::list;

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
 Portrait *portrait;

 ActorView *actor_view;
 InventoryView *inventory_view;
 PortraitView *portrait_view;
 PartyView *party_view;
 SpellView *spell_view;

 View *current_view;

 std::list<DraggableView *> container_gumps;
 std::list<DraggableView *> gumps;


 public:

 ViewManager(Configuration *cfg);
 virtual ~ViewManager();

 bool init(GUI *g, Text *t, Party *p, Player *player, TileManager *tm, ObjManager *om, Portrait *por);
 void reload();

 void update();

 void set_portrait_mode(Actor *actor, const char *name);
 void set_inventory_mode();
 void set_party_mode();
 void set_actor_mode();
 void set_spell_mode(Actor *caster, Obj *spell_container, bool eventMode=false);

 void close_spell_mode();

 View *get_current_view() { return(current_view); }
 ActorView *get_actor_view() { return(actor_view); }
 InventoryView *get_inventory_view() { return(inventory_view); }
 PortraitView *get_portrait_view() { return(portrait_view); }
 PartyView *get_party_view() { return(party_view); }
 SpellView *get_spell_view() { return(spell_view); }

 void close_current_view();

 void open_doll_view(Actor *actor);

 void open_container_view(Obj *obj) { open_container_view(NULL, obj); }
 void open_container_view(Actor *actor) { open_container_view(actor, NULL); }

 void close_container_view(Actor *actor);

 void open_mapeditor_view();
 void open_portrait_gump(Actor *a);
 void close_gump(DraggableView *gump);
 void close_all_gumps();
 bool set_current_view(View *view);

 protected:

 ContainerViewGump *get_container_view(Actor *actor, Obj *obj);
 void open_container_view(Actor *actor, Obj *obj);

 void add_view(View *view);
};

#endif /* __ViewManager_h__ */

