/*
 *  GameClock.cpp
 *  Nuvie
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

#include "nuvieDefs.h"
#include "Configuration.h"
#include "NuvieIO.h"

#include "GameClock.h"

GameClock::GameClock(Configuration *cfg)
{
 config = cfg;

 move_counter = 0;
 time_counter = 0;
 tick_counter = 0;
}
 
GameClock::~GameClock()
{

}

bool GameClock::load(NuvieIO *objlist)
{
/*
 std::string filename;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
 if(objlist.open(filename) == false)
   return false;
*/

 objlist->seek(0x1bf3); // start of time data

 minute = objlist->read1();
 hour = objlist->read1();
 day = objlist->read1();
 month = objlist->read1();
 year = objlist->read2();
 
 update_day_of_week();
 
 printf("Loaded game clock: %s %s\n",get_date_string(),get_time_string());
 
 return true;
}

bool GameClock::save(NuvieIO *objlist)
{
 objlist->seek(0x1bf3); // start of time data

 objlist->write1(minute);
 objlist->write1(hour);
 objlist->write1(day);
 objlist->write1(month);
 objlist->write2(year);

 return true;
}

void GameClock::inc_move_counter()
{
 move_counter++;

 if((move_counter % GAMECLOCK_TICKS_PER_MINUTE) == 0)
   inc_minute();
 else
   tick_counter++;

 return;
}

// move_counter by a minute.

void GameClock::inc_move_counter_by_a_minute()
{
 move_counter += GAMECLOCK_TICKS_PER_MINUTE;

 inc_minute();
}

// advance game time to the start of the next hour.

void GameClock::advance_to_next_hour()
{
 minute = 0;
 
 inc_hour();
}

void GameClock::inc_minute()
{

 if(minute == 59)
   {
    minute = 0;
    inc_hour();    
   }
 else
   {
    minute++;
    time_counter += 1;
    tick_counter += GAMECLOCK_TICKS_PER_MINUTE;
   }
 return;
}

void GameClock::inc_hour()
{

 if(hour == 23)
   {
    hour = 0;
    inc_day();
   }
 else
   {
    hour++;
    time_counter += 60;
    tick_counter += (60 * GAMECLOCK_TICKS_PER_MINUTE);
   }
 printf("%s\n",get_time_string());


 return;
}

void GameClock::inc_day()
{

 if(day == 28)
   {
    day = 1;
    inc_month();
   }
 else
   {
    day++;
    time_counter += 1440;
    tick_counter += (1440 * GAMECLOCK_TICKS_PER_MINUTE);
   }
 update_day_of_week();
 
 printf("%s\n",get_date_string());


 return;
}

void GameClock::inc_month()
{

 if(month == 12)
   {
    month = 1;
    inc_year();
   }
 else
   {
    month++;
    time_counter += 40320;
    tick_counter += (40320 * GAMECLOCK_TICKS_PER_MINUTE);
   }
 return;
}

void GameClock::inc_year()
{
 year++;
 time_counter += 483840;
 tick_counter += (483840 * GAMECLOCK_TICKS_PER_MINUTE);
 return;
}

uint32 GameClock::get_move_count()
{
 return move_counter;
}
 
char *GameClock::get_time_of_day_string()
{
 if(hour < 12)
   return "morning";

 if(hour >= 12 && hour <= 18)
   return "afternoon";

 return "evening";   
}
 
uint8 GameClock::get_hour()
{
 return hour;
}

uint8 GameClock::get_minute()
{
 return minute;
}

uint8 GameClock::get_day()
{
 return day;
}

uint8 GameClock::get_month()
{
 return month;
}

uint16 GameClock::get_year()
{
 return year;
}

uint8 GameClock::get_day_of_week()
{
 return day_of_week;
}
 
char *GameClock::get_date_string()
{

 sprintf(date_string, "%2d-%02d-%04d", month, day, year);
 
 return date_string;
}

char *GameClock::get_time_string()
{
 char c;
 uint8 tmp_hour;
 
 if(hour < 12)
   c = 'A';
 else
   c = 'P';
 
 if(hour > 12)
  tmp_hour = hour - 12;
 else
   {
    if(hour == 0)
      tmp_hour = 12;
    else
      tmp_hour = hour;
   }
   
 sprintf(time_string, "%02d:%02d %c.M.", tmp_hour, minute, c);
  
 return time_string;
}

inline void GameClock::update_day_of_week()
{
 day_of_week = day % 7;
 if(day_of_week == 0)
   day_of_week = 7;
}
