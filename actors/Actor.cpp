/*
 *  Actor.cpp
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
#include "nuvieDefs.h" /* include before cmath to get roundf() */
#include <cstdlib>
#include <cmath>
#include <cassert>
#include "U6misc.h"
#include "U6LList.h"
#include "Game.h"
#include "GameClock.h"
#include "MapWindow.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "U6UseCode.h"
#include "Party.h"
#include "CombatPathFinder.h"
#include "SeekPath.h"
#include "Converse.h"
#include "Effect.h"
#include "Actor.h"

static uint8 walk_frame_tbl[4] = {0,1,2,1};

class ActorManager;

Actor::Actor(Map *m, ObjManager *om, GameClock *c)
:sched(NULL)
{
 map = m;
 obj_manager = om;
 usecode = NULL;
 clock = c;
 pathfinder = NULL;

 direction = 0;
 walk_frame = 0;
 can_move = true;
 in_party = false;
 temp_actor = false;
 visible_flag = true;
// active = false;

 worktype = 0;
 sched_pos = 0;
 update_time = 0;

 memset(readied_objects,0,sizeof(readied_objects));
 moves = 0;
 light = 0;

 name ="";
 flags = 0;
 body_armor_class = 0;
 readied_armor_class = 0;
}

Actor::~Actor()
{
 // free sched array
 if(sched != NULL)
 {
    Schedule** cursched = sched;
    while(*cursched != NULL)
       free(*cursched++);

    free(sched);
 }
 if(pathfinder)
    delete pathfinder;
}

bool Actor::init()
{
 set_moves_left(dex);
 return true;
}

void Actor::init_from_obj(Obj *obj)
{

 obj_n = obj->obj_n;
 frame_n = obj->frame_n;

 init();
 show();
 return;
}

bool Actor::is_alive()
{
 return alive;
}


/* Returns true if another NPC `n' is in proximity to location `where'.
 */
bool Actor::is_nearby(MapCoord &where, uint8 thresh)
{
    MapCoord here(x, y, z);
    if(here.xdistance(where) <= thresh && here.ydistance(where) <= thresh && z == where.z)
        return(true);
    return(false);
}


bool Actor::is_nearby(Actor *other)
{
    MapCoord there(other->get_location());
    return(is_nearby(there));
}


bool Actor::is_nearby(uint8 actor_num)
{
    return(is_nearby(Game::get_game()->get_actor_manager()->get_actor(actor_num)));
}

bool Actor::is_at_position(Obj *obj)
{
 if(obj->x == x && obj->y == y && obj->z == z)
   return true;

 return false;
}

bool Actor::is_passable()
{
 Tile *tile;

 tile = obj_manager->get_obj_tile(obj_n,frame_n);

 return tile->passable;
}

void Actor::get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level)
{
 *ret_x = x;
 *ret_y = y;
 *ret_level = z;
}


MapCoord Actor::get_location()
{
    return(MapCoord(x, y, z));
}


uint16 Actor::get_tile_num()
{
 return obj_n;// + frame_n;
}

uint8 Actor::get_worktype()
{
 return worktype;
}

uint16 Actor::get_downward_facing_tile_num()
{
 return obj_manager->get_obj_tile_num(obj_n) + frame_n;
}

/* Set direction faced by actor and change walk frame.
 */
void Actor::set_direction(uint8 d)
{
 if(d < 4)
   direction = d;

 walk_frame = (walk_frame + 1) % 4;

 frame_n = direction * 4 + walk_frame_tbl[walk_frame];

}


/* Set direction as if moving in relative direction rel_x,rel_y.
 */
void Actor::set_direction(sint16 rel_x, sint16 rel_y)
{
    uint8 new_direction = direction;
    if(rel_x == 0 && rel_y == 0) // nowhere (just update frame)
        new_direction = direction;
    else if(rel_x == 0) // up or down
        new_direction = (rel_y < 0) ? NUVIE_DIR_N : NUVIE_DIR_S;
    else if(rel_y == 0) // left or right
        new_direction = (rel_x < 0) ? NUVIE_DIR_W : NUVIE_DIR_E;
// Add 2 to current direction if it is opposite the new direction
    else if(rel_x < 0 && rel_y < 0) // up-left
    {
        if(direction != NUVIE_DIR_N && direction != NUVIE_DIR_W)
            new_direction = direction + 2;
    }
    else if(rel_x > 0 && rel_y < 0) // up-right
    {
        if(direction != NUVIE_DIR_N && direction != NUVIE_DIR_E)
            new_direction = direction + 2;
    }
    else if(rel_x < 0 && rel_y > 0) // down-left
    {
        if(direction != NUVIE_DIR_S && direction != NUVIE_DIR_W)
            new_direction = direction + 2;
    }
    else if(rel_x > 0 && rel_y > 0) // down-right
    {
        if(direction != NUVIE_DIR_S && direction != NUVIE_DIR_E)
            new_direction = direction + 2;
    }
    // wrap
    if(new_direction >= 4)
        new_direction -= 4;
    set_direction(new_direction);
}

void Actor::face_location(MapCoord &loc)
{
 face_location(loc.x, loc.y);
}

/* Set direction towards an x,y location on the map.
 */
void Actor::face_location(uint16 lx, uint16 ly)
{
    set_direction(lx - x, ly - y);
}
#if 0
/* Set direction towards an x,y location on the map.
 */
void Actor::face_location(uint16 lx, uint16 ly)
{
    uint16 xdiff = abs(x - lx), ydiff = abs(y - ly);
    if(ydiff)
    {
        if(y < ly && direction != 2)
            set_direction(2); // down
        else if(y > ly && direction != 0)
            set_direction(0); // up
    }
    if(xdiff)
    {
        if(x < lx && direction != 1)
            set_direction(1); // right
        else if(x > lx && direction != 3)
            set_direction(3); // left
    }
}
#endif

void Actor::face_actor(Actor *a)
{
 uint16 ax, ay;
 uint8 al;

 a->get_location(&ax, &ay, &al);
 face_location(ax, ay);
}


/* Returns the proper (NPC) name of this actor if the Player knows it, or their
 * description if the name is unknown.
 */
const char *Actor::get_name()
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    Converse *converse = Game::get_game()->get_converse();
    Party *party = Game::get_game()->get_party();
    Actor *player = Game::get_game()->get_player()->get_actor();
    const char *talk_name = NULL; // name from conversation script

    if(in_party)
        name = party->get_actor_name(party->get_member_num(this));
    else if(((id_n == player->id_n) || is_met())
            && (talk_name = converse->npc_name(id_n)))
        name = talk_name;
    else
        name = actor_manager->look_actor(this, false);
    return(name.c_str());
}


bool Actor::moveRelative(sint16 rel_x, sint16 rel_y)
{
 return move(x + rel_x, y + rel_y, z);
}


bool Actor::check_move(sint16 new_x, sint16 new_y, sint8 new_z, ActorMoveFlags flags)
{
 Actor *a;
 bool ignore_actors = flags & ACTOR_IGNORE_OTHERS;
// bool ignore_danger = flags & ACTOR_IGNORE_DANGER;
 bool ignore_danger = true;

    if(z > 5)
        return(false);

    uint16 pitch = map->get_width(new_z);
    if(new_x < 0 || new_x >= pitch)
        return(false);
    if(new_y < 0 || new_y >= pitch)
        return(false);

    if(!ignore_actors)
       {
        a = map->get_actor(new_x,new_y,new_z);
        if(a)
          return (a->is_passable() || is_passable()); // we can move over or under some actors. eg mice, dragons etc.
        // FIXME: but I dont think we can pass dogs/cats/mongbats can we?
       }

//    if(map->is_passable(new_x,new_y,new_z) == false)
//        return(false);

    if(!ignore_danger)
        if(map->is_damaging(new_x, new_y, new_z))
            return false;

    return(true);
}

bool Actor::check_moveRelative(sint16 rel_x, sint16 rel_y, ActorMoveFlags flags)
{
 return check_move(x + rel_x, y + rel_y, z, flags);
}


bool Actor::can_be_moved()
{
 return can_move;
}

bool Actor::can_be_passed(Actor *other)
{
    return(other->is_passable() || is_passable());
}

bool Actor::move(sint16 new_x, sint16 new_y, sint8 new_z, ActorMoveFlags flags)
{
 const uint8 move_cost = 5; // base cost to move
 bool force_move = flags & ACTOR_FORCE_MOVE;
 bool open_doors = flags & ACTOR_OPEN_DOORS;
 bool ignore_actors = flags & ACTOR_IGNORE_OTHERS;
// bool ignore_danger = flags & ACTOR_IGNORE_DANGER;
 bool ignore_moves = flags & ACTOR_IGNORE_MOVES;
 bool ignore_danger = true;
 Obj *obj = NULL;
 MapCoord oldpos(x, y, z);

 clear_error();
 if(!usecode)
   usecode = obj_manager->get_usecode();
 // no moves left
 if(!(force_move||ignore_moves) && moves <= 0)
 {
    set_error(ACTOR_OUT_OF_MOVES);
//    return false;
    printf("warning: actor %d is out of moves\n", id_n);
 }

 // blocking actors are checked for later
 obj = obj_manager->get_obj(new_x,new_y,new_z);
 if(!force_move && !check_move(new_x, new_y, new_z, ACTOR_IGNORE_OTHERS))
   {
    // open door
    if(!(obj && usecode->is_unlocked_door(obj) && open_doors)
       || (!usecode->use_obj(obj, this)))
      {
       set_error(ACTOR_BLOCKED_BY_OBJECT);
       error_struct.blocking_obj = obj;
       return false; // blocked by object or map tile
      }
   }
 // avoid dangerous objects
 if(!ignore_danger && obj && obj_manager->is_damaging(new_x, new_y, new_z))
   {
    set_error(ACTOR_BLOCKED_BY_OBJECT);
    error_struct.blocking_obj = obj;
    return false;
   }
 // usecode must allow movement
 if(obj && usecode->has_passcode(obj))
   {
    if(!usecode->pass_obj(obj, this)) // calling item is this actor
      {
       set_error(ACTOR_BLOCKED_BY_OBJECT);
       error_struct.blocking_obj = obj;
       return false;
      }
   }

 Actor *other = map->get_actor(new_x, new_y, new_z);
 if(!ignore_actors && !force_move
    && other && other->is_visible() && !other->can_be_passed(this))
   {
    set_error(ACTOR_BLOCKED_BY_ACTOR);
    error_struct.blocking_actor = other;
    return false; // blocked by actor
   }

 // move
 x = new_x;
 y = new_y;
 z = new_z;

 can_move = true;
 if(!(force_move || ignore_moves)) // subtract from moves left
 {
    set_moves_left(moves - (move_cost+map->get_impedance(oldpos.x, oldpos.y, oldpos.z)));
    if(oldpos.x != x && oldpos.y != y) // diagonal move, double cost
        set_moves_left(moves - (move_cost+map->get_impedance(oldpos.x, oldpos.y, oldpos.z)));
 }

 // post-move
 // close door
 if(open_doors)
   {
    obj = obj_manager->get_obj(oldpos.x, oldpos.y, z);
    if(obj && usecode->is_door(obj))
       usecode->use_obj(obj, this);
   }

 Game *game = Game::get_game();
 // re-center map if actor is player character
 if(id_n == game->get_player()->get_actor()->id_n && game->get_player()->is_mapwindow_centered())
    game->get_map_window()->centerMapOnActor(this);
 // allows a delay to be set on actor movement, in lieu of using animations
 update_time = clock->get_ticks();
 return true;
}


void Actor::update()
{
    if(pathfinder)
    {
        // NOTE: don't delete pathfinder right after walking, because the scheduled
        // activity still needs to be checked, and depends on pathfinder existing
        if(pathfinder->reached_goal())
            delete_pathfinder();
        else walk_path();
    }

//    update_time = clock->get_ticks(); moved to move()
}

/* Returns true if actor moved. */
bool Actor::walk_path()
{
    pathfinder->update_location(); // set location from actor, if already moved

    // validate path and get move
    MapCoord next_loc, loc(x, y, z);
    if(!pathfinder->get_next_move(next_loc)) // nothing to do here
        return false;
    // FIXME: move to SchedPathFinder (or delete; worktype will handle refresh)
    if(next_loc == loc) // ran out of steps? get a new path
    {
        if(pathfinder->have_path())
            pathfinder->find_path();
        return false;
    }
    if(!move(next_loc.x,next_loc.y,next_loc.z,ACTOR_OPEN_DOORS))
        return false; // don't get a new path; probably just blocked by an actor
    set_direction(x-loc.x, y-loc.y);
    pathfinder->actor_moved();
    return true;
}

// gz 255 = current map plane
void Actor::pathfind_to(uint16 gx, uint16 gy, uint8 gz)
{
    if(gz == 255)
        gz = z;
    MapCoord d(gx, gy, gz);
    pathfind_to(d);
}

void Actor::pathfind_to(MapCoord &d)
{
    if(pathfinder)
    {
        pathfinder->set_actor(this);
        pathfinder->set_goal(d);
    }
    else
        set_pathfinder(new ActorPathFinder(this, d), new SeekPath);
    pathfinder->update_location();
}

// actor will take management of new_pf, and delete it when no longer needed
void Actor::set_pathfinder(ActorPathFinder *new_pf, Path *path_type)
{
    if(pathfinder != NULL && pathfinder != new_pf)
        delete_pathfinder();
    pathfinder = new_pf;
    if(path_type != 0)
        pathfinder->set_search(path_type);
}

void Actor::delete_pathfinder()
{
    delete pathfinder;
    pathfinder = NULL;
}

void Actor::set_in_party(bool state)
{
    in_party = state;
    delete_pathfinder();
    if(state == true) // joined
    {
//        obj_n = base_obj_n; U6Actor::set_worktype
        can_move = true;
        set_worktype(0x01); // U6_IN_PARTY
        status_flags |= ACTOR_STATUS_IN_PARTY;
    }
    else // left
    {
        if(alive)
          {
           inventory_drop_all();
           set_worktype(0x8f); // U6_WANDER_AROUND
           status_flags ^= ACTOR_STATUS_IN_PARTY;
          }
    }
}

void Actor::attack(MapCoord pos)
{
 return;
}

void Actor::attack(sint8 readied_obj_location, Actor *actor)
{
 const uint8 attack_cost = 10; // base cost to attack
 Obj *weapon_obj;
 const CombatType *combat_type;

 combat_type = get_weapon(readied_obj_location);
 
 if(combat_type == NULL)
   return;
 
 if(actor->defend(dex, combat_type->attack) == false)
  {
   if(combat_type->breaks_on_contact)
     {
      weapon_obj = readied_objects[readied_obj_location]->obj;
      remove_readied_object(readied_obj_location);
      inventory_remove_obj(weapon_obj);
     }
  } 
 set_moves_left(moves - attack_cost);
}

const CombatType *Actor::get_weapon(sint8 readied_obj_location)
{
 if(readied_obj_location == ACTOR_NO_READIABLE_LOCATION)
   return get_hand_combat_type();

 if(readied_objects[readied_obj_location])
   return readied_objects[readied_obj_location]->combat_type;

 return NULL;
}

U6LList *Actor::get_inventory_list()
{
 return obj_manager->get_actor_inventory(id_n);
}


bool Actor::inventory_has_object(uint16 obj_n, uint8 qual, bool match_zero_qual)
{
    if(inventory_get_object(obj_n, qual, NULL, true, match_zero_qual))
        return(true);
    return(false);
}

uint32 Actor::inventory_count_objects(bool inc_readied_objects)
{
 Obj *obj;
 uint32 count = 0;
 U6Link *link;
 U6LList *inventory = get_inventory_list();

 if(inc_readied_objects)
  {
   return inventory->count();
  }
 else
  {
   for(link=inventory->start();link != NULL;link=link->next)
     {
      obj = (Obj *)link->data;
      if((obj->status & 0x18) != 0x18)
        count++;
     }
  }

 return count;
}


/* Returns the number of objects in the actor's inventory with matching object
 * number and quality.
 */
uint32 Actor::inventory_count_object(uint16 obj_n, Obj *container)
{
    uint32 qty = 0;
    U6Link *link = 0;
    Obj *obj = 0;
    U6LList *inv = container ? container->container
                             : get_inventory_list();

    for(link = inv->start(); link != NULL; link = link->next)
    {
        obj = (Obj *)link->data;
        if(obj->container)
            qty += inventory_count_object(obj_n, obj);
        if(obj->obj_n == obj_n)
            qty += obj->qty;
    }
    return(qty);
}


/* Returns object descriptor of object in the actor's inventory, or NULL if no
 * matching object is found.
 */
Obj *Actor::inventory_get_object(uint16 obj_n, uint8 qual, Obj *container, bool search_containers, bool match_zero_qual)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;

 inventory = container ? container->container
                       : get_inventory_list();
 for(link=inventory->start();link != NULL;link=link->next)
   {
    obj = (Obj *)link->data;
    if(obj->obj_n == obj_n && ((match_zero_qual == false && qual == 0) || obj->quality == qual)) //FIXME should qual = 0 be an all quality search!?
      return(obj);
    else if(obj->container && search_containers)
    {
      if((obj = inventory_get_object(obj_n, qual, obj)))
        return(obj);
    }
   }

 return NULL;
}

Obj *Actor::inventory_get_readied_object(uint8 location)
{
 if(readied_objects[location] != NULL)
   return readied_objects[location]->obj;

 return NULL;
}

const CombatType *Actor::inventory_get_readied_object_combat_type(uint8 location)
{
 if(readied_objects[location] != NULL)
   return readied_objects[location]->combat_type;

 return NULL;
}


bool Actor::inventory_add_object(Obj *obj, Obj *container)
{
 U6LList *inventory = get_inventory_list(), *add_to = inventory;

 if(container) // assumes actor is holding the container
 {
   add_to = container->container;
   obj->status |= OBJ_STATUS_IN_CONTAINER;
   obj->x = container->objblk_n;
 }
 else
 {

 /*
   if(obj_manager->is_stackable(obj)) // find similiar objects outside containers
   {
     Obj *stack_with = inventory_get_object(obj->obj_n, 0, NULL, false, false);
     if(stack_with) // we stack onto the new object, and delete the old one
     {
      new_qty = obj_manager->get_obj_qty(obj) + obj_manager->get_obj_qty(stack_with);
      obj_manager->set_obj_qty(obj, new_qty);
      
       inventory_remove_obj(stack_with);
       delete_obj(stack_with);
     }
   }
  */
   // only objects outside containers are marked in_inventory
   obj->status |= OBJ_STATUS_IN_INVENTORY;
   
   if(obj->status & OBJ_STATUS_IN_CONTAINER)
     obj->status ^= OBJ_STATUS_IN_CONTAINER;
   
   // we have the item now so we don't consider it stealing if we get it at any time in the future.
   obj->status |= OBJ_STATUS_OK_TO_TAKE;
   obj->x = id_n;
 }

 return obj_manager->list_add_obj(add_to, obj);
}


/* Returns a pointer to new or old object. (already added to inventory)
 */
Obj *Actor::inventory_new_object(uint16 obj_n, uint32 qty, uint8 quality)
{
 Obj *obj = 0;
 uint8 frame_n = 0;

 if(obj_n > 1024)
   {
    frame_n = (uint8)floorf(obj_n / 1024);
	obj_n -= frame_n * 1024;
   }

 obj = new Obj;
 obj->obj_n = obj_n;
 obj->frame_n = frame_n;
 obj->quality = quality;
 obj->qty = qty;
 inventory_add_object(obj, NULL);

 return(obj);
}


/* Delete `qty' objects of type from inventory (or from a container).
 * Returns the number removed (may be less than requested).
 */
uint32
Actor::inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container)
{
 Obj *obj;
 uint16 oqty = 0;
 uint32 deleted = 0;

 while((obj = inventory_get_object(obj_n, quality, container, true, false))
       && (deleted < qty))
 {
    oqty = obj->qty;
    if(oqty <= (qty - deleted))
        inventory_remove_obj(obj, container);
    else
        obj->qty = oqty - qty;
    deleted += oqty;
 }
 return(deleted);
}


bool Actor::inventory_remove_obj(Obj *obj, Obj *container)
{
 U6LList *inventory;

 inventory = get_inventory_list();
 if(obj->is_readied())
    remove_readied_object(obj);
 if(!container)
    container = inventory_get_obj_container(obj);

 if(container)
 {
    obj->status &= ~OBJ_STATUS_IN_CONTAINER;
    return container->container->remove(obj);
 }
 obj->status &= ~OBJ_STATUS_IN_INVENTORY;
 return inventory->remove(obj);
}


/* Search inventory for obj and return pointer to object it is contained in.
 * Returns NULL if obj is not in a container, or is not found.
 */
Obj *Actor::inventory_get_obj_container(Obj *obj, Obj *container)
{
    U6LList *inventory = container ? container->container : get_inventory_list();
    for(U6Link *link = inventory->start(); link != NULL; link = link->next)
    {
        Obj *this_obj = (Obj *)link->data;
        if(this_obj == obj)
            return(container);
        else if(this_obj->container) // a new container to search in
        {
            Obj *result = inventory_get_obj_container(obj, this_obj);
            if(result)
                return(result);
        }
    }
    return(NULL);
}


float Actor::get_inventory_weight()
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 float weight = 0;

 if(obj_manager->actor_has_inventory(id_n) == false)
   return 0;

 inventory = obj_manager->get_actor_inventory(id_n);

 for(link=inventory->start();link != NULL;link=link->next)
  {
   obj = (Obj *)link->data;
   weight += obj_manager->get_obj_weight(obj);
  }

 return roundf(weight);
}

float Actor::get_inventory_equip_weight()
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 float weight = 0;

 if(obj_manager->actor_has_inventory(id_n) == false)
   return 0;

 inventory = obj_manager->get_actor_inventory(id_n);

 for(link=inventory->start();link != NULL;link=link->next)
  {
   obj = (Obj *)link->data;
   if((obj->status & 0x18) == 0x18) //object readied
      weight += obj_manager->get_obj_weight(obj);
  }

 return roundf(weight);
}


/* Can the actor carry a new object of this type?
 */
bool Actor::can_carry_object(uint16 obj_n, uint32 qty)
{
    float obj_weight = obj_manager->get_obj_weight(obj_n);
    if(qty) obj_weight *= qty;
    return(can_carry_weight(obj_weight));
}


/* Can the actor carry new object(s) of this weight?
 * (return from get_obj_weight())
 */
bool Actor::can_carry_weight(float obj_weight)
{
    // obj_weight /= 10;
    return((get_inventory_weight() + obj_weight) <= inventory_get_max_weight());
}


void Actor::inventory_parse_readied_objects()
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;

 if(obj_manager->actor_has_inventory(id_n) == false)
   return;

 inventory = obj_manager->get_actor_inventory(id_n);

 for(link=inventory->start();link != NULL;link=link->next)
  {
   obj = (Obj *)link->data;
   if((obj->status & 0x18) == 0x18) //object readied
      {
       add_readied_object(obj);
      }
  }

 return;
}

//FIX handle not readiable, no place to put, double handed objects
bool Actor::add_readied_object(Obj *obj)
{
 uint8 location;

 location =  get_object_readiable_location(obj->obj_n);

 switch(location)
   {
    case ACTOR_NOT_READIABLE : return false;

    case ACTOR_ARM : if(readied_objects[ACTOR_ARM] != NULL) //if full try other arm
                         location = ACTOR_ARM_2;
                     break;

    case ACTOR_HAND : if(readied_objects[ACTOR_HAND] != NULL) // if full try other hand
                          location = ACTOR_HAND_2;
                      break;
   }

 if(readied_objects[location] != NULL)
   return false;
                
 readied_objects[location] = new ReadiedObj;
 
 readied_objects[location]->obj = obj;
 readied_objects[location]->combat_type = get_object_combat_type(obj->obj_n);

 if(readied_objects[location]->combat_type != NULL)
   readied_armor_class += readied_objects[location]->combat_type->defence;

 obj->status |= 0x18; //set object to readied status
 return true;
}

void Actor::remove_readied_object(Obj *obj)
{
 uint8 location;

 for(location=0; location < ACTOR_MAX_READIED_OBJECTS; location++)
   {
    if(readied_objects[location] != NULL && readied_objects[location]->obj == obj)
      {
       remove_readied_object(location);
       break;
      }
   }

 return;
}

void Actor::remove_readied_object(uint8 location)
{
 Obj *obj;

 obj = inventory_get_readied_object(location);

 if(obj)
   {
    if(readied_objects[location]->combat_type)
      readied_armor_class -= readied_objects[location]->combat_type->defence;

    delete readied_objects[location];
    readied_objects[location] = NULL;
    obj->status ^= 0x18; // remove "readied" bit flag.
    obj->status |= OBJ_STATUS_IN_INVENTORY; // keep "in inventory"

    if(location == ACTOR_ARM && readied_objects[ACTOR_ARM_2] != NULL) //move contents of left hand to right hand.
      {
       readied_objects[ACTOR_ARM] = readied_objects[ACTOR_ARM_2];
       readied_objects[ACTOR_ARM_2] = NULL;
      }
   }

 return;
}

void Actor::remove_all_readied_objects()
{
 uint8 location;

 for(location=0; location < ACTOR_MAX_READIED_OBJECTS; location++)
   {
    if(readied_objects[location] != NULL)
      remove_readied_object(location);
   }

 return;
}

// returns true if the actor has one or more readied objects
bool Actor::has_readied_objects()
{
 uint8 location;

 for(location=0; location < ACTOR_MAX_READIED_OBJECTS; location++)
   {
    if(readied_objects[location] != NULL)
      return true;
   }

 return false;
 
}

void Actor::inventory_drop_all()
{
    U6LList *inv = NULL;
    Obj *obj = NULL;

    while(inventory_count_objects(true))
    {
        inv = get_inventory_list();
        obj = (Obj *)(inv->start()->data);
        if(!inventory_remove_obj(obj))
            break;
        obj->status |= OBJ_STATUS_OK_TO_TAKE;
        obj->x = x; obj->y = y; obj->z = z;
        obj_manager->add_obj(obj, true); // add to map
    }
}

// Moves inventory and all readied items into a container object.
void Actor::all_items_to_container(Obj *container_obj)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 uint8 i;

 if(!container_obj->container)
   container_obj->container = new U6LList();

 inventory = get_inventory_list();

 if(!inventory)
   return;

 for(link=inventory->start(); link != NULL;)
   {
    obj = (Obj *)link->data;
    link = link->next;
    inventory->remove(obj);

    if(temp_actor)
      obj->status |= OBJ_STATUS_TEMPORARY;
      
    container_obj->container->add(obj);
   }
 
 for(i=0; i < ACTOR_MAX_READIED_OBJECTS; i++)
   {
    if(readied_objects[i])
      {
       remove_readied_object(i);
      }
   }

 return;
}

void Actor::loadSchedule(unsigned char *sched_data, uint16 num)
{
 uint16 i;
 unsigned char *sched_data_ptr;

 sched = (Schedule**)malloc(sizeof(Schedule*) * (num+1));

 sched_data_ptr = sched_data;

 for(i=0;i<num;i++)
   {
    sched[i] = (Schedule *)malloc(sizeof(Schedule));

    sched[i]->hour = sched_data_ptr[0] & 0x1f; // 5 bits for hour
    sched[i]->day_of_week = sched_data_ptr[0] >> 5; // 3 bits for day of week
    sched[i]->worktype = sched_data_ptr[1];

    sched[i]->x = sched_data_ptr[2];
    sched[i]->x += (sched_data_ptr[3] & 0x3) << 8;

    sched[i]->y = (sched_data_ptr[3] & 0xfc) >> 2;
    sched[i]->y += (sched_data_ptr[4] & 0xf) << 6;

    sched[i]->z = (sched_data_ptr[4] & 0xf0) >> 4;
    sched_data_ptr += 5;
#ifdef DEBUG
    printf("#%04d %03x,%03x,%x hour %2d day of week %2d worktype %02x\n",id_n,sched[i]->x,sched[i]->y,sched[i]->z,sched[i]->hour,sched[i]->day_of_week,sched[i]->worktype);
#endif
   }

 sched[i] = NULL;

 return;
}

//FIX for day_of_week

bool Actor::updateSchedule(uint8 hour)
{
 uint8 day_of_week;
 uint16 new_pos;

 //hour = clock->get_hour();
 day_of_week = clock->get_day_of_week();

 new_pos = getSchedulePos(hour);

 if(new_pos == sched_pos) // schedules are the same so we do nothing.
  return false;

 sched_pos = new_pos;

 if(sched[sched_pos] == NULL)
   return false;

 // U6: temp. fix for walking statues; they shouldn't have schedules
 if(id_n >= 188 && id_n <= 200)
  {
   printf("warning: tried to update schedule for non-movable actor %d\n", id_n);
   return(false);
  }

 set_worktype(sched[sched_pos]->worktype);
 return true;
}

// returns the current schedule entry based on hour
uint16 Actor::getSchedulePos(uint8 hour)
{
 uint16 i;

 for(i=0;sched[i] != NULL;i++)
  {
   if(sched[i]->hour > hour)
     {
      if(i != 0)
        return i-1;
      else // i == 0 this means we are in the last schedule entry
        for(;sched[i+1] != NULL;)
          i++;
     }
  }

 if(i==0)
  return 0;

 return i-1;
}

/*
// returns the current schedule entry based on hour
uint16 Actor::getSchedulePos(uint8 hour, uint8 day_of_week)
{
 uint16 i,j;
 if(id_n == 133)
  printf(".");

 i = getSchedulePos(hour);

 for(j=i;sched[j] != NULL && sched[j]->hour == sched[i]->hour;j++)
  {
   if(sched[j]->day_of_week > day_of_week)
     {
      if(j != i)
        return j-1;
      else // hour is in the last schedule entry.
        {
         for(;sched[j+1] != NULL && sched[j+1]->hour == sched[i]->hour;) // move to the last schedule entry.
          j++;
        }
     }
  }

 if(j==i)
  return j;

 return j-1;
}

inline uint16 Actor::getSchedulePos(uint8 hour)
{
 uint16 i;
 uint8 cur_hour;

 for(i=0;sched[i] != NULL;i++)
  {
   if(sched[i]->hour > hour)
     {
      if(i != 0)
        return i-1;
      else // hour is in the last schedule entry.
        {
         for(;sched[i+1] != NULL;) // move to the last schedule entry.
          i++;

         if(sched[i]->day_of_week > 0) //rewind to the start of the hour set.
           {
            cur_hour = sched[i]->hour;
            for(;i >= 1 && sched[i-1]->hour == cur_hour;)
              i--;
           }
        }
     }
   else
      for(;sched[i+1] != NULL && sched[i+1]->hour == sched[i]->hour;) //skip to next hour set.
        i++;
  }

 if(sched[i] != NULL && sched[i]->day_of_week > 0) //rewind to the start of the hour set.
   {
    cur_hour = sched[i]->hour;
    for(;i >= 1 && sched[i-1]->hour == cur_hour;)
      i--;
   }

 if(i==0)
  return 0;

 return i-1;
}
*/

void Actor::set_worktype(uint8 new_worktype)
{
 worktype = new_worktype;
 work_location.x = x;
 work_location.y = y;
 work_location.z = z;
 return ;
}

/* Set NPC flag `bitflag' to 1.
 */
void Actor::set_flag(uint8 bitflag)
{
    if(bitflag > 7)
        return;
    this->set_flags(this->get_flags() | (1 << bitflag));
}


/* Set NPC flag `bitflag' to 0.
 */
void Actor::clear_flag(uint8 bitflag)
{
    if(bitflag > 7)
        return;
    this->set_flags(this->get_flags() & ~(1 << bitflag));
}

Obj *Actor::make_obj()
{
 Obj *obj;

 obj = new Obj();

 obj->x = x;
 obj->y = y;
 obj->z = z;

 obj->obj_n = obj_n;
 obj->frame_n = frame_n;
 obj->quality = id_n;

 return obj;
}

void Actor::clear()
{
 x = 0;
 y = 0;
 z = 0;
 hide();
 Actor::set_worktype(0);

}


/* Get pushed by `pusher' to location determined by `where'.
 */
bool Actor::push(Actor *pusher, uint8 where)
{
    if(where == ACTOR_PUSH_HERE) // move to pusher's square and use up moves
    {
        MapCoord to(pusher->x, pusher->y, pusher->z), from(get_location());
        if(to.distance(from) > 1 || z != to.z)
            return(false);
        face_location(to.x, to.y);
        move(to.x, to.y, to.z, ACTOR_FORCE_MOVE); // can even move onto blocked squares
        if(moves > 0)
            set_moves_left(0); // we use up our moves exchanging positions
        return(true);
    }
    else if(where == ACTOR_PUSH_ANYWHERE) // go to any neighboring direction
    {
        MapCoord from(get_location());
        const uint16 square = 1;
        if(this->push(pusher, ACTOR_PUSH_FORWARD))
            return(true); // prefer forward push
        for(uint16 x=(from.x-square); x<=(from.x+square); x+=square)
            for(uint16 y=(from.y-square); y<=(from.y+square); y+=square)
                if(x != from.x && y != from.y && move(x, y, from.z))
                    return(true);
    }
    else if(where == ACTOR_PUSH_FORWARD) // move away from pusher
    {
        MapCoord from(get_location());
        MapCoord pusher_loc(pusher->x, pusher->y, pusher->z);
        if(pusher_loc.distance(from) > 1 || z != pusher->z)
            return(false);
        sint8 rel_x = -(pusher_loc.x-from.x), rel_y = -(pusher_loc.y-from.y);
        if(moveRelative(rel_x, rel_y))
        {
            set_direction(rel_x, rel_y);
            return(true);
        }
    }
    return(false);
}

//returns true if successfully defended
//false if damage incurred
bool Actor::defend(uint8 attack, uint8 weapon_damage)
{
 uint8 damage;
 uint8 total_armor_class = body_armor_class + readied_armor_class;
 uint8 ac_saving_throw = 0;
 
 if(weapon_damage == 0)
   return true;
/* 
 if(readied_armor_class > 0)
  total_armor_class = readied_armor_class;
 else
  total_armor_class = body_armor_class;
*/
 printf("attack=%d, weapon_damage=%d defenders ac=%d(%d,%d)'body,armor'", attack, weapon_damage, total_armor_class, body_armor_class, readied_armor_class);

 if(NUVIE_RAND() % 30 >= (dex - attack) / 2)
   {
    if(weapon_damage == 255) // A weapon that does 255 damage kills every time.
      {
       hit(255, ACTOR_FORCE_HIT);
       return false;
      }

    damage = NUVIE_RAND() % weapon_damage;
    
    if(total_armor_class > 0)
      ac_saving_throw = NUVIE_RAND() % total_armor_class;

    printf("actual damage = %d ac_save = %d\n",damage, ac_saving_throw);

    if(damage > ac_saving_throw)
      {
       hit(damage-ac_saving_throw, true);
       return false; // actor took damage
      }
   }

 return true; // actor defended this attack
}

/* Subtract amount from hp. May die if hp is too low.
 */
void Actor::reduce_hp(uint8 amount)
{
 printf("hit %s for %d points\n", get_name(), amount);

    if(amount <= hp) hp -= amount;
    else hp = 0;
// FIX... game specific?
    if(hp == 0)
        die();
}

void Actor::die()
{
    hp = 0;
    alive = false;
    visible_flag = false;
    status_flags |= ACTOR_STATUS_DEAD;
}

void Actor::display_condition()
{
    MsgScroll *scroll = Game::get_game()->get_scroll();

    scroll->display_string(get_name());
    scroll->display_string(" ");
    if(hp < get_maxhp()/4) // 25%
        scroll->display_string("critical!\n");
    else
    {
        if(hp < get_maxhp()/2) // 50%
            scroll->display_string("heavily");
        else if(hp < get_maxhp()/1.33) // 75%
            scroll->display_string("lightly");
        else
            scroll->display_string("barely");
        scroll->display_string(" wounded.\n");
    }
}

/* Get hit and take damage by some indirect effect. (no source)
 */
void Actor::hit(uint8 dmg, bool force_hit)
{
 MsgScroll *scroll = Game::get_game()->get_scroll();
 uint8 total_armor_class = body_armor_class; //+ readied_armor_class;
      
 if(dmg == 0)
   {
    scroll->display_string(get_name());
    scroll->display_string(" grazed!\n");
   }
 else if(dmg > total_armor_class || force_hit)
   {
    new HitEffect(this);
    reduce_hp(force_hit ? dmg : dmg - total_armor_class);
    
//    if(!force_hit)
//      {
       if(hp == 0)
         {
          scroll->display_string(get_name());
          scroll->display_string(" killed!\n");
         }
       else
         {
          display_condition();
         }
//      }
   }
}

void Actor::attract_to(Actor *target)
{
    delete_pathfinder();
    set_pathfinder(new CombatPathFinder(this));
    ((CombatPathFinder*)pathfinder)->set_chase_mode(target);
}

void Actor::repel_from(Actor *target)
{
    delete_pathfinder();
    set_pathfinder(new CombatPathFinder(this, target));
    ((CombatPathFinder*)pathfinder)->set_flee_mode(target);
    ((CombatPathFinder*)pathfinder)->set_distance(2);
}

void Actor::add_light(uint8 val)
{
    light += val;
}

void Actor::subtract_light(uint8 val)
{
    if(light >= val)
        light -= val;
    else
        light = 0;
}

void Actor::set_moves_left(sint8 val)
{
    moves = clamp(val, -127, dex);
}

/* Set error/status information. */
void Actor::set_error(ActorErrorCode err)
{
    clear_error();
    error_struct.err = err;
}

void Actor::clear_error()
{
    error_struct.err = ACTOR_NO_ERROR;
}

ActorError *Actor::get_error()
{
    return(&error_struct);
}

// frozen by worktype or status
bool Actor::is_immobile()
{
    return(false);
}

void Actor::print()
{
    Actor *actor = this;
    printf("\n");
    printf("%s at %x, %x, %x\n", get_name(), actor->x, actor->y, actor->z);
    printf("id_n: %d\n", actor->id_n);

    printf("obj_n: %03d    frame_n: %d\n", actor->obj_n, actor->frame_n);
    printf("base_obj_n: %03d    old_frame_n: %d\n", actor->base_obj_n, actor->old_frame_n);

    uint8 direction = actor->direction;
    printf("direction: %d (%s)\n", direction, (direction==NUVIE_DIR_N)?"north":
                                              (direction==NUVIE_DIR_E)?"east":
                                              (direction==NUVIE_DIR_S)?"south":
                                              (direction==NUVIE_DIR_W)?"west":"???");
    printf("walk_frame: %d\n", actor->walk_frame);

    printf("can_move: %s\n", actor->can_move ? "true" : "false");
//    printf("alive: %s\n", actor->alive ? "true" : "false");
    printf("in_party: %s\n", actor->in_party ? "true" : "false");
    printf("visible_flag: %s\n", actor->visible_flag ? "true" : "false");
//    printf("met_player: %s\n", actor->met_player ? "true" : "false");
    printf("is_immobile: %s\n", actor->is_immobile() ? "true" : "false");

    printf("moves: %d\n", actor->moves);

    const char *wt_string = get_worktype_string(actor->worktype);
    if(!wt_string) wt_string = "???";
    printf("worktype: 0x%02x/%03d %s\n", actor->worktype, actor->worktype, wt_string);

    printf("NPC stats:\n");
    printf(" level: %d    exp: %d    hp: %d / %d\n", actor->level, actor->exp,
           actor->hp, actor->get_maxhp());
    printf(" strength: %d    dex: %d    int: %d\n", actor->strength, actor->dex,
           actor->intelligence);
    printf(" magic: %d\n", actor->magic);

    uint8 combat_mode = actor->combat_mode;
    wt_string = get_worktype_string(actor->combat_mode);
    if(!wt_string) wt_string = "???";
    printf("combat_mode: %d %s\n", combat_mode, wt_string);

    printf("NPC flags: ");
    print_b(actor->flags);
    printf("\n");

    printf("Status flags: ");
    print_b(actor->status_flags);
    printf("\n");

    uint32 inv = actor->inventory_count_objects(true);
    if(inv)
    {
        printf("Inventory (+readied): %d objects\n", inv);
        U6LList *inv_list = actor->get_inventory_list();
        for(U6Link *link = inv_list->start(); link != NULL; link=link->next)
        {
            Obj *obj = (Obj *)link->data;
            printf(" %24s (%03d:%d) qual=%d qty=%d    (weighs %f)\n",
                   obj_manager->look_obj(obj), obj->obj_n, obj->frame_n, obj->quality,
                   obj->qty, obj_manager->get_obj_weight(obj, false));
        }
        printf("(weight %f / %f)\n", actor->get_inventory_weight(),
               actor->inventory_get_max_weight());
    }
    if(actor->sched && *actor->sched)
    {
        printf("Schedule:\n");
        Schedule **s = actor->sched;
        uint32 sp = 0;
        do
        {
            wt_string = get_worktype_string(s[sp]->worktype);
            if(!wt_string) wt_string = "???";
            if(sp == actor->sched_pos && s[sp]->worktype == actor->worktype)
                printf("*%d: location=0x%03x,0x%03x,0x%x  time=%02d:00  day=%d  worktype=0x%02x(%s)*\n", sp, s[sp]->x, s[sp]->y, s[sp]->z, s[sp]->hour, s[sp]->day_of_week, s[sp]->worktype, wt_string);
            else
                printf(" %d: location=0x%03x,0x%03x,0x%x  time=%02d:00  day=%d  worktype=0x%02x(%s)\n", sp, s[sp]->x, s[sp]->y, s[sp]->z, s[sp]->hour, s[sp]->day_of_week, s[sp]->worktype, wt_string);
        } while(s[++sp]);
    }

    if(!actor->surrounding_objects.empty())
        printf("Actor has multiple tiles\n");
    if(actor->pathfinder)
        printf("Actor is on a path\n");
    printf("\n");
}
