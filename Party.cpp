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
#include "NuvieIOFile.h"
#include "Game.h"
#include "Converse.h"
#include "TimedEvent.h"

#include "ActorManager.h"
#include "Player.h"
#include "Map.h"

#include "Party.h"

Party::Party(Configuration *cfg)
{
 config = cfg;
 num_in_party = 0;
 formation = PARTY_FORM_STANDARD;
}

Party::~Party()
{
}

bool Party::init(Game *g, ActorManager *am)
{
 std::string filename;

 game = g;
 actor_manager = am;
 
 loadParty();
 
 return true;
}

bool Party::add_actor(Actor *actor)
{
 Converse *converse = game->get_converse();

 if(num_in_party < 16)
   {
    member[num_in_party].actor = actor;
    strncpy(member[num_in_party].name, converse->npc_name(actor->id_n), 14);
    member[num_in_party].name[13] = '\0';
    member[num_in_party].combat_position = 0;
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

bool Party::loadParty()
{
 std::string filename;
 NuvieIOFileRead objlist;
 uint8 actor_num;
 uint16 i;
 
 config_get_path(config,"savegame/objlist",filename);
 if(objlist.open(filename.c_str()) == false)
   return false;

 objlist.seek(0xff0);
 num_in_party = objlist.read1();
 
 
 objlist.seek(0xf00);
 for(i=0;i<num_in_party;i++)
  {
   objlist.readToBuf((unsigned char *)member[i].name,14); // read in Player name.
  }
 objlist.seek(0xfe0);  
 for(i=0;i<num_in_party;i++)
  {
   actor_num = objlist.read1();
   member[i].actor = actor_manager->get_actor(actor_num);
  }
 reform_party();
 
 return true;
}


/* Rearrange member slot positions based on the number of members and the
 * selected formation. Used only when adding or removing actors.
 */
void Party::reform_party()
{
    uint32 n;
    member[0].form_x = 0; member[0].form_y = 0;
    switch(formation)
    {
        case PARTY_FORM_COLUMN:
            for(n = 1; n < num_in_party; n++)
            {   member[n].form_x = 0; member[n].form_y = n; }
            break;
        case PARTY_FORM_ROW:
            for(n = 1; n < num_in_party; n++)
            {   member[n].form_x = -n; member[n].form_y = 0; }
            break;
        case PARTY_FORM_PHALANX: // FIXME: need to move party first
            member[1].form_x = 0; member[1].form_y = -1;
            member[2].form_x = -1; member[2].form_y = 0;
            member[3].form_x = 1; member[3].form_y = 0;
            member[4].form_x = -1; member[4].form_y = -1;
            member[5].form_x = 1; member[5].form_y = -1;
            member[6].form_x = -1; member[6].form_y = 1;
            member[7].form_x = 1; member[7].form_y = 1;
            for(n = 8; n < num_in_party; n++) // the rest "tail"
            {   member[n].form_x = 0; member[n].form_y = (n-8)+2; }
            break;
        case PARTY_FORM_DELTA:
        case PARTY_FORM_STANDARD:
        default:
            if(num_in_party >= 3)
                member[1].form_x = -1;
            else
                member[1].form_x = 0;
            member[1].form_y = 1;
            member[2].form_x = 1; member[2].form_y = 1;
            member[3].form_x = 0; member[3].form_y = 2;
            member[4].form_x = -2; member[4].form_y = 2;
            member[5].form_x = 2; member[5].form_y = 2;
            member[6].form_x = -1; member[6].form_y = 3;
            member[7].form_x = 1; member[7].form_y = 3;
    }
}


/* Update the actual locations of the party actors on the map, so that they are
 * in the proper formation.
 * If the new position is blocked or more than one space away, path-find to it.
 */
void Party::follow()
{
    bool quickmove = true;
    uint8 lz, az, dir = member[0].actor->get_direction();
    uint16 lx, ly, ax, ay; // leader & actor coordinates
    member[0].actor->get_location(&lx, &ly, &lz);

    for(uint32 m = 1; m < num_in_party; m++)
    {
        member[m].actor->get_location(&ax, &ay, &az);
        uint16 dx, dy; // destination
        dx = (dir == ACTOR_DIR_U) ? lx + member[m].form_x :
             (dir == ACTOR_DIR_R) ? lx - member[m].form_y :
             (dir == ACTOR_DIR_D) ? lx - member[m].form_x :
             (dir == ACTOR_DIR_L) ? lx + member[m].form_y : ax;
        dy = (dir == ACTOR_DIR_U) ? ly + member[m].form_y :
             (dir == ACTOR_DIR_R) ? ly + member[m].form_x :
             (dir == ACTOR_DIR_D) ? ly - member[m].form_y :
             (dir == ACTOR_DIR_L) ? ly - member[m].form_x : ay;
        if(ax == dx && ay == dy) // already there
        {
            member[m].actor->stop_walking();
            continue;
        }
        sint16 xdiff = dx - ax;
        sint16 ydiff = dy - ay;
        uint8 xdist = abs(xdiff);
        uint8 ydist = abs(ydiff);
        // quick move
        if(az != lz)
        {
            member[m].actor->stop_walking();
            member[m].actor->set_direction(xdiff, ydiff);
            member[m].actor->move(lx, ly, lz, ACTOR_FORCE_MOVE);
            continue;
        }
        else if(quickmove && xdist <= 1 && ydist <= 1)
        {
            member[m].actor->stop_walking();
            member[m].actor->set_direction(xdiff, ydiff);
            member[m].actor->moveRelative(xdiff < 0 ? -1 : xdiff > 0 ? 1 : 0,
                                             ydiff < 0 ? -1 : ydiff > 0 ? 1 : 0);
            if(ax == dx && ay == dy)
                continue;
        }
        // make everyone else path-find to stay synchronized
        quickmove = false;
        // walk there (delay based on distance, if leader moved towards party)
        MapCoord dest(dx, dy, lz), leader_dest(lx, ly, lz);
        uint8 dist = xdist > ydist ? xdist : ydist;
        uint8 delay = ((dir == ACTOR_DIR_L && dx < ax) // leader moved away
                       || (dir == ACTOR_DIR_D && dy > ay)
                       || (dir == ACTOR_DIR_R && dx > ax)
                       || (dir == ACTOR_DIR_U && dy < ay)) ? 0 : dist;
        if(delay > 2) delay = 2;
        member[m].actor->swalk(dest, leader_dest, 1, delay);
    }
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


/* Is everyone in the party at the coordinates?
 */
bool Party::is_at(uint16 x, uint16 y, uint8 z, uint32 threshold)
{
    for(uint32 p = 0; p < num_in_party; p++)
    {
        MapCoord member_loc(member[p].actor->get_location());
        if(member_loc.x != x || member_loc.y != y || member_loc.z != z)
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

bool Party::contains_actor(uint8 a)
{
 return(contains_actor(actor_manager->get_actor(a)));
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
    for(sint32 m = (num_in_party - 1); m >= 0; m--)
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
    // other actors won't move
    game->get_actor_manager()->set_update(false);
    // view will snap back to player after everyone has moved
    game->get_player()->uncontrol();
    autowalk = true;
}


/* Automatically walk (timed) to vehicle. (step_delay 0 = default speed)
 */
void Party::enter_vehicle(Obj *ship_obj, uint32 step_delay)
{
    MapCoord walkto(ship_obj->x, ship_obj->y, ship_obj->z);
    if(step_delay)
        new TimedPartyMoveToVehicle(&walkto, ship_obj, step_delay);
    else
        new TimedPartyMoveToVehicle(&walkto, ship_obj);
    // other actors won't move
    game->get_actor_manager()->set_update(false);
    // view will snap back to player after everyone has moved
    game->get_player()->uncontrol();
    autowalk = true;
}


/* Done automatically walking, return control to player character.
 */
void Party::stop_walking()
{
    game->get_player()->control();
    game->get_actor_manager()->set_update(true);
    autowalk = false;
}
