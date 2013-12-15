#ifndef __SunMoonRibbon_h__
#define __SunMoonRibbon_h__

/*
 *  SunMoonRibbon.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Dec 11 2013.
 *  Copyright (c) 2013. All rights reserved.
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

#include "SunMoonStripWidget.h"


class SunMoonRibbon : public SunMoonStripWidget {

private:
  SDL_Surface *bg_data;
  Weather *weather;
  bool retracted;
  uint16 current_time;

 public:
 SunMoonRibbon(Player *p, Weather *w, TileManager *tm);
 ~SunMoonRibbon();

 void init(Screen *screen);
 void Display(bool full_redraw);

 GUI_status MouseDown(int x, int y, int button);


 void extend() { retracted = false; }
 void retract() { retracted = true; }

 protected:
 void display_sun_moon(Tile *tile, uint8 pos);

 private:
 void loadBgImage(uint8 num);
 void display_surface_strip();
 void display_dungeon_strip();
 void update_hour(uint16 time);
};

#endif /* __SunMoonRibbon_h__ */

