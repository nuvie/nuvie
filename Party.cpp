/*
 *  Party.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Mar 23 2003.
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

#include "nuvieDefs.h"

#include "U6misc.h"
#include "NuvieIO.h"
#include "Game.h"
#include "Converse.h"
#include "TimedEvent.h"
#include "Configuration.h"
#include "ActorManager.h"
#include "SoundManager.h"
#include "Player.h"
#include "Map.h"
#include "U6UseCode.h"
#include "Party.h"

Party::Party(Configuration *cfg)
{
 config = cfg;
 map = NULL;


 formation = PARTY_FORM_STANDARD;
 num_in_party = 0;

}

Party::~Party()
{
}

bool Party::init(Game *g, ActorManager *am)
{
 std::string formation_string;

 game = g;
 actor_manager = am;
 map = g->get_game_map();

 autowalk=false;
 in_vehicle = false;

 config->value("config/party_formation", formation_string, "");
 if(formation_string == "row")
   formation = PARTY_FORM_ROW;
 else if(formation_string == "column")
   formation = PARTY_FORM_COLUMN;
 else if(formation_string == "delta")
   formation = PARTY_FORM_DELTA;
 else if(formation_string == "phalanx")
   formation = PARTY_FORM_PHALANX;
 else
   formation = PARTY_FORM_STANDARD;

 return true;
}

bool Party::load(NuvieIO *objlist)
{
 uint8 actor_num;
 uint16 i;

 autowalk = false;
 in_vehicle = false;

 objlist->seek(0xff0);
 num_in_party = objlist->read1();


 objlist->seek(0xf00);
 for(i=0;i<num_in_party;i++)
  {
   objlist->readToBuf((unsigned char *)member[i].name,14); // read in Player name.
  }
 objlist->seek(0xfe0);
 for(i=0;i<num_in_party;i++)
  {
   actor_num = objlist->read1();
   member[i].actor = actor_manager->get_actor(actor_num);
   member[i].actor->set_in_party(true);
  }

 objlist->seek(0x1c12); // combat mode flag
 in_combat_mode = (bool)objlist->read1();

 reform_party();

 autowalk=false;
 // this may not be the proper way to get in_vehicle at start, but we havn't
 // found the relevant data in objlist yet (maybe only vehicle worktype?)
 MapCoord vehicle_loc = actor_manager->get_actor(0)->get_location();
 if(is_at(vehicle_loc.x, vehicle_loc.y, vehicle_loc.z))
  {
   set_in_vehicle(true);
   hide();
  }

 update_music();

 return true;
}

bool Party::save(NuvieIO *objlist)
{
 uint16 i;

 objlist->seek(0xff0);
 objlist->write1(num_in_party);


 objlist->seek(0xf00);
 for(i=0;i<num_in_party;i++)
  {
   objlist->writeBuf((unsigned char *)member[i].name,14);
  }

 objlist->seek(0xfe0);
 for(i=0;i<num_in_party;i++)
  {
   objlist->write1(member[i].actor->get_actor_num());
  }

 objlist->seek(0x1c12); // combat mode flag
 objlist->write1((uint8)in_combat_mode);

 return true;
}

bool Party::add_actor(Actor *actor)
{
 Converse *converse = game->get_converse();

 if(num_in_party < 16)
   {
    actor->set_in_party(true);
    member[num_in_party].actor = actor;
    strncpy(member[num_in_party].name, converse->npc_name(actor->id_n), 14);
    member[num_in_party].name[13] = '\0';
    member[num_in_party].combat_position = 0;
    member[num_in_party].leader_x = member[0].actor->get_location().x;
    member[num_in_party].leader_y = member[0].actor->get_location().y;
    num_in_party++;

    reform_party();
    return true;
   }

 return false;
}


// remove actor from member array shuffle remaining actors down if required.
bool Party::remove_actor(Actor *actor)
{
 uint8 i;

 for(i=0;i< num_in_party;i++)
  {
   if(member[i].actor->id_n == actor->id_n)
     {
      member[i].actor->set_in_party(false);
      if(i != (num_in_party - 1))
        {
         for(;i+1<num_in_party;i++)
           member[i] = member[i+1];
        }
      num_in_party--;

      reform_party();
      return true;
     }
  }

 return false;
}

void Party::split_gold()
{
}

void Party::gather_gold()
{
}

uint8 Party::get_party_size()
{
 return num_in_party;
}

Actor *Party::get_actor(uint8 member_num)
{
 if(num_in_party <= member_num)
   return NULL;

 return member[member_num].actor;
}

char *Party::get_actor_name(uint8 member_num)
{
 if(num_in_party <= member_num)
   return NULL;

 return member[member_num].name;
}


/* Returns position of actor in party or -1.
 */
sint8 Party::get_member_num(Actor *actor)
{
    for(int i=0; i < num_in_party; i++)
    {
        if(member[i].actor->id_n == actor->id_n)
            return(i);
    }
    return(-1);
}

sint8 Party::get_member_num(uint8 a)
{
 return(get_member_num(actor_manager->get_actor(a)));
}

uint8 Party::get_actor_num(uint8 member_num)
{
 if(num_in_party <= member_num)
   return 0; // hmm how should we handle this error.

 return member[member_num].actor->id_n;
}

/* Rearrange member slot positions based on the number of members and the
 * selected formation. Used only when adding or removing actors.
 */
void Party::reform_party()
{
    uint32 n;
    sint32 x, y, max_x;
    bool even_row;
    member[0].form_x = 0; member[0].form_y = 0;
    switch(formation)
    {
        case PARTY_FORM_COLUMN: // line up behind Avatar
            x = 0; y = 1;
            for(n = 1; n < num_in_party; n++)
            {
                member[n].form_x = x;
                member[n].form_y = y++;
                if(y == 5)
                {
                    x += 1;
                    y = 0;
                }
            }
            break;
        case PARTY_FORM_ROW: // line up left of Avatar
            x = -1; y = 0;
            for(n = 1; n < num_in_party; n++)
            {
                member[n].form_x = x--;
                member[n].form_y = y;
                if(x == -5)
                {
                    y += 1;
                    x = 0;
                }
            }
            break;
        case PARTY_FORM_PHALANX: // surround Avatar in close formation
            /*203
               1*/
            if(num_in_party <= 4)
            {
                member[1].form_x = 0; member[1].form_y = 1;
                member[2].form_x = -1; member[2].form_y = 0;
                member[3].form_x = 1; member[3].form_y = 0;
            }
            /*102
              354*/
            else if(num_in_party <= 6)
            {
                member[1].form_x = -1; member[1].form_y = 0;
                member[2].form_x = 1; member[2].form_y = 0;
                member[3].form_x = -1; member[3].form_y = 1;
                member[4].form_x = 1; member[4].form_y = 1;
                member[5].form_x = 0; member[5].form_y = 1;
            }
            /*1 2
              304
              576*/
            else if(num_in_party <= 8)
            {
                member[1].form_x = -1; member[1].form_y = -1;
                member[2].form_x = 1; member[2].form_y = -1;
                member[3].form_x = -1; member[3].form_y = 0;
                member[4].form_x = 1; member[4].form_y = 0;
                member[5].form_x = -1; member[5].form_y = 1;
                member[6].form_x = 1; member[6].form_y = 1;
                member[7].form_x = 0; member[7].form_y = 1;
            }
            else
            {
                member[1].form_x = -1; member[1].form_y = 0;
                member[2].form_x = 1; member[2].form_y = 0;
                member[3].form_x = -1; member[3].form_y = 1;
                member[4].form_x = 1; member[4].form_y = 1;
                member[5].form_x = 0; member[5].form_y = 1;
                member[6].form_x = -1; member[6].form_y = 2;
                member[7].form_x = 1; member[7].form_y = 2;
                member[8].form_x = 0; member[8].form_y = 2;
                x = -2; y = 1;
                for(n = 9; n < num_in_party; n++)
                {
                    member[n].form_x = x; member[n].form_y = y;
                    if(n <= 12) // alternate the first 4 extras on the sides
                    {
                        if(x == -2)
                            x = 2;
                        else
                        {
                            x = -2;
                            y += 1;
                        }
                    }
                    else // line the rest in rows behind the group (alternate columns)
                    {
                        if(x == -2)
                            x = 2;
                        else if(x == 2)
                            x = -1;
                        else if(x == -1)
                            x = 1;
                        else if(x == 1)
                            x = 0;
                        else
                        {
                            x = -2;
                            y += 1;
                        }
                    }
                }
            }
            break;
        case PARTY_FORM_DELTA: // open triangle formation with Avatar at front
            x = -1; y = 1;
            for(n = 1; n < num_in_party; n++)
            {
                member[n].form_x = x;
                member[n].form_y = y;
                // alternate X once, then move down
                x = -x;
                if(x == 0 || x < 0)
                {
                    x -= 1;
                    ++y;
                }
                if(y == 5) // start at top of triangle
                {
                    y -= ((-x) - 1);
                    x = 0;
                }
            }
            break;
//        case PARTY_FORM_DIAMOND: // like DELTA but... a diamond
//            break;
//        case PARTY_FORM_CROWD: // random positions
//            break;
//        case PARTY_FORM_COMBAT: // positions determined by COMBAT mode
//            break;
        case PARTY_FORM_STANDARD: // U6
        default:
            // put first follower behind or behind and to the left of Avatar
            member[1].form_x = (num_in_party >= 3) ? -1 : 0;
            member[1].form_y = 1;
            x = y = 1;
            even_row = false;
            for(n = 2, max_x = 1; n < num_in_party; n++)
            {
                member[n].form_x = x;
                member[n].form_y = y;
                // alternate & move X left/right by 2
                x = (x == 0) ? x - 2 : (x < 0) ? -x : -x - 2;
                if(x > max_x || (x < 0 && -x > max_x)) // reached row max.
                {
                    ++y; even_row = !even_row; // next row
                    ++max_x; // increase row max.
                    x = even_row ? 0 : -1; // next guy starts at center or left by 2
                }
            }
    }
}


/* Get map location of a party member.
 */
inline MapCoord Party::get_location(uint8 m)
{
    return(member[m].actor->get_location());
}


/* Returns absolute location where party member `m' SHOULD be (based on party
 * formation and leader location.
 */
MapCoord Party::get_formation_coords(uint8 m)
{
    MapCoord a(member[m].actor->get_location()); // my location
    MapCoord l(member[0].actor->get_location()); // leader location
    uint8 ldir = member[0].actor->get_direction(); // leader direciton
    // intended location
    return(MapCoord((ldir == ACTOR_DIR_U) ? l.x + member[m].form_x : // X
                    (ldir == ACTOR_DIR_R) ? l.x - member[m].form_y :
                    (ldir == ACTOR_DIR_D) ? l.x - member[m].form_x :
                    (ldir == ACTOR_DIR_L) ? l.x + member[m].form_y : a.x,
                    (ldir == ACTOR_DIR_U) ? l.y + member[m].form_y : // Y
                    (ldir == ACTOR_DIR_R) ? l.y + member[m].form_x :
                    (ldir == ACTOR_DIR_D) ? l.y - member[m].form_y :
                    (ldir == ACTOR_DIR_L) ? l.y - member[m].form_x : a.y,
                    a.z // Z
                   ));
}


/* Update the actual locations of the party actors on the map, so that they are
 * in the proper formation.
 */
void Party::follow()
{
    LineTestResult lt;
    MapCoord leader = member[0].actor->get_location();

    for(uint32 m = 1; m < num_in_party; m++)
    {
        if(!member[m].actor->is_visible()) // hidden, no point walking there
        {
            member[m].actor->stop_walking(); // in case we were searching
            member[m].actor->move(leader.x, leader.y, leader.z, ACTOR_FORCE_MOVE);
            continue;
        }
        MapCoord here = member[m].actor->get_location();
        MapCoord target = get_formation_coords(m); // target in formation
        MapCoord dest = target; // where we decide to move to
        bool target_can_see_leader = !map->lineTest(target.x,target.y,leader.x,leader.y,leader.z,LT_HitUnpassable,lt);
        //bool member_can_see_target = !map->lineTest(here.x,here.y,target.x,target.y,leader.z,LT_HitUnpassable,lt);
        if(target_can_see_leader && here == target) // Do nothing if already there
        {
            member[m].leader_x = leader.x; // update known location of leader
            member[m].leader_y = leader.y;
            member[m].actor->stop_walking(); // in case we were searching
            continue; // next
        }
        // use assumed leader location
        find_leader(m);
        MapCoord follow(member[m].leader_x, member[m].leader_y, leader.z);
        // If there is no clear line-of-site from target position to the leader,
        // the two positions are probably in different rooms, and we prefer the
        // room with the leader.
        if(!target_can_see_leader)
        {
            // Add to speed if a few spaces from target and leader
            // Add more if offscreen or at last known leader location and leader
            // isn't there
            uint32 speed = (here.distance(follow) > 2 && here.distance(dest) > 1) ? 2 : 1;
            if(here.distance(follow) > 5 || (follow != leader && here.x == follow.x && here.y == follow.y))
                speed += (here.distance(follow)/4);
            member[m].actor->swalk(follow, dest, speed);
        }
        else
        {
            // Add to speed if a few spaces from target
            // Add more if offscreen
            uint32 speed = here.distance(dest) > 1 ? 2 : 1;
            if(here.distance(follow) > 5)
                speed += (here.distance(follow)/4);
            member[m].actor->swalk(dest, leader, speed);
        }
    }
}


/* Update known location of leader for party member `m'. This should be done
 * when any party member has moved to help following.
 */
void Party::find_leader(uint8 m)
{
    MapCoord to = member[0].actor->get_location(), from = member[m].actor->get_location();
    MapCoord follow = to;
    LineTestResult lt;

    // can/should I go straight to leader?
    // (at last known position, or leader is visible, or leader is too far away)
    if(!map->lineTest(from.x, from.y, to.x, to.y, to.z, LT_HitUnpassable, lt)
       || from.distance(to) > 6 || (from.x == member[m].leader_x && from.y == member[m].leader_y))
    {
        member[m].leader_x = to.x;
        member[m].leader_y = to.y;
        return;
    }
#if 0 // FIXME: this doesn't work as well as I wanted, and U6 didn't do it
    // determine person to follow
    assert(num_in_party);
    for(uint32 n = num_in_party - 1; n > 0; n--)
    {
        if(n == m) continue;
        follow = member[n].actor->get_location();
        // they can see leader AND I can see them
        if(!map->lineTest(follow.x,follow.y,to.x,to.y,to.z,LT_HitUnpassable,lt)
           && !map->lineTest(from.x,from.y,follow.x,follow.y,to.z,LT_HitUnpassable,lt))
        {
            member[m].leader_x = follow.x;
            member[m].leader_y = follow.y;
            return;
        }
    }
#endif
}


bool Party::has_obj(uint16 obj_n, uint8 quality)
{
 uint16 i;

 for(i=0;i<num_in_party;i++)
  {
   if(member[i].actor->inventory_get_object(obj_n, quality) != NULL) // we got a match
     return true;
  }

 return false;
}


uint16 Party::who_has_obj(uint16 obj_n, uint8 quality)
{
    uint16 i;
    for(i = 0; i < num_in_party; i++)
    {
        if(member[i].actor->inventory_get_object(obj_n, quality) != NULL)
            return(member[i].actor->get_actor_num());
    }
    return(0);
}


/* Is everyone in the party at the coordinates?
 */
bool Party::is_at(uint16 x, uint16 y, uint8 z, uint32 threshold)
{
    for(uint32 p = 0; p < num_in_party; p++)
    {
        MapCoord loc(x, y, z);
        if(!member[p].actor->is_nearby(loc, threshold))
            return(false);
    }
    return(true);
}

bool Party::is_at(MapCoord &xyz, uint32 threshold)
{
 return(is_at(xyz.x,xyz.y,xyz.z,threshold));
}

bool Party::contains_actor(Actor *actor)
{
 if(get_member_num(actor) >= 0)
    return(true);

 return(false);
}

bool Party::contains_actor(uint8 actor_num)
{
 return(contains_actor(actor_manager->get_actor(actor_num)));
}

void Party::set_in_combat_mode(bool value)
{
  // You can't enter combat mode while in a vehicle.
  if(value && in_vehicle)
     return;

  in_combat_mode = value;

  update_music();
}

void Party::update_music()
{
 SoundManager *s = Game::get_game()->get_sound_manager();
 MapCoord pos;

 if(in_vehicle)
   {
    s->musicPlayFrom("boat");
    return;
   }
 else if(in_combat_mode)
   {
    s->musicPlayFrom("combat");
    return;
   }

 pos = get_location(0); // FIX we need to get the position of the party leader.

 switch(pos.z)
  {
   case 0 : s->musicPlayFrom("random"); break;
   case 5 : s->musicPlayFrom("gargoyle"); break;
   default : s->musicPlayFrom("dungeon"); break;
  }

 return;
}

void Party::show()
{
 uint16 i;

 for(i=0;i<num_in_party;i++)
  {
   member[i].actor->show();
  }

 return;
}

void Party::hide()
{
 uint16 i;

 for(i=0;i<num_in_party;i++)
  {
   member[i].actor->hide();
  }

 return;
}

/* Move and center everyone in the party to one location.
 */
bool Party::move(uint16 dx, uint16 dy, uint8 dz)
{
    for(sint32 m = 0; m < num_in_party; m++)
        if(!member[m].actor->move(dx, dy, dz, ACTOR_FORCE_MOVE))
            return(false);
    return(true);
}


/* Automatically walk (timed) to a destination, and then teleport to new
 * location (optional). Used to enter/exit dungeons.
 * (step_delay 0 = default speed)
 */
void Party::walk(MapCoord *walkto, MapCoord *teleport, uint32 step_delay)
{
    if(step_delay)
        new TimedPartyMove(walkto, teleport, step_delay);
    else
        new TimedPartyMove(walkto, teleport);

    game->pause_world(); // other actors won't move
    game->pause_user(); // don't allow input
    // view will snap back to player after everyone has moved
    game->get_player()->set_mapwindow_centered(false);
    autowalk = true;
}


/* Enter a moongate and teleport to a new location.
 * (step_delay 0 = default speed)
 */
void Party::walk(Obj *moongate, MapCoord *teleport, uint32 step_delay)
{
    MapCoord walkto(moongate->x, moongate->y, moongate->z);
    if(step_delay)
        new TimedPartyMove(&walkto, teleport, moongate, step_delay);
    else
        new TimedPartyMove(&walkto, teleport, moongate);

    game->pause_world(); // other actors won't move
    game->pause_user(); // don't allow input
    // view will snap back to player after everyone has moved
    game->get_player()->set_mapwindow_centered(false);
    autowalk = true;
}



/* Automatically walk (timed) to vehicle. (step_delay 0 = default speed)
 */
void Party::enter_vehicle(Obj *ship_obj, uint32 step_delay)
{
    MapCoord walkto(ship_obj->x, ship_obj->y, ship_obj->z);

    dismount_from_horses();

    if(step_delay)
        new TimedPartyMoveToVehicle(&walkto, ship_obj, step_delay);
    else
        new TimedPartyMoveToVehicle(&walkto, ship_obj);

    game->pause_world(); // other actors won't move
    game->pause_user(); // don't allow input
    // view will snap back to player after everyone has moved
    game->get_player()->set_mapwindow_centered(false);
    autowalk = true;
}

void Party::set_in_vehicle(bool value)
{
 in_vehicle = value;

 update_music();

 return;
}

/* Done automatically walking, return view to player character.
 */
void Party::stop_walking()
{
    game->get_player()->set_mapwindow_centered(true);
    game->unpause_all(); // allow user input, unfreeze actors
    autowalk = false;
    update_music();
}

void Party::dismount_from_horses()
{
 UseCode *usecode = Game::get_game()->get_usecode();

 for(uint32 m = 0; m < num_in_party; m++)
   {
    if(member[m].actor->obj_n == OBJ_U6_HORSE_WITH_RIDER)
      {
       Obj *my_obj = member[m].actor->make_obj();
       usecode->use_obj(my_obj, member[m].actor);
       delete my_obj;
      }
   }

 return;
}
