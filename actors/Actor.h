#ifndef __Actor_h__
#define __Actor_h__

/*
 *  Actor.h
 *  Nuvie
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

#include <list>

using std::list;

#include "U6def.h"
#include "U6LList.h"
#include "Map.h"
#include "ObjManager.h"
#include "GameClock.h"

#define ACTOR_HEAD   0
#define ACTOR_NECK   1
#define ACTOR_BODY   2
#define ACTOR_ARM    3
#define ACTOR_ARM_2  4 
#define ACTOR_HAND   5
#define ACTOR_HAND_2 6
#define ACTOR_FOOT   7
#define ACTOR_NOT_READIABLE 8

// directions
#define ACTOR_DIR_U 0
#define ACTOR_DIR_R 1
#define ACTOR_DIR_D 2
#define ACTOR_DIR_L 3

#define ACTOR_FORCE_MOVE true
#define ACTOR_IGNORE_OTHERS true

class Map;
class MapCoord;
class UseCode;
class PathFinder;

typedef struct {
uint16 x;
uint16 y;
uint8 z;
uint8 hour;
uint8 worktype;
} Schedule;

class Actor
{
 friend class ActorManager;
 friend class MapWindow;
 friend class LPath;
 friend class Party;
 friend class Player;
 friend class PathFinder;
 friend class ZPath;

 protected:

 uint8 id_n;
 
 Map *map;
 ObjManager *obj_manager;
 GameClock *clock;
 UseCode *usecode;
 PathFinder *pathfinder;
 
 uint16 x;
 uint16 y;
 uint16 z;
 
 uint8 worktype;
 
 uint16 obj_n;
 uint16 frame_n;
 uint16 old_obj_n;
 uint16 old_frame_n;
 
 uint8 direction;
 uint8 walk_frame;
 
 bool can_twitch;
 bool alive;
 bool met_player;
 
 bool in_party;

 bool moved; // actor has moved this turn (FIXME: unused)

 uint8 strength;
 uint8 dex;
 uint8 intelligence;
 uint8 hp;
 uint8 level;
 uint16 exp;
 uint8 magic;
 uint8 combat_mode;

 uint8 flags;
 
 char *name;
 
 U6LList *obj_inventory;
 
 Obj *readied_objects[8];
 
 Schedule **sched;
 
 //current schedule pos;
 uint16 sched_pos;

 list<Obj *> surrounding_objects; //used for multi-tile actors.
 
 public:
 
 Actor(Map *m, ObjManager *om, GameClock *c);
 virtual ~Actor();
 
 virtual bool init();
 
// bool is_visible() { return(MapCoord(x,y,z).is_visible()); }
 bool is_alive();
 bool is_nearby(Actor *other);
 bool is_nearby(uint8 a);
 void get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level);
 uint16 get_tile_num();
 uint8 get_actor_num() { return(id_n); }
 uint8 get_flags() { return(flags); }
 
 uint8 get_strength() { return(strength); }
 uint8 get_dexterity() { return(dex); }
 uint8 get_intelligence() { return(intelligence); }
 uint8 get_hp() { return(hp); }
 uint8 get_maxhp() { return(((level * 30) <= 255) ? (level * 30) : 255); } // U6
 uint8 get_level() { return(level); }
 uint16 get_exp() { return(exp); }
 uint8 get_magic() { return(magic); }
 uint8 get_combat_mode() { return combat_mode; }
 
 void set_strength(uint8 val) { strength = val; }
 void set_dexterity(uint8 val) { dex = val; }
 void set_intelligence(uint8 val) { intelligence = val; }
 void set_hp(uint8 val) { hp = val; }
 void set_level(uint8 val) { level = val; }
 void set_exp(uint16 val) { exp = val; }
 void set_magic(uint8 val) { magic = val; }

 uint8 get_worktype();
 virtual void set_worktype(uint8 new_worktype);
  
 uint8 get_direction() { return(direction); }
 void set_direction(sint16 rel_x, sint16 rel_y);
 virtual void set_direction(uint8 d);
 void face_location(MapCoord &loc) { face_location(loc.x, loc.y); }
 void face_location(uint16 lx, uint16 ly);
 void face_actor(Actor *a);

 void set_flags(uint8 newflags) { flags = newflags; }
 void set_flag(uint8 bitflag);
 void clear_flag(uint8 bitflag);

 bool moveRelative(sint16 rel_x, sint16 rel_y);
 virtual bool move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move=false);
 virtual bool check_move(sint16 new_x, sint16 new_y, sint8 new_z, bool ignore_actors=false);
 
 virtual void update();
 void set_in_party(bool state);
 void swalk(MapCoord &d, uint8 speed = 1, uint8 delay = 0);
 void swalk(MapCoord &d, MapCoord &d2, uint8 speed = 1, uint8 delay = 0);
 void lwalk(MapCoord &d, uint8 speed = 1, uint8 delay = 0);
// void stop_walking() { delete pathfinder; pathfinder = NULL; }
 void stop_walking();
 virtual void preform_worktype() { return; }
 
 U6LList *get_inventory_list();
 bool inventory_has_object(uint16 obj_n, uint8 qual = 0);
 uint32 inventory_count_objects(bool inc_readied_objects);
 uint32 inventory_count_object(uint16 obj_n, uint8 qual = 0, Obj *container = 0);
 Obj *inventory_get_object(uint16 obj_n, uint8 qual = 0, Obj *container = 0);
 Obj *inventory_get_readied_object(uint8 location);
 bool inventory_add_object(uint16 obj_n, uint32 qty, uint8 quality);
 bool inventory_add_object(Obj *obj);
 uint32 inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container = 0);
 bool inventory_remove_obj(Obj *obj);
 float inventory_get_max_weight() { return((strength * 2)); }
 float get_inventory_weight();
 float get_inventory_equip_weight();
 
 virtual uint8 get_object_readiable_location(uint16 obj_n) { return ACTOR_NOT_READIABLE; }
 bool add_readied_object(Obj *obj);
 void remove_readied_object(Obj *obj);
 void remove_readied_object(uint8 location);
 
 virtual void twitch() { return; }
 
 protected:
 
 void loadSchedule(unsigned char *schedule_data, uint16 num);
 virtual bool updateSchedule();
 uint16 getSchedulePos(uint8 hour);
 
 void inventory_parse_readied_objects(); //this is used to initialise the readied_objects array on load.
};

#endif /* __Actor_h__ */
