/*
 *  ContainerViewGump.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Mar 24 2012.
 *  Copyright (c) 2012. All rights reserved.
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
#include "U6misc.h"
#include "Event.h"
#include "GUI.h"
#include "GUI_button.h"

#include "Party.h"
#include "Actor.h"
#include "ViewManager.h"

#include "ContainerWidgetGump.h"
#include "ContainerViewGump.h"


ContainerViewGump::ContainerViewGump(Configuration *cfg) : DraggableView(cfg)
{
	bg_image = NULL; gump_button = NULL; up_arrow_button = NULL; down_arrow_button = NULL;
	container_widget = NULL; font = NULL; actor = NULL; container_obj = NULL;
}

ContainerViewGump::~ContainerViewGump()
{
}

bool ContainerViewGump::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
	View::init(x,y,t,p,tm,om);

	SetRect(area.x, area.y, 111, 101);

	actor = p->get_actor(p->get_leader());

	container_widget = new ContainerWidgetGump(config, this);
	container_widget->init(actor, 21, 29, tile_manager, obj_manager, t);

	AddWidget(container_widget);

	std::string datadir = GUI::get_gui()->get_data_dir();
	std::string imagefile;
	std::string path;

	build_path(datadir, "images", path);
	datadir = path;
	build_path(datadir, "gumps", path);
	datadir = path;

	gump_button = loadButton(datadir, "gump", 0, 27);

	build_path(datadir, "container", path);
	datadir = path;

	up_arrow_button = loadButton(datadir, "cont_up", 83, 35);
	down_arrow_button = loadButton(datadir, "cont_down", 83, 66);

	build_path(datadir, "bag_bg.bmp", imagefile);
	bg_image = SDL_LoadBMP(imagefile.c_str());

	set_bg_color_key(0, 0x70, 0xfc);

	font = new GUI_Font(GUI_FONT_GUMP);
	font->SetColoring( 0x08, 0x08, 0x08, 0x80, 0x58, 0x30, 0x00, 0x00, 0x00);

	return true;
}

void ContainerViewGump::set_actor(Actor *a)
{
	actor = a;
	container_widget->set_actor(a);
}

void ContainerViewGump::set_container_obj(Obj *o)
{
	container_obj = o;
	container_widget->set_container(container_obj);
}

void ContainerViewGump::Display(bool full_redraw)
{
 //display_level_text();
 //display_spell_list_text();
 SDL_Rect dst;
 dst = area;
 SDL_BlitSurface(bg_image, NULL, surface, &dst);

 DisplayChildren(full_redraw);


 update_display = false;
 screen->update(area.x, area.y, area.w, area.h);

 return;
}

GUI_status ContainerViewGump::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
	//close gump and return control to Magic class for clean up.
	if(caller == gump_button)
	{
		Game::get_game()->get_view_manager()->close_container_view(this);
		return GUI_YUM;
	}
	else if(caller == down_arrow_button)
	{
		container_widget->down_arrow();
		return GUI_YUM;
	}
	else if(caller == up_arrow_button)
	{
		container_widget->up_arrow();
		return GUI_YUM;
	}

    return GUI_PASS;
}

GUI_status ContainerViewGump::MouseDown(int x, int y, int button)
{
	 if(button == SDL_BUTTON_WHEELDOWN)
	 {
		 container_widget->down_arrow();
		return GUI_YUM;
	 }
	 else if(button == SDL_BUTTON_WHEELUP)
	 {
		 container_widget->up_arrow();
		 return GUI_YUM;
	 }

	return DraggableView::MouseDown(x, y, button);
}

GUI_status ContainerViewGump::MouseUp(int x, int y, int button)
{
	return DraggableView::MouseUp(x, y, button);
}
