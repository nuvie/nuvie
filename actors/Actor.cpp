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
#include "U6LList.h"
#include "Game.h"
#include "GameClock.h"
#include "MapWindow.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "U6UseCode.h"
#include "Party.h"
#include "LPath.h"
#include "ZPath.h"
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

 worktype = 0;
 sched_pos = 0;

 memset(readied_objects,0,sizeof(readied_objects));
 moves = 1;

 name ="";
 flags = 0;
 armor_class = 0;
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
 return true;
}

void Actor::init_from_obj(Obj *obj)
{

 x = obj->x;
 y = obj->y;
 z = obj->z;

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
        new_direction = (rel_y < 0) ? ACTOR_DIR_U : ACTOR_DIR_D;
    else if(rel_y == 0) // left or right
        new_direction = (rel_x < 0) ? ACTOR_DIR_L : ACTOR_DIR_R;
// Add 2 to current direction if it is opposite the new direction
    else if(rel_x < 0 && rel_y < 0) // up-left
    {
        if(direction != ACTOR_DIR_U && direction != ACTOR_DIR_L)
            new_direction = direction + 2;
    }
    else if(rel_x > 0 && rel_y < 0) // up-right
    {
        if(direction != ACTOR_DIR_U && direction != ACTOR_DIR_R)
            new_direction = direction + 2;
    }
    else if(rel_x < 0 && rel_y > 0) // down-left
    {
        if(direction != ACTOR_DIR_D && direction != ACTOR_DIR_L)
            new_direction = direction + 2;
    }
    else if(rel_x > 0 && rel_y > 0) // down-right
    {
        if(direction != ACTOR_DIR_D && direction != ACTOR_DIR_R)
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
 * description if the name is unknown. If the name field is already set, that
 * will be returned instead.
 */
const char *Actor::get_name()
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    Converse *converse = Game::get_game()->get_converse();
    Party *party = Game::get_game()->get_party();
    const char *talk_name = NULL; // name from conversation script

    if(name == "")
    {
        if(in_party)
            name = party->get_actor_name(party->get_member_num(this));
        else if(is_met() && (talk_name = converse->npc_name(id_n)))
            name = talk_name;
        else
            name = actor_manager->look_actor(this, false);
    }
    return(name.c_str());
}


bool Actor::moveRelative(sint16 rel_x, sint16 rel_y)
{
 return move(x + rel_x, y + rel_y, z);
}


bool Actor::check_move(sint16 new_x, sint16 new_y, sint8 new_z, bool ignore_actors)
{
 Actor *a;

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
        // FIXME: but I dont think we can pass dogs/cats/reapers can we?
       }

//    if(map->is_passable(new_x,new_y,new_z) == false)
//        return(false);

    return(true);
}


bool Actor::can_be_moved()
{
 return can_move;
}

bool Actor::move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move)
{
 // no moves left (FIXME: ignore for any player-actor)
 if(!force_move && moves == 0 && id_n != 0) // vehicle actor has no move limit
    return false;

 // blocking actors are checked for later
 if(!force_move && !check_move(new_x, new_y, new_z, ACTOR_IGNORE_OTHERS))
    return false;

 // usecode must allow movement
 usecode = obj_manager->get_usecode();
 Obj *obj = obj_manager->get_obj(new_x,new_y,new_z);
 if(obj && usecode->has_passcode(obj))
  {
    if(!usecode->pass_obj(obj, this)) // calling item is this actor
       return(false);
  }

 // switch position with party members
 Actor *other = map->get_actor(new_x, new_y, new_z);
 if(other && other->is_visible() && !force_move && !other->is_passable() && !is_passable() && !other->push(this, ACTOR_PUSH_HERE, x, y, z))
    return false; // blocked by actor

 // move
 x = new_x;
 y = new_y;
 z = new_z;

 can_move = true;
 if(!force_move && moves > 0)
    --moves;

 // post-move
 // update known location of leader, for party members
 Game *game = Game::get_game();
 if(in_party)
 {
    Party *party = game->get_party();
    sint8 party_member = party->get_member_num(this);
    if(party_member > 0)
        party->find_leader(party_member);
 }
 // re-center map if actor is player character
 if(id_n == game->get_player()->get_actor()->id_n && game->get_player()->is_mapwindow_centered())
    game->get_map_window()->centerMapOnActor(this);
 return true;
}


void Actor::update()
{
    if(moves == 0)
        moves = 1;
/*
 uint8 new_direction;

 // do actor stuff here.
 if(standing)
  {
   if(NUVIE_RAND()%50 == 1)
     {
      walk_frame = NUVIE_RAND()%4;
      frame_n = direction * 4 + walk_frame_tbl[walk_frame];
     }
   else
    {
     if(!in_party)
       {
        if(NUVIE_RAND()%80 == 1)
          {
           new_direction = NUVIE_RAND()%4;
           set_direction(new_direction);
           switch(new_direction)
            {
             case 0 : moveRelative(0,-1); break;
             case 1 : moveRelative(1,0); break;
             case 2 : moveRelative(0,1); break;
             case 3 : moveRelative(-1,0); break;
            }
          }
       }
    }
  }
*/
 //if(!in_party) // don't do scheduled activities while partying
 //updateSchedule();
 if(pathfinder)
 {
    if(pathfinder->reached_goal()) // check schedule after walk, before stopping
       stop_walking();
    else
        pathfinder->walk_path();
 }
}


/* Start walking to a short-range destination on the map (Follow).
 */
void Actor::swalk(MapCoord &d, uint8 speed, uint8 delay)
{
    if(pathfinder && (!pathfinder->can_follow()))
        stop_walking();
    if(!pathfinder)
        pathfinder = new ZPath(this, d);
    else
        pathfinder->set_dest(d); // use existing path-finder
    pathfinder->set_speed(speed);
    pathfinder->wait(delay);
}


/* Start walking to a short-range destination on the map. Allow a secondary
 * destination (to follow another actor in formation, for example.)
 */
void Actor::swalk(MapCoord &d, MapCoord &d2, uint8 speed, uint8 delay)
{
    swalk(d, speed);
    if(pathfinder)
    {
        pathfinder->set_dest2(d2);
        pathfinder->wait(delay);
    }
}


/* Start walking to a long-range destination on the map (Travel).
 */
void Actor::lwalk(MapCoord &d, uint8 speed, uint8 delay)
{
    if(pathfinder && (!pathfinder->can_travel()))
        stop_walking();
    if(!pathfinder)
        pathfinder = new LPath(this, d);
    else
        pathfinder->set_dest(d); // use existing path-finder
    pathfinder->set_speed(speed);
    pathfinder->wait(delay);
}


void Actor::stop_walking()
{
    delete pathfinder;
    pathfinder = NULL;
}


void Actor::set_in_party(bool state)
{
    in_party = state;
    stop_walking();
    if(state == true) // joined
    {
//        obj_n = base_obj_n; U6Actor::set_worktype
        can_move = true;
        set_worktype(0x01); // U6_IN_PARTY
    }
    else // left
    {
        if(alive)
          {
           inventory_drop_all();
           set_worktype(0x8f); // U6_WANDER_AROUND
          }
    }
}

void Actor::attack(MapCoord pos)
{
 return;
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
uint32 Actor::inventory_count_object(uint16 obj_n, uint8 qual, Obj *container)
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
            qty += inventory_count_object(obj_n, qual, obj);
        if(obj->obj_n == obj_n && obj->quality == qual)
            qty += (obj->qty > 0) ? obj->qty : 1; // quantity 0 counts as 1
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
   return readied_objects[location];

 return NULL;
}


bool Actor::inventory_add_object(Obj *obj, Obj *container, bool stack)
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
   if(stack && obj_manager->is_stackable(obj)) // find similiar objects outside containers
   {
     Obj *stack_with = inventory_get_object(obj->obj_n, obj->quality, NULL, false);
     if(stack_with) // we stack onto the new object, and delete the old one
     {
       if(obj->qty == 0) // quantity 0 adds as 1
         obj->qty = 1;
       obj->qty += (stack_with->qty > 0) ? stack_with->qty : 1;
       // FIXME: how to determine max. stack size? (it varies)
       inventory_remove_obj(stack_with);
       delete_obj(stack_with);
     }
   }
   // only objects outside containers are marked in_inventory
   obj->status |= OBJ_STATUS_IN_INVENTORY;
   
   // we have the item now so we don't consider it stealing if we get it at any time in the future.
   obj->status |= OBJ_STATUS_OK_TO_TAKE;
   obj->x = id_n;
 }
 return add_to->addAtPos(0, obj);
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
 obj->qty = qty;
 obj->quality = quality;
 inventory_add_object(obj, NULL, true);

 return(obj);
}


/* Delete `qty' objects of type from inventory (or from a container).
 * Returns the number removed (may be less than requested).
 */
uint32
Actor::inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container)
{
 Obj *obj;
 uint8 oqty = 0;
 uint32 deleted = 0;

 while((obj = inventory_get_object(obj_n, quality, container, true))
       && (deleted < qty))
 {
    oqty = obj->qty ? obj->qty : 1;
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

    case ACTOR_ARM : if(readied_objects[ACTOR_ARM] == NULL)
                       readied_objects[ACTOR_ARM] = obj;
                     else
                      {
                       if(readied_objects[ACTOR_ARM_2] == NULL)
                         readied_objects[ACTOR_ARM_2] = obj;
                       else
                         return false;
                      }
                     break;

    case ACTOR_HAND : if(readied_objects[ACTOR_HAND] == NULL)
                        readied_objects[ACTOR_HAND] = obj;
                      else
                       {
                        if(readied_objects[ACTOR_HAND_2] == NULL)
                          readied_objects[ACTOR_HAND_2] = obj;
                        else
                          return false;
                       }
                      break;

    default : if(readied_objects[location] == NULL)
                readied_objects[location] = obj;
              else
                return false;
              break;
   }

 obj->status |= 0x18; //set object to readied status
 return true;
}

void Actor::remove_readied_object(Obj *obj)
{
 uint8 location;

 for(location=0;location<8;location++)
   {
    if(readied_objects[location] == obj)
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
    readied_objects[location] = NULL;
    obj->status ^= 0x18; // remove "readied" bit flag.
    obj->status |= OBJ_STATUS_IN_INVENTORY; // keep "in inventory"
   }

 return;
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
 
 for(i=0;i<8;i++)
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

/*
 sched_pos = getSchedulePos(clock->get_hour());

 if(sched[sched_pos] != NULL)
    set_worktype(sched[sched_pos]->worktype);
*/


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

 MapCoord sched_dest(sched[sched_pos]->x, sched[sched_pos]->y,
                     sched[sched_pos]->z);
 lwalk(sched_dest);
 set_worktype(sched[sched_pos]->worktype);
 work_location = sched_dest;
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


/* Get pushed by `pusher' to location determined by `where' (with optional
 * coordinates).
 */
bool Actor::push(Actor *pusher, uint8 where, uint16 tx, uint16 ty, uint16 tz)
{
    Actor *player_actor = Game::get_game()->get_player()->get_actor();
    // prevent multiple pushes when move was forced (and didn't check moves)
    if(moves == 0 && pusher != player_actor)
        return(false);
    if(where == ACTOR_PUSH_HERE) // push towards tx,ty,tz (FIXME: should use pusher's coords)
    {
        MapCoord to(tx, ty, tz), from(get_location());
        if(to.distance(from) > 1 || z != tz)
            return(false);
        Actor *other = map->get_actor(tx, ty, tz);
        if(!other || !other->is_visible() || (in_party && other->in_party && this != player_actor))
        {
            // move only if both in party
            face_location(to.x, to.y);
            bool push_ret = move(to.x, to.y, to.z, ACTOR_FORCE_MOVE);
            if(push_ret && moves > 0)
                --moves; // we can't get pushed again this turn
            return(push_ret);
        }
    }
    else if(where == ACTOR_PUSH_ANYWHERE) // go to any neighboring direction
    {
        MapCoord from(get_location());
        uint16 square = 1;
        for(uint16 x=(from.x-square); x<=(from.x+square); x+=square)
            for(uint16 y=(from.y-square); y<=(from.y+square); y+=square)
                if(x != from.x && y != from.y && move(x, y, from.z))
                    return(true);
    }
    else if(where == ACTOR_PUSH_FORWARD)
    {
// FIXME: only move forward, assuming tx,ty is behind actor (or use pusher coords?)
    }
    else if(where == ACTOR_PUSH_TO)
    {
// FIXME: replaces HERE, HERE will use pusher's loc
    }
    return(false);
}


void Actor::defend(uint8 attack, uint8 weapon_damage)
{
 uint8 damage;
 uint8 ac;
   
 if(NUVIE_RAND() % 30 >= (dex - attack) / 2)
   {
    damage = NUVIE_RAND() % weapon_damage;


    if(damage > ac)
      {
       hit(damage - ac);
      }
   }

 return;
}

/* Subtract amount from hp. May die if hp is too low.
 */
void Actor::reduce_hp(uint8 amount)
{
 fprintf(stderr, "hit %s for %d points", get_name(), amount);

    if(amount <= hp) hp -= amount;
    else hp = 0;
// FIX... game specific?
    if(hp == 0)
        die();
}


/* Replace actor object with dead body, complete with matching wardrobe.
 */
void Actor::die()
{
    hp = 0;
    alive = false;
}


/* Get hit and take damage by some indirect effect. (no source)
 */
void Actor::hit(uint8 dmg)
{
 uint8 ac = 0;
 MsgScroll *scroll = Game::get_game()->get_scroll();

 if(armor_class > 0)
  ac = NUVIE_RAND() % armor_class;
     
 if(dmg > ac)
   {
    new HitEffect(this);
    reduce_hp(dmg - ac);
        
    scroll->display_string(get_name());
                         
    if(hp == 0)
      scroll->display_string(" Killed!\n");
    else
      {
       if(hp < 15)
         scroll->display_string(" Critical!\n");
      }

   }
}

