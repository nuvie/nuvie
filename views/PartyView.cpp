/*
 *  PartyView.cpp
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

#include "U6def.h"

#include "Actor.h"
#include "Party.h"
#include "Player.h"
#include "PartyView.h"
#include "Text.h"

extern GUI_status inventoryViewButtonCallback(void *data);

PartyView::PartyView(Configuration *cfg) : View(cfg)
{

}

PartyView::~PartyView()
{

}

bool PartyView::init(void *vm, uint16 x, uint16 y, Text *t, Party *p, Player *pl, TileManager *tm, ObjManager *om)
{
 View::init(x,y,t,p,tm,om);
 
 view_manager = vm;
 player = pl;
 
 return true;
}

GUI_status PartyView::MouseUp(int x,int y,int button)
{
 x -= area.x;
 y -= area.y;
 
 if(x >= 16 || y < 16)
   return GUI_PASS;

 uint8 party_size = party->get_party_size();
 
 if(y > party_size * 16 + 16)
	 return GUI_PASS;

 set_party_member((y - 16) / 16);
 
 inventoryViewButtonCallback(view_manager);
 
 return GUI_YUM;
}

void PartyView::Display(bool full_redraw)
{
 uint8 i;
 Actor *actor;
 Tile *actor_tile;
 char *actor_name;
 char hp_string[4];
 uint8 party_size = party->get_party_size();
 
 if(full_redraw || update_display)
  {
   update_display = false;
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   
   display_sun_moon_strip();

   if(party_size > 5)
     party_size = 5; //only show the first 5 party members.
     
   for(i=0;i<party_size;i++)
     {
      actor = party->get_actor(i);
      actor_tile = tile_manager->get_tile(actor->get_downward_facing_tile_num());
      screen->blit(area.x,area.y+16+1+i*16,actor_tile->data,8,16,16,16,true);
      
      actor_name = party->get_actor_name(i);
      text->drawString(screen, actor_name, area.x + 16 + 8, area.y + 16 + 1 + i * 16 + 8, 0);
      sprintf(hp_string,"%3d",actor->get_hp());
      text->drawString(screen, hp_string, area.x + 112, area.y + 16 + 1 + i * 16, 0);
     }
     
   screen->update(area.x, area.y, area.w, area.h);
  }
  
 return;
}

void PartyView::display_sun_moon_strip()
{
 uint8 level = player->get_location_level();
 
 if(level == 0)
   display_surface_strip();
 else
   display_dungeon_strip();

 return;
}

void PartyView::display_surface_strip()
{
 uint8 i;
 Tile *tile;
 
 for(i=0;i<9;i++)
   {
    tile = tile_manager->get_tile(352+i);
    screen->blit(area.x+i*16,area.y,tile->data,8,16,16,16,true);
   }

 return;
}

void PartyView::display_dungeon_strip()
{
 uint8 i;
 Tile *tile;
 
 tile = tile_manager->get_tile(372);
 screen->blit(area.x,area.y,tile->data,8,16,16,16,true);

 tile = tile_manager->get_tile(373);

 for(i=1;i<8;i++)
   {
    screen->blit(area.x+i*16,area.y,tile->data,8,16,16,16,true);
   }

 tile = tile_manager->get_tile(374);
 screen->blit(area.x+7*16+8,area.y,tile->data,8,16,16,16,true);

 return;
}
