/*
 *  U6UseCode.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Fri May 30 2003.
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
#include <cassert>
#include <cstdio>
#include "nuvieDefs.h"
#include "U6LList.h"
#include "Game.h"
#include "ViewManager.h"
#include "ActorManager.h"
#include "Actor.h"
#include "U6Actor.h"
#include "Party.h"
#include "Player.h"
#include "MsgScroll.h"
#include "Map.h"
#include "GameClock.h"
#include "Book.h"
#include "Event.h"
#include "MapWindow.h"
#include "TimedEvent.h"
#include "U6UseCode.h"

// numbered by entrance quality, "" = no name
static const char *u6_dungeons[21] =
{
    "",
    "Deceit",
    "Despise",
    "Destard",
    "Wrong",
    "Covetous",
    "Shame",
    "Hythloth",
    "GSA",
    "Control",
    "Passion",
    "Diligence",
    "Tomb of Kings",
    "Ant Mound",
    "Swamp Cave",
    "Spider Cave",
    "Cyclops Cave",
    "Heftimus Cave",
    "Heroes' Hole",
    "Pirate Cave",
    "Buccaneer's Cave"
};

// Red moongate teleport locations.
static const struct { uint16 x; uint16 y; uint8 z; } red_moongate_tbl[] = 
{ {0x0,   0x0,   0x0},
  {0x383, 0x1f3, 0x0},
  {0x3a7, 0x106, 0x0},
  {0x1b3, 0x18b, 0x0},
  {0x1f7, 0x166, 0x0},
  {0x93,  0x373, 0x0},
  {0x397, 0x3a6, 0x0},
  {0x44,  0x2d,  0x5},
  {0x133, 0x160, 0x0},
  {0xbc,  0x2d,  0x5},
  {0x9f,  0x3ae, 0x0},
  {0x2e3, 0x2bb, 0x0},
  {0x0,   0x0,   0x0},
  {0x0,   0x0,   0x0},
  {0x0,   0x0,   0x0},
  {0xe3,  0x83,  0x0},
  {0x17,  0x16,  0x1},
  {0x80,  0x56,  0x5},
  {0x6c,  0xdd,  0x5},
  {0x39b, 0x36c, 0x0},
  {0x127, 0x26,  0x0},
  {0x4b,  0x1fb, 0x0},
  {0x147, 0x336, 0x0},
  {0x183, 0x313, 0x0},
  {0x33f, 0xa6,  0x0},
  {0x296, 0x43,  0x0}
 };


U6UseCode::U6UseCode(Configuration *cfg) : UseCode(cfg)
{
    init_objects();
}

U6UseCode::~U6UseCode()
{
    free(uc_objects);
}


/* Create and fill usecode definition list.
 */
void U6UseCode::init_objects()
{
    uc_objects = NULL;
    uc_objects_size = 0; uc_object_count = 0;

// (object,frame,distance to trigger,event(s),function)
    add_usecode(OBJ_U6_OAKEN_DOOR,   255,0,USE_EVENT_USE,&U6UseCode::use_door);
    add_usecode(OBJ_U6_WINDOWED_DOOR,255,0,USE_EVENT_USE,&U6UseCode::use_door);
    add_usecode(OBJ_U6_CEDAR_DOOR,   255,0,USE_EVENT_USE,&U6UseCode::use_door);
    add_usecode(OBJ_U6_STEEL_DOOR,   255,0,USE_EVENT_USE,&U6UseCode::use_door);
    add_usecode(OBJ_U6_KEY,          255,0,USE_EVENT_USE,&U6UseCode::use_key);

    add_usecode(OBJ_U6_SIGN,      255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_BOOK,      255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_SCROLL,    255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_PICTURE,   255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_SIGN_ARROW,255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_TOMBSTONE, 255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_CROSS,     255,0,USE_EVENT_LOOK,&U6UseCode::look_sign);
    add_usecode(OBJ_U6_CODEX,       0,0,USE_EVENT_LOOK,&U6UseCode::look_sign);

    add_usecode(OBJ_U6_CRATE,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_CRATE,      255,0,USE_EVENT_USE,&U6UseCode::use_container);
    add_usecode(OBJ_U6_BARREL,       0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_BARREL,     255,0,USE_EVENT_USE,&U6UseCode::use_container);
    add_usecode(OBJ_U6_CHEST,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_CHEST,      255,0,USE_EVENT_USE,&U6UseCode::use_container);
    add_usecode(OBJ_U6_SECRET_DOOR,255,0,USE_EVENT_USE|USE_EVENT_SEARCH,&U6UseCode::use_secret_door);
    add_usecode(OBJ_U6_BAG,        255,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_DRAWER,     255,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_STONE_LION,   1,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_PLANT,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_GRAVE,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_DEAD_ANIMAL,255,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_DEAD_BODY,  255,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_DEAD_CYCLOPS, 0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_DEAD_GARGOYLE,0,0,USE_EVENT_SEARCH,&U6UseCode::use_container);
    add_usecode(OBJ_U6_REMAINS,    255,0,USE_EVENT_SEARCH,&U6UseCode::use_container);

    add_usecode(OBJ_U6_V_PASSTHROUGH,255,0,USE_EVENT_USE,&U6UseCode::use_passthrough);
    add_usecode(OBJ_U6_H_PASSTHROUGH,255,0,USE_EVENT_USE,&U6UseCode::use_passthrough);

    add_usecode(OBJ_U6_LEVER, 255,0,USE_EVENT_USE,&U6UseCode::use_switch);
    add_usecode(OBJ_U6_SWITCH,255,0,USE_EVENT_USE,&U6UseCode::use_switch);

    add_usecode(OBJ_U6_CHURN, 255,0,USE_EVENT_USE,&U6UseCode::use_churn);

    add_usecode(OBJ_U6_CRANK, 255,0,USE_EVENT_USE,&U6UseCode::use_crank);

    add_usecode(OBJ_U6_FIREPLACE, 255,0,USE_EVENT_USE,&U6UseCode::use_firedevice);
    add_usecode(OBJ_U6_CANDLE,    255,0,USE_EVENT_USE,&U6UseCode::use_firedevice);
    add_usecode(OBJ_U6_CANDELABRA,255,0,USE_EVENT_USE,&U6UseCode::use_firedevice);
    add_usecode(OBJ_U6_BRAZIER,   0,0,USE_EVENT_USE,&U6UseCode::use_firedevice);
    add_usecode(OBJ_U6_BRAZIER,   1,0,USE_EVENT_USE,&U6UseCode::use_firedevice);

    add_usecode(OBJ_U6_ORB_OF_THE_MOONS,255,0,USE_EVENT_USE,&U6UseCode::use_orb);
    add_usecode(OBJ_U6_RED_GATE,  1,0,USE_EVENT_PASS,&U6UseCode::enter_red_moongate); //FIX we only want to go through frame_n 1
    add_usecode(OBJ_U6_LADDER,255,0,USE_EVENT_USE,&U6UseCode::use_ladder);
    add_usecode(OBJ_U6_CAVE,  255,0,USE_EVENT_PASS,&U6UseCode::enter_dungeon);
    add_usecode(OBJ_U6_HOLE,  255,0,USE_EVENT_PASS,&U6UseCode::enter_dungeon);

    add_usecode(OBJ_U6_CLOCK,     255,0,USE_EVENT_LOOK,&U6UseCode::look_clock);
    add_usecode(OBJ_U6_SUNDIAL,   255,0,USE_EVENT_LOOK,&U6UseCode::look_clock);
    add_usecode(OBJ_U6_MIRROR,    255,0,USE_EVENT_LOOK,&U6UseCode::look_mirror);
    add_usecode(OBJ_U6_WELL,      255,0,USE_EVENT_USE,&U6UseCode::use_well);
    
    add_usecode(OBJ_U6_POTION,    255,0,USE_EVENT_USE,&U6UseCode::use_potion);
    add_usecode(OBJ_U6_BUTTER,      0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_WINE,        0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_MEAD,        0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_ALE,         0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_BREAD,       0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_MEAT_PORTION,0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_ROLLS,       0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_CAKE,        0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_CHEESE,      0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_RIBS,        0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_MEAT,        0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_GRAPES,      0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_HAM,         0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_GARLIC,      0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_SNAKE_VENOM, 0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_HORSE_CHOPS, 0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_HONEY,       0,0,USE_EVENT_USE,&U6UseCode::use_food);
    add_usecode(OBJ_U6_DRAGON_EGG,  0,0,USE_EVENT_USE,&U6UseCode::use_food);

    add_usecode(OBJ_U6_COW, 255, 0, USE_EVENT_USE,&U6UseCode::use_cow);
    add_usecode(OBJ_U6_HORSE, 255,0,USE_EVENT_USE,&U6UseCode::use_horse);
    add_usecode(OBJ_U6_HORSE_WITH_RIDER, 255,0,USE_EVENT_USE,&U6UseCode::use_horse);
    
    add_usecode(OBJ_U6_SHIP, 255,0,USE_EVENT_USE,&U6UseCode::use_boat);
    add_usecode(OBJ_U6_SKIFF,255,0,USE_EVENT_USE,&U6UseCode::use_boat);
    add_usecode(OBJ_U6_RAFT,   0,0,USE_EVENT_USE,&U6UseCode::use_boat);
//    add_usecode(OBJ_U6_BALLOON_BASKET,0,0,USE_EVENT_USE,&U6UseCode::use_balloon);
    
    add_usecode(OBJ_U6_QUEST_GATE,  0,0,USE_EVENT_PASS,&U6UseCode::pass_quest_barrier);

    add_usecode(OBJ_U6_VORTEX_CUBE, 0,0,USE_EVENT_USE,&U6UseCode::use_vortex_cube);
    add_usecode(OBJ_U6_PULL_CHAIN,  0,0,USE_EVENT_USE,&U6UseCode::use_bell);
    add_usecode(OBJ_U6_BELL,      255,0,USE_EVENT_USE,&U6UseCode::use_bell);
    add_usecode(OBJ_U6_SHOVEL,      0,0,USE_EVENT_USE,&U6UseCode::use_shovel);
    add_usecode(OBJ_U6_FOUNTAIN,    0,0,USE_EVENT_USE,&U6UseCode::use_fountain);
    add_usecode(OBJ_U6_RUBBER_DUCKY,0,0,USE_EVENT_USE,&U6UseCode::use_rubber_ducky);

    add_usecode(OBJ_U6_PANPIPES,   0,0,USE_EVENT_USE,&U6UseCode::play_instrument);
    add_usecode(OBJ_U6_HARPSICHORD,0,0,USE_EVENT_USE,&U6UseCode::play_instrument);
    add_usecode(OBJ_U6_HARP,       0,0,USE_EVENT_USE,&U6UseCode::play_instrument);
    add_usecode(OBJ_U6_LUTE,       0,0,USE_EVENT_USE,&U6UseCode::play_instrument);
    add_usecode(OBJ_U6_XYLOPHONE,  0,0,USE_EVENT_USE,&U6UseCode::play_instrument);
}


/* Add usecode object to the list.
 */
void U6UseCode::add_usecode(uint16 obj, uint8 frame, uint8 dist, uint8 ev,
                            bool (U6UseCode::*func)(Obj *, uint8))
{
    if((uc_object_count + 1) >= uc_objects_size)
    {
        uc_objects_size += 8;
        uc_objects = (uc_obj *)realloc(uc_objects, uc_objects_size*sizeof(uc_obj));
    }
    uc_objects[uc_object_count++].set(obj, frame, dist, ev, func);
}


/* Is the object a food (or drink) item?
 */
bool U6UseCode::is_food(Obj *obj)
{
    uint16 n = obj->obj_n;
    return(n == OBJ_U6_BUTTER || n == OBJ_U6_CHEESE
           || n == OBJ_U6_WINE || n == OBJ_U6_MEAD || n == OBJ_U6_ALE
           || n == OBJ_U6_BREAD || n == OBJ_U6_ROLLS || n == OBJ_U6_CAKE
           || n == OBJ_U6_MEAT_PORTION || n == OBJ_U6_MEAT || n == OBJ_U6_RIBS
           || n == OBJ_U6_HAM || n == OBJ_U6_HORSE_CHOPS
           || n == OBJ_U6_GRAPES || n == OBJ_U6_HONEY || n == OBJ_U6_GARLIC
           || n == OBJ_U6_SNAKE_VENOM || n == OBJ_U6_DRAGON_EGG);
}


/* Is there USE usecode for an object?
 */
bool U6UseCode::has_usecode(Obj *obj)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_USE);
    if(uc < 0)
        return(false);
    return(true);
}


/* Is there LOOK usecode for an object?
 */
bool U6UseCode::has_lookcode(Obj *obj)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_LOOK);
    if(uc < 0)
        return(false);
    return(true);
}


/* Is there PASS usecode for an object? This doesn't check to see if an object
 * can be passed. (call the usecode for that)
 */
bool U6UseCode::has_passcode(Obj *obj)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_PASS);
    if(uc < 0)
        return(false);
    return(true);
}


/* USE object. Actor is the actor using the object.
 * Returns false if there is no usecode for that object.
 */
bool U6UseCode::use_obj(Obj *obj, Actor *actor)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_USE);
    if(uc < 0)
        return(false);
    set_itemref(actor, actor2_ref);
    return(uc_event(uc, USE_EVENT_USE, obj));
}


/* LOOK at object. Actor is the actor looking at the object.
 * Returns false if there is no usecode for that object.
 */
bool U6UseCode::look_obj(Obj *obj, Actor *actor)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_LOOK);
    if(uc < 0)
        return(false);
    set_itemref(actor);
    return(uc_event(uc, USE_EVENT_LOOK, obj));
}


/* PASS object. Actor is the actor trying to pass the object.
 * Returns false if there is no usecode for that object.
 */
bool U6UseCode::pass_obj(Obj *obj, Actor *actor)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_PASS);
    if(uc < 0)
        return(false);
    set_itemref(actor);
    return(uc_event(uc, USE_EVENT_PASS, obj));
}


/* SEARCH nearby object. Actor is the actor searching.
 * Returns false if there is no usecode for that object.
 */
bool U6UseCode::search_obj(Obj *obj, Actor *actor)
{
    sint16 uc = get_ucobject_index(obj->obj_n, obj->frame_n, USE_EVENT_SEARCH);
    if(uc < 0)
        return(false);
    set_itemref(actor);
    return(uc_event(uc, USE_EVENT_SEARCH, obj));
}


/* Return index of usecode definition in list for object N:F, or -1 if none.
 */
sint16 U6UseCode::get_ucobject_index(uint16 n, uint8 f, uint8 ev)
{
    for(uint32 o = 0; o < uc_object_count; o++)
        if(uc_objects[o].obj_n == n && (uc_objects[o].frame_n == 0xFF
                                        || uc_objects[o].frame_n == f)
           && (uc_objects[o].trigger & ev))
            return(o);
    return(-1);
}


/* Call usecode function `uco' from uc object list, with event `ev', for `obj'.
 * The meaning of the return value is different for each event, but false will
 * also be returned if `uco' isn't a valid index. (check that it is valid first)
 */
bool U6UseCode::uc_event(sint16 uco, uint8 ev, Obj *obj)
{
    if(uco < 0 || uco >= uc_object_count)
        return(false);
//printf("__pfn=%x\n", uc_objects[uco].ucf.__pfn_or_delta2.__pfn);
    if(uc_objects[uco].trigger & ev)
    {
//        printf("Usecode #%02d (%d:%d), event %d (%s)\n", uco, obj->obj_n,
        printf("Object %d:%d, event %s\n", obj->obj_n,
               obj->frame_n, (ev == USE_EVENT_USE) ? "USE"
                             : (ev == USE_EVENT_LOOK) ? "LOOK"
                             : (ev == USE_EVENT_PASS) ? "PASS"
                             : (ev == USE_EVENT_SEARCH) ? "SEARCH"
                             : "???");
        bool ucret = (this->*uc_objects[uco].ucf)(obj, ev);
        int_ref = 0;
        actor_ref = NULL; // clear references
        actor2_ref = NULL;
        obj_ref = NULL;
	mapcoord_ref = NULL;
        return(ucret); // return from usecode function
    }
    return(false); // doesn't respond to event
}


void U6UseCode::lock_door(Obj *obj)
{
    if(is_unlocked_door(obj))
        obj->frame_n += 4;
}


void U6UseCode::unlock_door(Obj *obj)
{
    if(is_locked_door(obj))
        obj->frame_n -= 4;
}


bool U6UseCode::use_door(Obj *obj, uint8 ev)
{
 Obj *key_obj;
 bool print = (actor_ref == player->get_actor());

 if(is_locked_door(obj)) // locked door
   {
    key_obj = player->get_actor()->inventory_get_object(OBJ_U6_KEY, obj->quality);
    if(key_obj != NULL) // we have the key for this door so lets unlock it.
      {
       unlock_door(obj);
       if(print) scroll->display_string("\nunlocked\n");
      }
    else
       if(print) scroll->display_string("\nlocked\n");

    return true;
   }
  
 if(obj->frame_n <= 3) //the door is open
   {
    if(!map->is_passable(obj->x,obj->y,obj->z) || map->actor_at_location(obj->x, obj->y, obj->z)) //don't close door if blocked
      {
       if(print) scroll->display_string("\nNot now!\n");
      }
    else //close the door
      {
       obj->frame_n += 4;
       if(print) scroll->display_string("\nclosed!\n");
      }
   }
 else
   {
    obj->frame_n -= 4;
    if(print) scroll->display_string("\nopened!\n");
   }
   
 return true;
}

bool U6UseCode::use_ladder(Obj *obj, uint8 ev)
{
 Party *party = Game::get_game()->get_party();
 uint16 x = obj->x, y = obj->y;
 uint8 z;
 if(obj->frame_n == 0) // DOWN
   {
    if(obj->z == 0)//handle the transition from the surface to the first dungeon level
      {
       x = (obj->x & 0x07) | (obj->x >> 2 & 0xF8);
       y = (obj->y & 0x07) | (obj->y >> 2 & 0xF8);
      }
    z = obj->z + 1;
   }
 else //UP
   {
    if(obj->z == 1)//we use obj->quality to tell us which surface chunk to come up in.
      {
       x = obj->x / 8 * 8 * 4 + ((obj->quality & 0x03) * 8) + (obj->x - obj->x / 8 * 8);
       y = obj->y / 8 * 8 * 4 + ((obj->quality >> 2 & 0x03) * 8) + (obj->y - obj->y / 8 * 8);
      }
    z = obj->z - 1;
   }
 MapCoord ladder(obj->x, obj->y, obj->z), destination(x, y, z);
 party->walk(&ladder, &destination, 100);
 return true;
}


bool U6UseCode::use_passthrough(Obj *obj, uint8 ev)
{
 uint16 new_x, new_y;
 uint8 new_frame_n;
 char action_string[6]; // either 'Open' or 'Close'
 bool print = (actor_ref == player->get_actor());
 
 new_x = obj->x;
 new_y = obj->y;
 new_frame_n = obj->frame_n;
 
 if(obj->frame_n < 2) // the pass through is currently closed.
  {
   if(obj->obj_n == OBJ_U6_V_PASSTHROUGH)
     new_y--;
   else // OBJ_U6_H_PASSTHROUGH
     new_x--;

   new_frame_n = 2; // open the pass through
   strcpy(action_string,"Open");
  }
 else // the pass through is currently open.
  {
   if(obj->obj_n == OBJ_U6_V_PASSTHROUGH)
     new_y++;
   else // OBJ_U6_H_PASSTHROUGH
     new_x++;

   new_frame_n = 0; // close the pass through
   strcpy(action_string,"Close");
  }

 if(!map->actor_at_location(new_x, new_y, obj->z))
    {
     obj_manager->move(obj, new_x, new_y, obj->z);
     obj->frame_n = new_frame_n;
     if(print)
       {
        scroll->display_string("\n");
        scroll->display_string(action_string);
        scroll->display_string(" the passthrough.\n");
       }
    }
 else if(print)
    scroll->display_string("\nNot now!\n");
  
 return true;
}

// for use with levers and switches, target_obj_n is either OBJ_U6_PORTCULLIS or OBJ_U6_ELECTRIC_FIELD
bool U6UseCode::use_switch(Obj *obj, uint8 ev)
{
 Obj *doorway_obj;
 Obj *portc_obj;
 U6LList *obj_list;
 U6Link *link;
 uint16 target_obj_n = 0;
 char *message = NULL;
 bool print = (actor_ref == player->get_actor());

 if(obj->obj_n == OBJ_U6_LEVER)
 {
     target_obj_n = OBJ_U6_PORTCULLIS;
     message = "\nSwitch the lever, you hear a noise.\n";
 }
 else if(obj->obj_n == OBJ_U6_SWITCH)
 {
     target_obj_n = OBJ_U6_ELECTRIC_FIELD;
     message = "\nOperate the switch, you hear a noise.\n";
 }

 doorway_obj = obj_manager->find_obj(OBJ_U6_DOORWAY, obj->quality, obj->z);
 for(;doorway_obj != NULL;doorway_obj = obj_manager->find_next_obj(doorway_obj))
   {
    obj_list = obj_manager->get_obj_list(doorway_obj->x,doorway_obj->y,doorway_obj->z);

    for(portc_obj=NULL,link=obj_list->start();link != NULL;link=link->next) // find existing portcullis.
     {
      if(((Obj *)link->data)->obj_n == target_obj_n)
        {
         portc_obj = (Obj *)link->data;
         break;
        }
     }

    if(portc_obj == NULL) //no barrier object, so lets create one.
     {
      portc_obj = obj_manager->copy_obj(doorway_obj);
      portc_obj->obj_n = target_obj_n;
      portc_obj->quality = 0;
      if(target_obj_n == OBJ_U6_PORTCULLIS)
        {
         if(portc_obj->frame_n == 9) //FIX Hack for cream buildings might need one for virt wall. 
           portc_obj->frame_n = 1;
        }
      else
         portc_obj->frame_n = 0;
         
      obj_manager->add_obj(portc_obj,true);
     }
    else //delete barrier object.
     {
      obj_list->remove(portc_obj);
      delete portc_obj;
     }
   }
 
 toggle_frame(obj);
 if(print)
   scroll->display_string(message);
 return true;
}


bool U6UseCode::use_firedevice(Obj *obj, uint8 ev)
{
    if(obj->obj_n == OBJ_U6_BRAZIER && obj->frame_n == 2)
        return(false); // holy flames can't be doused
    if(obj->obj_n == OBJ_U6_FIREPLACE)
    {
        if(obj->frame_n == 1 || obj->frame_n == 3)
        {
            use_firedevice_message(obj,false);
            obj->frame_n--;
        }
        else
        {
            use_firedevice_message(obj,true);
            obj->frame_n++;
        }
    }
    else
    {
        toggle_frame(obj);
        use_firedevice_message(obj,(bool)obj->frame_n);
    }
    return(true);
}


bool U6UseCode::use_secret_door(Obj *obj, uint8 ev)
{
    if(ev == USE_EVENT_USE)
    {
        if(obj->frame_n == 1 || obj->frame_n == 3)
            obj->frame_n--;
        else
            obj->frame_n++;
        return(true);
    }
    else if(ev == USE_EVENT_SEARCH)
    {
        scroll->display_string("a secret door");
        if(obj->frame_n == 0)
            obj->frame_n++;
        return(true);
    }
    return(false);
}


/* Use: Open/close container. If container is open, Search.
 * Search: Dump container contents.
 */
bool U6UseCode::use_container(Obj *obj, uint8 ev)
{
    if(ev == USE_EVENT_USE)
    {
        if(obj->obj_n == OBJ_U6_CHEST || obj->obj_n == OBJ_U6_CRATE || obj->obj_n == OBJ_U6_BARREL)
            toggle_frame(obj); //open / close object
        if(obj->frame_n == 0)
        {
            scroll->display_string("\nSearching here, you find ");
            bool found_objects = search_obj(obj, actor_ref);
            scroll->display_string(found_objects ? ".\n" : "nothing.\n");
            return(found_objects);
        }
    }
    else if(ev == USE_EVENT_SEARCH) // search message already printed
    {
        return(UseCode::search_container(obj));
//        if(obj->container && obj->container->end())
//        {
//            new TimedContainerSearch(obj);
//            return(true);
//        }
    }
    return(false);
}


bool U6UseCode::use_vortex_cube(Obj *obj, uint8 ev)
{
    scroll->display_string("\nYou've finished the game!!\nPity we haven't implemented the end sequence yet.\n");
    return(true);
}


/* Use bell or pull-chain, ring and animate nearby bell.
 */
bool U6UseCode::use_bell(Obj *obj, uint8 ev)
{
    Obj *bell = NULL;
    if(ev != USE_EVENT_USE)
        return(false);
    if(obj->obj_n == OBJ_U6_BELL)
        bell = obj;
    else // FIXME: bell isn't always next to chain (Tomb of Kings)
    {
        bell = obj_manager->get_obj_of_type_from_location(OBJ_U6_BELL, obj->x + 1, obj->y, obj->z);
        if(!bell)
            bell = obj_manager->get_obj_of_type_from_location(OBJ_U6_BELL, obj->x - 1, obj->y, obj->z);
    }
    if(bell)
        obj_manager->animate_forwards(bell, 2);
    return(true);
}


//cranks control drawbridges.

bool U6UseCode::use_crank(Obj *obj, uint8 ev)
{
 uint16 x,y;
 uint8 level;
 uint16 b_width;
 bool bridge_open;
 Obj *start_obj;

 start_obj = drawbridge_find(obj);
 
 if(start_obj->frame_n == 3) // bridge open
    bridge_open = true;
 else
    bridge_open = false;

 x = start_obj->x;
 y = start_obj->y;
 level = start_obj->z;
 drawbridge_remove(x, y, level, &b_width);

 // find and animate chain
 if(!(start_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_CHAIN, obj->x+1, obj->y, obj->z)))
   start_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_CHAIN, obj->x-1, obj->y, obj->z);
 if(start_obj)
   obj_manager->animate_forwards(start_obj, 3);

 if(bridge_open)
 {
   obj_manager->animate_backwards(obj, 3); // animate crank
   drawbridge_close(x, y, level, b_width);
 }
 else
 {
   obj_manager->animate_forwards(obj, 3);
   drawbridge_open(x, y, level, b_width);
 }

 return true;
}

Obj *U6UseCode::drawbridge_find(Obj *crank_obj)
{
 uint16 i,j;
 Obj *start_obj, *tmp_obj;
 
 for(i=0;i<6;i++) // search on right side of crank.
  {
   start_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_DRAWBRIDGE, crank_obj->x+1, crank_obj->y+i,  crank_obj->z);
   if(start_obj != NULL) // this means we are using the left crank.
     return start_obj;
  }

 for(i=0;i<6;i++) // search on left side of crank.
  {
   tmp_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_DRAWBRIDGE, crank_obj->x-1, crank_obj->y+i,  crank_obj->z);
   
   if(tmp_obj != NULL) // this means we are using the right crank.
     {
      //find the start of the drawbridge on the left.
      // we do this by searching to the left of the crank till we hit the crank on the otherside.
      // we then move right 1 tile and down 'i' tiles to the start object. :) 
      for(j=1; j < crank_obj->x; j++)
        {
         tmp_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_CRANK, crank_obj->x-j, crank_obj->y,  crank_obj->z);
         if(tmp_obj && tmp_obj->obj_n == OBJ_U6_CRANK)
           {
            start_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_DRAWBRIDGE, tmp_obj->x+1, tmp_obj->y+i,  tmp_obj->z);
            return start_obj;
           }
        }
     }
  }

 return NULL;
}

void U6UseCode::drawbridge_open(uint16 x, uint16 y, uint8 level, uint16 b_width)
{
 uint16 i,j;
 Obj *obj;
 
 y++;
 
 for(i=0;;i++)
  {
   obj = new_obj(OBJ_U6_DRAWBRIDGE,3,x,y+i,level); //left side chain
   obj_manager->add_obj(obj,true);
     
   obj = new_obj(OBJ_U6_DRAWBRIDGE,5,x+b_width-1,y+i,level); //right side chain
   obj_manager->add_obj(obj,true);

   for(j=0;j<b_width-2;j++)
    {     
     obj = new_obj(OBJ_U6_DRAWBRIDGE,4,x+1+j,y+i,level);
     obj_manager->add_obj(obj,true);
    }
    
   if(map->is_passable(x,y+i+1,level)) //we extend the drawbridge until we hit a passable tile.
    break; 
  }
 
 i++;
 
 for(j=0;j<b_width-2;j++) //middle bottom tiles
  {
   obj = new_obj(OBJ_U6_DRAWBRIDGE,1,x+1+j,y+i,level);  
   obj_manager->add_obj(obj,true);
  }
  
 obj = new_obj(OBJ_U6_DRAWBRIDGE,0,x,y+i,level); //bottom left
 obj_manager->add_obj(obj,true);
     
 obj = new_obj(OBJ_U6_DRAWBRIDGE,2,x+b_width-1,y+i,level);  // bottom right
 obj_manager->add_obj(obj,true);

 scroll->display_string("\nOpen the drawbridge.\n");
 
 return;
}

void U6UseCode::drawbridge_close(uint16 x, uint16 y, uint8 level, uint16 b_width)
{
 uint16 i;
 Obj *obj;
 
 y--;
 
 obj = new_obj(OBJ_U6_DRAWBRIDGE,6,x-1,y,level);  //left side
 obj_manager->add_obj(obj,true);
 
 obj = new_obj(OBJ_U6_DRAWBRIDGE,8,x+b_width-1,y,level);  //right side     
 obj_manager->add_obj(obj,true);
 
 for(i=0;i<b_width-1;i++)
  {
   obj = new_obj(OBJ_U6_DRAWBRIDGE,7,x+i,y,level);  //middle        
   obj_manager->add_obj(obj,true);
  }

 scroll->display_string("\nClose the drawbridge.\n");
}

bool U6UseCode::use_orb(Obj *obj, uint8 ev)
{
 Obj *gate;
 uint16 x,y,ox,oy;
 uint8 px,py,z,oz;
 uint8 position;
 Actor *lord_british;
 ActorManager *actor_manager;
 
 player->get_actor()->get_location(&x,&y,&z);
 
 actor_manager = Game::get_game()->get_actor_manager();
 lord_british = actor_manager->get_actor(U6_LORD_BRITISH_ACTOR_NUM);
 
 // The player must ask Lord British about the orb before it can be used.
 // This sets the flag 0x20 in LB's flags field which allows the orb to be used.
 
 if((lord_british->get_flags() & U6_LORD_BRITISH_ORB_CHECK_FLAG) == 0)
   {
    scroll->display_string("\nYou can't figure out how to use it.\n");
    return false;
   }

 if(!mapcoord_ref)
  {
   Game::get_game()->get_event()->freeselect_mode(obj, "Where: ");
   return true;
  }

 ox=mapcoord_ref->x;
 oy=mapcoord_ref->y;
 oz=mapcoord_ref->z;
 
 px=3+ox-x;
 py=2+oy-y;
  
 if( px > 5 || py > 4 ||           // Moongate out of range.
     actor2_ref ||                 // Actor at location.
     !map->is_passable(ox,oy,oz))  // Location not passable.
   {
    scroll->display_string("Failed.\n");
    return false;
   }
  
 position=px+py*5;
  
 if(position >= 12 && position <= 14) // The three middle positions go noware.
    position = 0;
    
 gate=new_obj(OBJ_U6_RED_GATE,1,ox,oy,z);
 gate->quality=position;
 gate->status |= OBJ_STATUS_TEMPORARY;
  
 obj_manager->add_obj(gate,true);
 scroll->display_string("a red moon gate appears.\n");
  
 return true;
}
		  

void U6UseCode::drawbridge_remove(uint16 x, uint16 y, uint8 level, uint16 *bridge_width)
{ 
 uint16 w,h;

 //remove end of closed drawbridge.
 // if present.
 if(x > 0)
   obj_manager->remove_obj_type_from_location(OBJ_U6_DRAWBRIDGE,x-1,y,level);
 
 *bridge_width = 0;
 
 //remove the rest of the bridge.
 for(h=0,w=1;w != 0;h++)
  {
   for(w=0;;w++)
    {
     if(obj_manager->remove_obj_type_from_location(OBJ_U6_DRAWBRIDGE,x+w,y+h,level) == false)
       {
        if(w != 0)
          *bridge_width = w;
        break;
       }
    }
  }

 return;
}


/* Use shovel in one of 8 directions. If used in a dungeon level get a chance of
 * finding gold or a fountain (to make a wish).
 */
bool U6UseCode::use_shovel(Obj *obj, uint8 ev)
{
    Obj *dug_up_obj = NULL;
    MapCoord from = actor_ref->get_location(),
             dig_at;

    if(!mapcoord_ref) // get direction (FIXME: should return relative dir)
    {
        if((obj->status & 0x18) != 0x18)
//        {
//            scroll->display_string("\nNot readied.\n");
//            return(false);
//        }
        scroll->display_string("Telekinesis!\n");
        Game::get_game()->get_event()->useselect_mode(obj, "Direction: ");
        return(true);
    }
    dig_at = *mapcoord_ref;

    // print direction (FIXME: relative dir is what should have been returned)
    sint8 xdir = (dig_at.x == from.x) ? 0 : (dig_at.x < from.x) ? -1 : 1;
    sint8 ydir = (dig_at.y == from.y) ? 0 : (dig_at.y < from.y) ? -1 : 1;
    if(xdir == 0 && ydir == -1) // and also, move direction names somewhere else
        scroll->display_string("North.\n\n");
    else if(xdir == 1 && ydir == -1)
        scroll->display_string("Northeast.\n\n");
    else if(xdir == 1 && ydir == 0)
        scroll->display_string("East.\n\n");
    else if(xdir == 1 && ydir == 1)
        scroll->display_string("Southeast.\n\n");
    else if(xdir == 0 && ydir == 1)
        scroll->display_string("South.\n\n");
    else if(xdir == -1 && ydir == 1)
        scroll->display_string("Southwest.\n\n");
    else if(xdir == -1 && ydir == 0)
        scroll->display_string("West.\n\n");
    else if(xdir == -1 && ydir == -1)
        scroll->display_string("Northwest.\n\n");
    else
    {
        scroll->display_string("nowhere.\n\n");
        return(false); // ??
    }
    // FIX: how are dig-able tiles determined?
    if(!Game::get_game()->get_map_window()->in_dungeon_level())
    {
        scroll->display_string("No effect\n");
        return(false); // ??
    }

    // 10% chance of anything
    if(NUVIE_RAND() % 10)
    {
        scroll->display_string("Failed\n");
        return(false);
    }
    // Door #1 or Door #2?
    scroll->display_string("You dig a hole.\n");
    Obj *fountain = obj_manager->get_obj_of_type_from_location(OBJ_U6_FOUNTAIN,dig_at.x,dig_at.y,dig_at.z);
    if((NUVIE_RAND() % 2) && !fountain)
    {
        scroll->display_string("You find a water fountain.\n");
        dug_up_obj = new_obj(OBJ_U6_FOUNTAIN,0, dig_at.x,dig_at.y,dig_at.z);
    }
    else
    {
        scroll->display_string("You find a gold nugget.\n");
        dug_up_obj = new_obj(OBJ_U6_GOLD_NUGGET,0, dig_at.x,dig_at.y,dig_at.z);
    }
    dug_up_obj->status |= OBJ_STATUS_OK_TO_TAKE;
    obj_manager->add_obj(dug_up_obj, true);
    return(true);
}


/* Magic fountain. Make a wish!
 */
bool U6UseCode::use_fountain(Obj *obj, uint8 ev)
{
    std::string wish("");
#if 0 /* do highlevel MsgScroll stuff first */
    scroll->display_string("Make a wish?\n");
//    get Y/N single char no enter no print
//  Y
    scroll->display_string("Wish for: \n\n");
    // get string
    if(wish != "Food" && wish != "Mutton" && wish != "Wine" && wish != "Fruit"
       && wish != "Meat")
    {
        scroll->display_string("Failed\n");
        return(false);
    }
#endif
    // 25% chance of anything
    if(NUVIE_RAND() % 4)
    {
        scroll->display_string("No effect\n");
        return(false);
    }
    scroll->display_string("You got food");
    // must be able to carry it
    if(!actor_ref->can_carry_object(OBJ_U6_MEAT_PORTION, 1))
    {
        scroll->display_string(", but you can't carry it.\n");
        return(false);
    }
    scroll->display_string(".\n");
    actor_ref->inventory_new_object(OBJ_U6_MEAT_PORTION, 1);
    return(true);
}


bool U6UseCode::use_rubber_ducky(Obj *obj, uint8 ev)
{
    if(actor_ref == player->get_actor())
        scroll->display_string("- Quack! Quack!\n");
    return(true);
}


bool U6UseCode::play_instrument(Obj *obj, uint8 ev)
{
    if(actor_ref == player->get_actor())
    {
        char musicmsg[256];
        snprintf(musicmsg, 256, "You'd be playing %s right now...*if Nuvie had sound effects.\n",
                 obj->obj_n == OBJ_U6_PANPIPES ? "panpipes"
                 : obj->obj_n == OBJ_U6_HARPSICHORD ? "a harpsichord"
                 : obj->obj_n == OBJ_U6_HARP ? "a harp"
                 : obj->obj_n == OBJ_U6_LUTE ? "a lute"
                 : obj->obj_n == OBJ_U6_XYLOPHONE ? "a xylophone"
                 : "a musical instrument");
        scroll->display_string(musicmsg);
    }
    return(true);
}


bool U6UseCode::use_firedevice_message(Obj *obj, bool lit)
{
 if(actor_ref != player->get_actor())
   return true;
 scroll->display_string("\n");
 scroll->display_string(obj_manager->get_obj_name(obj));
 if(lit)
   scroll->display_string(" is lit.\n");
 else
   scroll->display_string(" is doused.\n");

 return true;
}


/* Event: Eat/drink food object.
 */
bool U6UseCode::use_food(Obj *obj, uint8 ev)
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    if(ev == USE_EVENT_USE)
    {
        if(actor_ref == player->get_actor())
        {
            if(obj->obj_n == OBJ_U6_WINE || obj->obj_n == OBJ_U6_MEAD
               || obj->obj_n == OBJ_U6_ALE || obj->obj_n == OBJ_U6_SNAKE_VENOM)
                scroll->display_string("\nYou drink it.\n");
            else // FIXME: if object is alcoholic drink, add to drunkeness
                scroll->display_string("\nYou eat the food.\n");
        }
        if(obj->qty <= 1)
        {
            if(obj->status & OBJ_STATUS_IN_INVENTORY)
                actor_manager->get_actor(obj->x)->inventory_remove_obj(obj);
            else
                obj_manager->remove_obj(obj);
            obj_manager->delete_obj(obj);
        }
        else obj->qty -= 1;
        return(true);
    }
    return(false);
}


/* Event: Use potion. If actor2 is passed, give them the potion, else select
 * actor2.
 */
bool U6UseCode::use_potion(Obj *obj, uint8 ev)
{
    ActorManager *am = Game::get_game()->get_actor_manager();
    if(ev == USE_EVENT_USE)
    {
        if(!actor2_ref && !obj_ref)
            Game::get_game()->get_event()->freeselect_mode(obj, "On whom? ");
        else if(!actor2_ref)
            scroll->display_string("nobody\n");
        else
        {
            Party *party = Game::get_game()->get_party();
            sint8 party_num = party->get_member_num(actor2_ref);
            scroll->display_string(party_num >= 0 ? party->get_actor_name(party_num)
                                   : am->look_actor(actor2_ref));
            scroll->display_string("\n");
            switch(obj->frame_n) // FIXME
            {
                case 0: // BLUE -> ???
                    scroll->display_string("Drink blue potion!\n");
                    break;
                case 1: // RED -> Dispel
                    scroll->display_string("Drink dispel potion!\n");
                    break;
                case 2: // YELLOW -> Heal
                    scroll->display_string("Drink healing potion!\n");
                    break;
                case 3: // GREEN -> Poison
                    scroll->display_string("Drink poison potion!\n");
                    break;
                case 4: // ORANGE -> Sleep
                    scroll->display_string("Drink sleeping potion!\n");
                    break;
                case 5: // PURPLE -> Protect
                    scroll->display_string("Drink protection potion!\n");
                    break;
                case 6: // BLACK -> Invisibility
                    scroll->display_string("Drink invisibility potion!\n");
                    break;
                case 7: // WHITE -> X-Ray
                    scroll->display_string("Drink x-ray vision potion!\n");
                    break;
                default:
                    scroll->display_string("No effect\n");
            }
            if(obj->status & OBJ_STATUS_IN_INVENTORY)
                am->get_actor(obj->x)->inventory_remove_obj(obj);
            else
                obj_manager->remove_obj(obj);
            obj_manager->delete_obj(obj);
        }
        return(true);
    }
    return(false);
}


/* Use a key on itemref (a door).
 * Return true if target object is valid?
 */
bool U6UseCode::use_key(Obj *obj, uint8 ev)
{
    if(ev == USE_EVENT_USE)
    {
        if(!obj_ref && !actor2_ref)
            Game::get_game()->get_event()->useselect_mode(obj, "On ");
        else if(!obj_ref)
            scroll->display_string("nothing\n");
        else
        {
            scroll->display_string(obj_manager->get_obj_name(obj_ref));
            scroll->display_string("\n");
            if(obj_ref->quality == obj->quality && is_closed_door(obj_ref))
            {
                if(is_locked_door(obj_ref))
                {
                    unlock_door(obj_ref);
                    scroll->display_string("\nunlocked!\n");
                }
                else
                {
                    lock_door(obj_ref);
                    scroll->display_string("\nlocked!\n");
                }
            }
            else
                scroll->display_string("No effect\n");
        }
        return(true);
    }
    return(false);
}


/* Enter and exit sea-going vessels.
 */
bool U6UseCode::use_boat(Obj *obj, uint8 ev)
{
 ActorManager *actor_manager = Game::get_game()->get_actor_manager();
 Party *party = Game::get_game()->get_party();
 Actor *ship_actor;
 uint16 lx, ly;
 uint8 lz;
 
 if(ev != USE_EVENT_USE)
    return(false);

 ship_actor = actor_manager->get_actor(0); //get the vehicle actor.
 
 // get out of boat
 if(party->is_in_vehicle()) 
  {
   ship_actor->get_location(&lx,&ly,&lz); //retrieve actor position for land check.
   
   if(use_boat_find_land(&lx,&ly,&lz)) //we must be next to land to disembark
     {
      Obj *obj = ship_actor->make_obj();

      party->show();
      ship_actor->hide();
      player->set_actor(party->get_actor(0));
      player->move(lx,ly,lz);
      ship_actor->obj_n = OBJ_U6_NO_VEHICLE;
      ship_actor->frame_n = 0;
      ship_actor->init();
      ship_actor->move(0,0,0,ACTOR_FORCE_MOVE);
      obj_manager->add_obj(obj);
     }
   else
     {
      scroll->display_string("\nOnly next to land.\n");
      return false;
     }
   
   party->set_in_vehicle(false);
     
   return true;
  }
  
 
 if(obj->quality != 0)    //deed check
   {
    if(party->has_obj(OBJ_U6_SHIP_DEED, obj->quality) == false)
      {
       scroll->display_string("\nA deed is required.\n");
       return false;
      }
   }

 if(obj->obj_n == OBJ_U6_SHIP) //If we are using a ship we need to use its center object.
   {
    obj = use_boat_find_center(obj); //return the center object
    if(obj == NULL)
     {
      scroll->display_string("\nShip not usable\n");
      return false;
     }
   }

 // walk to vehicle if necessary
 if(!party->is_at(obj->x, obj->y, obj->z))
    party->enter_vehicle(obj);
 else
   {
    ship_actor->init_from_obj(obj);
    ship_actor->show(); // Swift!
    obj_manager->remove_obj(obj);
    obj_manager->delete_obj(obj);

    party->hide(); // set in-vehicle
    player->set_actor(ship_actor);
    party->set_in_vehicle(true);
   }
 return(true);
}

inline Obj *U6UseCode::use_boat_find_center(Obj *obj)
{
 Obj *new_obj;
 uint16 x, y;
 uint8 ship_direction = (obj->frame_n % 8) / 2; //find the direction based on the frame_n 
 
 if(obj->frame_n >= 8 && obj->frame_n < 16) //center obj
  return obj;

 x = obj->x;
 y = obj->y;
 
 if(obj->frame_n < 8) //front obj
   {
    switch(ship_direction)
      {
       case ACTOR_DIR_U : y++; break;
       case ACTOR_DIR_R : x--; break;
       case ACTOR_DIR_D : y--; break;
       case ACTOR_DIR_L : x++; break;
      } 
   }
 else
   {
    if(obj->frame_n >= 16 && obj->frame_n < 24) //back obj
      {
       switch(ship_direction)
         {
          case ACTOR_DIR_U : y--; break;
          case ACTOR_DIR_R : x++; break;
          case ACTOR_DIR_D : y++; break;
          case ACTOR_DIR_L : x--; break;
         } 
      }
   }

 new_obj = obj_manager->get_objBasedAt(x,y,obj->z,true);
 
 if(new_obj != NULL && new_obj->obj_n == OBJ_U6_SHIP)
   return new_obj;

 return NULL;
}

inline bool U6UseCode::use_boat_find_land(uint16 *x, uint16 *y, uint8 *z)
{
 if(map->is_passable(*x, *y-1, *z))  //UP
   {
    *y = *y - 1;
    return true;
   }
 if(map->is_passable(*x+1, *y, *z)) //RIGHT
   {
    *x = *x + 1;
    return true;
   }
 if(map->is_passable(*x, *y+1, *z)) //DOWN
   {
    *y = *y + 1;
    return true;
   }
 if(map->is_passable(*x-1, *y, *z)) //LEFT
   {
    *x = *x - 1;
    return true;
   }

 return false;
}

/* using a cow fills an empty bucket in the player's inventory with milk */
bool U6UseCode::use_cow(Obj *obj, uint8 ev)
{
 if(ev != USE_EVENT_USE)
    return(false);
 
 return fill_bucket(OBJ_U6_BUCKET_OF_MILK);
}

/* using a well fills an empty bucket in the player's inventory with water */
bool U6UseCode::use_well(Obj *obj, uint8 ev)
{
 if(ev != USE_EVENT_USE)
    return(false);
 
 return fill_bucket(OBJ_U6_BUCKET_OF_WATER);
}

bool U6UseCode::fill_bucket(uint16 filled_bucket_obj_n)
{
 Player *player = Game::get_game()->get_player();
 Actor *player_actor;
 Obj *bucket;

 player_actor = player->get_actor();
 
 if(!player_actor->inventory_has_object(OBJ_U6_BUCKET))
   {
    if(player_actor->inventory_has_object(OBJ_U6_BUCKET_OF_MILK) || 
       player_actor->inventory_has_object(OBJ_U6_BUCKET_OF_WATER) )
      {
       scroll->display_string("\nYou need an empty bucket.\n");
       return false;
      }
    else
      {
       scroll->display_string("\nYou need a bucket.\n");
       return false;
      }
   }
 
 // Fill the first empty bucket in player's inventory.
 
 bucket = player_actor->inventory_get_object(OBJ_U6_BUCKET);
 player_actor->inventory_remove_obj(bucket);
 
 bucket->obj_n = filled_bucket_obj_n;
 
 player_actor->inventory_add_object(bucket);
 
 scroll->display_string("\nDone\n");
 
 return true;
}

bool U6UseCode::use_churn(Obj *obj, uint8 ev)
{
 ViewManager *view_manager = Game::get_game()->get_view_manager();
 Player *player = Game::get_game()->get_player();
 Actor *player_actor;
 Obj *bucket;
 Obj *butter;
 
 player_actor = player->get_actor();

 if(!player_actor->inventory_has_object(OBJ_U6_BUCKET_OF_MILK))
   {
    scroll->display_string("\nYou need some milk.\n");
	return false;
   }

 bucket = player_actor->inventory_get_object(OBJ_U6_BUCKET_OF_MILK);
 player_actor->inventory_remove_obj(bucket);
 
 bucket->obj_n = OBJ_U6_BUCKET;
 
 butter = new Obj();
 
 butter->obj_n = OBJ_U6_BUTTER;
 player_actor->inventory_add_object(butter);
 player_actor->inventory_add_object(bucket);
 
 view_manager->set_inventory_mode();
 
 view_manager->update(); //FIX this should be moved higher up in UseCode
 
 scroll->display_string("\nDone\n");
 return true;
}

bool U6UseCode::use_horse(Obj *obj, uint8 ev)
{
 ActorManager *actor_manager = Game::get_game()->get_actor_manager();
 Actor *actor, *player_actor;
 Obj *actor_obj;

 if(ev != USE_EVENT_USE)
    return(false);
 
 actor = actor_manager->get_actor(obj->quality); // horse or horse with rider
 if(!actor)
   return false;

 player_actor = actor_ref;
 if(player_actor->get_actor_num() == U6_SHERRY_ACTOR_NUM)
   {
    scroll->display_string("Sherry says: \"Eeek!!! I'm afraid of horses!\"\n");
    return false;
   }
 else if(player_actor->get_actor_num() == U6_BEHLEM_ACTOR_NUM)
   {
    scroll->display_string("BehLem says: \"Horses are for food!\"\n");
    return false;
   }
 else if(obj->obj_n == OBJ_U6_HORSE && player_actor->obj_n == OBJ_U6_HORSE_WITH_RIDER)
   {
    scroll->display_string("You're already on a horse!\n");
    return false;
   }
 
 actor_obj = actor->make_obj();
	 
 //dismount from horse. revert to original actor type.
 //Add a temporary horse actor onto the map.
 if(obj->obj_n == OBJ_U6_HORSE_WITH_RIDER)
   {
    actor->clear();
	
    actor_obj->obj_n = actor->base_obj_n; //revert to normal actor type
    actor_obj->frame_n = actor->old_frame_n;

	actor->init_from_obj(actor_obj);

    // create a temporary horse on the map.
	actor_manager->create_temp_actor(OBJ_U6_HORSE, obj->x, obj->y, obj->z, WORKTYPE_U6_ANIMAL_WANDER);
   }
 else if(!actor_manager->is_temp_actor(actor))// Try to mount horse. Don't use permenant Actors eg Smith, push-me pull-you
   {
    scroll->display_string("\nHorse not boardable!\n");
   }
 else // mount up.
   {
    actor_manager->clear_actor(actor); //clear the temp horse actor from the map.

    actor_obj->obj_n = OBJ_U6_HORSE_WITH_RIDER;
    
	player_actor->move(actor_obj->x,actor_obj->y,actor_obj->z); //this will center the map window
    player_actor->init_from_obj(actor_obj);
	
	delete actor_obj;
   }

 return true;
}


/* Pass: Allow normal move if player's Quest Flag is set.
 */
bool U6UseCode::pass_quest_barrier(Obj *obj, uint8 ev)
{
    if(ev == USE_EVENT_PASS)
        if(player->get_quest_flag() == 0)
        {
            // block everyone, only print message when player attempts to pass
            if(actor_ref == player->get_actor())
                scroll->message("\n\"Thou art not upon a Sacred Quest!\n"
                                "Passage denied!\"\n\n");
            return(false);
        }
    return(true);
}


/* Event: Look
 * True: Nothing special. Allow search.
 * False: Displayed book data for object. Disallow search.
 */
bool U6UseCode::look_sign(Obj *obj, uint8 ev)
{
    char *data;
    Book *book = Game::get_game()->get_event()->get_book(); // ??

    if(ev == USE_EVENT_LOOK && obj->quality != 0)
    {
        // read
        if(actor_ref == player->get_actor())
        {
            scroll->display_string(":\n\n");
            if((data = book->get_book_data(obj->quality - 1)))
            {
             // FIX Any alternate-font text is in < >, Runic is capitalized,
             // Gargish is lower-case. Translations follow untranslated text and
             // are wrapped in & &.
             if(data[0] == '<' && data[strlen(data)-1] == '>') //Britannian text is wrapped in '<' '>' chars
                {
                 scroll->display_string(&data[1],strlen(data)-2, 1); // 1 for britannian font.
                 scroll->display_string("\n",1);
                }
             else
                {
                 scroll->display_string(data,strlen(data),0);
                 scroll->display_string("\n",0);
                }
             free(data);
            }

        }
        return(false);
    }
    return(true);
}


/* Event: Look
 * Display the current time.
 */
bool U6UseCode::look_clock(Obj *obj, uint8 ev)
{
    GameClock *clock = Game::get_game()->get_clock();
    if(obj->obj_n == OBJ_U6_SUNDIAL
       && (clock->get_hour() < 5 || clock->get_hour() > 19))
        return(true); // don't get time from sundial at night
    if(ev == USE_EVENT_LOOK && actor_ref == player->get_actor())
    {
        scroll->display_string("\nThe time is ");
        scroll->display_string(clock->get_time_string());
    }
    return(true);
}


/* test (need to determine use of true/false return)
 */
bool U6UseCode::look_mirror(Obj *obj, uint8 ev)
{
    ViewManager *view_manager = Game::get_game()->get_view_manager();
    if(ev == USE_EVENT_LOOK && actor_ref == player->get_actor())
    {
        uint16 x, y;
        uint8 z;
        actor_ref->get_location(&x, &y, &z);
        if(x == obj->x && y > obj->y && y <= (obj->y + 2))
        {
            scroll->display_string("\nYou can see yourself!");
            view_manager->set_portrait_mode(actor_ref, NULL);
        }
        return(true);
    }
    return(false);
}


/* if not in party mode, say that you cannot enter and do normal move
 * else walk all party members to cave, give dungeon name, and move to dungeon
 */
bool U6UseCode::enter_dungeon(Obj *obj, uint8 ev)
{
    Party *party = Game::get_game()->get_party();
    char *prefix = "", *dungeon_name = "";
    uint16 x = obj->x, y = obj->y;
    uint8 z = obj->z;

    if(obj->quality < 21)
        dungeon_name = (char *)u6_dungeons[obj->quality];
    if(obj->quality >= 1 && obj->quality <= 7)
        prefix = "dungeon ";
    else if(obj->quality >= 9 && obj->quality <= 11)
        prefix = "shrine of ";
    else
        prefix = "";

    // don't activate if autowalking from linking exit
    if(ev == USE_EVENT_PASS && actor_ref == player->get_actor() && !party->get_autowalk())
    {
        if(obj->quality != 0/* && Shamino is in party and alive*/)
        {
            // scroll->printf("%s says, \"This is the %s%s.\"\n\n", prefix, dungeon_name);
            scroll->display_string("Shamino says, \"This is the ");
            scroll->display_string(prefix);
            scroll->display_string(dungeon_name);
            scroll->display_string(".\"\n\n");
            scroll->display_prompt();
        }
        MapCoord entrance(x, y, z);
        // going down
        if(z == 0) // from surface, do superchunk translation
        {
            x = (x & 0x07) | (x >> 2 & 0xF8);
            y = (y & 0x07) | (y >> 2 & 0xF8);
        }
        if(z < 5)
          z += 1;
        else
          z -= 1;

        MapCoord exit(x, y, z);
        if(obj->obj_n == OBJ_U6_HOLE) // fall down hole faster
            party->walk(&entrance, &exit, 100);
        else
            party->walk(&entrance, &exit);
        return(true);
    }
    else if(ev == USE_EVENT_PASS && party->get_autowalk()) // party can use now
        if(party->contains_actor(actor_ref))
            return(true);
    return(false);
}

/* if not in party mode, say that you cannot enter and do normal move
 * else walk all party members to moongate and teleport.
 */
bool U6UseCode::enter_red_moongate(Obj *obj, uint8 ev)
{
    Party *party = Game::get_game()->get_party();
    uint16 x = obj->x, y = obj->y;
    uint8 z = obj->z;
    if (obj->frame_n != 1) return false; // FIXME is this check needed?

    // don't activate if autowalking from linking exit
    if(ev == USE_EVENT_PASS && actor_ref == player->get_actor() && !party->get_autowalk())
    {
        MapCoord entrance(x, y, z);
        if(obj->quality > 25)
           {
            printf("Error: invalid moongate destination %d\n",obj->quality);
            return false;
           }

        if((obj->quality > 0 && obj->quality < 12) ||
           (obj->quality > 14 && obj->quality < 26) ) //positions 0, 12, 13 and 14 go noware.
           {
            x = red_moongate_tbl[obj->quality].x; // set our moongate destination from the lookup table.
            y = red_moongate_tbl[obj->quality].y;
            z = red_moongate_tbl[obj->quality].z;
           }
                      
        MapCoord exit(x, y, z);
        party->walk(&entrance, &exit);
        return(true);
    }
    else if(ev == USE_EVENT_PASS && party->get_autowalk()) // party can use now
        if(party->contains_actor(actor_ref))
            return(true);
    return(false);
}

