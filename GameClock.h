#ifndef __GameClock_h__
#define __GameClock_h__

/*
 *  GameClock.h
 *  Nuive
 *
 *  Created by Eric Fry on Fri May 02 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
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

#include "U6def.h"

#include "Configuration.h"

#define GAMECLOCK_TICKS_PER_MINUTE   4

class GameClock
{
 Configuration *config;
 
 uint8 minute;
 uint8 hour;
 uint8 day;
 uint8 month;
 uint16 year;
 
 uint32 move_counter;
 
 char date_string[11];
 char time_string[11];
 
 public:
 
 GameClock(Configuration *cfg);
 ~GameClock();
 
 bool init();
 
 void inc_move_counter();
 void inc_move_counter_by_a_minute();
 
 void inc_minute();
 void inc_hour();
 void inc_day();
 void inc_month();
 void inc_year();
 
 uint32 get_move_count();
 
 char *get_time_of_day_string();
 
 uint8 get_hour();
 uint8 get_minute();
 
 char *get_date_string();
 char *get_time_string();
 
};


#endif /* __GameClock_h__ */
