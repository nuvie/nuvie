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
#include "NuvieBmpFile.h"

class Configuration;
class GUI;
class TileManager;
class ObjManager;
class Portrait;
class Party;
class Player;
class Font;
class Actor;
class Obj;

class View;
class PartyView;
class PortraitView;
class InventoryView;
class ActorView;
class SpellView;
class ContainerViewGump;
class DollViewGump;
class DraggableView;
class SunMoonRibbon;

using std::list;

typedef enum { CURSOR_HEAD, CURSOR_NECK, CURSOR_CHEST, CURSOR_RIGHT_HAND, CURSOR_LEFT_HAND, CURSOR_RIGHT_RING, CURSOR_LEFT_RING, CURSOR_FEET,
               CURSOR_LEFT, CURSOR_RIGHT, CURSOR_COMBAT, CURSOR_CHECK, CURSOR_HEART, CURSOR_PARTY, CURSOR_INVENTORY } gumpCursorPos;

class ViewManager
{
 protected:

 Configuration *config;
 int game_type;
 GUI *gui;
 Font *font;
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
 std::list<DraggableView *> doll_gumps;
 std::list<DraggableView *> gumps;

 uint8 doll_next_party_member;

 SunMoonRibbon *ribbon;
 NuvieBmpFile bmp;
 std::string DollDataDirString;

 public:

 ViewManager(Configuration *cfg);
 virtual ~ViewManager();

 bool init(GUI *g, Font *f, Party *p, Player *player, TileManager *tm, ObjManager *om, Portrait *por);
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
 void open_sign_gump(const char *sign_text, uint16 length);
 void open_scroll_gump(const char *text, uint16 length);
 void close_gump(DraggableView *gump);
 void close_all_gumps();
 bool gumps_are_active() { return !gumps.empty(); }

 bool set_current_view(View *view);

// custom doll functions shared between DollWidget and DollViewGump
 std::string getDollDataDirString();
 SDL_Surface *loadAvatarDollImage(SDL_Surface *avatar_doll);
 SDL_Surface *loadCustomActorDollImage(SDL_Surface *actor_doll, uint8 actor_num);
 SDL_Surface *loadGenericDollImage();

 protected:

 Actor *doll_view_get_next_party_member();
 DollViewGump *get_doll_view(Actor *actor);
 ContainerViewGump *get_container_view(Actor *actor, Obj *obj);
 void open_container_view(Actor *actor, Obj *obj);
 void add_gump(DraggableView *gump);
 void add_view(View *view);
 void move_gump_to_top(DraggableView *gump);
};

#endif /* __ViewManager_h__ */

