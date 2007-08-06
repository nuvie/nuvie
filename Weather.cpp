/*
 *  Weather.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Aug 04 2006.
 *  Copyright (c) 2006. All rights reserved.
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

#include <list>
#include <cassert>
#include "nuvieDefs.h"
#include "Configuration.h"
#include "CallBack.h"

#include "NuvieIO.h"
#include "U6misc.h"
#include "U6LList.h"
#include "Weather.h"
#include "Game.h"
#include "GameClock.h"
#include "Objlist.h"
#include "TimedEvent.h"
#include "ViewManager.h"
#include "MapWindow.h"
#include "Map.h"

//the longest we will go before having a change in wind direction
#define WEATHER_MAX_WIND 30 

#define WEATHER_MAX_ECLIPSE_LENGTH 30

Weather::Weather(Configuration *cfg, nuvie_game_t type)
{
	config = cfg;
	gametype = type;
 
	wind_dir = NUVIE_DIR_NONE;
	wind_timer = NULL;
	
	eclipse_start = 0;
	eclipse_length = 0;
	eclipse_timer = NULL;
}

Weather::~Weather()
{

}
 
bool Weather::load(NuvieIO *objlist)
{
	uint8 eclipse_length;

	clear_wind();
	clear_eclipse();
    
	if(gametype == NUVIE_GAME_U6)
	{
		wind_dir = load_wind(objlist);
		set_wind_change_callback(); //set a timer to change the wind direction in the future.
		send_wind_change_notification_callback();
		
		eclipse_length = load_eclipse_length(objlist);
		if(eclipse_length > 0)
			start_eclipse(eclipse_length);
		load_moonstones(objlist);
	}

	return true;
}

bool Weather::load_moonstones(NuvieIO *objlist)
{

 objlist->seek(OBJLIST_OFFSET_U6_MOONSTONES);

 // endian issues, and size of z coordinate doesn't match, so can't do a straight read to buffer
 for (uint8 moonstone=0;moonstone<8;moonstone++)
 {
   uint8 little;
   uint8 bigend;
   uint16 x,y;
   uint8 z;
   little=objlist->read1();//little endian..
   bigend=objlist->read1();//little endian..
   x=little+(256*bigend);
   little=objlist->read1();//little endian..
   bigend=objlist->read1();//little endian..
   y=little+(256*bigend);
   little=objlist->read1();//little endian..
   bigend=objlist->read1();//little endian..
   assert(!bigend);
   z=little; //bigend should be zero
   PERR("Read moonstone %x, buried at (%X,%X,%X)\n",moonstone,x,y,z);
   moonstones[moonstone]=MapCoord(x,y,z);
 }
 return true;

}

MapCoord Weather::get_moonstone(uint8 moonstone) 
{
  if (moonstone<8) // FIXME: hardcoded constant
    return moonstones[moonstone];
  PERR("get_moonstone(%d): Moonstone out of range\n",moonstone);
  return MapCoord(0,0,0);
}
bool Weather::set_moonstone(uint8 moonstone,MapCoord where) 
{
  if (moonstone<8) // FIXME: hardcoded constant
  {
    moonstones[moonstone]=where;
    return true;
  }
  PERR("set_moonstone(%d): Moonstone out of range\n",moonstone);
  return false;
}

uint8 Weather::load_wind(NuvieIO *objlist)
{
	const uint8 wind_tbl[8] = {
		NUVIE_DIR_N,
		NUVIE_DIR_NE,
		NUVIE_DIR_E,
		NUVIE_DIR_SE,
		NUVIE_DIR_S,
		NUVIE_DIR_SW,
		NUVIE_DIR_W,
		NUVIE_DIR_NW };
     
	uint8 objlist_wind;
    
	objlist->seek(OBJLIST_OFFSET_U6_WIND_DIR);
	objlist_wind = objlist->read1();
    
	if(objlist_wind > 7 || objlist_wind == 0xff) //objlist 0xff = Calm 'C'
		return NUVIE_DIR_NONE;

	return wind_tbl[objlist_wind];
}

void Weather::clear_eclipse()
{
	if(eclipse_timer)
		eclipse_timer->stop_timer(); //stop current timer
	
	end_eclipse();
}

void Weather::clear_wind()
{
	if(wind_timer)
		wind_timer->stop_timer();
	
	wind_dir = NUVIE_DIR_NONE;
	
	return;
}

//returns any eclipse time saved in the objlist. (measured in game ticks)
uint8 Weather::load_eclipse_length(NuvieIO *objlist)
{
	objlist->seek(OBJLIST_OFFSET_U6_ECLIPSE);
	return objlist->read1();
}

bool Weather::save(NuvieIO *objlist)
{
	if(gametype == NUVIE_GAME_U6)
	{
		save_wind(objlist);
		save_eclipse(objlist);
		save_moonstones(objlist);
	}
	
    return true;
}

bool Weather::save_wind(NuvieIO *objlist)
{
	const uint8 wind_tbl[] = {
		OBJLIST_U6_WIND_DIR_N,
		OBJLIST_U6_WIND_DIR_S,
		OBJLIST_U6_WIND_DIR_E,
		OBJLIST_U6_WIND_DIR_W,
		OBJLIST_U6_WIND_DIR_NE,
		OBJLIST_U6_WIND_DIR_SE,
		OBJLIST_U6_WIND_DIR_SW,
		OBJLIST_U6_WIND_DIR_NW,
		OBJLIST_U6_WIND_DIR_C };
        
	objlist->seek(OBJLIST_OFFSET_U6_WIND_DIR);
	objlist->write1(wind_tbl[wind_dir]);
    
	return true;
}

bool Weather::save_eclipse(NuvieIO *objlist)
{
	GameClock *clock = Game::get_game()->get_clock();
	uint8 new_length = 0;

	if(eclipse_timer)
	{
		//work out how long we have left on the eclipse
		new_length = eclipse_length - (clock->get_game_ticks() - eclipse_start);
	}
	
	//save it into the objlist
  objlist->seek(OBJLIST_OFFSET_U6_ECLIPSE);
  objlist->write1(new_length);

	return true;
}
bool Weather::save_moonstones(NuvieIO *objlist)
{
  objlist->seek(OBJLIST_OFFSET_U6_MOONSTONES);
  for (uint8 moonstone=0;moonstone<8;moonstone++)
  {
    uint16 x,y;
    uint8 z;
    x=moonstones[moonstone].x;
    y=moonstones[moonstone].y;
    z=moonstones[moonstone].z;
    objlist->write1((uint8)(x&255));//little endian..
    objlist->write1((uint8)(x>>8));
    objlist->write1((uint8)(y&255));//little endian..
    objlist->write1((uint8)(y>>8));
    objlist->write1(z);//little endian..
    objlist->write1(0);
    PERR("Wrote moonstone %x, buried at (%X,%X,%X)\n",moonstone,x,y,z);
  }
  return true;
}

string Weather::get_wind_dir_str()
{
	const char names[9][3] = {"N","E","S","W","NE","SE","SW","NW","C"};
	string s;
 
	s = names[wind_dir];
    
	return s;
}

void Weather::change_wind_dir()
{
	uint8 new_wind_dir;

	new_wind_dir = NUVIE_RAND() % 9;

  set_wind_dir(new_wind_dir);
	return;
}

bool Weather::set_wind_dir(uint8 new_wind_dir)
{
	uint8 old_wind_dir = wind_dir;
	
	if(new_wind_dir >= 9)
		return false;

	clear_wind();
	
	wind_dir = new_wind_dir;

	if(wind_dir != old_wind_dir)
		send_wind_change_notification_callback();

	set_wind_change_callback();

	return true;
}

inline void Weather::set_wind_change_callback()
{
	uint16 length = (NUVIE_RAND() % WEATHER_MAX_WIND) + 1;
	uint8 *cb_msgid = new uint8;
	*cb_msgid = WEATHER_CB_CHANGE_WIND_DIR;
	wind_timer = new GameTimedCallback((CallBack *)this, cb_msgid, length);
	PERR("Adding wind change timer. Length = %d\n",length);
}

inline void Weather::send_wind_change_notification_callback()
{
	std::list<CallBack *>::iterator cb_iter;
	for(cb_iter = wind_change_notification_list.begin(); cb_iter != wind_change_notification_list.end(); cb_iter++)
		(*cb_iter)->callback(WEATHER_CB_CHANGE_WIND_DIR, (CallBack *)this, NULL);
}

void Weather::start_eclipse(uint8 length)
{
	GameClock *clock = Game::get_game()->get_clock();
  ViewManager *view_manager = Game::get_game()->get_view_manager();
  MapWindow *map_window = Game::get_game()->get_map_window();
	
	if(eclipse_timer != NULL || length == 0)
		return;

	eclipse_length = length;

	eclipse_start = clock->get_game_ticks();
	set_eclipse_callback(eclipse_length);
  
  view_manager->update();
	map_window->updateAmbience();

	PERR("Starting Eclipse. Length=%d\n",eclipse_length);
	
	return;
}

inline void Weather::set_eclipse_callback(uint8 length)
{
	uint8 *cb_msgid = new uint8;
	*cb_msgid = WEATHER_CB_END_ECLIPSE;
	eclipse_timer = new GameTimedCallback((CallBack *)this, cb_msgid, length);
}

void Weather::end_eclipse()
{
	eclipse_timer = NULL;
	eclipse_length = 0;
	eclipse_start = 0;
	
	PERR("Eclipse End.\n");

	return;
}

bool Weather::add_wind_change_notification_callback(CallBack *caller)
{
	wind_change_notification_list.push_back(caller);
    
	return true;
}


uint16 Weather::callback(uint16 msg, CallBack *caller, void *data)
{
	uint8 *cb_msgid = (uint8 *)callback_user_data;
 
	switch(*cb_msgid)
	{
		case WEATHER_CB_CHANGE_WIND_DIR : wind_timer = NULL; change_wind_dir(); break;
		case WEATHER_CB_END_ECLIPSE : end_eclipse(); break;
		default : PERR("Weather: Unknown callback!\n"); break;
	}
    
	delete cb_msgid;
    
	return 1;
}

