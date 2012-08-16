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
#include "Event.h"
#include "Configuration.h"
#include "CommandBar.h"
#include "Usecode.h"

extern GUI_status inventoryViewButtonCallback(void *data);
extern GUI_status actorViewButtonCallback(void *data);

#define U6 Game::get_game()->get_game_type()==NUVIE_GAME_U6
#define SE Game::get_game()->get_game_type()==NUVIE_GAME_SE
#define MD Game::get_game()->get_game_type()==NUVIE_GAME_MD

static const uint8 ACTION_BUTTON = 3;

PartyView::PartyView(Configuration *cfg) : View(cfg)
{
 player = NULL; view_manager = NULL;
}

PartyView::~PartyView()
{

}

bool PartyView::init(void *vm, uint16 x, uint16 y, Text *t, Party *p, Player *pl, TileManager *tm, ObjManager *om)
{
 View::init(x,y,t,p,tm,om);
 // PartyView is 8px wider than other Views, for the arrows
 // ...and 3px taller, for the sky (SB-X)
 if(U6)
   SetRect(area.x, area.y, area.w+8, area.h+3);
 else
   SetRect(area.x, area.y, area.w, area.h);

 view_manager = vm;
 player = pl;
 row_offset = 0;

 return true;
}

GUI_status PartyView::MouseUp(int x,int y,int button)
{
 x -= area.x;
 y -= area.y;

 if(y < 18 && U6) // clicked on skydisplay
   return GUI_PASS;
 if(y < 4 && MD)
   return GUI_PASS;

 int rowH = 16;
 if(MD)
     rowH = 24;

 uint8 party_size = party->get_party_size();
 if(SE)
 {
     if(party_size > 7) party_size = 7;
 }
 else if(party_size > 5) party_size = 5; // can only display/handle 5 at a time

 SDL_Rect arrow_rects_U6[2] = {{0,18,8,8},{0,90,8,8}};
 SDL_Rect arrow_rects[2] = {{0,6,7,8},{0,102,7,8}};
 SDL_Rect arrow_up_rect_MD[1] = {{0,15,7,8}};

 if(HitRect(x,y,U6? arrow_rects_U6[0]: (MD ? arrow_up_rect_MD[0] : arrow_rects[0]))) //up arrow hit rect
   {
    if(up_arrow())
      Redraw();
    return GUI_YUM;
   }
  if(HitRect(x,y,U6? arrow_rects_U6[1]: arrow_rects[1])) //down arrow hit rect
   {
    if(down_arrow())
      Redraw();
    return GUI_YUM;
   }

 int x_offset = 7;
 int y_offset = 18;
 if(SE)
 {
     x_offset = 6;
     y_offset = 2;
 }
 else if(MD)
     y_offset = 4;
 if(y > party_size * rowH + y_offset-1) // clicked below actors
   return GUI_YUM;

 if(x >= x_offset)
  {
   Event *event = Game::get_game()->get_event();
   CommandBar *command_bar = Game::get_game()->get_command_bar();
   bool party_view_targeting;
   config->value("config/input/party_view_targeting", party_view_targeting, false);

   if(button == ACTION_BUTTON && event->get_mode() == MOVE_MODE
      && command_bar->get_selected_action() > 0) // Exclude attack mode too
   {
      if(command_bar->try_selected_action() == false) // start new action
         return GUI_PASS; // false if new event doesn't need target
   }
   if((party_view_targeting || (button == ACTION_BUTTON
      && command_bar->get_selected_action() > 0)) && event->can_target_icon())
   {
      x += area.x;
      y += area.y;
      Actor *actor = get_actor(x, y);
      if(actor)
      {
         event->select_actor(actor);
         return GUI_YUM;
      }
   }
   set_party_member(((y - y_offset) / rowH) + row_offset);
   if(x >= x_offset + 17) // clicked an actor name
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
    int rowH = 16;
    int y_offset = 18;
    if(MD)
    {
        rowH = 24; y_offset = 0;
    }
    if(SE)
    {
        y_offset = 2;
        if(party_size > 7) party_size = 7;
    }
    else if(party_size > 5) party_size = 5; // can only display/handle 5 at a time

    if(y > party_size * rowH + y_offset) // clicked below actors
      return NULL;

    if(x >= 8)
     {
    	return party->get_actor(((y - y_offset) / rowH) + row_offset);
     }

    return NULL;
}

bool PartyView::drag_accept_drop(int x, int y, int message, void *data)
{
	DEBUG(0,LEVEL_DEBUGGING,"PartyView::drag_accept_drop()\n");
	if(message == GUI_DRAG_OBJ)
	{
		MsgScroll *scroll = Game::get_game()->get_scroll();

		Obj *obj = (Obj*)data;
		Actor *actor = get_actor(x, y);
		if(actor)
		{
			Event *event = Game::get_game()->get_event();
			event->display_move_text(actor, obj);

			if((!Game::get_game()->get_usecode()->has_getcode(obj)
			    || Game::get_game()->get_usecode()->get_obj(obj, actor))
			   && event->can_move_obj_between_actors(obj, player->get_actor(), actor))
			{
				if(actor == player->get_actor()) // get
					player->subtract_movement_points(3);
				else // get plus move
					player->subtract_movement_points(8);
				if(!obj->is_in_inventory() &&
				   obj_manager->obj_is_damaging(obj, Game::get_game()->get_player()->get_actor()))
					return false;
				DEBUG(0,LEVEL_DEBUGGING,"Drop Accepted\n");
				return true;
			}
		}
		scroll->display_string("\n\n");
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
	if(actor)
	{
		obj_manager->moveto_inventory(obj, actor);
	}
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
 int rowH = 16;
 if(MD)
   rowH = 24;

 if(full_redraw || update_display)
  {
   update_display = false;
   uint8 end_offset = row_offset + 5;
   if(MD)
      fill_md_background(bg_color, area);
   else
      screen->fill(bg_color, area.x, area.y, area.w, area.h);
   if(U6)
      display_sun_moon_strip();

   display_arrows();

   if(SE)
	   end_offset = row_offset + 7;
   if(end_offset > party_size)
	   end_offset = party_size;

   for(i=row_offset;i<end_offset;i++)
     {
      int x_offset = 8;
      int y_offset = 18;
      hp_text_color = 0; //standard text color

      if(U6)
        hp_text_color = 0x48; //standard text color 
      if (SE)
      {
        x_offset = 6; y_offset = 1;
      }
      if(MD)
        y_offset = 5;
      actor = party->get_actor(i);
      actor_tile = tile_manager->get_tile(actor->get_downward_facing_tile_num());

      screen->blit(area.x+x_offset,area.y + y_offset + (i-row_offset)*rowH,actor_tile->data,8,16,16,16,true);
      actor_name = party->get_actor_name(i);

      if(SE)
      {
        x_offset = 4; y_offset = 0;
      }
      if(MD)
        y_offset = -3;
      // FIXME: Martian Dreams text is somewhat center aligned
      text->drawString(screen, actor_name, area.x + x_offset + 24, area.y + y_offset + (i-row_offset) * rowH + 8, 0);
      sprintf(hp_string,"%3d",actor->get_hp());
      if(actor->is_poisoned()) //actor is poisoned, display their hp in green
        hp_text_color = 0xa;
      else
       {
        if(actor->get_hp() < 10) //actor is critical, display their hp in red.
          hp_text_color = 0x0c; 
       }
      if(SE)
      {
        x_offset = -7; y_offset = 3;
      }
      if(MD)
      {
        x_offset = -16; y_offset = 14;
      }
      text->drawString(screen, hp_string, strlen(hp_string), area.x + x_offset + 112, area.y + y_offset + (i-row_offset) * rowH, 0, hp_text_color);
     }

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

    int height = 16;
    uint16 x = area.x + skypos[pos].x, y = area.y + skypos[pos].y;
    if (skypos[pos].y == 6) // goes through the bottom if not reduced
      height = 10;
    screen->blit(x,y, tile->data,8 ,16, height,16,true);
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
    if((row_offset+ (SE? 7:5)) < party->get_party_size())
    {
        row_offset++;
        return(true);
    }
    return(false);
}


void PartyView::display_arrows()
{
    int x_offset = 0; int y_offset = 0;
    uint8 font_color = 0x48;
    if(SE || MD)
    {
        font_color = 0;
        x_offset = 2;
        y_offset = 12;
    }
    uint8 max_party_size = 5;
    uint8 party_size = party->get_party_size();
    if(SE)
        max_party_size = 7;
    if(party_size <= max_party_size) // reset
        row_offset = 0;

    if((party_size - row_offset) > max_party_size) // display bottom arrow
        text->drawChar(screen, 25, area.x - x_offset, area.y + 90 + y_offset, font_color);
    if(MD)
        y_offset = 3;
    if(row_offset > 0) // display top arrow
        text->drawChar(screen, 24, area.x - x_offset, area.y + 18 - y_offset, font_color);
}
// </SB-X> 
