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

#include "U6def.h"
#include "U6LList.h"
#include "Map.h"
#include "ObjManager.h"
#include "GameClock.h"

// directions
#define ACTOR_DIR_U 0
#define ACTOR_DIR_R 1
#define ACTOR_DIR_D 2
#define ACTOR_DIR_L 3

#define ACTOR_FORCE_MOVE true

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
 uint8 direction;
 uint8 walk_frame;
 
 bool standing;
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
 
 Schedule **sched;
 
 //current schedule pos;
 uint16 sched_pos;

 public:
 
 Actor(Map *m, ObjManager *om, GameClock *c);
 virtual ~Actor();
 
// bool is_visible() { return(MapCoord(x,y,z).is_visible()); }
 bool is_alive();
 bool is_nearby(Actor *other);
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

 void set_strength(uint8 val) { strength = val; }
 void set_dexterity(uint8 val) { dex = val; }
 void set_intelligence(uint8 val) { intelligence = val; }
 void set_hp(uint8 val) { hp = val; }
 void set_level(uint8 val) { level = val; }
 void set_exp(uint16 val) { exp = val; }
 void set_magic(uint8 val) { magic = val; }

 uint8 get_worktype();
 virtual void set_worktype(uint8 new_worktype);
  
 void set_direction(uint8 d);
 uint8 get_direction() { return(direction); }
 void face_location(uint16 lx, uint16 ly);
 void face_actor(Actor *a);

 void set_flags(uint8 newflags) { flags = newflags; }
 void set_flag(uint8 bitflag);
 void clear_flag(uint8 bitflag);

 bool moveRelative(sint16 rel_x, sint16 rel_y);
 bool move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move=false);
 bool check_move(sint16 new_x, sint16 new_y, sint8 new_z);
 
 virtual void update();
 void set_in_party(bool state);
 void swalk(MapCoord &d, uint8 speed = 1);
 void swalk(MapCoord &d, MapCoord &d2, uint8 speed = 1);
 void lwalk(MapCoord &d, uint8 speed = 1);
// void stop_walking() { delete pathfinder; pathfinder = NULL; }
 void stop_walking();
 virtual void preform_worktype() { return; }
 
 U6LList *get_inventory_list();
 bool inventory_has_object(uint16 obj_n, uint8 qual = 0);
 uint32 inventory_count_object(uint16 obj_n, uint8 qual = 0, Obj *container = 0);
 Obj *inventory_get_object(uint16 obj_n, uint8 qual = 0, Obj *container = 0);
 bool inventory_add_object(uint16 obj_n, uint32 qty, uint8 quality);
 uint32 inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container = 0);
 float inventory_get_max_weight() { return((strength * 2)); }
 float get_inventory_weight();
 float get_inventory_equip_weight();

 protected:
 
 void loadSchedule(unsigned char *schedule_data, uint16 num);
 void updateSchedule();
 uint16 getSchedulePos(uint8 hour);
 
};

#endif /* __Actor_h__ */
