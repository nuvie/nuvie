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
#include "nuvieDefs.h"
#include "Configuration.h"

#include "GUI.h"
#include "ContainerWidgetGump.h"


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
  cursor_tile = NULL;
  empty_tile = &gump_empty_tile;
  obj_font_color = 15;
  bg_color = 0;
  fill_bg = false;

  cursor_x = cursor_y = 0;
  show_cursor = true;
}

ContainerWidgetGump::~ContainerWidgetGump()
{

}

bool ContainerWidgetGump::init(Actor *a, uint16 x, uint16 y, uint16 w, uint16 h, TileManager *tm, ObjManager *om, Font *f)
{
 tile_manager = tm;
 obj_manager = om;

 rows = h;
 cols = w;

 //objlist_offset_x = 0;
 //objlist_offset_y = 0;

 //72 =  4 * 16 + 8
 GUI_Widget::Init(NULL, x, y, CONTAINER_WIDGET_COLS * 16, CONTAINER_WIDGET_GUMP_HEIGHT);

 set_actor(a);
 set_accept_mouseclick(true, 0);//USE_BUTTON); // accept [double]clicks from button1 (even if double-click disabled we need clicks)

 cursor_tile = tile_manager->get_gump_cursor_tile();

 return true;
}

void ContainerWidgetGump::MoveRelative(int dx, int dy)
{
	if(Game::get_game()->is_orig_style()) // FIXME should probably use backfill.
		GUI::get_gui()->force_full_redraw();
	GUI_Widget::MoveRelative(dx, dy);
}

void ContainerWidgetGump::Display(bool full_redraw)
{
	display_inventory_list();
	if(show_cursor)
	{
		screen->blit(area.x+cursor_x*16,area.y+cursor_y*16,(unsigned char *)cursor_tile->data,8,16,16,16,true);
	}
	screen->update(area.x, area.y, area.w, area.h);
}

void ContainerWidgetGump::cursor_right()
{
	if(cursor_x < cols - 1)
	{
		if(get_obj_at_location((cursor_x+1) * 16,cursor_y * 16) != NULL)
		{
			cursor_x++;
		}
	}
}

void ContainerWidgetGump::cursor_left()
{
	if(cursor_x > 0)
	{
		cursor_x--;
	}
}

void ContainerWidgetGump::cursor_up()
{
	if(cursor_y > 0)
	{
		cursor_y--;
	}
	else
	{
		up_arrow();
	}
}

void ContainerWidgetGump::cursor_down()
{
	if(get_obj_at_location(0,(cursor_y+1) * 16) != NULL) //check that we can move down one row.
	{
		if(cursor_y < rows - 1)
		{
			cursor_y++;
		}
		else
		{
			down_arrow();
		}

		for(;cursor_x > 0;cursor_x--)
		{
			if(get_obj_at_location(cursor_x * 16,cursor_y * 16) != NULL)
			{
				break;
			}
		}
	}
}

GUI_status ContainerWidgetGump::KeyDown(SDL_keysym key)
{
	switch(key.sym)
	    {
	        case SDLK_UP:
	        case SDLK_KP8:
	            cursor_up();
	            break;
	        case SDLK_DOWN:
	        case SDLK_KP2:
	            cursor_down();
	            break;
	        case SDLK_LEFT:
	        case SDLK_KP4:
	        	cursor_left();
	            break;
	        case SDLK_RIGHT:
	        case SDLK_KP6:
	            cursor_right();
	            break;
	        case SDLK_RETURN:
	        case SDLK_KP_ENTER:
	        	selected_obj = get_obj_at_location(cursor_x * 16,cursor_y * 16);
	        	if(selected_obj)
	        	{
	        		try_click();
	        	}
	        	break;
	        default:
	        	return GUI_PASS;
	    }

	return GUI_YUM;
}

void ContainerWidgetGump::set_actor(Actor *a)
{
	cursor_x = cursor_y = 0;
	ContainerWidget::set_actor(a);
}
