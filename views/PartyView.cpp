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

#include "nuvieDefs.h"

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
 // PartyView is 8px bigger than other Views, for the arrows
 SetRect(area.x, area.y, area.w+8, area.h);

 view_manager = vm;
 player = pl;
 row_offset = 0;

 return true;
}

GUI_status PartyView::MouseUp(int x,int y,int button)
{
printf("PartyView:%x\n",this);
 x -= area.x;
 y -= area.y;

 if(x >= 8+16 || y < 16) // clicked right of actors or on skydisplay
   return GUI_PASS;

 uint8 party_size = party->get_party_size();
 if(party_size > 5) party_size = 5; // can only display/handle 5 at a time

 SDL_Rect arrow_rects[2] = {{0,17,8,8},{0,89,8,8}};
 if(HitRect(x,y,arrow_rects[0])) //up arrow hit rect
   {
    if(up_arrow())
      Redraw();
    return GUI_YUM;
   }
  if(HitRect(x,y,arrow_rects[1])) //down arrow hit rect
   {
    if(down_arrow())
      Redraw();
    return GUI_YUM;
   }

 if(y > party_size * 16 + 16) // clicked below actors
   return GUI_YUM;

 if(x >= 8) // clicked an actor icon
   {
    set_party_member(((y - 16) / 16) + row_offset);
    inventoryViewButtonCallback(view_manager);
   }
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

   for(i=row_offset;i<((party_size>=5)?5:party_size)+row_offset;i++)
     {
      actor = party->get_actor(i);
      actor_tile = tile_manager->get_tile(actor->get_downward_facing_tile_num());
      screen->blit(area.x+8,area.y+16+1+(i-row_offset)*16,actor_tile->data,8,16,16,16,true);

      actor_name = party->get_actor_name(i);
      text->drawString(screen, actor_name, area.x+8 + 16 + 8, area.y + 16 + 1 + (i-row_offset) * 16 + 8, 0);
      sprintf(hp_string,"%3d",actor->get_hp());
      text->drawString(screen, hp_string, area.x+8 + 112, area.y + 16 + 1 + (i-row_offset) * 16, 0);
     }

   display_arrows();

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
    screen->blit(area.x+8 +i*16,area.y,tile->data,8,16,16,16,true);
   }

 return;
}

void PartyView::display_dungeon_strip()
{
 uint8 i;
 Tile *tile;

 tile = tile_manager->get_tile(372);
 screen->blit(area.x+8,area.y,tile->data,8,16,16,16,true);

 tile = tile_manager->get_tile(373);

 for(i=1;i<8;i++)
   {
    screen->blit(area.x+8 +i*16,area.y,tile->data,8,16,16,16,true);
   }

 tile = tile_manager->get_tile(374);
 screen->blit(area.x+8 +7*16+8,area.y,tile->data,8,16,16,16,true);

 return;
}


bool PartyView::up_arrow()
{
    if(row_offset > 0)
    {
        row_offset--;
        return(true);
    }
    return(false);
}


bool PartyView::down_arrow()
{
    if((row_offset+5) < party->get_party_size())
    {
        row_offset++;
        return(true);
    }
    return(false);
}


void PartyView::display_arrows()
{
    uint8 party_size = party->get_party_size();
    if(party_size <= 5) // reset
        row_offset = 0;

    if(row_offset > 0) // display top arrow
        text->drawChar(screen, 24, area.x, area.y + 16 + 1, 0x48);

    if((party_size - row_offset) > 5) // display bottom arrow
        text->drawChar(screen, 25, area.x, area.y + 88 + 1, 0x48);
}
 
