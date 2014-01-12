/*
 *  ActorView.cpp
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

#include "nuvieDefs.h"

#include "GUI_button.h"

#include "View.h"
#include "Actor.h"
#include "Party.h"
#include "Portrait.h"
#include "ActorView.h"
#include "Font.h"
#include "Player.h"
#include "Event.h"

extern GUI_status inventoryViewButtonCallback(void *data);
extern GUI_status partyViewButtonCallback(void *data);

#define MD Game::get_game()->get_game_type()==NUVIE_GAME_MD

ActorView::ActorView(Configuration *cfg) : View(cfg)
{
 portrait = NULL;
 portrait_data = NULL;
 in_party = false;
 cursor_pos.x = 2;
 cursor_pos.px = cursor_pos.py = 0;
 cursor_tile = NULL;
 show_cursor = false;
}

ActorView::~ActorView()
{
 if(portrait_data != NULL)
   free(portrait_data);
}

bool ActorView::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Font *f, Party *p, TileManager *tm, ObjManager *om, Portrait *port)
{
 if(Game::get_game()->get_game_type()==NUVIE_GAME_U6)
	View::init(x,y,f,p,tm,om);
 else
	View::init(x,y-2,f,p,tm,om);

 portrait = port;

 add_command_icons(tmp_screen, view_manager);

 set_party_member(0);
 cursor_tile = tile_manager->get_cursor_tile();

 return true;
}

bool ActorView::set_party_member(uint8 party_member)
{
 in_party = false;

 if(View::set_party_member(party_member)
    && !Game::get_game()->get_event()->using_control_cheat())
 {
    in_party = true;
    if(party_button) party_button->Show();
 }
 else
 {
    if(left_button) left_button->Hide();
    if(right_button) right_button->Hide();
    if(party_button) party_button->Hide();
 }

 if(portrait) // this might not be set yet. if called from View::init()
   {
    if(portrait_data)
      free(portrait_data);

    if(in_party)
        portrait_data = portrait->get_portrait_data(party->get_actor(cur_party_member));
    else
    {
        Player *player = Game::get_game()->get_player();
        portrait_data = portrait->get_portrait_data(player->get_actor());
    }
    if(portrait_data == NULL)
      return false;
   }

 return true;
}


void ActorView::Display(bool full_redraw)
{

 if(portrait_data != NULL && (full_redraw || update_display || Game::get_game()->is_original_plus_full_map()))
  {
   update_display = false;
   if(MD)
   {
     fill_md_background(area);
     screen->blit(area.x+1,area.y+16,portrait_data,8,portrait->get_portrait_width(),portrait->get_portrait_height(),portrait->get_portrait_width(), true);
   }
   else
   {
     screen->fill(bg_color, area.x, area.y, area.w, area.h);
     screen->blit(area.x,area.y+8,portrait_data,8,portrait->get_portrait_width(),portrait->get_portrait_height(),portrait->get_portrait_width(), false);
   }
   display_name();
   display_actor_stats();
   DisplayChildren(); //draw buttons
   screen->update(area.x, area.y, area.w, area.h);
  }

 if(show_cursor && cursor_tile != NULL)
  {
   screen->blit(cursor_pos.px, cursor_pos.py, (unsigned char *)cursor_tile->data,
                8, 16, 16, 16, true, NULL);
   screen->update(cursor_pos.px, cursor_pos.py, 16, 16);
  }

}

void ActorView::add_command_icons(Screen *tmp_screen, void *view_manager)
{
 int x_off = 0; // U6 and MD
 int y = 80; // U6
 Tile *tile;
 SDL_Surface *button_image;
 SDL_Surface *button_image2;

 if(Game::get_game()->get_game_type()==NUVIE_GAME_SE)
 {
	x_off = 1; y = 96;
 }
 else if(MD)
	y = 100;

 //FIX need to handle clicked button image, check image free on destruct.

 tile = tile_manager->get_tile(MD?282:387); //left arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 left_button = new GUI_Button(this, x_off, y, button_image, button_image2, this);
 this->AddWidget(left_button);

 tile = tile_manager->get_tile(MD?279:384); //party view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 party_button = new GUI_Button(view_manager, 16 + x_off, y, button_image, button_image2,this);
 this->AddWidget(party_button);

 tile = tile_manager->get_tile(MD?281:386); //inventory view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 inventory_button = new GUI_Button(view_manager, 2*16 + x_off, y, button_image, button_image2, this);
 this->AddWidget(inventory_button);

 tile = tile_manager->get_tile(MD?283:388); //right arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 right_button = new GUI_Button(this, 3*16 + x_off, y, button_image, button_image2, this);
 this->AddWidget(right_button);

 return;
}

void ActorView::display_name()
{
 char *name;
 int y_off = 0;
 if(MD)
	y_off = 4;
 else if(Game::get_game()->get_game_type()==NUVIE_GAME_SE)
	y_off = 1;

 if(in_party)
	name = party->get_actor_name(cur_party_member);
 else
	name = (char *) Game::get_game()->get_player()->get_actor()->get_name(true);

 if(name == NULL)
  return;

 font->drawString(screen, name, area.x + ((136) - strlen(name) * 8) / 2, area.y + y_off);

 return;
}

void ActorView::display_actor_stats()
{
 Actor *actor;
 char buf[10];
 int x_off = 0;
 int y_off = 0;
 uint8 hp_text_color = 0; //standard text color

 if (MD)
	x_off = -1;
 else if(Game::get_game()->get_game_type()==NUVIE_GAME_SE)
 {
	x_off = 2; y_off = - 6;
 }
 else //U6
	hp_text_color = 0x48; //standard text color)

 if(in_party)
	actor = party->get_actor(cur_party_member);
 else
	actor = Game::get_game()->get_player()->get_actor();

 if(actor->is_poisoned()) //actor is poisoned, display their hp in green
	hp_text_color = 0xa;
 else if(actor->get_hp() < 10) //actor is critical, display their hp in red.
	hp_text_color = 0x0c;

 sprintf(buf,"STR:%d",actor->get_strength());
 font->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16);

 sprintf(buf,"DEX:%d",actor->get_dexterity());
 font->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 8);

 sprintf(buf,"INT:%d",actor->get_intelligence());
 font->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 2 * 8);

 if (MD || Game::get_game()->get_game_type()==NUVIE_GAME_SE)
 {
	sprintf(buf,"HP:%3d",actor->get_hp());
	font->drawString(screen, buf, strlen(buf), area.x + 5 * 16 + x_off, area.y + y_off + 16 + 3 * 8, hp_text_color, 0);

	sprintf(buf,"HM:%3d",actor->get_maxhp());
	font->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 4 * 8);

	sprintf(buf,"Lev:%2d",actor->get_level());
	font->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 5 * 8);

	font->drawString(screen, "Exper:", area.x + 5 * 16 + x_off, area.y + y_off + 16 + 6 * 8);
	sprintf(buf,"%6d",actor->get_exp());
	font->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 7 * 8);
	return;
 }

 font->drawString(screen, "Magic", area.x + 5 * 16, area.y + 16 + 4 * 8);
 sprintf(buf,"%d/%d",actor->get_magic(),actor->get_maxmagic());
 font->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 5 * 8);

 font->drawString(screen, "Health", area.x + 5 * 16, area.y + 16 + 6 * 8);
 sprintf(buf,"%3d",actor->get_hp());
 font->drawString(screen, buf, strlen(buf), area.x + 5 * 16, area.y + 16 + 7 * 8, hp_text_color, 0);
 sprintf(buf,"   /%d",actor->get_maxhp());
 font->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 7 * 8);

 font->drawString(screen, "Lev/Exp", area.x + 5 * 16, area.y + 16 + 8 * 8);
 sprintf(buf,"%d/%d",actor->get_level(),actor->get_exp());
 font->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 9 * 8);

 return;
}

/* Move the cursor around and use command icons.
 */
GUI_status ActorView::KeyDown(SDL_keysym key)
{
	if(!show_cursor) // FIXME: don't rely on show_cursor to get/pass focus
		return(GUI_PASS);
	switch(key.sym)
	{
		//	keypad arrow keys
		case SDLK_KP1:
		case SDLK_KP7:
		case SDLK_LEFT:
		case SDLK_KP4:
			moveCursorToButton(cursor_pos.x - 1);
			break;
		case SDLK_KP9:
		case SDLK_KP3:
		case SDLK_RIGHT:
		case SDLK_KP6:
			moveCursorToButton(cursor_pos.x + 1);
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			select_button();
			break;
		case SDLK_UP: // would otherwise move invisible mapwindow cursor
		case SDLK_DOWN:
		case SDLK_KP8:
		case SDLK_KP2:
			break;
		default:
//			set_show_cursor(false); // newAction() can move cursor here
			return GUI_PASS;
	}
	return(GUI_YUM);
}

/* Put cursor over one of the command icons. */
void ActorView::moveCursorToButton(sint8 button_num)
{
	if(button_num < 0 || button_num > 3)
		return;
	cursor_pos.x = button_num;
	update_cursor();
	update_display = true;
}

/* Update on-screen location (px,py) of cursor.
 */
void ActorView::update_cursor()
{
	cursor_pos.px = ((cursor_pos.x + 1) * 16) - 16;
	cursor_pos.py = party_button->area.y;
	cursor_pos.px += area.x;
	//cursor_pos.py += area.y;
}

void ActorView::set_show_cursor(bool state)
{
	show_cursor = state;
	update_display = true;
}

void ActorView::select_button()
{
	ViewManager *view_manager = Game::get_game()->get_view_manager();
	if(cursor_pos.x == 0) // left
		View::callback(BUTTON_CB, left_button, view_manager);
	if(cursor_pos.x == 1) // party
		View::callback(BUTTON_CB, party_button, view_manager);
	if(cursor_pos.x == 2) // inventory
		View::callback(BUTTON_CB, inventory_button, view_manager);
	if(cursor_pos.x == 3) // right
		View::callback(BUTTON_CB, right_button, view_manager);
}
