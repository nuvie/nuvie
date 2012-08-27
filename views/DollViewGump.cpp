/*
 *  DollViewGump.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Mar 19 2012.
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
#include <math.h>
#include "nuvieDefs.h"
#include "U6misc.h"
#include "Event.h"
#include "GUI.h"
#include "GUI_button.h"

#include "Party.h"
#include "Actor.h"
#include "ViewManager.h"

#include "ContainerViewGump.h"
#include "DollWidget.h"
#include "DollViewGump.h"


DollViewGump::DollViewGump(Configuration *cfg) : DraggableView(cfg),
	bg_image(NULL), gump_button(NULL), combat_button(NULL), heart_button(NULL), party_button(NULL), inventory_button(NULL),
	doll_widget(NULL), font(NULL), actor(NULL)
{
	bg_image = NULL;
}

DollViewGump::~DollViewGump()
{
}

bool DollViewGump::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
	View::init(x,y,t,p,tm,om);

	SetRect(area.x, area.y, 108, 136);

	actor = p->get_actor(p->get_leader());

	doll_widget = new DollWidget(config, this);
	doll_widget->init(actor, 26, 16, tile_manager, obj_manager);

	AddWidget(doll_widget);

	std::string datadir = GUI::get_gui()->get_data_dir();
	std::string imagefile;
	std::string path;

	SDL_Surface *image, *image1;

	build_path(datadir, "images", path);
	datadir = path;
	build_path(datadir, "gumps", path);
	datadir = path;

	gump_button = loadButton(datadir, "gump", 0, 112);

	build_path(datadir, "left_arrow.bmp", imagefile);
	image = SDL_LoadBMP(imagefile.c_str());

	left_button = new GUI_Button(this, 23, 7, image, image, this);
	this->AddWidget(left_button);

	build_path(datadir, "right_arrow.bmp", imagefile);
	image = SDL_LoadBMP(imagefile.c_str());

	right_button = new GUI_Button(this, 86, 7, image, image, this);
	this->AddWidget(right_button);

	build_path(datadir, "doll", path);
	datadir = path;

	build_path(datadir, "doll_bg.bmp", imagefile);
	bg_image = SDL_LoadBMP(imagefile.c_str());

	SDL_SetColorKey(bg_image, SDL_SRCCOLORKEY, SDL_MapRGB(bg_image->format, 0, 0x70, 0xfc));

	build_path(datadir, "combat_btn_up.bmp", imagefile);
	image = SDL_LoadBMP(imagefile.c_str());
	build_path(datadir, "combat_btn_down.bmp", imagefile);
	image1 = SDL_LoadBMP(imagefile.c_str());

	combat_button = new GUI_Button(NULL, 23, 92, image, image1, this);
	this->AddWidget(combat_button);

	heart_button = loadButton(datadir, "heart", 23, 108);
	party_button = loadButton(datadir, "party", 47, 108);
	inventory_button = loadButton(datadir, "inventory", 71, 108);

	font = new GUI_Font(GUI_FONT_GUMP);
	font->SetColoring( 0x08, 0x08, 0x08, 0x80, 0x58, 0x30, 0x00, 0x00, 0x00);

	return true;
}




void DollViewGump::Display(bool full_redraw)
{
 //display_level_text();
 //display_spell_list_text();
 SDL_Rect dst;
 dst = area;
 dst.w = 108;
 dst.h = 136;
 SDL_BlitSurface(bg_image, NULL, surface, &dst);

 int w,h;
 font->TextExtent(actor->get_name(), &w, &h);
 if(w < 58)
 {
	 w = (58 - w) / 2;
 }
 else
	 w = 0;
 font->TextOut(screen->get_sdl_surface(), area.x + 29 + w, area.y + 7, actor->get_name());

 displayEquipWeight();

 DisplayChildren(full_redraw);


 update_display = false;
 screen->update(area.x, area.y, area.w, area.h);

 return;
}

void DollViewGump::displayEquipWeight()
{
	uint8 strength = actor->get_strength();
	float equip_weight = ceilf(actor->get_inventory_equip_weight());
	char string[4]; //nnn\0

	snprintf(string, 4, "%d", (int)equip_weight);
	font->TextOut(screen->get_sdl_surface(), area.x + (((int)equip_weight > 9) ? 59 : 64), area.y + 82, string);

	snprintf(string, 4, "%d", strength);
	font->TextOut(screen->get_sdl_surface(), area.x + ((strength > 9) ? 76 : 81), area.y + 82, string);
}

GUI_status DollViewGump::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
	Event *event = Game::get_game()->get_event();
	//close gump and return control to Magic class for clean up.
	if(caller == gump_button)
	{
		//FIXME close gump.
		release_focus();
		Hide();
		return GUI_YUM;
	}
	else if(caller == right_button)
	{
		actor = party->get_actor((party->get_member_num(actor) + 1) % party->get_party_size());
		doll_widget->set_actor(actor);
	}
	else if(caller == left_button)
	{
		uint8 party_mem_num = party->get_member_num(actor);
		if(party_mem_num > 0)
			party_mem_num--;
		else
			party_mem_num = party->get_party_size() - 1;

		actor = party->get_actor(party_mem_num);
		doll_widget->set_actor(actor);
	}
	else if(caller == inventory_button)
	{
		Game::get_game()->get_view_manager()->open_container_view(actor);
	}
	else if(caller == heart_button)
	{
		Game::get_game()->get_view_manager()->open_portrait_gump(actor);
	}
	else if(caller == combat_button && event->get_mode() != INPUT_MODE
	        && event->get_mode() != ATTACK_MODE && event->get_mode() != CAST_MODE)
	{
		event->newAction(COMBAT_MODE);
	}
	else if(caller == party_button) // FIXME: What is this supposed to do?
	{

	}
	else if(caller == doll_widget)
	{
		Event *event = Game::get_game()->get_event();
		if(event->get_mode() != MOVE_MODE && event->get_mode() != EQUIP_MODE)
		{
			Obj *obj = (Obj *)data;
			event->select_view_obj(obj, actor);
		}
	}

    return GUI_PASS;
}

GUI_status DollViewGump::MouseDown(int x, int y, int button)
{
	return DraggableView::MouseDown(x, y, button);
}

GUI_status DollViewGump::MouseUp(int x, int y, int button)
{
	return DraggableView::MouseUp(x, y, button);
}
