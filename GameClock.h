#ifndef __GameClock_h__
#define __GameClock_h__

/*
 *  GameClock.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri May 02 2003.
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
#include "SDL.h"
#define GAMECLOCK_TICKS_PER_MINUTE   4

class Configuration;
class NuvieIO;

class GameClock
{
 Configuration *config;

 uint8 minute;
 uint8 hour;
 uint8 day;
 uint8 month;
 uint16 year;
 uint8 day_of_week;

 uint32 move_counter; // player steps taken since start
 uint32 time_counter; // game minutes
// uint32 tick_counter; // moves/turns since last minute

 char date_string[11];
 char time_string[11];

 bool active; // clock is active and running (false = paused)

 public:

 GameClock(Configuration *cfg);
 ~GameClock();

 bool load(NuvieIO *objlist);
 bool save(NuvieIO *objlist);

 void set_active(bool state) { active = state; }
 bool get_active()           { return(active); }

 void inc_move_counter();
 void inc_move_counter_by_a_minute();

 void advance_to_next_hour();

 void inc_minute();
 void inc_hour();
 void inc_day();
 void inc_month();
 void inc_year();

 uint32 get_move_count();

 const char *get_time_of_day_string();

 uint8 get_hour();
 uint8 get_minute();

 uint8 get_day();
 uint8 get_month();
 uint16 get_year();
 uint8 get_day_of_week();

 char *get_date_string();
 char *get_time_string();

 uint32 get_ticks() { return(SDL_GetTicks()); } // milliseconds since start
 uint32 get_game_ticks() { return(time_counter/**GAMECLOCK_TICKS_PER_MINUTE+tick_counter*/); }
// uint32 get_time()  { return(time_counter); } // get_game_ticks() is preferred
 uint32 get_turn()  { return(move_counter); }

 protected:

 void init();
 inline void update_day_of_week();
};


#endif /* __GameClock_h__ */
