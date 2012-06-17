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
#include <cmath>
#include "nuvieDefs.h"

#include "Actor.h"
#include "Party.h"
#include "Player.h"
#include "GameClock.h"
#include "PartyView.h"
#include "Text.h"
#include "Weather.h"
#include "Script.h"
#include "MsgScroll.h"

extern GUI_status inventoryViewButtonCallback(void *data);
extern GUI_status actorViewButtonCallback(void *data);

PartyView::PartyView(Configuration *cfg) : View(cfg)
{

}

PartyView::~PartyView()
{

}

bool PartyView::init(void *vm, uint16 x, uint16 y, Text *t, Party *p, Player *pl, TileManager *tm, ObjManager *om)
{
 View::init(x,y,t,p,tm,om);
 // PartyView is 8px wider than other Views, for the arrows
 // ...and 3px taller, for the sky (SB-X)
 SetRect(area.x, area.y, area.w+8, area.h+3);

 view_manager = vm;
 player = pl;
 row_offset = 0;

 return true;
}

GUI_status PartyView::MouseUp(int x,int y,int button)
{
 x -= area.x;
 y -= area.y;

 if(y < 18) // clicked on skydisplay
   return GUI_PASS;

 uint8 party_size = party->get_party_size();
 if(party_size > 5) party_size = 5; // can only display/handle 5 at a time

 SDL_Rect arrow_rects[2] = {{0,18,8,8},{0,90,8,8}};
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

 if(y > party_size * 16 + 18) // clicked below actors
   return GUI_YUM;

 if(x >= 8)
  {
   set_party_member(((y - 18) / 16) + row_offset);
   if(x >= 8+18) // clicked an actor name
     {
      actorViewButtonCallback(view_manager);
     }
   else // clicked an actor icon
     {
      inventoryViewButtonCallback(view_manager);
     }
  }
 return GUI_YUM;
}

Actor *PartyView::get_actor(int x, int y)
{
    x -= area.x;
    y -= area.y;

    uint8 party_size = party->get_party_size();
    if(party_size > 5) party_size = 5; // can only display/handle 5 at a time

    if(y > party_size * 16 + 18) // clicked below actors
      return NULL;

    if(x >= 8)
     {
    	return party->get_actor(((y - 18) / 16) + row_offset);
     }

    return NULL;
}

bool PartyView::drag_accept_drop(int x, int y, int message, void *data)
{
	DEBUG(0,LEVEL_DEBUGGING,"PartyView::drag_accept_drop()\n");
	if(message == GUI_DRAG_OBJ)
	{
		Obj *obj = (Obj*)data;
		Actor *actor = get_actor(x, y);

	    MsgScroll *scroll = Game::get_game()->get_scroll();
	    scroll->display_string("Move-");
	    scroll->display_string(obj_manager->look_obj(obj, OBJ_SHOW_PREFIX));
	    scroll->display_string(" To ");
	    scroll->display_string(actor->get_name());
	    scroll->display_string(".");

		if(actor && Game::get_game()->get_script()->call_actor_get_obj(actor, obj) == true)
		{
			DEBUG(0,LEVEL_DEBUGGING,"Drop Accepted\n");
			return true;
		}

		scroll->display_string("\n");
	    scroll->display_prompt();
	}

	Redraw();

	DEBUG(0,LEVEL_DEBUGGING,"Drop Refused\n");
	return false;
}

void PartyView::drag_perform_drop(int x, int y, int message, void *data)
{
 DEBUG(0,LEVEL_DEBUGGING,"InventoryWidget::drag_perform_drop()\n");
 Obj *obj;

 if(message == GUI_DRAG_OBJ)
   {
    DEBUG(0,LEVEL_DEBUGGING,"Drop into inventory.\n");
    obj = (Obj *)data;

	Actor *actor = get_actor(x, y);
    obj_manager->moveto_inventory(obj, actor);

    MsgScroll *scroll = Game::get_game()->get_scroll();
    scroll->display_string("\n\n");
    scroll->display_prompt();

    Redraw();
   }

 return;
}

void PartyView::Display(bool full_redraw)
{
 uint8 i;
 uint8 hp_text_color;
 Actor *actor;
 Tile *actor_tile;
 char *actor_name;
 char hp_string[4];
 uint8 party_size = party->get_party_size();

 if(full_redraw || update_display)
  {
   update_display = false;
   screen->fill(bg_color, area.x, area.y, area.w, area.h);

   display_sun_moon_strip();

   for(i=row_offset;i<((party_size>=5)?5:party_size)+row_offset;i++)
     {
      hp_text_color = 0x48; //standard text color
      actor = party->get_actor(i);
      actor_tile = tile_manager->get_tile(actor->get_downward_facing_tile_num());
      screen->blit(area.x+8,area.y+18+(i-row_offset)*16,actor_tile->data,8,16,16,16,true);

      actor_name = party->get_actor_name(i);
      text->drawString(screen, actor_name, area.x+8 + 16 + 8, area.y + 18 + (i-row_offset) * 16 + 8, 0);
      sprintf(hp_string,"%3d",actor->get_hp());
      if(actor->is_poisoned()) //actor is poisoned, display their hp in green
        hp_text_color = 0xa;
      else
       {
        if(actor->get_hp() < 10) //actor is critical, display their hp in red.
          hp_text_color = 0x0c; 
       }
      text->drawString(screen, hp_string, strlen(hp_string), area.x+8 + 112, area.y + 18 + (i-row_offset) * 16, 0, hp_text_color);
     }

   display_arrows();

   screen->update(area.x, area.y, area.w, area.h);
  }

 return;
}

void PartyView::display_sun_moon_strip()
{
 uint8 level = player->get_location_level();

 if(level == 0 || level == 5)
   display_surface_strip();
 else
   display_dungeon_strip();

 return;
}

void PartyView::display_surface_strip()
{
 uint8 i;
 Tile *tile;
 GameClock *clock = Game::get_game()->get_clock();
 Weather *weather = Game::get_game()->get_weather();
 bool eclipse = weather->is_eclipse();
 
 display_sun(clock->get_hour(), 0/*minutes*/, eclipse);

 if(!eclipse)
	 display_moons(clock->get_day(), clock->get_hour());

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
// <SB-X>
void PartyView::display_sun_moon(Tile *tile, uint8 pos)
{
    struct { sint16 x, y; } skypos[15] = // sky positions relative to area
    {
        { 8 + 7*16 - 0*8, 6 }, // 7*16 is the first position on the right side
        { 8 + 7*16 - 1*8, 3 },
        { 8 + 7*16 - 2*8, 1 },
        { 8 + 7*16 - 3*8, -1 },
        { 8 + 7*16 - 4*8, -2 },
        { 8 + 7*16 - 5*8, -3 },
        { 8 + 7*16 - 6*8, -4 },
        { 8 + 7*16 - 7*8, -4 },
        { 8 + 7*16 - 8*8, -4 },
        { 8 + 7*16 - 9*8, -3 },
        { 8 + 7*16 - 10*8, -2 },
        { 8 + 7*16 - 11*8, -1 },
        { 8 + 7*16 - 12*8, 1 },
        { 8 + 7*16 - 13*8, 3 },
        { 8 + 7*16 - 14*8, 6 }
    };

    uint16 x = area.x + skypos[pos].x, y = area.y + skypos[pos].y;
    screen->blit(x,y, tile->data,8,16,16,16,true);
}

void PartyView::display_sun(uint8 hour, uint8 minute, bool eclipse)
{
	uint16 sun_tile = 0;
	if(eclipse)
		sun_tile = 363; //eclipsed sun
	else if(hour == 5 || hour == 19)
		sun_tile = 361; //orange sun
	else if(hour > 5 && hour < 19)
		sun_tile = 362; //yellow sun
	else return; //no sun
	display_sun_moon(tile_manager->get_tile(sun_tile), hour - 5);
}

void PartyView::display_moons(uint8 day, uint8 hour, uint8 minute)
{
    uint8 phase = 0;
    // trammel (starts 1 hour ahead of sun)
    phase = uint8(nearbyint((day-1)/TRAMMEL_PHASE)) % 8;
    Tile *tileA = tile_manager->get_tile((phase == 0) ? 584 : 584 + (8-phase)); // reverse order in tilelist
    uint8 posA = ((hour + 1) + 3*phase) % 24; // advance 3 positions each phase-change

    // felucca (starts 1 hour behind sun)
    // ...my FELUCCA_PHASE may be wrong but this method works with it...
    sint8 phaseb = (day-1) % uint8(nearbyint(FELUCCA_PHASE*8)) - 1;
    phase = (phaseb >= 0) ? phaseb : 0;
    Tile *tileB = tile_manager->get_tile((phase == 0) ? 584 : 584 + (8-phase)); // reverse order in tilelist
    uint8 posB = ((hour - 1) + 3*phase) % 24; // advance 3 positions per phase-change

    if(posA >= 5 && posA <= 19)
        display_sun_moon(tileA, posA - 5);
    if(posB >= 5 && posB <= 19)
        display_sun_moon(tileB, posB - 5);
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
        text->drawChar(screen, 24, area.x, area.y + 18, 0x48);

    if((party_size - row_offset) > 5) // display bottom arrow
        text->drawChar(screen, 25, area.x, area.y + 90, 0x48);
}
// </SB-X> 
