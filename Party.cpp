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
#include "NuvieIOFile.h"
#include "Game.h"
#include "Converse.h"
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


bool Party::loadParty()
{
 std::string filename;
 NuvieIOFileRead objlist;
 uint8 actor_num;
 uint16 i;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
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
    member[0].form_x = 0; member[0].form_y = 0;
    switch(formation)
    {
        case PARTY_FORM_COLUMN:
            for(uint32 n = 1; n < num_in_party; n++)
            {   member[n].form_x = 0; member[n].form_y = n; }
            break;
        case PARTY_FORM_ROW:
            for(uint32 n = 1; n < num_in_party; n++)
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
            for(uint32 n = 8; n < num_in_party; n++) // the rest "tail"
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
             (dir == ACTOR_DIR_R) ? ly - member[m].form_x :
             (dir == ACTOR_DIR_D) ? ly - member[m].form_y :
             (dir == ACTOR_DIR_L) ? ly + member[m].form_x : ay;
        if(ax == dx && ay == dy) // already there
            continue;

        // try quick move
        if(((abs(ax - dx) <= 1 && abs(ay - dy) <= 1) || az != lz)
           && member[m].actor->move(dx, dy, lz))
        {
            member[m].actor->stop_walking();
            if(dy < ay)
                member[m].actor->set_direction(ACTOR_DIR_U);
            else if(dy > ay)
                member[m].actor->set_direction(ACTOR_DIR_D);
            else if(dx < ax)
                member[m].actor->set_direction(ACTOR_DIR_L);
            else
                member[m].actor->set_direction(ACTOR_DIR_R);
            continue;
        }
        // walk there
        MapCoord dest(dx, dy, lz), leader_dest(lx, ly, lz);
        member[m].actor->swalk(dest, leader_dest);
    }
}


/* Move and center everyone in the party to one location.
 */
bool Party::move(uint16 dx, uint16 dy, uint8 dz)
{
    for(sint32 m = (num_in_party - 1); m >= 0; m--)
        if(!member[m].actor->move(dx, dy, dz))
            return(false);
    return(true);
}
