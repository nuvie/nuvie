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
#include <string>

using std::list;
using std::string;

#include "Map.h"
#include "ObjManager.h"

#define ACTOR_NO_READIABLE_LOCATION -1
#define ACTOR_HEAD   0
#define ACTOR_NECK   1
#define ACTOR_BODY   2
#define ACTOR_ARM    3
#define ACTOR_ARM_2  4
#define ACTOR_HAND   5
#define ACTOR_HAND_2 6
#define ACTOR_FOOT   7
#define ACTOR_NOT_READIABLE 8

// move-flags
#define ACTOR_FORCE_MOVE true
#define ACTOR_IGNORE_OTHERS true

#define ACTOR_FORCE_HIT true

// push-flags
#define ACTOR_PUSH_ANYWHERE 0
#define ACTOR_PUSH_HERE     1
#define ACTOR_PUSH_FORWARD  2
#define ACTOR_PUSH_TO       3

#define ACTOR_SHOW_BLOOD true

#define ACTOR_STATUS_POISONED 0x8
#define ACTOR_STATUS_DEAD 0x10
#define ACTOR_STATUS_IN_PARTY 0xc0

#define ACTOR_NO_ERROR 0
#define ACTOR_OUT_OF_MOVES 1
#define ACTOR_BLOCKED 2
#define ACTOR_BLOCKED_BY_OBJECT 3
#define ACTOR_BLOCKED_BY_ACTOR 4

#define ACTOR_MAX_READIED_OBJECTS 8

class Map;
class MapCoord;
class UseCode;
class PathFinder;
class U6LList;
class GameClock;

typedef struct {
uint16 x;
uint16 y;
uint8 z;
uint8 hour;
uint8 day_of_week; // 0 = any day 1..7
uint8 worktype;
} Schedule;

typedef enum { ATTACK_TYPE_NONE, ATTACK_TYPE_HAND, ATTACK_TYPE_THROWN, ATTACK_TYPE_MISSLE } AttackType;

typedef struct {
uint16 obj_n;

uint8 defence;
uint8 attack;

uint16 hit_range;
AttackType attack_type; 

uint16 missle_tile_num;
uint16 thrown_obj_n;

bool breaks_on_contact;
} CombatType;

typedef uint8 ActorErrorCode;
typedef struct {
    ActorErrorCode err;
} ActorError;

typedef struct {
Obj *obj;
const CombatType *combat_type;
} ReadiedObj;

class Actor
{
 friend class ActorManager;
 friend class MapWindow;
 friend class LPath;
 friend class Party;
 friend class Player;
 friend class PathFinder;
 friend class ZPath;
 friend class U6UseCode;
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
 MapCoord work_location;

 uint16 obj_n;
 uint16 frame_n;
 uint16 base_obj_n;
 uint16 old_frame_n;

 uint8 direction;
 uint8 walk_frame;
 
 uint8 status_flags;
 
 bool can_move;
 bool alive;
 bool temp_actor;
 bool met_player;

 bool in_party;
 bool visible_flag;

 uint8 moves; // number of moves actor has this turn
 uint8 light; // level of light around actor (normally 0)

 ActorError error_struct; // error/status; result of previous action

 uint8 strength;
 uint8 dex;
 uint8 intelligence;
 uint8 hp;
 uint8 level;
 uint16 exp;
 uint8 magic;
 uint8 combat_mode;

 uint8 flags;

 uint8 body_armor_class;
 uint8 readied_armor_class;
 
 string name;

 U6LList *obj_inventory;

 ReadiedObj *readied_objects[ACTOR_MAX_READIED_OBJECTS];

 Schedule **sched;

 //current schedule pos;
 uint16 sched_pos;

 list<Obj *> surrounding_objects; //used for multi-tile actors.

 public:

 Actor(Map *m, ObjManager *om, GameClock *c);
 virtual ~Actor();

 virtual bool init();
 void init_from_obj(Obj *obj);

// bool is_visible() { return(MapCoord(x,y,z).is_visible()); }
 bool is_alive();
 bool is_nearby(Actor *other);
 bool is_nearby(uint8 actor_num);
 bool is_nearby(MapCoord &where, uint8 thresh = 5);
 bool is_at_position(Obj *obj);
 bool is_passable();
 //for lack of a better name:
 bool is_met() { return(flags & 0x01); }
 bool is_poisoned() { return(status_flags & ACTOR_STATUS_POISONED); }
 virtual void set_poisoned(bool poisoned) { return; }
 virtual bool is_immobile(); // frozen by worktype or status
 virtual bool is_sleeping() { return(false); }

 void set_name(const char *actor_name) {  name=actor_name; }
 const char *get_name();

 void get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level);
 MapCoord get_location();
 uint16 get_tile_num();
 virtual uint16 get_downward_facing_tile_num();
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
 void add_light(uint8 val);
 void subtract_light(uint8 val);
 void heal() { set_hp(get_maxhp()); }
 
 uint8 get_worktype();
 virtual void set_worktype(uint8 new_worktype);

 uint8 get_direction() { return(direction); }
 void set_direction(sint16 rel_x, sint16 rel_y);
 virtual void set_direction(uint8 d);
 void face_location(MapCoord &loc);
 void face_location(uint16 lx, uint16 ly);
 void face_actor(Actor *a);

 void set_flags(uint8 newflags) { flags = newflags; }
 void set_flag(uint8 bitflag);
 void clear_flag(uint8 bitflag);
 void show() { visible_flag = true; }
 void hide() { visible_flag = false; }
 void set_error(ActorErrorCode err);
 void clear_error();
 ActorError *get_error();

 bool is_visible() { return visible_flag; }

 bool moveRelative(sint16 rel_x, sint16 rel_y);
 virtual bool move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move=false);
 virtual bool check_move(sint16 new_x, sint16 new_y, sint8 new_z, bool ignore_actors=false);
 bool check_moveRelative(sint16 rel_x, sint16 rel_y, bool ignore_actors=false);

 virtual bool Actor::can_be_moved();
 virtual void update();
 void set_in_party(bool state);
 void swalk(MapCoord &d, uint8 speed = 1, uint8 delay = 0);
 void swalk(MapCoord &d, MapCoord &d2, uint8 speed = 1, uint8 delay = 0);
 void lwalk(MapCoord &d, uint8 speed = 1, uint8 delay = 0);
// void stop_walking() { delete pathfinder; pathfinder = NULL; }
 void stop_walking();
 virtual void preform_worktype() { return; }
 
 // combat methods
 void attack(MapCoord pos); // attack at a given map location
 void attack(sint8 readied_obj_location, Actor *actor);
 bool defend(uint8 hit, uint8 weapon_damage); // defend against a hit
 const CombatType *get_weapon(sint8 readied_obj_location);
 
 void hit(uint8 dmg, bool force_hit=false);
 void hit(uint8 dmg, Actor *attacker) { hit(dmg); }
 void hit(uint8 dmg, Obj *src_obj)    { hit(dmg); }
 void reduce_hp(uint8 amount);
 virtual void die();
 virtual bool weapon_can_hit(const CombatType *weapon, uint16 target_x, uint16 target_y) { return true; }
 
 U6LList *get_inventory_list();
 bool inventory_has_object(uint16 obj_n, uint8 qual = 0, bool match_zero_qual = true);
 uint32 inventory_count_objects(bool inc_readied_objects);
 uint32 inventory_count_object(uint16 obj_n, Obj *container = 0);
 Obj *inventory_get_object(uint16 obj_n, uint8 qual = 0, Obj *container = 0, bool search_containers = true, bool match_zero_qual = true);
 Obj *inventory_get_readied_object(uint8 location);
 const CombatType *inventory_get_readied_object_combat_type(uint8 location);
 Obj *inventory_get_obj_container(Obj *obj, Obj *container = 0);
 bool inventory_add_object(Obj *obj, Obj *container = 0);
 bool inventory_remove_obj(Obj *obj, Obj *container = 0);
 Obj *inventory_new_object(uint16 obj_n, uint32 qty, uint8 quality = 0);
 uint32 inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container = 0);
 float inventory_get_max_weight() { return((strength * 2)); }
 float get_inventory_weight();
 float get_inventory_equip_weight();
 void inventory_drop_all();
 void all_items_to_container(Obj *container_obj);
 bool can_carry_weight(float obj_weight); // return from get_obj_weight()
 bool can_carry_object(uint16 obj_n, uint32 qty = 0);
 bool can_carry_object(Obj *obj) { return(can_carry_weight(obj_manager->get_obj_weight(obj, true))); }

 virtual uint8 get_object_readiable_location(uint16 obj_n) { return ACTOR_NOT_READIABLE; }
 virtual const CombatType *get_object_combat_type(uint16 obj_n) { return NULL; }
 
 bool add_readied_object(Obj *obj);
 void remove_readied_object(Obj *obj);
 void remove_readied_object(uint8 location);

 void remove_all_readied_objects();
 bool has_readied_objects();

 virtual void twitch() { return; }
 bool push(Actor *pusher, uint8 where = ACTOR_PUSH_ANYWHERE, uint16 tx = 0, uint16 ty = 0, uint16 tz = 0);

 Obj *make_obj();
 uint16 get_obj_n() { return(obj_n); }
 virtual void clear();
 protected:

 void loadSchedule(unsigned char *schedule_data, uint16 num);
 virtual bool updateSchedule(uint8 hour);
 uint16 getSchedulePos(uint8 hour);
// uint16 getSchedulePos(uint8 hour, uint8 day_of_week);
// inline uint16 Actor::getSchedulePos(uint8 hour);

 void inventory_parse_readied_objects(); //this is used to initialise the readied_objects array on load.
 
 virtual const CombatType *get_hand_combat_type() { return NULL; }

};

#endif /* __Actor_h__ */
