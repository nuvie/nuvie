/*
 *  Party.cpp
 *  Nuive
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
#include "Game.h"
#include "Converse.h"
#include "Party.h"

Party::Party(Configuration *cfg)
{
 config = cfg;
 num_in_party = 0;
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
 U6File objlist;
 uint8 actor_num;
 uint16 i;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
 if(objlist.open(filename,"rb") == false)
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
 
 return true;
}
