#ifndef __Weather_h__
#define __Weather_h__

/*
 *  Weather.h
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

#include "nuvieDefs.h"
#include "CallBack.h"
#include "Map.h"

class Configuration;
class NuvieIO;
class CallBack;
class GameTimedCallback;

using std::list;
using std::string;

//our callbacks

#define WEATHER_CB_CHANGE_WIND_DIR 1
#define WEATHER_CB_END_ECLIPSE     2

class Weather: public CallBack
{
	Configuration *config;
	nuvie_game_t gametype; // what game is being played?
 
	uint8 wind_dir;
	std::list<CallBack *>wind_change_notification_list;
 
	uint32 eclipse_start;
	uint8 eclipse_length;
	GameTimedCallback *eclipse_timer;
	GameTimedCallback *wind_timer;
	MapCoord moonstones[8]; //FIXME hardcoded constant!
 
	public:

	Weather(Configuration *cfg, nuvie_game_t type);
	~Weather();

	bool load(NuvieIO *objlist);
	bool save(NuvieIO *objlist);
 
	std::string get_wind_dir_str();
	uint8 get_wind_dir() { return wind_dir; }
	bool set_wind_dir(uint8 new_wind_dir);
	bool add_wind_change_notification_callback(CallBack *caller);
	bool set_moonstone(uint8 moonstone,MapCoord where);
	MapCoord get_moonstone(uint8 moonstone);
  
	bool is_eclipse() { return((eclipse_timer != NULL ? true : false)); }
	void start_eclipse(uint8 length = 0);

	uint16 callback(uint16 msg, CallBack *caller, void *data = NULL);
 
	protected:
 
	uint8 load_wind(NuvieIO *objlist);
        bool load_moonstones(NuvieIO *objlist);
        bool save_moonstones(NuvieIO *objlist);
	bool save_wind(NuvieIO *objlist);
	void change_wind_dir();
	inline void set_wind_change_callback();
	inline void send_wind_change_notification_callback();
	void clear_wind();
	
	void clear_eclipse();
	uint8 load_eclipse_length(NuvieIO *objlist);
	bool save_eclipse(NuvieIO *objlist);
	inline void set_eclipse_callback(uint8 length);
	void end_eclipse();
};

#endif /* __Weather_h__ */
