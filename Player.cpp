/*
 *  player.cpp
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

#include "Configuration.h"
#include "U6misc.h"
#include "NuvieIO.h"
#include "ActorManager.h"
#include "Actor.h"
#include "ObjManager.h"
#include "MapWindow.h"
#include "GameClock.h"
#include "Party.h"
#include "U6objects.h"
#include "Player.h"

Player::Player(Configuration *cfg)
{
 config = cfg;
 config->value("config/GameType",game_type);

 karma = 0;
 questf = 0;
}

bool Player::init(ObjManager *om, ActorManager *am, MapWindow *mw, GameClock *c, Party *p)
{

 clock = c;
 actor_manager = am;
 obj_manager = om;
 map_window = mw;
 party = p;

 init();
 
 return true;
}

void Player::init()
{
 actor = NULL;
 
 party_mode = true;
 mapwindow_centered = true;

}

bool Player::load(NuvieIO *objlist)
{
 init();
 
 objlist->seek(0xf00);
 
 objlist->readToBuf((unsigned char *)name,14); // read in Player name. 
 
 if(game_type == NUVIE_GAME_U6)
   {
    objlist->seek(0x1bf1); // U6 Quest Flag
    questf = objlist->read1();

    objlist->seek(0x1bf9); // Player Karma.
    karma = objlist->read1();

    objlist->seek(0x1c71); // Player Gender.
    gender = objlist->read1();
   }
   
 if(game_type == NUVIE_GAME_MD)
   {
    objlist->seek(0x1d27); // Player Gender.
    gender = objlist->read1();
   }

 set_actor(find_actor());
  
 return true;
}

bool Player::save(NuvieIO *objlist)
{
 if(game_type == NUVIE_GAME_U6)
   {
    objlist->seek(0x1bf1); // U6 Quest Flag
    objlist->write1(questf);

    objlist->seek(0x1bf9); // Player Karma.
    objlist->write1(karma);

    objlist->seek(0x1c71); // Player Gender.
    objlist->write1(gender);
   }
   
 if(game_type == NUVIE_GAME_MD)
   {
    objlist->seek(0x1d27); // Player Gender.
    objlist->write1(gender);
   }

 return true;
}

/* Returns the first actor set as Player.
 */
Actor *Player::find_actor()
{
    for(uint32 p = 0; p < 255; p++)
    {
        Actor *actor = actor_manager->get_actor(p);
        if(actor->get_worktype() == 0x02) // WT_U6_PLAYER
            return(actor);
    }
    return(actor_manager->get_actor(1)); // U6NPC_AVATAR
}


// keep MapWindow focused on Player actor, or remove focus
void Player::set_mapwindow_centered(bool state)
{
 uint16 x,y;
 uint8 z;

 mapwindow_centered = state;
 if(mapwindow_centered == false)
    return;
 map_window->centerMapOnActor(actor);
 
 get_location(&x,&y,&z);
 obj_manager->update(x,y,z); // spawn eggs when teleporting. eg red moongate.
}
 
void Player::set_actor(Actor *new_actor)
{
 actor = new_actor;
 actor->set_worktype(0x2); // WT_U6_PLAYER
 actor_manager->set_player(actor);
 map_window->centerMapOnActor(actor);
}

Actor *Player::get_actor()
{
 return actor;
}

void Player::get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level)
{
 actor->get_location(ret_x,ret_y,ret_level);
}

uint8 Player::get_location_level()
{
 return actor->z;
}

char *Player::get_name()
{
 return name;
}


/* Add to Player karma. Handle appropriately the karma min/max limits.
 */
void Player::add_karma(uint8 val)
{
    karma = ((karma + val) <= 99) ? karma + val : 99;
}


/* Subtract from Player karma. Handle appropriately the karma min/max limits.
 */
void Player::subtract_karma(uint8 val)
{
    karma = ((karma - val) >= 0) ? karma - val : 0;
}


char *Player::get_gender_title()
{
 if(gender == 0)
   return "milord";
 else
   return "milady";
}


// walk to adjacent square
void Player::moveRelative(sint16 rel_x, sint16 rel_y)
{
 uint16 x, y;
 uint8 z;
 actor->get_location(&x, &y, &z);

 // don't allow diagonal move between blocked tiles (player only)
 if(rel_x && rel_y && !actor->check_move(x + rel_x, y + 0, z, ACTOR_IGNORE_OTHERS)
                   && !actor->check_move(x + 0, y + rel_y, z, ACTOR_IGNORE_OTHERS))
  return;

 // change direction only if we can move there
 //if(actor->check_move(x + rel_x, y + rel_y, z, ACTOR_IGNORE_OTHERS))
 actor->moves = 1; // player can always move (FIXME: check in Actor::move()?)
 if(actor->moveRelative(rel_x,rel_y))
 {
  actor->set_direction(rel_x, rel_y);
//   map_window->moveMapRelative(rel_x,rel_y);
   if(party_mode && party->is_leader(actor)) // lead party
     party->follow();
   else if(actor->id_n == 0) // using vehicle; drag party along
   {
     MapCoord new_xyz = actor->get_location();
     party->move(new_xyz.x, new_xyz.y, new_xyz.z);
   }
   actor_manager->updateActors(x, y, z);
   clock->inc_move_counter(); // SB-X move to gameclock
   obj_manager->update(x, y, z); // remove temporary objs, spawn eggs
   //actor_manager->cache_update(x,y,z); // clean actor cache remove temp actors
 }
}

// teleport-type move
void Player::move(sint16 new_x, sint16 new_y, uint8 new_level)
{
   actor->moves = 1; // player can always move (FIXME: check in Actor::move()?)
   if(actor->move(new_x, new_y, new_level, ACTOR_FORCE_MOVE))
   {
    //map_window->centerMapOnActor(actor);
    if(party->is_leader(actor)) // lead party
      {
       party->move(new_x, new_y, new_level); // center everyone first
       party->follow(); // then try to move them to correct positions
      }
    actor_manager->updateActors(new_x, new_y, new_level);
    obj_manager->update(new_x, new_y, new_level); // remove temporary objs, spawn eggs
   }
}

void Player::moveLeft()
{
 moveRelative(-1,0);
}

void Player::moveRight()
{
 moveRelative(1,0);
}

void Player::moveUp()
{
 moveRelative(0,-1);
}

void Player::moveDown()
{
 moveRelative(0,1);
}

void Player::pass()
{
 uint16 x,y;
 uint8 z;
 
 clock->inc_move_counter_by_a_minute();
 if(party_mode && party->is_leader(actor)) // lead party
    party->follow();
 get_location(&x, &y, &z);
 actor_manager->updateActors(x,y,z); // SB-X move to gameclock
}



/* Enter party mode, with everyone following actor. (must be in the party)
 */
bool Player::set_party_mode(Actor *new_actor)
{
    if(party->contains_actor(new_actor))
    {
        party_mode = true;
        actor = new_actor;
        return(true);
    }
    return(false);
}


/* Enter solo mode as actor. (must be in the party)
 */
bool Player::set_solo_mode(Actor *new_actor)
{
    if(party->contains_actor(new_actor))
    {
        party_mode = false;
        actor = new_actor;
        actor->stop_walking();
        return(true);
    }
    return(false);
}


/* Returns the delay in continuous movement for the actor type we control.
 */
uint32 Player::get_walk_delay()
{
/*    if(actor->obj_n == OBJ_U6_BALLOON?)
        return(??); // ???
    else */if(actor->obj_n == OBJ_U6_SHIP)
        return(20); // 5x normal
    else if(actor->obj_n == OBJ_U6_SKIFF)
        return(50); // 2x normal
    else if(actor->obj_n == OBJ_U6_RAFT)
        return(100); // normal
    else if(actor->obj_n == OBJ_U6_HORSE_WITH_RIDER)
        return(50); // 2x normal
    else
        return(100); // normal movement about ?? spaces per second
}
