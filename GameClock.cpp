/*
 *  GameClock.cpp
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
#include "U6File.h"

#include "GameClock.h"

GameClock::GameClock(Configuration *cfg)
{
 config = cfg;

 move_counter = 0;
}
 
GameClock::~GameClock()
{

}

bool GameClock::init()
{
 std::string filename;
 U6File objlist;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
 if(objlist.open(filename,"rb") == false)
   return false;

 objlist.seek(0x1bf3); // Start of Actor position info

 minute = objlist.read1();
 hour = objlist.read1();
 month = objlist.read1();
 day = objlist.read1();
 
 year = objlist.read2();
 
 printf("Inital gametime: %s %s\n",get_date_string(),get_time_string());
 
 return true;
}
 
void GameClock::inc_move_counter()
{
 move_counter++;
 
 if((move_counter % GAMECLOCK_TICKS_PER_MINUTE) == 0)
   inc_minute();
 
 return;
}

// move_counter by a minute.

void GameClock::inc_move_counter_by_a_minute()
{
 move_counter += GAMECLOCK_TICKS_PER_MINUTE;
 
 inc_minute();
}

void GameClock::inc_minute()
{

 if(minute == 59)
   {
    minute = 0;
    inc_hour();    
   }
 else
    minute++;

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
   hour++;
 
 printf("%s\n",get_time_string());
   
 return;
}

void GameClock::inc_day()
{

 if(day == 30)
   {
    day = 1;
    inc_month();
   }
 else
   day++;
   
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
   month++;
   
 return;
}

void GameClock::inc_year()
{
 year++;

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

