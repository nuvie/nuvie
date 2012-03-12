/*
 *  SpellViewGump.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Mar 8 2012.
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

#include "SpellViewGump.h"


#define NEWMAGIC_BMP_W 144
#define NEWMAGIC_BMP_H 82

SpellViewGump::SpellViewGump(Configuration *cfg) : SpellView(cfg)
{
	num_spells_per_page = 10;
	bg_image = NULL;
}

SpellViewGump::~SpellViewGump()
{
}

bool SpellViewGump::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
	View::init(x,y,t,p,tm,om);

	SetRect(area.x, area.y, 162, 108);

	std::string datadir = GUI::get_gui()->get_data_dir();
	std::string imagefile;
	std::string path;

	SDL_Surface *image, *image1;

	build_path(datadir, "images", path);
	datadir = path;
	build_path(datadir, "gumps", path);
	datadir = path;

	build_path(datadir, "gump_btn_up.bmp", imagefile);
	image = SDL_LoadBMP(imagefile.c_str());
	build_path(datadir, "gump_btn_down.bmp", imagefile);
	image1 = SDL_LoadBMP(imagefile.c_str());

	gump_button = new GUI_Button(NULL, 0, 9, image, image1, this);
	this->AddWidget(gump_button);

	build_path(datadir, "spellbook", path);
	datadir = path;

	build_path(datadir, "spellbook_left_arrow.bmp", imagefile);
	image = SDL_LoadBMP(imagefile.c_str());

	left_button = new GUI_Button(this, 27, 4, image, image, this);
	this->AddWidget(left_button);

	build_path(datadir, "spellbook_right_arrow.bmp", imagefile);
	image = SDL_LoadBMP(imagefile.c_str());

	right_button = new GUI_Button(this, 132, 4, image, image, this);
	this->AddWidget(right_button);

	return true;
}


uint8 SpellViewGump::fill_cur_spell_list()
{
	uint8 count = SpellView::fill_cur_spell_list();

	//load spell images
	uint8 i;
	char filename[24]; // spellbook_spell_xxx.bmp\0
	std::string datadir = GUI::get_gui()->get_data_dir();
	std::string path;

	build_path(datadir, "images", path);
	datadir = path;
	build_path(datadir, "gumps", path);
	datadir = path;
	build_path(datadir, "spellbook", path);
	datadir = path;

	std::string imagefile;

	SDL_FreeSurface(bg_image);

	//build_path(datadir, "", spellbookdir);

	build_path(datadir, "spellbook_bg.bmp", imagefile);
	bg_image = SDL_LoadBMP(imagefile.c_str());
	if(bg_image == NULL)
	{
		DEBUG(0,LEVEL_ERROR,"Failed to load spellbook_bg.bmp from '%s' directory\n", datadir.c_str());
		return count;
	}

	SDL_SetColorKey(bg_image, SDL_SRCCOLORKEY, SDL_MapRGB(bg_image->format, 0, 0x70, 0xfc));

	for(i=0;i<count;i++)
	{
		sprintf(filename, "spellbook_spell_%03d.bmp", cur_spells[i]);
		build_path(datadir, filename, imagefile);
		SDL_Surface *spell_image = SDL_LoadBMP(imagefile.c_str());
		if(spell_image == NULL)
		{
			DEBUG(0,LEVEL_ERROR,"Failed to load %s from '%s' directory\n", filename, datadir.c_str());
		}
		else
		{
			SDL_Rect dst;

			dst.w = 58;
			dst.h = 13;

			uint8 base = (level-1) * 16;

			uint8 spell = cur_spells[i] - base;
			dst.x = ((spell < 5) ? 25 : 88);
			dst.y = 18 + (spell % 5) * 14;
			SDL_BlitSurface(spell_image, NULL, bg_image, &dst);
			SDL_FreeSurface(spell_image);
		}
	}

	loadCircleString(datadir);

	return count;
}

void SpellViewGump::loadCircleString(std::string datadir)
{
	std::string imagefile;
	char filename[7]; // n.bmp\0

	sprintf(filename, "%d.bmp", level);
	build_path(datadir, filename, imagefile);

	SDL_Surface *s = SDL_LoadBMP(imagefile.c_str());
	if(s != NULL)
	{
		SDL_Rect dst;
		dst.x = 70;
		dst.y = 7;
		dst.w = 4;
		dst.h = 6;
		SDL_BlitSurface(s, NULL, bg_image, &dst);
	}

	switch(level)
	{
		case 1 : loadCircleSuffix(datadir, "st.bmp"); break;
		case 2 : loadCircleSuffix(datadir, "nd.bmp"); break;
		case 3 : loadCircleSuffix(datadir, "rd.bmp"); break;
		default: break;
	}
}

void SpellViewGump::loadCircleSuffix(std::string datadir, std::string image)
{
	std::string imagefile;

	build_path(datadir, image, imagefile);
	SDL_Surface *s = SDL_LoadBMP(imagefile.c_str());
	if(s != NULL)
	{
		SDL_Rect dst;
		dst.x = 75;
		dst.y = 7;
		dst.w = 7;
		dst.h = 6;
		SDL_BlitSurface(s, NULL, bg_image, &dst);
	}
}

void SpellViewGump::Display(bool full_redraw)
{
 //display_level_text();
 //display_spell_list_text();
 SDL_Rect dst;
 dst = area;
 dst.w = 162;
 dst.h = 108;
 SDL_BlitSurface(bg_image, NULL, surface, &dst);

 DisplayChildren(full_redraw);


 update_display = false;
 screen->update(area.x, area.y, area.w, area.h);

 return;
}

GUI_status SpellViewGump::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
	//close gump and return control to Magic class for clean up.
	if(caller == gump_button)
	{
		//Simulate a global key down event.
		SDL_Event e;
		e.type = SDL_KEYDOWN;
		e.key.keysym.sym = SDLK_ESCAPE;

		Game::get_game()->get_event()->handleEvent(&e);

		return GUI_YUM;
	}
	else if(caller == left_button)
	{
		move_left();
		return GUI_YUM;
	}
	else if(caller == right_button)
	{
		move_right();
		return GUI_YUM;
	}

    return GUI_PASS;
}

