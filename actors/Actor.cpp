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
#include <cstdlib>
#include "Game.h"
#include "Map.h"
#include "ObjManager.h"
#include "U6UseCode.h"
#include "LPath.h"
#include "ZPath.h"
#include "Actor.h"

static uint8 walk_frame_tbl[4] = {0,1,2,1};


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
 can_twitch = true;
 in_party = false;
 worktype = 0;
 sched_pos = 0;
 
 memset(readied_objects,0,sizeof(readied_objects));
 
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

bool Actor::is_alive()
{
 return alive;
}


/* Returns true if another NPC `n' is in proximity to this one.
 */
bool Actor::is_nearby(Actor *other)
{
    MapCoord here(x,y,z), where(0,0,0);
    other->get_location(&where.x, &where.y, &where.z);
    if(here.xdistance(where) < 5 && here.ydistance(where) < 5 && z == where.z)
        return(true);
    return(false);
}


bool Actor::is_nearby(uint8 a)
{
    return(is_nearby(Game::get_game()->get_actor_manager()->get_actor(a)));
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
    if(rel_x == 0 && rel_y == 0) // nowhere
        return;
    if(rel_x == 0) // up,down
        set_direction(rel_y < 0 ? ACTOR_DIR_U : ACTOR_DIR_D);
    else if(rel_y == 0) // left,right
        set_direction(rel_x < 0 ? ACTOR_DIR_L : ACTOR_DIR_R);
    else if(rel_x < 0 && rel_y < 0) // up-left
    {
        if(direction != ACTOR_DIR_U && direction != ACTOR_DIR_L)
            set_direction(direction + 2);
        else
            set_direction(direction);
    }
    else if(rel_x > 0 && rel_y < 0) // up-right
    {
        if(direction != ACTOR_DIR_U && direction != ACTOR_DIR_R)
            set_direction(direction + 2);
        else
            set_direction(direction);
    }
    else if(rel_x < 0 && rel_y > 0) // down-left
    {
        if(direction != ACTOR_DIR_D && direction != ACTOR_DIR_L)
            set_direction(direction + 2);
        else
            set_direction(direction);
    }
    else if(rel_x > 0 && rel_y > 0) // down-right
    {
        if(direction != ACTOR_DIR_D && direction != ACTOR_DIR_R)
            set_direction(direction + 2);
        else
            set_direction(direction);
    }
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


bool Actor::moveRelative(sint16 rel_x, sint16 rel_y)
{
 return move(x + rel_x, y + rel_y, z);
}


bool Actor::check_move(sint16 new_x, sint16 new_y, sint8 new_z, bool ignore_actors)
{
    if(z > 5)
        return(false);

    uint16 pitch = map->get_width(new_z);
    if(new_x < 0 || new_x >= pitch)
        return(false);
    if(new_y < 0 || new_y >= pitch)
        return(false);

    if(map->is_passable(new_x,new_y,new_z) == false)
        return(false);

    if(!ignore_actors && map->get_actor(new_x,new_y,new_z))
        return(false);
    return(true);
}


bool Actor::move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move)
{
 // blocking actors are checked for later
 if(!force_move && !check_move(new_x, new_y, new_z, ACTOR_IGNORE_OTHERS))
    return false;

 // usecode must allow movement
 usecode = obj_manager->get_usecode();
 Obj *obj = obj_manager->get_obj(new_x,new_y,new_z);
 if(obj && usecode->can_pass(obj))
  {
    if(!usecode->pass_obj(obj, this)) // calling item is this actor
       return(false);
  }


 // switch position with party members (FIXME: move to own method?)
 Actor *other = map->get_actor(new_x, new_y, new_z);
 if(!force_move && other) // not forcing move, check actor
 {
     Party *party = Game::get_game()->get_party();
     // don't switch with party leader or actors who have moved (unless leader)
     if(!party->contains_actor(other) || (party->get_member_num(other) == 0)
        || (other->moved && (party->get_member_num(this) != 0)))
        return false; // blocked by actor
     other->face_location(x, y);
     other->move(x, y, z, ACTOR_FORCE_MOVE);
 }

 // move
 x = new_x;
 y = new_y;
 z = new_z;

 can_twitch = true;
  
 // post-move
/*
 if(obj)
  {
   if(obj->obj_n == OBJ_U6_CHAIR)  // make the actor sit on a chair.
     frame_n = (obj->frame_n * 4) + 3;
  }
*/
 // re-center map if actor is player character
 Game *game = Game::get_game();
 if(id_n == game->get_player()->get_actor()->id_n && !game->get_player()->get_uncontrolled())
    game->get_map_window()->centerMapOnActor(this);
 moved = true;
 return true;
}


void Actor::update()
{
 moved = false;
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
 updateSchedule();
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
}

U6LList *Actor::get_inventory_list()
{
 return obj_manager->get_actor_inventory(id_n); 
}


bool Actor::inventory_has_object(uint16 obj_n, uint8 qual)
{
    if(inventory_get_object(obj_n, qual))
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
            inventory_count_object(obj_n, qual, obj);
        if(obj->obj_n == obj_n && obj->quality == qual)
            qty += obj->qty;
    }
    return(qty);
}


/* Returns object descriptor of object in the actor's inventory, or NULL if no
 * matching object is found.
 */
Obj *Actor::inventory_get_object(uint16 obj_n, uint8 qual, Obj *container)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 
 inventory = container ? container->container
                       : get_inventory_list();
 for(link=inventory->start();link != NULL;link=link->next)
   {
    obj = (Obj *)link->data;
    if(obj->obj_n == obj_n && obj->quality == qual)
      return(obj);
    else if(obj->container)
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

bool Actor::inventory_add_object(Obj *obj)
{
 U6LList *inventory = get_inventory_list();
 return inventory->addAtPos(0, obj);
}
 
bool Actor::inventory_add_object(uint16 obj_n, uint32 qty, uint8 quality)
{
 U6LList *inventory = 0;
 U6Link *link = 0;
 Obj *obj = 0;
 uint32 origqty = 0, newqty = 0;
 
 inventory = get_inventory_list();

 // find same type of object in inventory, don't look in containers
 for(link = inventory->start(); link != NULL; link = link->next)
 {
   obj = (Obj *)link->data;
   if(obj->obj_n == obj_n && obj->quality == quality)
     break;
   obj = 0;
 }
 // define and link (when stacking, quantity of 0 is 1)
 origqty = (obj && obj->qty) ? obj->qty : 1;
 newqty = qty ? qty : 1;
 if(obj && ((origqty + newqty) <= 255))
 {
    origqty += newqty;
    obj->qty = origqty;
 }
 else // don't stack
   while(newqty)
   {
     obj = new Obj;
     obj->obj_n = obj_n;
     obj->quality = quality;
     obj->qty = newqty <= 255 ? newqty : 255;
     inventory->addAtPos(0, obj);
     newqty -= obj->qty;
   }
 return true;
}


uint32
Actor::inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 uint8 oqty = 0;
 uint32 deleted = 0;
 
 inventory = container ? container->container : get_inventory_list();
 for(link = inventory->start(); deleted < qty && link != NULL; link = link->next)
 {
    obj = (Obj *)link->data;
    if(obj->container)
        deleted += inventory_del_object(obj_n, qty - deleted, quality, obj);
    if((deleted < qty) && (obj->obj_n == obj_n) && (obj->quality == quality))
    {
        oqty = obj->qty ? obj->qty : 1;
        if(oqty <= (qty - deleted))
            inventory->remove(obj);
        else
            obj->qty = oqty - qty;
        deleted += oqty;
    }
 }
 return(deleted);
}

bool Actor::inventory_remove_obj(Obj *obj)
{
 U6LList *inventory;

 inventory = get_inventory_list();
 return inventory->remove(obj);
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

 weight /= 10;
 
 return weight;
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

 weight /= 10;
 
 return weight;
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

    sched[i]->hour = sched_data_ptr[0];
    sched[i]->worktype = sched_data_ptr[1];
    
    sched[i]->x = sched_data_ptr[2];
    sched[i]->x += (sched_data_ptr[3] & 0x3) << 8; 
    
    sched[i]->y = (sched_data_ptr[3] & 0xfc) >> 2;
    sched[i]->y += (sched_data_ptr[4] & 0xf) << 6;
    
    sched[i]->z = (sched_data_ptr[4] & 0xf0) >> 4;
    sched_data_ptr += 5;
#ifdef DEBUG
    printf("#%04d %03x,%03x,%x hour %2d worktype %02x\n",id_n,sched[i]->x,sched[i]->y,sched[i]->z,sched[i]->hour,sched[i]->worktype);
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

bool Actor::updateSchedule()
{
 uint8 hour;
 uint16 new_pos;
 
 hour = clock->get_hour();
 
 new_pos = getSchedulePos(hour);
 
 if(new_pos == sched_pos) // schedules are the same so we do nothing.
  return false;

 sched_pos = new_pos;
 
 if(sched[sched_pos] == NULL)
   return false;
 
 MapCoord sched_dest(sched[sched_pos]->x, sched[sched_pos]->y,
                     sched[sched_pos]->z);
 lwalk(sched_dest);
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

void Actor::set_worktype(uint8 new_worktype)
{
 worktype = new_worktype;
 
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

