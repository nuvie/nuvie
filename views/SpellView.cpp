/*
 *  SpellView.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jun 19 2011.
 *  Copyright (c) 2011. All rights reserved.
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

#include "Screen.h"
#include "U6LList.h"
#include "U6misc.h"
#include "U6Bmp.h"
#include "GUI.h"
#include "GUI_button.h"
#include "DollWidget.h"
#include "InventoryWidget.h"
#include "SpellView.h"
#include "Party.h"
#include "Text.h"
#include "Actor.h"
#include "Event.h"
#include "MapWindow.h"
#include "MsgScroll.h"
#include "UseCode.h"
#include "ViewManager.h"
#include "Script.h"
#include "U6objects.h"
#include "Magic.h"

static const char circle_num_tbl[][8] = {"1ST", "2ND", "3RD", "4TH", "5TH", "6TH", "7TH", "8TH"};
static const int obj_n_reagent[8]={OBJ_U6_MANDRAKE_ROOT, OBJ_U6_NIGHTSHADE, OBJ_U6_BLACK_PEARL, OBJ_U6_BLOOD_MOSS, OBJ_U6_SPIDER_SILK, OBJ_U6_GARLIC, OBJ_U6_GINSENG, OBJ_U6_SULFUROUS_ASH};

#define NEWMAGIC_BMP_W 144
#define NEWMAGIC_BMP_H 82

SpellView::SpellView(Configuration *cfg) : DraggableView(cfg)
{
	spell_container = NULL; background = NULL;
	level = 1; all_spells_mode = false;
	spell_num = 0; event_mode = false;
	num_spells_per_page = 8; caster = NULL;
}

SpellView::~SpellView()
{
}

bool SpellView::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
 View::init(x,y,t,p,tm,om);

 SetRect(area.x, area.y, NEWMAGIC_BMP_W, NEWMAGIC_BMP_H+16);
 string filename;

 config_get_path(config,"newmagic.bmp",filename);
 background = new U6Bmp();
 if(background->load(filename) == false)
	 return false;

 add_command_icons(tmp_screen, view_manager);

 return true;
}


void SpellView::PlaceOnScreen(Screen *s, GUI_DragManager *dm, int x, int y)
{
 GUI_Widget::PlaceOnScreen(s,dm,x,y);
}

void SpellView::set_spell_caster(Actor *actor, Obj *s_container, bool eventMode)
{
	caster = actor;
	spell_container = s_container;
	event_mode = eventMode;

	for(int shift=0;shift<8;shift++)
	{
		caster_reagents[shift] = caster->inventory_count_object(obj_n_reagent[shift]);
	}

	level = (spell_container->quality/16)+1;
	spell_num = spell_container->quality - (16 * level);


	if(spell_container->find_in_container(OBJ_U6_SPELL, MAGIC_ALL_SPELLS, OBJ_MATCH_QUALITY))
		all_spells_mode = true;
	else
		all_spells_mode = false;

	fill_cur_spell_list();
	update_buttons();
	Game::get_game()->set_mouse_pointer(1); // crosshairs
}

void SpellView::Display(bool full_redraw)
{
 if(full_redraw || update_display)
   {
    screen->fill(bg_color, area.x, area.y + NEWMAGIC_BMP_H, area.w, area.h - NEWMAGIC_BMP_H);

    screen->blit(area.x, area.y, background->get_data(), 8,  NEWMAGIC_BMP_W, NEWMAGIC_BMP_H, NEWMAGIC_BMP_W, true);
   }

 display_level_text();
 display_spell_list_text();

 DisplayChildren(full_redraw);
#if 1 // FIXME: This shouldn't need to be in the loop
 update_buttons(); // It doesn't seem to hurt speed though
 screen->update(area.x, area.y, area.w, area.h);
#else
 if(full_redraw || update_display)
   {
    update_display = false;
    screen->update(area.x, area.y, area.w, area.h);
   }
#endif
 return;
}

uint8 SpellView::fill_cur_spell_list()
{
	Magic *m = Game::get_game()->get_magic();
	int j=0;
	for(int i=0;i<16;i++)
	{
		cur_spells[i] = -1;

		if(m->get_spell((level-1)*16+i) != NULL && (all_spells_mode || spell_container->find_in_container(OBJ_U6_SPELL, (level-1)*16+i, OBJ_MATCH_QUALITY)))
			cur_spells[j++] = (level-1)*16+i;
	}

	return j;
}

sint8 SpellView::get_selected_index()
{
	for(uint8 i=0;i<16;i++)
	{
		if(cur_spells[i] == spell_container->quality)
		{
			return (sint8)i;
		}
	}

	return -1;
}

void SpellView::set_prev_level()
{
	if(level == 1)
		return;

	uint8 old_level = level;


	uint8 num_spells = 0;
	for(;num_spells == 0;)
	{
		level--;

		if(level == 0)
			break;

		num_spells = fill_cur_spell_list();
	}

	if(num_spells == 0)
	{
		level = old_level;
		fill_cur_spell_list();
	}

	if(num_spells > num_spells_per_page)
		spell_container->quality = cur_spells[num_spells_per_page];
	else
	{
		spell_container->quality = cur_spells[0];
	}
}

void SpellView::set_next_level()
{
	if(level == 8)
		return;

	uint8 old_level = level;


	uint8 num_spells = 0;
	for(;num_spells == 0;)
	{
		level++;

		if(level == 9)
			break;

		num_spells = fill_cur_spell_list();
	}

	if(num_spells == 0)
	{
		level = old_level;
		fill_cur_spell_list();
	}
	else
		spell_container->quality = cur_spells[0];
}

void SpellView::move_left()
{
	sint8 index = get_selected_index();
	if(index < 0)
		index = 0;

	if(index >= num_spells_per_page)
	{
		spell_container->quality = cur_spells[0];
	}
	else
	{
		set_prev_level();
	}

	update_buttons();
	update_display = true;
}

void SpellView::move_right()
{
	sint8 index = get_selected_index();
	if(index < 0)
		index = 0;

	if(index >= num_spells_per_page || cur_spells[num_spells_per_page] == -1)
	{
		set_next_level();
	}
	else
	{
		spell_container->quality = cur_spells[num_spells_per_page];
	}

	update_buttons();
	update_display = true;
}

void SpellView::move_up()
{
	sint8 index = get_selected_index();

	if(index > 0 && index != num_spells_per_page)
	{
		spell_container->quality = cur_spells[index-1];
		update_display = true;
	}
	else
		move_left();

}

void SpellView::move_down()
{
	sint8 index = get_selected_index();

	if(index != -1 && index < 15 && index != (num_spells_per_page-1))
	{
		if(cur_spells[index+1] != -1)
		{
			spell_container->quality = cur_spells[index+1];
			update_display = true;
		}
		else
			move_right();
	}
	else
		move_right();
}

void SpellView::display_level_text()
{
 text->drawString(screen, circle_num_tbl[level-1], area.x + 96 + 8, area.y+NEWMAGIC_BMP_H, 0);
 text->drawString(screen, "level", area.x + 96, area.y+NEWMAGIC_BMP_H+8, 0);
 return;
}

void SpellView::display_spell_list_text()
{
	Magic *m = Game::get_game()->get_magic();

	sint8 index = get_selected_index();

	if(index >= num_spells_per_page)
		index = num_spells_per_page;
	else
		index = 0;

	for(uint8 i=0;i<num_spells_per_page;i++)
	{
		sint16 spell_num = cur_spells[i+index];
		if(spell_num != -1)
		{
			Spell *spell = m->get_spell((uint8)spell_num);

			display_spell_text(spell, i, spell_container->quality);
		}
	}
}

void SpellView::display_spell_text(Spell *spell, uint16 line_num, uint8 selected_spell)
{
	char num_str[4];
	line_num++;

	text->drawString(screen, spell->name, area.x + 16, area.y+(line_num * 8), 0);
	snprintf(num_str, 3, "%d", get_available_spell_count(spell));
	text->drawString(screen, num_str, area.x + NEWMAGIC_BMP_W-24, area.y+(line_num * 8), 0);

	if(spell->num == selected_spell)
		text->drawChar(screen, 0x9a, area.x + 8, area.y+(line_num * 8));

 return;
}

uint16 SpellView::get_available_spell_count(Spell *s)
{
	sint32 min_reagents = -1;
	for(int shift=0;shift<8;shift++)
	{
		if(1<<shift&s->reagents)
		{
			if(min_reagents == -1 || caster_reagents[shift] < min_reagents)
				min_reagents = caster_reagents[shift];
		}
	}

	if(min_reagents == -1)
		min_reagents = 0;

	return (uint16)min_reagents;
}

void SpellView::add_command_icons(Screen *tmp_screen, void *view_manager)
{
 Tile *tile;
 SDL_Surface *button_image;
 SDL_Surface *button_image2;

 tile = tile_manager->get_tile(412); //left arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 left_button = new GUI_Button(this, 2*16, NEWMAGIC_BMP_H, button_image, button_image2, this);
 this->AddWidget(left_button);

 tile = tile_manager->get_tile(413); //right arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 right_button = new GUI_Button(this, 3*16, NEWMAGIC_BMP_H, button_image, button_image2, this);
 this->AddWidget(right_button);

}

void SpellView::event_mode_select_spell()
{
	sint16 spell_num = get_selected_spell();
	Game::get_game()->get_event()->select_spell_num(spell_num);
	release_focus();
}

/* Move the cursor around
 */
GUI_status SpellView::KeyDown(SDL_keysym key)
{
    switch(key.sym)
    {
        case SDLK_UP:
        case SDLK_KP8:
            move_up();
            break;
        case SDLK_DOWN:
        case SDLK_KP2:
            move_down();
            break;
        case SDLK_LEFT:
        case SDLK_KP4:
        	move_left();
            break;
        case SDLK_RIGHT:
        case SDLK_KP6:
            move_right();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        	if(event_mode)
        	{
        		event_mode_select_spell();
        		return GUI_YUM;
        	}

        	return GUI_PASS;

        	break;
        case SDLK_ESCAPE:
        	if(event_mode)
        	{
        		Game::get_game()->get_event()->select_spell_num(-1);
    			release_focus();
    			return GUI_YUM;
        	}
        	return GUI_PASS;
        	break;
        case SDLK_SPACE:
        	return GUI_PASS;
            break;
        case SDLK_TAB :

        	break;

        default:
            return GUI_PASS;
    }
    return(GUI_YUM);
}

GUI_status SpellView::MouseDown(int x, int y, int button)
{
	y -= area.y;
	x -= area.x;
	Event *event = Game::get_game()->get_event();

	if(button == SDL_BUTTON_WHEELUP)
	{
		move_up();
		return GUI_PASS;
	}
	if(button == SDL_BUTTON_WHEELDOWN)
	{
		move_down();
		return GUI_PASS;
	}
	if(x < 0 && y > 0 && y < 162) // cast selected spell on the map
	{
		int wx, wy;
		MapWindow *map_window = Game::get_game()->get_map_window();
		uint8 z = Game::get_game()->get_player()->get_actor()->get_z();

		simulate_return();
		if(event->get_mode() == INPUT_MODE)
		{
			y += area.y;
			x += area.x;
			map_window->mouseToWorldCoords(x, y, wx, wy);
			map_window->moveCursor(wx - map_window->get_cur_x(), wy - map_window->get_cur_y());
			event->select_target(uint16(wx), uint16(wy), z);
		}
		return GUI_YUM;
	}
	if(x > 1 && (y > 101 || x > 137)) // cancel spell
	{
		event->set_mode(CAST_MODE);
		event->cancelAction();
		return GUI_PASS;
	}
	if(y < 8 || y > 71 || x < 16 || x > 134) // do nothing
	{
		return GUI_YUM;
	}

	sint8 index = get_selected_index();

	if(index >= num_spells_per_page)
		index = num_spells_per_page;
	else
		index = 0;


	y = (y / num_spells_per_page) - 1;

	//printf("x = %d, y = %d index=%d\n", x, y, index);

	if(cur_spells[index+y] != -1)
	{
		spell_container->quality = cur_spells[index+y];
		update_display = true;

    	if(event_mode)
    	{
    		event_mode_select_spell();
    	}
    	else
    	{
    		simulate_return();
    	}
	}

	return GUI_YUM;
}

void SpellView::hide_buttons()
{
	if(left_button) left_button->Hide();
	if(right_button) right_button->Hide();
}

void SpellView::show_buttons()
{
	if(left_button) left_button->Show();
	if(right_button) right_button->Show();
}

void SpellView::update_buttons()
{
	show_buttons();
	sint8 index = get_selected_index();

	if(level == 1 && index <= (num_spells_per_page-1) && left_button)
		left_button->Hide();

	uint8 old_level = level;
	uint8 num_spells = 0;
	for(;num_spells == 0;)
	{
		level++;
		if(level == 9)
			break;
		num_spells = fill_cur_spell_list();
	}
	level = old_level;
	fill_cur_spell_list();

	if(right_button && ((level < 8 && num_spells == 0) || level == 8)
	   && cur_spells[num_spells_per_page*(1+index/num_spells_per_page)] == -1)
		right_button->Hide();
}

GUI_status SpellView::callback(uint16 msg, GUI_CallBack *caller, void *data)
{

	if(caller == left_button)
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

void SpellView::simulate_return() //Simulate a global key down event.
{
	SDL_Event e;
	e.type = SDL_KEYDOWN;
	e.key.keysym.sym = SDLK_RETURN;
	Game::get_game()->get_event()->handleEvent(&e);
}
