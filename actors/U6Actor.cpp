/*
 *  U6Actor.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jul 27 2003.
 *  Copyright (c) Nuvie Team 2003. All rights reserved.
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
#include "U6UseCode.h"
#include "PathFinder.h"
#include "U6Actor.h"

//static const uint8 sleep_objects[8];

// A list of readiable objects and their readied location.

static const struct {uint16 obj_n; uint8 readiable_location;}
 readiable_objects[] = {
{OBJ_U6_LEATHER_HELM, ACTOR_HEAD },
{OBJ_U6_CHAIN_COIF, ACTOR_HEAD },
{OBJ_U6_IRON_HELM, ACTOR_HEAD },
{OBJ_U6_SPIKED_HELM, ACTOR_HEAD },
{OBJ_U6_WINGED_HELM, ACTOR_HEAD },
{OBJ_U6_BRASS_HELM, ACTOR_HEAD },
{OBJ_U6_GARGOYLE_HELM, ACTOR_HEAD },
{OBJ_U6_MAGIC_HELM, ACTOR_HEAD },
{OBJ_U6_WOODEN_SHIELD, ACTOR_ARM },
{OBJ_U6_CURVED_HEATER, ACTOR_ARM },
{OBJ_U6_WINGED_SHIELD, ACTOR_ARM },
{OBJ_U6_KITE_SHIELD, ACTOR_ARM },
{OBJ_U6_SPIKED_SHIELD, ACTOR_ARM },
{OBJ_U6_BLACK_SHIELD, ACTOR_ARM },
{OBJ_U6_DOOR_SHIELD, ACTOR_ARM },
{OBJ_U6_MAGIC_SHIELD, ACTOR_ARM },
{OBJ_U6_CLOTH_ARMOUR, ACTOR_BODY },
{OBJ_U6_LEATHER_ARMOR, ACTOR_BODY },
{OBJ_U6_RING_MAIL, ACTOR_BODY },
{OBJ_U6_SCALE_MAIL, ACTOR_BODY },
{OBJ_U6_CHAIN_MAIL, ACTOR_BODY },
{OBJ_U6_PLATE_MAIL, ACTOR_BODY },
{OBJ_U6_MAGIC_ARMOUR, ACTOR_BODY },
{OBJ_U6_SPIKED_COLLAR, ACTOR_BODY },
{OBJ_U6_GUILD_BELT, ACTOR_BODY },
{OBJ_U6_GARGOYLE_BELT, ACTOR_BODY },
{OBJ_U6_LEATHER_BOOTS, ACTOR_FOOT },
{OBJ_U6_SWAMP_BOOTS, ACTOR_FOOT },

{OBJ_U6_SLING, ACTOR_ARM },
{OBJ_U6_CLUB, ACTOR_ARM },
{OBJ_U6_MAIN_GAUCHE, ACTOR_ARM },
{OBJ_U6_SPEAR, ACTOR_ARM },
{OBJ_U6_THROWING_AXE, ACTOR_ARM },
{OBJ_U6_DAGGER, ACTOR_ARM },
{OBJ_U6_MACE, ACTOR_ARM },
{OBJ_U6_MORNING_STAR, ACTOR_ARM },
{OBJ_U6_BOW, ACTOR_ARM },
{OBJ_U6_CROSSBOW, ACTOR_ARM },
{OBJ_U6_SWORD, ACTOR_ARM },
{OBJ_U6_TWO_HANDED_HAMMER, ACTOR_ARM },
{OBJ_U6_TWO_HANDED_AXE, ACTOR_ARM },
{OBJ_U6_TWO_HANDED_SWORD, ACTOR_ARM },
{OBJ_U6_HALBERD, ACTOR_ARM },
{OBJ_U6_GLASS_SWORD, ACTOR_ARM },
{OBJ_U6_BOOMERANG, ACTOR_ARM },
{OBJ_U6_TRIPLE_CROSSBOW, ACTOR_ARM },

{OBJ_U6_MAGIC_BOW, ACTOR_ARM },
{OBJ_U6_SPELLBOOK, ACTOR_ARM },

{OBJ_U6_ANKH_AMULET, ACTOR_NECK },
{OBJ_U6_SNAKE_AMULET, ACTOR_NECK },
{OBJ_U6_AMULET_OF_SUBMISSION, ACTOR_NECK },

{OBJ_U6_STAFF, ACTOR_ARM },
{OBJ_U6_LIGHTNING_WAND, ACTOR_ARM },
{OBJ_U6_FIRE_WAND, ACTOR_ARM },
{OBJ_U6_STORM_CLOAK, ACTOR_BODY },
{OBJ_U6_RING, ACTOR_HAND },
{OBJ_U6_FLASK_OF_OIL, ACTOR_ARM },

{OBJ_U6_TORCH, ACTOR_ARM },

{OBJ_U6_SCYTHE, ACTOR_ARM },
{OBJ_U6_PITCHFORK, ACTOR_ARM },
{OBJ_U6_RAKE, ACTOR_ARM },
{OBJ_U6_PICK, ACTOR_ARM },
{OBJ_U6_SHOVEL, ACTOR_ARM },
{OBJ_U6_HOE, ACTOR_ARM },

{OBJ_U6_CLEAVER, ACTOR_ARM },
{OBJ_U6_KNIFE, ACTOR_ARM },

{OBJ_U6_LUTE, ACTOR_ARM },

{OBJ_U6_PLIERS, ACTOR_ARM },
{OBJ_U6_HAMMER, ACTOR_ARM },

{OBJ_U6_PROTECTION_RING, ACTOR_HAND },
{OBJ_U6_REGENERATION_RING, ACTOR_HAND },
{OBJ_U6_INVISIBILITY_RING, ACTOR_HAND },

{OBJ_U6_NOTHING, ACTOR_NOT_READIABLE} }; // this last element terminates the array.


U6Actor::U6Actor(Map *m, ObjManager *om, GameClock *c): Actor(m,om,c)
{
 
}

U6Actor::~U6Actor()
{
}

void U6Actor::update()
{
 Actor::update();
 preform_worktype();
 return;
}

bool U6Actor::updateSchedule()
{
 bool ret;
 if((ret = Actor::updateSchedule()) == true) //walk to next schedule location if required.
   {
    if(sched[sched_pos] != NULL && (sched[sched_pos]->x != x || sched[sched_pos]->y != y || sched[sched_pos]->z != z))
       set_worktype(WORKTYPE_U6_WALK_TO_LOCATION);
   }

 return ret;
}

bool U6Actor::move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move)
{
 bool ret = Actor::move(new_x,new_y,new_z,force_move);
 
 if(ret == true)
  {
   Obj *obj = obj_manager->get_obj(new_x,new_y,new_z); // Ouch, we get obj in Actor::move() too :(
   if(obj)
     {
      if(obj->obj_n == OBJ_U6_CHAIR)  // make the actor sit on a chair.
       {
        frame_n = (obj->frame_n * 4) + 3;
        direction = obj->frame_n;
        can_twitch = false;
       }
       
      if(obj->obj_n == OBJ_U6_THRONE  && obj->frame_n == 3) //make actor sit on LB's throne.
       {
        frame_n = 8 + 3; //sitting facing south.
        direction = 2;
        can_twitch = false;
       }
     }
  }

 return ret;
}

uint8 U6Actor::get_object_readiable_location(uint16 obj_n)
{
 uint16 i;
 
 for(i=0;readiable_objects[i].obj_n != OBJ_U6_NOTHING;i++)
   {
    if(obj_n == readiable_objects[i].obj_n)
      return readiable_objects[i].readiable_location;
   }

 return ACTOR_NOT_READIABLE;
}

void U6Actor::twitch()
{
 // twitch
 switch(obj_n)
  {
   case OBJ_U6_MUSICIAN : if(NUVIE_RAND()%3 == 1)
                            {
                             walk_frame = NUVIE_RAND()%2;
                             frame_n = direction * 2 + walk_frame;
                            }
                           break;
   case OBJ_U6_JESTER : if(can_twitch && NUVIE_RAND()%5 == 1) //jester's move around a lot!
                          {
                           walk_frame = NUVIE_RAND()%3;
                           frame_n = direction * 4 + walk_frame;
                          }
                        break;

   default : if(can_twitch && NUVIE_RAND()%50 == 1)
               {
                walk_frame = NUVIE_RAND()%3;
                frame_n = direction * 4 + walk_frame;
               }
             break;
  }

 return;
}

void U6Actor::preform_worktype()
{
 uint8 walk_frame_tbl[4] = {0,1,2,1};

 switch(worktype)
  {
   case WORKTYPE_U6_FACE_NORTH :
   case WORKTYPE_U6_FACE_EAST  :
   case WORKTYPE_U6_FACE_SOUTH :
   case WORKTYPE_U6_FACE_WEST  :
     break;
   case WORKTYPE_U6_IN_PARTY :
   case WORKTYPE_U6_WALK_TO_LOCATION : wt_walk_to_location();
                                      break;
                                      

   case WORKTYPE_U6_WALK_NORTH_SOUTH :
   case WORKTYPE_U6_WALK_EAST_WEST   : wt_walk_straight(); break;

   case WORKTYPE_U6_WORK :
   case WORKTYPE_U6_WANDER_AROUND   : wt_wander_around(); break;
   case WORKTYPE_U6_BEG : wt_beg(); break;
//   case WORKTYPE_U6_
//                     break;
  }

 return;
}

void U6Actor::set_worktype(uint8 new_worktype)
{
 if(new_worktype == worktype)
   return;

 if(worktype == WORKTYPE_U6_SLEEP || worktype == WORKTYPE_U6_PLAY_LUTE)
   { 
    obj_n = old_obj_n;
    frame_n = old_frame_n;
   }
 Actor::set_worktype(new_worktype);

 //FIX from here.

 switch(worktype)
  {
   case WORKTYPE_U6_FACE_NORTH : set_direction(ACTOR_DIR_U); break;
   case WORKTYPE_U6_FACE_EAST  : set_direction(ACTOR_DIR_R); break;
   case WORKTYPE_U6_FACE_SOUTH : set_direction(ACTOR_DIR_D); break;
   case WORKTYPE_U6_FACE_WEST  : set_direction(ACTOR_DIR_L); break;

   case WORKTYPE_U6_SLEEP : wt_sleep(); break;
   case WORKTYPE_U6_PLAY_LUTE : wt_play_lute(); break;
  }
}


void U6Actor::wt_walk_to_location()
{
 if(pathfinder && pathfinder->reached_goal() && sched[sched_pos] != NULL)
    set_worktype(sched[sched_pos]->worktype);
 return;
}


void U6Actor::wt_walk_straight()
{
 uint8 dir = get_direction();
 set_direction(dir); //update walk frame FIX this!
 if(worktype == WORKTYPE_U6_WALK_NORTH_SOUTH)
   {
    if(dir != ACTOR_DIR_U && dir != ACTOR_DIR_D)
      dir = ACTOR_DIR_D;
    if(dir == ACTOR_DIR_U) // move up if blocked face down
       {
        if(moveRelative(0,-1) == false)
          set_direction(ACTOR_DIR_D);
       }
    else // move down if blocked face up
       {
        if(moveRelative(0,1) == false)
          set_direction(ACTOR_DIR_U);
       }
   }
 else //WORKTYPE_U6_WALK_EAST_WEST
   {
    if(dir != ACTOR_DIR_L && dir != ACTOR_DIR_R)
      dir = ACTOR_DIR_R;
    if(dir == ACTOR_DIR_L) //move left if blocked face right
       {
        if(moveRelative(-1,0) == false)
          set_direction(ACTOR_DIR_R);
       }
    else  //move right if blocked face left
       {
        if(moveRelative(1,0) == false)
          set_direction(ACTOR_DIR_L);
       }
   }
}

void U6Actor::wt_wander_around()
{
 uint8 new_direction;
 
 if(NUVIE_RAND()%8 == 1)
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

 return;
}


/* Wander around, approach and talk to the player character if visible.
 */
void U6Actor::wt_beg()
{
    static uint8 mode = 0;// 0 = waiting for target, 1 = following,
    Player *player = Game::get_game()->get_player();          // 2 = just loiter
    Actor *actor = player->get_actor();

    if(mode == 0)
        if(is_nearby(actor)) // look for victi... er, player
            mode = 1;
    if(mode == 1)
    {
        mode = is_nearby(actor) ? 1 : 0; // still visible?
        Party *party = player->get_party();
        MapCoord me(x,y,z), them(0,0,0);
        for(uint32 p = 0; p < party->get_party_size(); p++)
        {
            party->get_actor(p)->get_location(&them.x, &them.y, &them.z);
            if(me.xdistance(them) <= 1 && me.ydistance(them) <= 1
               && z == them.z)
            {
                // talk to me :)
                stop_walking();
                Game::get_game()->get_converse()->start(this);
                mode = 2;
                return; // done
            }
        }
        // get closer
        actor->get_location(&them.x, &them.y, &them.z);
        swalk(them, 1);
    }
    else
        wt_wander_around();
}


void U6Actor::wt_sleep()
{ 
 Obj *obj = obj_manager->get_obj(x,y,z);
 if(obj)
   {
    if(obj->obj_n == OBJ_U6_BED)
      {
       if(obj->frame_n == 1 || obj->frame_n == 5) //horizontal bed
         {
          old_obj_n = obj_n;
          old_frame_n = frame_n;
          obj_n = OBJ_U6_PERSON_SLEEPING;
          frame_n = 0;
         }
       if(obj->frame_n == 7 || obj->frame_n == 10) //vertical bed
         {
          old_obj_n = obj_n;
          old_frame_n = frame_n;
          obj_n = OBJ_U6_PERSON_SLEEPING;
          frame_n = 1;
         }
      }
   }

 can_twitch = false;

 return;
}

void U6Actor::wt_play_lute()
{
 old_obj_n = obj_n;
 old_frame_n = frame_n;
 
 obj_n = OBJ_U6_MUSICIAN;
 frame_n = 4;
 
 return;
}
