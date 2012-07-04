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
#include "Text.h"
#include "Player.h"

extern GUI_status inventoryViewButtonCallback(void *data);
extern GUI_status partyViewButtonCallback(void *data);

#define MD Game::get_game()->get_game_type()==NUVIE_GAME_MD

ActorView::ActorView(Configuration *cfg) : View(cfg)
{
 portrait = NULL;
 portrait_data = NULL;
}

ActorView::~ActorView()
{
 if(portrait_data != NULL)
   free(portrait_data);
}

bool ActorView::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om, Portrait *port)
{
 if(Game::get_game()->get_game_type()==NUVIE_GAME_U6)
	View::init(x,y,t,p,tm,om);
 else
	View::init(x-8,y-2,t,p,tm,om);

 portrait = port;

 add_command_icons(tmp_screen, view_manager);

 set_party_member(0);

 return true;
}

bool ActorView::set_party_member(uint8 party_member)
{
 in_party = false;
 Player *player = Game::get_game()->get_player();

 // make sure we are currently controlling a party member
 if(!player->get_party()->contains_actor(player->get_actor()))
 {
    if(left_button) left_button->Hide();
    if(right_button) right_button->Hide();
    if(party_button) party_button->Hide();
 }
 else if(View::set_party_member(party_member))
 {
    in_party = true;
    if(party_button) party_button->Show();
 }
 else // shouldn't happen
    return false;

 if(portrait) // this might not be set yet. if called from View::init()
   {
    if(portrait_data)
      free(portrait_data);

    if(in_party)
        portrait_data = portrait->get_portrait_data(party->get_actor(cur_party_member));
    else
        portrait_data = portrait->get_portrait_data(player->get_actor());
    if(portrait_data == NULL)
      return false;
   }

 return true;
}


void ActorView::Display(bool full_redraw)
{
 if(portrait_data != NULL && (full_redraw || update_display))
  {
   update_display = false;
   screen->fill(bg_color, area.x, area.y, area.w, area.h);
   screen->blit(area.x,area.y+8,portrait_data,8,56,64,56,false);
   display_name();
   display_actor_stats();
   DisplayChildren(); //draw buttons
   screen->update(area.x, area.y, area.w, area.h);
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

 text->drawString(screen, name, area.x + ((136) - strlen(name) * 8) / 2, area.y + y_off, 0);

 return;
}

void ActorView::display_actor_stats()
{
 Actor *actor;
 char buf[10];
 int x_off = 0;
 int y_off = 0;
 if (MD)
	x_off = -1;
 else if(Game::get_game()->get_game_type()==NUVIE_GAME_SE)
 {
	x_off = 2; y_off = - 6;
 }

 if(in_party)
	actor = party->get_actor(cur_party_member);
 else
	actor = Game::get_game()->get_player()->get_actor();

 sprintf(buf,"STR:%d",actor->get_strength());
 text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16, 0);

 sprintf(buf,"DEX:%d",actor->get_dexterity());
 text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 8, 0);

 sprintf(buf,"INT:%d",actor->get_intelligence());
 text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 2 * 8, 0);

 if (MD || Game::get_game()->get_game_type()==NUVIE_GAME_SE)
 {
	sprintf(buf,"HP:%3d",actor->get_hp());
	text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 3 * 8, 0);

	sprintf(buf,"HM:%3d",actor->get_maxhp());
	text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 4 * 8, 0);

	sprintf(buf,"Lev:%2d",actor->get_level());
	text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 5 * 8, 0);

	text->drawString(screen, "Exper:", area.x + 5 * 16 + x_off, area.y + y_off + 16 + 6 * 8, 0);
	sprintf(buf,"%6d",actor->get_exp());
	text->drawString(screen, buf, area.x + 5 * 16 + x_off, area.y + y_off + 16 + 7 * 8, 0);
	return;
 }

 text->drawString(screen, "Magic", area.x + 5 * 16, area.y + 16 + 4 * 8, 0);
 sprintf(buf,"%d/%d",actor->get_magic(),actor->get_maxmagic());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 5 * 8, 0);

 text->drawString(screen, "Health", area.x + 5 * 16, area.y + 16 + 6 * 8, 0);
 sprintf(buf,"%3d/%d",actor->get_hp(),actor->get_maxhp());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 7 * 8, 0);

 text->drawString(screen, "Lev/Exp", area.x + 5 * 16, area.y + 16 + 8 * 8, 0);
 sprintf(buf,"%d/%d",actor->get_level(),actor->get_exp());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 9 * 8, 0);

 return;
}

