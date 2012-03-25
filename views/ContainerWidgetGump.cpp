/*
 *  ContainerWidgetGump.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Mar 20 2012.
 *  Copyright (c) 2012 The Nuvie Team. All rights reserved.
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
#include <cassert>
#include "nuvieDefs.h"
#include "U6LList.h"
#include "Configuration.h"

#include "GUI.h"
#include "GamePalette.h"
#include "ContainerWidgetGump.h"
#include "Actor.h"
#include "Text.h"
#include "GameClock.h"
#include "Event.h"
#include "MsgScroll.h"
#include "TimedEvent.h"
#include "UseCode.h"
#include "MapWindow.h"

#include "InventoryFont.h"
#include "ViewManager.h"


static const Tile gump_empty_tile = {
		0,
		false,
		false,
		false,
		false,
		false,
		true,
		false,
		false,
		0,
		//uint8 qty;
		//uint8 flags;

		0,
		0,
		0,

		{
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
			255,255,255,255,255,143,142,141,141,142,143,255,255,255,255,255,
			255,255,255,255,143,141,141,142,142,141,141,143,255,255,255,255,
			255,255,255,143,141,142,143,143,143,143,142,141,143,255,255,255,
			255,255,143,141,142,143,0,0,0,0,143,142,141,143,255,255,
			255,255,142,141,143,0,0,0,0,0,0,143,141,142,255,255,
			255,255,141,142,143,0,0,0,0,0,0,143,142,141,255,255,
			255,255,141,142,143,0,0,0,0,0,0,143,142,141,255,255,
			255,255,142,141,143,0,0,0,0,0,0,143,141,142,255,255,
			255,255,143,141,142,143,0,0,0,0,143,142,141,143,255,255,
			255,255,255,143,141,142,143,143,143,143,142,141,143,255,255,255,
			255,255,255,255,143,141,141,142,142,141,141,143,255,255,255,255,
			255,255,255,255,255,143,142,141,141,142,143,255,255,255,255,255,
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
		}
};


ContainerWidgetGump::ContainerWidgetGump(Configuration *cfg, GUI_CallBack *callback) : ContainerWidget(cfg, callback)
{
 
}

ContainerWidgetGump::~ContainerWidgetGump()
{

}

bool ContainerWidgetGump::init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om, Text *t)
{
 tile_manager = tm;
 obj_manager = om;


 //objlist_offset_x = 0;
 //objlist_offset_y = 0;

 //72 =  4 * 16 + 8
 GUI_Widget::Init(NULL, x, y, 64, 48);

 set_actor(a);
 set_accept_mouseclick(true, 0);//USE_BUTTON); // accept [double]clicks from button1 (even if double-click disabled we need clicks)
 empty_tile = &gump_empty_tile;
 obj_font_color = 15;
 bg_color = 0;
 fill_bg = false;

 return true;
}
