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

extern GUI_status inventoryViewButtonCallback(void *data);
extern GUI_status partyViewButtonCallback(void *data);

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
 View::init(x,y,t,p,tm,om);
 
 portrait = port;
 
 add_command_icons(tmp_screen, view_manager);
 
 set_party_member(0);
 
 return true;
}

bool ActorView::set_party_member(uint8 party_member)
{
 if(View::set_party_member(party_member) == false)
  return false;

 if(portrait) // this might not be set yet. if called from View::init()
   {
    if(portrait_data)
      free(portrait_data);

    portrait_data = portrait->get_portrait_data(party->get_actor(cur_party_member));
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
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   screen->blit(area.x,area.y+8,portrait_data,8,56,64,56,false);
   display_name();
   display_actor_stats();
   DisplayChildren(); //draw buttons
   screen->update(area.x, area.y, area.w, area.h);
  }
 
}

void ActorView::add_command_icons(Screen *tmp_screen, void *view_manager)
{
 Tile *tile;
 SDL_Surface *button_image;
 SDL_Surface *button_image2;
 GUI_Widget *button;
 
 //FIX need to handle clicked button image, check image free on destruct.
 
 tile = tile_manager->get_tile(387); //left arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(this, 0, 80, button_image, button_image2, viewLeftButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(384); //party view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(view_manager, 16, 80, button_image, button_image2,partyViewButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(386); //inventory view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(view_manager, 2*16, 80, button_image, button_image2, inventoryViewButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(388); //right arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(this, 3*16, 80, button_image, button_image2, viewRightButtonCallback);
 this->AddWidget(button);
 
 return;
}

void ActorView::display_name()
{
 char *name;
 
 name = party->get_actor_name(cur_party_member);
 
 if(name == NULL)
  return;
  
 text->drawString(screen, name, area.x + ((136) - strlen(name) * 8) / 2, area.y, 0);
 
 return;
}

void ActorView::display_actor_stats()
{
 Actor *actor;
 char buf[10];
 
 actor = party->get_actor(cur_party_member);
 
 sprintf(buf,"STR:%d",actor->get_strength());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16, 0);

 sprintf(buf,"DEX:%d",actor->get_dexterity());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 8, 0);

 sprintf(buf,"INT:%d",actor->get_intelligence());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 2 * 8, 0);

 text->drawString(screen, "Magic", area.x + 5 * 16, area.y + 16 + 4 * 8, 0);
 sprintf(buf,"%d/%d",actor->get_magic(),0);
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 5 * 8, 0);
 
 text->drawString(screen, "Health", area.x + 5 * 16, area.y + 16 + 6 * 8, 0);
 sprintf(buf,"%3d/%d",actor->get_hp(),actor->get_maxhp());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 7 * 8, 0);

 text->drawString(screen, "Lev/Exp", area.x + 5 * 16, area.y + 16 + 8 * 8, 0);
 sprintf(buf,"%d/%d",actor->get_level(),actor->get_exp());
 text->drawString(screen, buf, area.x + 5 * 16, area.y + 16 + 9 * 8, 0);

 return;
}

