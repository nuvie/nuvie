#ifndef __PartyView_h__
#define __PartyView_h__

/*
 *  PartyView.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Aug 22 2003.
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

#include "View.h"

class Configuration;
class TileManager;
class ObjManager;
class Screen;
class Font;
class Party;
class Player;
class Actor;
class SunMoonStripWidget;

class PartyView : public View {

 Player *player;
 void *view_manager;
 uint16 row_offset; // first party member displayed
 SunMoonStripWidget *sun_moon_widget;

 public:
 PartyView(Configuration *cfg);
 ~PartyView();

 bool init(void *vm, uint16 x, uint16 y, Font *f, Party *p, Player *pl, TileManager *tm, ObjManager *om);
 GUI_status MouseUp(int x,int y,int button);
 GUI_status MouseDown(int x,int y,int button) { return(GUI_YUM); }
 GUI_status MouseWheel(sint32 x, sint32 y);
 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);
 void Display(bool full_redraw);
 void update() { update_display = true; }
 void display_sun_moon_strip();
 void set_party_view_targeting(bool val) { party_view_targeting = val; }

 protected:

 void display_surface_strip();
 void display_dungeon_strip();
 void display_arrows();
 void display_sun_moon(Tile *tile, uint8 pos);
 void display_sun(uint8 hour, uint8 minute, bool eclipse);
 void display_moons(uint8 day, uint8 hour, uint8 minute=0);

 bool up_arrow();
 bool down_arrow();
 bool party_view_targeting;

 Actor *get_actor(int x, int y);


};

#endif /* __PartyView_h__ */

