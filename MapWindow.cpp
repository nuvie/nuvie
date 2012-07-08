/*
 *  MapWindow.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Mar 21 2003.
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
#include <cassert>
#include "nuvieDefs.h"

#include "Configuration.h"
#include "U6misc.h"
#include "U6LList.h"
#include "Actor.h"
#include "TileManager.h"
#include "ActorManager.h"
#include "MapWindow.h"
#include "Map.h"
#include "Event.h"
#include "MsgScroll.h"
#include "Effect.h" /* for initial fade-in */

#include "AnimManager.h"
#include "SoundManager.h"

#include "GUI.h"
#include "GUI_widget.h"
#include "Game.h"
#include "GameClock.h"
#include "GamePalette.h"
#include "Weather.h"
#include "Script.h"

#define USE_BUTTON 1 /* FIXME: put this in a common location */
#define WALK_BUTTON 3
#define ACTION_BUTTON 3
#define DRAG_BUTTON 1

// This should make the mouse-cursor hovering identical to that in U6.
static const uint8 movement_array[9 * 9] =
{
    9, 9, 2, 2, 2, 2, 2, 3, 3,
    9, 9, 9, 2, 2, 2, 3, 3, 3,
    8, 9, 9, 2, 2, 2, 3, 3, 4,
    8, 8, 8, 9, 2, 3, 4, 4, 4,
    8, 8, 8, 8, 1, 4, 4, 4, 4,
    8, 8, 8, 7, 6, 5, 4, 4, 4,
    8, 7, 7, 6, 6, 6, 5, 5, 4,
    7, 7, 7, 6, 6, 6, 5, 5, 5,
    7, 7, 6, 6, 6, 6, 6, 5, 5
};

static const Tile grid_tile = {
		0,
		false,
		false,
		false,
		false,
		false,
		true,
		false,
		false,
		0,
		//uint8 qty;
		//uint8 flags;

		0,
		0,
		0,

		{
				54,255,255,255,58,255,255,255,58,255,255,255,58,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				58,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				58,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				58,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
				255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255

		}
};

MapWindow::MapWindow(Configuration *cfg): GUI_Widget(NULL, 0, 0, 0, 0)
{

 config = cfg;
 config->value("config/GameType",game_type);

 uint16 x_off = config_get_video_x_offset(config);
 uint16 y_off = config_get_video_y_offset(config);

 GUI_Widget::Init(NULL, x_off, y_off, 0, 0);

 screen = NULL;
 //surface = NULL;
 anim_manager = NULL;

 cur_x = 0; mousecenter_x = 0;
 cur_y = 0; mousecenter_y = 0;
 cur_x_add = cur_y_add = 0;
 vel_x = vel_y = 0;
 last_boundary_fill_x = last_boundary_fill_y = 0;

 cursor_x = 0;
 cursor_y = 0;
 show_cursor = false;
 show_use_cursor = false;
 show_grid = false;
 x_ray_view = false;
 freeze_blacking_location = false;
 enable_blacking = true;

 new_thumbnail = false;
 thumbnail = NULL;
 overlay = NULL;
 overlay_level = MAP_OVERLAY_DEFAULT;

 cur_level = 0;

 tmp_map_buf = NULL;

 selected_obj = NULL;
 selected_actor = NULL;
 config->value("config/cheats/enable_hackmove", hackmove);
 walking = false;
 config->value("config/input/enable_doubleclick",enable_doubleclick,true);

 roof_mode = Game::get_game()->is_roof_mode();

 draw_brit_lens_anim = false;
 draw_garg_lens_anim = false;

 window_updated = true;
 roof_display = ROOF_DISPLAY_NORMAL;
}

MapWindow::~MapWindow()
{
 set_overlay(NULL); // free
 free(tmp_map_buf);
 delete anim_manager;
}

bool MapWindow::init(Map *m, TileManager *tm, ObjManager *om, ActorManager *am)
{
// int game_type; Why is this local, and retrieved again here? --SB-X

 game = Game::get_game();
 map = m;
 tile_manager = tm;
 obj_manager = om;
 actor_manager = am;
 uint16 map_w = 11, map_h = 11;

 if(game->is_new_style())
 {
	 offset_x -= 8;
	 offset_y -= 4;
	 map_w = 21;
	 map_h = 13;
 }

 anim_manager = new AnimManager(offset_x, offset_y);

// config->value("config/GameType",game_type);

 switch(game_type)
   {
    case NUVIE_GAME_U6 : cursor_tile = tile_manager->get_tile(365);
                         use_tile = tile_manager->get_tile(364);
                         break;

    case NUVIE_GAME_MD : cursor_tile = tile_manager->get_tile(265);
                         use_tile = tile_manager->get_tile(264);
                         break;

    case NUVIE_GAME_SE : cursor_tile = tile_manager->get_tile(381);
                         use_tile = tile_manager->get_tile(380);
                         break;
   }

 area.x = offset_x;
 area.y = offset_y;

 set_windowSize(map_w,map_h);

 // hide the window until game is fully loaded and does fade-in
 get_overlay(); // this allocates `overlay`
 overlay_level = MAP_OVERLAY_ONTOP;
 assert(SDL_FillRect(overlay, NULL, game->get_palette()->get_bg_color()) == 0);

 if(enable_doubleclick)
   set_accept_mouseclick(true, USE_BUTTON); // allow double-clicks (single-clicks aren't used for anything)

 wizard_eye_info.eye_tile = tile_manager->get_tile(TILE_U6_WIZARD_EYE);
 wizard_eye_info.moves_left = 0;
 wizard_eye_info.caller = NULL;

 if(roof_mode)
	 loadRoofTiles();

 return true;
}

bool MapWindow::set_windowSize(uint16 width, uint16 height)
{
 win_width = width;
 win_height = height;

 area.w = win_width * 16;
 area.h = win_height * 16;

 // We make the temp map +1 bigger on the top and left edges
 // and +2 bigger on the bottom and right edges

 // The +1 is for the boundary fill function

 // The additional +1 on the right/bottom edges is needed
 // to hide objects on boundarys when wall is in darkness

 tmp_map_width = win_width + 3;
 tmp_map_height = win_height + 3;

 tmp_map_buf = (uint16 *)nuvie_realloc(tmp_map_buf, tmp_map_width * tmp_map_height * sizeof(uint16));
 if(tmp_map_buf == NULL)
   return false;

// if(surface != NULL)
//   delete surface;
// surface = new Surface;

// if(surface->init(win_width*16,win_height*16) == false)
//   return false;
if(game->is_orig_style())
{
 clip_rect.x = area.x+8;
 clip_rect.w = (win_width - 1) * 16;
 clip_rect.h = (win_height - 1) * 16;

 if(game_type == NUVIE_GAME_U6)
   clip_rect.y = area.y+8;
 else
   {
    clip_rect.y = area.y+16;
    clip_rect.h -= 16;
   }
}
else
{
	 clip_rect.x = 0;//area.x;
	 clip_rect.y = 0;//area.y;
	 clip_rect.w = win_width * 16;
	 clip_rect.h = win_height * 16;
}
 anim_manager->set_area(clip_rect);

 reset_mousecenter();

 updateBlacking();

 return true;
}

void MapWindow::set_show_cursor(bool state)
{
 show_cursor = state;
}

void MapWindow::set_show_use_cursor(bool state)
{
 show_use_cursor = state;
}

void MapWindow::set_show_grid(bool state)
{
	show_grid = state;
}

void MapWindow::set_x_ray_view(bool state)
{
    x_ray_view = state;
    updateBlacking();
}

void MapWindow::set_freeze_blacking_location(bool state)
{
    freeze_blacking_location = state;
}

void MapWindow::set_enable_blacking(bool state)
{
	enable_blacking = state;
	updateBlacking();
}

void MapWindow::moveLevel(uint8 new_level)
{
 cur_level = new_level;

 updateBlacking();
}

void MapWindow::moveMap(sint16 new_x, sint16 new_y, sint8 new_level, uint8 new_x_add, uint8 new_y_add)
{
 uint16 map_side_length;

 if(new_level == 0)
   map_side_length = 1024;
 else
   map_side_length = 256;
/*
 if(new_x >= 0 && new_x <= map_side_length - win_width)
    {
     if(new_y >= 0 && new_y <= map_side_length - win_height)
        {
 */
         cur_x = new_x;
         cur_y = new_y;
         cur_level = new_level;
         cur_x_add = new_x_add;
         cur_y_add = new_y_add;
         updateBlacking();
 //       }
 //    }
}

void MapWindow::moveMapRelative(sint16 rel_x, sint16 rel_y)
{
 moveMap(cur_x + rel_x, cur_y + rel_y, cur_level);
}

/* Move map by relative pixel amount.
 */
void MapWindow::shiftMapRelative(sint16 rel_x, sint16 rel_y)
{
    uint8 tile_pitch = 16;
    uint32 total_px = (cur_x * tile_pitch) + cur_x_add,
           total_py = (cur_y * tile_pitch) + cur_y_add;
    total_px += rel_x;
    total_py += rel_y;
    moveMap(total_px / tile_pitch, total_py / tile_pitch, cur_level,
            total_px % tile_pitch, total_py % tile_pitch);
}

/* Center MapWindow on a location.
 */
void MapWindow::centerMap(uint16 x, uint16 y, uint8 z)
{
 moveMap(x - ((win_width - 1) / 2), y - ((win_height - 1) / 2), z);
}

void MapWindow::centerMapOnActor(Actor *actor)
{
 uint16 x;
 uint16 y;
 uint8 z;

 actor->get_location(&x,&y,&z);

 centerMap(x, y, z);

 return;
}

void MapWindow::centerCursor()
{

 cursor_x = (win_width - 1) / 2;
 cursor_y = (win_height - 1) / 2;

 return;
}

void MapWindow::moveCursor(sint16 new_x, sint16 new_y)
{
 if(new_x < 0 || new_x >= win_width)
   return;

 if(new_y < 0 || new_y >= win_height)
   return;

 cursor_x = new_x;
 cursor_y = new_y;

 return;
}

void MapWindow::moveCursorRelative(sint16 rel_x, sint16 rel_y)
{
 moveCursor(cursor_x + rel_x, cursor_y + rel_y);
}


const char *MapWindow::look(uint16 x, uint16 y, bool show_prefix)
{
 Actor *actor;

 if(tmp_map_buf[(y+1) * tmp_map_width + (x+1)] == 0) //black area
   return tile_manager->lookAtTile(0,0,true); // nothing to see here. ;)

 actor = actor_manager->get_actor(cur_x + x, cur_y + y, cur_level);
 if(actor != NULL)
   return actor_manager->look_actor(actor, show_prefix);

 return map->look(cur_x + x, cur_y + y, cur_level);
}


Obj *MapWindow::get_objAtCursor()
{

 if(tmp_map_buf[(cursor_y+1) * tmp_map_width + (cursor_x+1)] == 0) //black area
   return NULL; // nothing to see here. ;)

 return obj_manager->get_obj(cur_x + cursor_x, cur_y + cursor_y, cur_level,OBJ_SEARCH_TOP, OBJ_EXCLUDE_IGNORED);
}

Actor *MapWindow::get_actorAtCursor()
{
 //Actor *actor;

 if(tmp_map_buf[(cursor_y+1) * tmp_map_width + (cursor_x+1)] == 0) //black area
   return NULL; // nothing to see here. ;)

 return actor_manager->get_actor(cur_x + cursor_x, cur_y + cursor_y, cur_level);
}

MapCoord MapWindow::get_cursorCoord()
{
 return( MapCoord( cur_x+cursor_x, cur_y+cursor_y, cur_level) );
}

void MapWindow::get_level(uint8 *level)
{
 *level = cur_level;
}

void MapWindow::get_pos(uint16 *x, uint16 *y, uint8 *px, uint8 *py)
{
 *x = cur_x;
 *y = cur_y;
 if(px)
   *px = cur_x_add;
 if(py)
   *py = cur_y_add;
}

void MapWindow::get_windowSize(uint16 *width, uint16 *height)
{
	*width = win_width;
	*height = win_height;
}

/* Returns true if the location at the coordinates is visible on the map window.
 */
bool MapWindow::in_window(uint16 x, uint16 y, uint8 z)
{
    return( (z == cur_level && x >= cur_x && x <= (cur_x + win_width)
             && y >= cur_y && y <= (cur_y + win_height)) );
}


/* Update player position if walking to mouse cursor. Update map position.
 */
void MapWindow::update()
{
    GameClock *clock = game->get_clock();
    Event *event = game->get_event();
    static bool game_started = false; // set to true on the first update()
    static uint32 last_update_time = clock->get_ticks();
    uint32 update_time = clock->get_ticks();

    // do fade-in on the first update (game has loaded now)
    if(game_started == false)
    {
//        new FadeEffect(FADE_PIXELATED_ONTOP, FADE_IN, 0x31);
        new GameFadeInEffect(game->get_palette()->get_bg_color());
        game_started = true;
    }

    anim_manager->update(); // update animations

    if(vel_x || vel_y) // this slides the map
    {
        if((update_time - last_update_time) >= 100) // only move every 10th sec
        {
            sint32 sx = vel_x / 10, sy = vel_y / 10;
            if(vel_x && !sx) // move even if vel_x/vel_y was < 10
                sx = (vel_x < 0) ? -1 : 1;
            if(vel_y && !sy)
                sy = (vel_y < 0) ? -1 : 1;

            shiftMapRelative(sx, sy);
            last_update_time = update_time;
        }
    }

    if(walking && !game->user_paused())
    {
        int mx, my; // bit-AND buttons with mouse state to test
        if(SDL_GetMouseState(&mx, &my) & (SDL_BUTTON(USE_BUTTON) | SDL_BUTTON(ACTION_BUTTON)))
        {
        	mx = screen->get_translated_x((uint16)mx);
        	my = screen->get_translated_y((uint16)my);
        	if(is_wizard_eye_mode())
        	{
        		int wx, wy;
        		mouseToWorldCoords(mx, my, wx, wy);
        		sint16 rx, ry;
        	    get_movement_direction((uint16)wx, (uint16)wy, rx, ry);
        	    moveMapRelative((rx == 0) ? 0 : rx < 0 ? -1 : 1,
        	                         (ry == 0) ? 0 : ry < 0 ? -1 : 1);
        	    wizard_eye_update();
        	}
        	else
        		event->walk_to_mouse_cursor((uint32)mx,
        									(uint32)my);
        }
    }

}

// moved from updateBlacking() so you don't have to update all blacking (SB-X)
void MapWindow::updateAmbience()
{
     //Dusk starts at 19:00
     //It's completely dark by 20:00
     //Dawn starts at 5:00
     //It's completely bright by 5:45
     //Dusk and dawn operate by changing the ambient light, not by changing the radius of the avatar's light globe

    if(!screen)
        return;

     GameClock *clock = game->get_clock();
	 Weather *weather = game->get_weather();
	
     int h = clock->get_hour();

	 // TODO: Move this out so that the configuration doesn't need to be iterated through each time
	 int min_brightness;
     config->value("config/cheats/min_brightness", min_brightness, 0);
 
	 int a;
     if(x_ray_view == true)
         a = 255;
     else if(in_dungeon_level())
	     a = min_brightness;
     else if( weather->is_eclipse() ) //solar eclipse
         a = min_brightness;
     else if( h == 19 ) //Dusk -- Smooth transition between 255 and min_brightness during first 59 minutes
         a = 255 - (uint8)( ( 255.0f - min_brightness ) * (float)clock->get_minute() / 59.0f );
     else if( h == 5 ) //Dawn -- Smooth transition between min_brightness and 255 during first 45 minutes
         a = min_brightness + ( 255.0f - min_brightness ) * (float)clock->get_minute() / 45.0f;
     else if( h > 5 && h < 19 ) //Day
         a = 255;
     else //Night
         a = min_brightness;
 
     if(a > 255)
         a = 255;
     if(a < 0xab && clock->get_timer(GAMECLOCK_TIMER_U6_LIGHT) != 0) //FIXME U6 specific
    	 a = 0xaa; //FIXME this is an approximation

	 screen->set_ambient( a );

     //Clear the opacity map
     screen->clearalphamap8( 0, 0, win_width, win_height, screen->get_ambient() );
}

void MapWindow::updateBlacking()
{
 generateTmpMap();

 updateAmbience();

 m_ViewableObjects.clear();
 m_ViewableTiles.clear();

 draw_brit_lens_anim = false;
 draw_garg_lens_anim = false;

 window_updated = true;
}

void MapWindow::Display(bool full_redraw)
{
 uint16 i,j;
 uint16 *map_ptr;
 uint16 map_width;
 Tile *tile;
 //unsigned char *ptr;



 //map_ptr = map->get_map_data(cur_level);
 map_width = map->get_width(cur_level);

 //map_ptr += cur_y * map_width + cur_x;
  map_ptr = tmp_map_buf;
  map_ptr += (1 * tmp_map_width + 1);// * sizeof(uint16); //remember our tmp map is 1 bigger all around.

  for(i=0;i<win_height;i++)
  {
   for(j=0;j<win_width;j++)
     {
      sint16 draw_x = area.x + (j*16), draw_y = area.y + (i*16);
      //draw_x -= (cur_x_add <= draw_x) ? cur_x_add : draw_x;
      //draw_y -= (cur_y_add <= draw_y) ? cur_y_add : draw_y;
      draw_x -= cur_x_add;
      draw_y -= cur_y_add;
      if(map_ptr[j] == 0)
        screen->clear(draw_x,draw_y,16,16,&clip_rect); //blackout tile.
      else
        {
         if(map_ptr[j] >= 16 && map_ptr[j] < 48) //lay down the base tile for shoreline tiles
           {
            tile = tile_manager->get_anim_base_tile(map_ptr[j]);
            screen->blit(draw_x,draw_y,(unsigned char *)tile->data,8,16,16,16,tile->transparent,&clip_rect);
           }

         tile = tile_manager->get_tile(map_ptr[j]);
         screen->blit(draw_x,draw_y,(unsigned char *)tile->data,8,16,16,16,tile->transparent,&clip_rect);

         if(window_updated)
           {
            TileInfo ti;
            ti.t=tile;
            ti.x=j+cur_x;
            ti.y=i+cur_y;
            m_ViewableTiles.push_back(ti);
           }
        }

     }
   //map_ptr += map_width;
   map_ptr += tmp_map_width ;//* sizeof(uint16);
  }

 drawObjs();

 //drawAnims();

 if(roof_mode && roof_display != ROOF_DISPLAY_OFF)
 {
	 drawRoofs();
 }

 if(game->get_clock()->get_timer(GAMECLOCK_TIMER_U6_STORM) != 0) //FIXME u6 specific.
   drawRain();

 if(show_grid)
 {
	drawGrid();
 }

 if(show_cursor)
  {
   screen->blit(area.x+cursor_x*16,area.y+cursor_y*16,(unsigned char *)cursor_tile->data,8,16,16,16,true,&clip_rect);
  }

 if(show_use_cursor)
  {
   screen->blit(area.x+cursor_x*16,area.y+cursor_y*16,(unsigned char *)use_tile->data,8,16,16,16,true,&clip_rect);
  }

// screen->fill(0,8,8,win_height*16-16,win_height*16-16);

 screen->blitalphamap8(area.x, area.y, &clip_rect);

 if(game->get_clock()->get_timer(GAMECLOCK_TIMER_U6_INFRAVISION) != 0)
   drawActors();

 if(overlay && overlay_level == MAP_OVERLAY_DEFAULT)
   screen->blit(area.x, area.y, (unsigned char *)(overlay->pixels), overlay->format->BitsPerPixel, overlay->w, overlay->h, overlay->pitch, true, &clip_rect);

 if(new_thumbnail)
   create_thumbnail();

 if(is_wizard_eye_mode())
	 screen->blit((win_width/2)*16,(win_height/2)*16,(unsigned char *)wizard_eye_info.eye_tile->data,8,16,16,16,true,&clip_rect);

 if(game->is_orig_style())
	 drawBorder();

 if(overlay && overlay_level == MAP_OVERLAY_ONTOP)
   screen->blit(area.x, area.y, (unsigned char *)(overlay->pixels), overlay->format->BitsPerPixel, overlay->w, overlay->h, overlay->pitch, true, &clip_rect);

// ptr = (unsigned char *)screen->get_pixels();
// ptr += 8 * screen->get_pitch() + 8;

// screen->blit(8,8,ptr,8,(win_width-1) * 16,(win_height-1) * 16, win_width * 16, false);

 if(game->is_orig_style())
	 screen->update(area.x+8,area.y+8,win_width*16-16,win_height*16-16);
 else
	 screen->update(area.x,area.y,win_width*16,win_height*16);

 if(window_updated)
  {
   window_updated = false;
   game->get_sound_manager()->update_map_sfx();
  }

}

void MapWindow::drawActors()
{
 uint16 i;
 Actor *actor;

 for(i=0;i < 256;i++)
   {
    actor = actor_manager->get_actor(i);

    if(actor->z == cur_level)
      {
       if(actor->x >= cur_x && actor->x < cur_x + win_width)
         {
          if(actor->y >= cur_y && actor->y < cur_y + win_height)
            {
             if(tmp_map_buf[(actor->y - cur_y + 1) * tmp_map_width + (actor->x - cur_x + 1)] != 0)
               {
                drawActor(actor);
               }
            }
         }
      }
   }
}

//FIX need a function for multi-tile actors.
inline void MapWindow::drawActor(Actor *actor)
{
    if(actor->is_visible()/* && actor->obj_n != 0*/
       && (!(actor->obj_flags&OBJ_STATUS_INVISIBLE) || actor->is_in_party() || actor == actor_manager->get_player())
       && actor->get_corpser_flag() == false)
    {
        Tile *tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->obj_n)+actor->frame_n);
        Tile *rtile = 0;

        if(actor->obj_flags&OBJ_STATUS_INVISIBLE)
        {
            rtile = new Tile(*tile);
            for(int x = 0; x < 256; x++)
                if(rtile->data[x] != 0x00)
                    rtile->data[x] = 0xFF;
                else
                    rtile->data[x] = 0x0B;
        }
        else if(actor->status_flags&ACTOR_STATUS_PROTECTED) // actually this doesn't appear when using a protection ring
        {
            rtile = new Tile(*tile);
            for(int x = 0; x < 256; x++)
                if(rtile->data[x] == 0x00)
                    rtile->data[x] = 0x0C;
        }
        else if(actor->is_cursed())
        {
            rtile = new Tile(*tile);
            for(int x = 0; x < 256; x++)
                if(rtile->data[x] == 0x00)
                    rtile->data[x] = 0x9;
        }
        if(rtile != 0)
        {
            drawNewTile(rtile, actor->x-cur_x,actor->y-cur_y, false);
            drawNewTile(rtile, actor->x-cur_x,actor->y-cur_y, true);
            delete rtile;
        }
        else
        {
            drawTile(tile, actor->x-cur_x,actor->y-cur_y, false);
            drawTile(tile, actor->x-cur_x,actor->y-cur_y, true);
        }
 
        // draw light coming from actor
        if(actor->light > 0 && screen->updatingalphamap)
        {
            //if(actor->light > 5)
            //    DEBUG(0,LEVEL_WARNING,"%s's light level is %d\n",actor->get_name(),actor->light);
            screen->drawalphamap8globe(actor->x-cur_x, actor->y-cur_y, actor->light);
        }
    }
}

void MapWindow::drawObjs()
{
 //FIX we need to make this more efficent.

 drawObjSuperBlock(true,false); //draw force lower objects
 drawObjSuperBlock(false,false); //draw lower objects

 drawActors();

 drawAnims();

 drawObjSuperBlock(false,true); //draw top objects

 drawLensAnim();
 return;
}

inline void MapWindow::drawLensAnim()
{
	if(draw_brit_lens_anim)
	{
		if(cur_x < 0x399)
			drawTile(tile_manager->get_tile(TILE_U6_BRITANNIAN_LENS_ANIM_2),0x398 - cur_x, 0x353 - cur_y, true);
		if(cur_x + win_width > 0x39a)
			drawTile(tile_manager->get_tile(TILE_U6_BRITANNIAN_LENS_ANIM_1),0x39a - cur_x, 0x353 - cur_y, true);
	}

	if(draw_garg_lens_anim)
	{
		if(cur_x < 0x39d)
			drawTile(tile_manager->get_tile(TILE_U6_GARGOYLE_LENS_ANIM_2),0x39c - cur_x, 0x353 - cur_y, true);
		if(cur_x + win_width > 0x39e)
			drawTile(tile_manager->get_tile(TILE_U6_GARGOYLE_LENS_ANIM_1),0x39e - cur_x, 0x353 - cur_y, true);
	}
}

void MapWindow::drawObjSuperBlock(bool draw_lowertiles, bool toptile)
{
 U6Link *link;
 U6LList *obj_list;
 Obj *obj;
 sint16 x,y;
 uint16 stop_x, stop_y;

 if(cur_x < 0)
   stop_x = 0;
 else
   stop_x = cur_x;

 if(cur_y < 0)
   stop_y = 0;
 else
   stop_y = cur_y;

    for(y=cur_y+win_height; y >= stop_y; y--)
      {
       for(x=cur_x+win_width;x >= stop_x; x--)
         {
          obj_list =obj_manager->get_obj_list(x,y,cur_level);
          if(obj_list)
           {
            for(link=obj_list->start();link != NULL;link=link->next)
              {
               obj = (Obj *)link->data;
               drawObj(obj, draw_lowertiles, toptile);
              }
           }
         }
      }

}

inline void MapWindow::drawObj(Obj *obj, bool draw_lowertiles, bool toptile)
{
 sint16 x,y;
 Tile *tile;

 y = obj->y - cur_y;
 x = obj->x - cur_x;

 if(x < 0 || y < 0)
   return;

 if(window_updated)
 {
   m_ViewableObjects.push_back(obj);

   if(game_type == NUVIE_GAME_U6 && cur_level == 0 && obj->y == 0x353 && tmp_map_buf[(y+1)*tmp_map_width+(x+1)] != 0)
   {
	   if(obj->obj_n == 394 && obj->x == 0x399)
	   {
		   draw_brit_lens_anim = true;
	   }
	   else if(obj->obj_n == 396 && obj->x == 0x39d)
	   {
		   draw_garg_lens_anim = true;
	   }
   }
 }

 //don't show invisible objects. 
 if(obj->status & OBJ_STATUS_INVISIBLE)
    return;
    
 tile = tile_manager->get_original_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);

 if(draw_lowertiles == false && (tile->flags3 & 0x4) && toptile == false) //don't display force lower tiles.
   return;

 if(draw_lowertiles == true && !(tile->flags3 & 0x4))
   return;

 if(tmp_map_buf[(y+1)*tmp_map_width+(x+1)] == 0) //don't draw object if area is in darkness.
    return;
 else
    {
     // We don't show objects on walls if the area to the right or bottom of the wall is in darkness
     if(tmp_map_buf[(y+1)*tmp_map_width+(x+2)] == 0 && !(tile->flags1 & TILEFLAG_WALL))
        return;
     else
      {
       if(tmp_map_buf[(y+2)*tmp_map_width+(x+1)] == 0 && !(tile->flags1 & TILEFLAG_WALL))//(obj->obj_n < 290 || obj->obj_n > 302))
         return;
      }
    }

 //Draw a lightglobe in the middle of the 16x16 tile.
 if( !draw_lowertiles &&
     toptile &&
     tile->flags2 & 0x3 &&
     screen->updatingalphamap )
	 	 screen->drawalphamap8globe( obj->x - cur_x, obj->y - cur_y, (tile->flags2 & 0x3) );


  drawTile(tile,obj->x - cur_x, obj->y - cur_y, toptile);

}

/* The pixeldata in the passed Tile pointer will be used if use_tile_data is
 * true, otherwise the current tile is derived from tile_num. This can't be
 * used with animated tiles. It only applies to the base tile in multi-tiles.
 */
inline void MapWindow::drawTile(Tile *tile, uint16 x, uint16 y, bool toptile,
                                bool use_tile_data)
{
 bool dbl_width, dbl_height;
 uint16 tile_num;

 tile_num = tile->tile_num;

 if(window_updated)
   {
    TileInfo ti;
    ti.t=tile;
    ti.x=x;
    ti.y=y;
    m_ViewableTiles.push_back(ti);
   }

 dbl_width = tile->dbl_width;
 dbl_height = tile->dbl_height;

 if(x < win_width && y < win_height)
   drawTopTile(use_tile_data?tile:tile_manager->get_tile(tile_num),x,y,toptile);

 if(dbl_width)
   {
    tile_num--;
    if(x > 0 && y < win_height)
      {
       tile = tile_manager->get_tile(tile_num);
       drawTopTile(tile,x-1,y,toptile);
      }
   }

 if(dbl_height)
   {
    tile_num--;
    if(y > 0 && x < win_width)
      {
       tile = tile_manager->get_tile(tile_num);
       drawTopTile(tile,x,y-1,toptile);
      }
   }

 if(x > 0 && dbl_width && y > 0 && dbl_height)
   {
    tile_num--;
    tile = tile_manager->get_tile(tile_num);
    drawTopTile(tile,x-1,y-1,toptile);
   }

}

inline void MapWindow::drawNewTile(Tile *tile, uint16 x, uint16 y, bool toptile)
{
    drawTile(tile, x,y, toptile, true);
}

inline void MapWindow::drawTopTile(Tile *tile, uint16 x, uint16 y, bool toptile)
{



// if(tile->boundary)
//  {
//    screen->blit(cursor_tile->data,8,x*16,y*16,16,16,false);
//   }
// FIXME: Don't use pixel offset (x_add,y_add) here, pass it via params?
 if(toptile)
    {
     if(tile->toptile)
//        screen->blit(x*16,y*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
        screen->blit(area.x+(x*16)-cur_x_add,area.y+(y*16)-cur_y_add,tile->data,8,16,16,16,tile->transparent,&clip_rect);
    }
 else
    {
     if(!tile->toptile)
//        screen->blit(x*16,y*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
        screen->blit(area.x+(x*16)-cur_x_add,area.y+(y*16)-cur_y_add,tile->data,8,16,16,16,tile->transparent,&clip_rect);
    }
}

void MapWindow::drawBorder()
{
 Tile *tile;
 Tile *tile1;
 uint16 i;

 if(game_type != NUVIE_GAME_U6)
   return;

 tile = tile_manager->get_tile(432);
 screen->blit(area.x,area.y,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(434);
 screen->blit(area.x+(win_width-1)*16,area.y,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(435);
 screen->blit(area.x,area.y+(win_height-1)*16,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(437);
 screen->blit(area.x+(win_width-1)*16,area.y+(win_height-1)*16,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(433);
 tile1 = tile_manager->get_tile(436);

 for(i=1;i < win_width-1;i++)
   {
    screen->blit(area.x+i*16,area.y,tile->data,8,16,16,16,true,&clip_rect);
    screen->blit(area.x+i*16,area.y+(win_height-1)*16,tile1->data,8,16,16,16,true,&clip_rect);
   }

 tile = tile_manager->get_tile(438);
 tile1 = tile_manager->get_tile(439);

  for(i=1;i < win_height-1;i++)
   {
    screen->blit(area.x,area.y+i*16,tile->data,8,16,16,16,true,&clip_rect);
    screen->blit(area.x+(win_width-1)*16,area.y+i*16,tile1->data,8,16,16,16,true,&clip_rect);
   }
}

void MapWindow::drawRoofs()
{
	if(roof_display == ROOF_DISPLAY_NORMAL && map->has_roof(cur_x + (win_width - 1) / 2, cur_y + (win_height - 1) / 2, cur_level)) //Don't draw roof tiles if player is underneath.
		return;

	uint16 *roof_map_ptr = map->get_roof_data(cur_level);

    SDL_Rect src, dst;
    src.w = 16;
    src.h = 16;
    dst.w = 16;
    dst.h = 16;
	if(roof_map_ptr)
	{
		roof_map_ptr += cur_y * 1024 + cur_x;
	for(uint16 i=0;i<win_height;i++)
	  {
	   for(uint16 j=0;j<win_width;j++)
	     {
		   if(roof_map_ptr[j] != 0)
		   {
	      dst.x = area.x + (j*16);
	      dst.y = area.y + (i*16);
	      dst.x -= cur_x_add;
	      dst.y -= cur_y_add;

	       src.x = (roof_map_ptr[j] % MAPWINDOW_ROOFTILES_IMG_W) * 16;
	       src.y = (roof_map_ptr[j] / MAPWINDOW_ROOFTILES_IMG_W) * 16;

	       SDL_BlitSurface(roof_tiles, &src, surface, &dst);
		   }
	     }
	   roof_map_ptr += 1024;
	  }
	}
}

void MapWindow::drawRain()
{
	int c = win_width * win_height;
	for(int i=0;i<c;i++)
	{
		//FIXME this assumes we have a mapwindow border. fix this when we go full screen.
		uint16 x = area.x + NUVIE_RAND()%((win_width-1)*16-2) + 8;
		uint16 y = area.y + NUVIE_RAND()%((win_height-1)*16-2) + 8;

		//FIXME the original does something with the palette if a pixel is black then draw gray etc.
		//We can't do this easily here because we don't have the original 8 bit display surface.
		screen->put_pixel(118, x, y);
		screen->put_pixel(118, x+1, y+1);
		screen->put_pixel(0, x+2, y+2);
	}
}

void MapWindow::drawGrid()
{
	for(uint16 i=0;i<win_height;i++)
	  {
	   for(uint16 j=0;j<win_width;j++)
	     {
		   screen->blit(area.x + (j*16) - cur_x_add, area.y + (i*16) - cur_y_add, (unsigned char *)grid_tile.data, 8, 16, 16, 16, true);
	     }
	  }
}

void MapWindow::generateTmpMap()
{
 unsigned char *map_ptr;
 uint16 pitch;
 uint16 x, y;
 Tile *tile;
 
 map_ptr = map->get_map_data(cur_level);
 pitch = map->get_width(cur_level);

 if(enable_blacking == false)
 {
	 uint16 *ptr = tmp_map_buf;
	for(y=0;y<tmp_map_height;y++)
	{
		for(x=0;x<tmp_map_width;x++)
		{
			uint16 x1 = cur_x + x - 1;
			uint16 y1 = cur_y + y - 1;
			WRAP_COORD(x1,cur_level);
			WRAP_COORD(y1,cur_level);
			*ptr = map_ptr[y1 * pitch + x1];
			ptr++;
		}
	}
	return;
 }

 memset(tmp_map_buf, 0, tmp_map_width * tmp_map_height * sizeof(uint16));

 if(freeze_blacking_location == false)
  {
   x = cur_x + ((win_width - 1) / 2);
   y = cur_y + ((win_height - 1) / 2);
  }
 else // SB-X
  {
   x = last_boundary_fill_x;
   y = last_boundary_fill_y;
  }
 
 WRAP_COORD(x,cur_level);
 WRAP_COORD(y,cur_level);
 
 //This is for U6. Sherry needs to pass through walls
 //We shift the boundary fill start location off the wall tile so it flood
 //fills correctly. We move east for vertical wall tiles and south for
 //horizontal wall tiles. 
 if(game_type == NUVIE_GAME_U6 && obj_manager->is_boundary(x,y,cur_level))
  {
   tile = obj_manager->get_obj_tile(x, y, cur_level, false);
   if((tile->flags1 & TILEFLAG_WALL_MASK) == (TILEFLAG_WALL_NORTH | TILEFLAG_WALL_SOUTH))
     x = WRAPPED_COORD(x + 1, cur_level);
   else
     y = WRAPPED_COORD(y + 1, cur_level);
  }
 last_boundary_fill_x = x; last_boundary_fill_y = y;
 boundaryFill(map_ptr, pitch, x, y);

 reshapeBoundary();
}

void MapWindow::boundaryFill(unsigned char *map_ptr, uint16 pitch, uint16 x, uint16 y)
{
 unsigned char current;
 uint16 *ptr;
 uint16 pos;
 uint16 p_cur_x, p_cur_y; //wrapped cur_x - 1 and wrapped cur_y - 1
 
 p_cur_x = WRAPPED_COORD(cur_x - 1,cur_level);
 p_cur_y = WRAPPED_COORD(cur_y - 1,cur_level);
	
 if(x == WRAPPED_COORD(p_cur_x - 1, cur_level) || x == WRAPPED_COORD(p_cur_x + tmp_map_width, cur_level))
   return;

 if(y == WRAPPED_COORD(p_cur_y - 1, cur_level) || y == WRAPPED_COORD(p_cur_y + tmp_map_height, cur_level))
   return;
 
 if(p_cur_y > y)
	 pos = pitch - p_cur_y + y;
 else
	 pos = y - p_cur_y;
 
 pos *= tmp_map_width;
 
 if(p_cur_x > x)
	 pos += pitch - p_cur_x + x;
	else
		pos += x - p_cur_x;

 ptr = &tmp_map_buf[pos];

 if(*ptr != 0)
   return;

 current = map_ptr[y * pitch + x];

 *ptr = (uint16)current;

 if(!x_ray_view && map->is_boundary(x,y,cur_level)) //hit the boundary wall tiles
  {
   if(boundaryLookThroughWindow(*ptr, x, y) == false)
      return;
  }

 uint16 xp1,xm1;
 uint16 yp1,ym1;
 
 xp1 = WRAPPED_COORD(x+1,cur_level);
 xm1 = WRAPPED_COORD(x-1,cur_level);

 yp1 = WRAPPED_COORD(y+1,cur_level);
 ym1 = WRAPPED_COORD(y-1,cur_level);
 
 boundaryFill(map_ptr, pitch, xp1,   y);
 boundaryFill(map_ptr, pitch,   x, yp1);
 boundaryFill(map_ptr, pitch, xp1, yp1);
 boundaryFill(map_ptr, pitch, xm1, ym1);
 boundaryFill(map_ptr, pitch, xm1,   y);
 boundaryFill(map_ptr, pitch,   x, ym1);
 boundaryFill(map_ptr, pitch, xp1, ym1);
 boundaryFill(map_ptr, pitch, xm1, yp1);


 return;
}

bool MapWindow::boundaryLookThroughWindow(uint16 tile_num, uint16 x, uint16 y)
{
 Tile *tile;
 Actor *actor;
 uint16 a_x, a_y;
 uint8 a_z;
 Obj  *obj;

 tile = tile_manager->get_tile(tile_num);
 if(!(tile->flags2 & TILEFLAG_WINDOW))
   {
    obj = obj_manager->get_objBasedAt(x,y,cur_level,true);
    if(obj) //check for a windowed object.
      {
       tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
       if(!(tile->flags2 & TILEFLAG_WINDOW))
          return false;
      }
    else
       return false;
   }

 actor = actor_manager->get_player();
 actor->get_location(&a_x,&a_y,&a_z);

 if(a_x == x)
   {
    if(a_y == WRAPPED_COORD(y - 1,cur_level) || a_y == WRAPPED_COORD(y + 1,cur_level))
      return true;
   }

 if(a_y == y)
   {
    if(a_x == WRAPPED_COORD(x - 1,cur_level) || a_x == WRAPPED_COORD(x + 1,cur_level))
       return true;
   }

 return false;
}

//reshape walls based on new blacked out areas.

void MapWindow::reshapeBoundary()
{
 uint16 x,y;
 uint8 flag, original_flag;
 Tile *tile;

 for(y=1;y <= win_height;y++)
   {
    for(x=1;x <= win_width;x++)
      {
       if(tmpBufTileIsBoundary(x,y))
         {
          tile = tile_manager->get_tile(tmp_map_buf[y*tmp_map_width + x]);

          if((tile->tile_num >= 140 && tile->tile_num <= 187)) //main U6 wall tiles FIX for WOU games
            {
             flag = 0;
             original_flag = tile->flags1 & TILEFLAG_WALL_MASK;
            }
          else
            continue;

          //generate the required wall flags
          if(tmpBufTileIsWall(x, y-1, NUVIE_DIR_N))
            flag |= TILEFLAG_WALL_NORTH;
          if(tmpBufTileIsWall(x+1, y, NUVIE_DIR_E))
            flag |= TILEFLAG_WALL_EAST;
          if(tmpBufTileIsWall(x, y+1, NUVIE_DIR_S))
            flag |= TILEFLAG_WALL_SOUTH;
          if(tmpBufTileIsWall(x-1, y, NUVIE_DIR_W))
            flag |= TILEFLAG_WALL_WEST;

          //we want to keep existing tile if it is pointing to non-wall tiles which are not blacked
          //this is used to support cookfire walls which aren't considered walls in tileflags.
          
          if(tmpBufTileIsBlack(x,y-1) == false && (original_flag & TILEFLAG_WALL_NORTH))
            flag |= TILEFLAG_WALL_NORTH;
          if(tmpBufTileIsBlack(x+1,y) == false && (original_flag & TILEFLAG_WALL_EAST))
            flag |= TILEFLAG_WALL_EAST;
          if(tmpBufTileIsBlack(x,y+1) == false && (original_flag & TILEFLAG_WALL_SOUTH))
            flag |= TILEFLAG_WALL_SOUTH;
          if(tmpBufTileIsBlack(x-1,y) == false && (original_flag & TILEFLAG_WALL_WEST))
            flag |= TILEFLAG_WALL_WEST;

          if(flag == 0) //isolated border tiles
            continue;

          if(flag == 48) // 0011 top right corner
            {
             if(tmpBufTileIsBlack(x,y-1) && tmpBufTileIsBlack(x+1,y)) //replace with blacked corner tile
               {
                //Oh dear! this is evil. FIX
                tmp_map_buf[y*tmp_map_width + x] = 266 + 2 * (((tile->tile_num - tile->tile_num % 16) - 140) / 16);
                continue;
               }
            }

          if(flag == 192) // 1100 bottom left corner
            {
             if(tmpBufTileIsBlack(x,y+1) && tmpBufTileIsBlack(x-1,y)) //replace with blacked corner tile
               {
                //Oh dear! this is evil. FIX
                tmp_map_buf[y*tmp_map_width + x] = 266 + 1 + 2 * (((tile->tile_num - tile->tile_num % 16) - 140) / 16);
                continue;
               }
            }

          if((tile->flags1 & TILEFLAG_WALL_MASK) == flag) // complete match no work needed
            continue;

          // Look for a suitable tile to transform into

        // ERIC 05/03/05 flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;

          if(((tile->flags1) & TILEFLAG_WALL_MASK) > flag && flag != 144) // 1001 _| corner
            {
             //flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
             for(;((tile->flags1 ) & TILEFLAG_WALL_MASK) != flag && tile->flags1 & TILEFLAG_WALL_MASK;)
               tile = tile_manager->get_tile(tile->tile_num - 1);
            }
          else
            {
             //flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
             for(;((tile->flags1 ) & TILEFLAG_WALL_MASK) != flag && tile->flags1 & TILEFLAG_WALL_MASK;)
               tile = tile_manager->get_tile(tile->tile_num + 1);
            }

          if((tile->flags1 & TILEFLAG_WALL_MASK) == flag)
             tmp_map_buf[y*tmp_map_width + x] = tile->tile_num;
         }
      }
   }
}

inline bool MapWindow::tmpBufTileIsBlack(uint16 x, uint16 y)
{
 if(tmp_map_buf[y*tmp_map_width + x] == 0)
   return true;

 return false;
}

bool MapWindow::tmpBufTileIsBoundary(uint16 x, uint16 y)
{
 uint16 tile_num;
 Tile *tile;

 tile_num = tmp_map_buf[y*tmp_map_width + x];

 if(tile_num == 0)
   return false;

 tile = tile_manager->get_tile(tile_num);

 if(tile->boundary)
   return true;

 if(obj_manager->is_boundary(WRAPPED_COORD(cur_x-1+x, cur_level), WRAPPED_COORD(cur_y-1+y, cur_level), cur_level))
   return true;

 return false;
}

bool MapWindow::tmpBufTileIsWall(uint16 x, uint16 y, uint8 direction)
{
 uint16 tile_num;
 Tile *tile;
 uint8 mask = 0;
  
 tile_num = tmp_map_buf[y*tmp_map_width + x];

 if(tile_num == 0)
   return false;

 switch(direction)
 {
  case NUVIE_DIR_N : mask = TILEFLAG_WALL_SOUTH; break;
  case NUVIE_DIR_S : mask = TILEFLAG_WALL_NORTH; break;
  case NUVIE_DIR_E : mask = TILEFLAG_WALL_WEST; break;
  case NUVIE_DIR_W : mask = TILEFLAG_WALL_EAST; break;
 }

 tile = tile_manager->get_tile(tile_num);

 if(tile->flags1 & TILEFLAG_WALL)
   {
    if(tile->flags1 & mask)
      return true;
   }
   
// if(obj_manager->is_boundary(cur_x-1+x, cur_y-1+y, cur_level))
//  return true;

 tile = obj_manager->get_obj_tile(WRAPPED_COORD(cur_x+x-1, cur_level), WRAPPED_COORD(cur_y+y-1, cur_level), cur_level, false);
 if(tile != NULL)
   {
    if(tile->flags2 & TILEFLAG_BOUNDARY)
     {
      if(tile->flags1 & mask)
        return true;
     }
   }

 return false;
}

/* Returns true if any object could be placed at world coordinates x,y.
 * If actor is set a line-of-site check must pass. (z is always cur_level)
 */
bool MapWindow::can_drop_obj(uint16 x, uint16 y, Actor *actor)
{
    LineTestResult lt;
    MapCoord actor_loc;
    if(hackmove)
        return true;

    if(actor)
    {
        actor_loc = actor->get_location();
        // can't ever drop at the actor location
        if(actor_loc.x == x && actor_loc.y == y)
            return false;
    }

    if(tmpBufTileIsBlack(x - cur_x, y - cur_y))
        return false;

    if(!map->is_passable(x, y, cur_level)
       || (actor && map->lineTest(actor_loc.x, actor_loc.y, x, y, cur_level, LT_HitUnpassable, lt)))
    {
        // We can place an object on a bench or table. Or on any other object if
        // the object is passable and not on a boundary.
        Obj *obj = lt.hitObj ? lt.hitObj : obj_manager->get_obj(x, y, cur_level);
        Tile *obj_tile;
        if(!obj) obj_tile = map->get_tile(x, y, cur_level);
        else     obj_tile = obj_manager->get_obj_tile(obj->obj_n,obj->frame_n);
        if(!(obj_tile->flags3 & TILEFLAG_CAN_PLACE_ONTOP ||
            (obj_tile->passable && ((obj && !map->is_boundary(obj->x, obj->y, cur_level))
                                   || !obj_tile->boundary))))
            return false;
    }
    return true;
}

bool MapWindow::can_get_obj(Actor *actor, Obj *obj)
{
	if(obj->is_in_inventory() || actor->get_z() != obj->z)
		return false;

	LineTestResult lt;
	if(map->lineTest(actor->get_x(), actor->get_y(), obj->x, obj->y, obj->z, LT_HitUnpassable, lt))
	{
		if(lt.hitObj != obj)
			return false;
	}

	return true;
}

bool MapWindow::drag_accept_drop(int x, int y, int message, void *data)
{
 DEBUG(0,LEVEL_DEBUGGING,"MapWindow::drag_accept_drop()\n");
 uint16 map_width;

 x -= area.x;
 y -= area.y;

 x /= 16;
 y /= 16;


 if(message == GUI_DRAG_OBJ)
   {
    map_width = map->get_width(cur_level);
    x = (cur_x + x) % map_width;
    y = (cur_y + y) % map_width;

    Obj *obj = (Obj *)data;
    MsgScroll *scroll = Game::get_game()->get_scroll();

    if(obj->is_in_inventory() == false) //obj on map.
    {


    	Actor *p = actor_manager->get_player();
    	LineTestResult lt;
    	if(can_get_obj(p, obj))  //make sure there is a clear line from player to object
    	{
    		Actor *target_actor = map->get_actor(x, y, cur_level);
    		if(target_actor)
    		{
    	    	scroll->display_string("Move-");
    	    	scroll->display_string(obj_manager->look_obj(obj, OBJ_SHOW_PREFIX));
    	    	scroll->display_string(" To ");
    			scroll->display_string(target_actor->get_name());
    			scroll->display_string(".\n");

    			if(target_actor == p || (target_actor->is_in_party() && can_drop_obj(x, y, p)))
    			{
    				return Game::get_game()->get_script()->call_actor_get_obj(target_actor, obj);
    			}
    		}
    		else
    		{
    			return can_drop_obj(x, y, p);
    		}
    	}
    }
    else if(can_drop_obj(x, y, actor_manager->get_player())) //object in inventory
    {
    	Actor *a = map->get_actor(x, y, cur_level);
    	if(a)
    	{
    		if(a->is_in_party()==false)
    			return false;

    		if(Game::get_game()->get_script()->call_actor_get_obj(a, obj) == false)
    			return false;
    	}
      return true;
    }

    game->get_scroll()->message("\n\nNot Possible\n\n");
  }

 return false;
}

void MapWindow::drag_perform_drop(int x, int y, int message, void *data)
{
    DEBUG(0,LEVEL_DEBUGGING,"MapWindow::drag_perform_drop()\n");
    Event *event = game->get_event();
    uint16 map_width = map->get_width(cur_level);

    x -= area.x;
    y -= area.y;

    if(message == GUI_DRAG_OBJ)
    {
        x = (cur_x + x / 16) % map_width;
        y = (cur_y + y / 16) % map_width;
//        Obj *target_obj = obj_manager->get_obj(x,y, cur_level);
//        Actor *target_actor = actor_manager->get_actor(x,y,cur_level);
        Obj *obj = (Obj *)data;

        Actor *a = map->get_actor(x, y, cur_level);
        if(a && a->is_in_party())
        {
        	obj_manager->moveto_inventory(obj, a);
        	Game::get_game()->get_scroll()->display_string("\n");
            Game::get_game()->get_scroll()->display_prompt();
        }
        else
        {
        	// drop on ground or into a container
        	event->newAction(DROP_MODE); // FIXME: drops no matter what the mode is
        	event->select_obj(obj);
        	if(obj->qty == 0 || obj->qty == 1)
        		event->select_target(x, y);
        	else
        		event->set_drop_target(x, y); // pre-select target
        }
        // FIXME: need to re-add dropping onto a container or actor
//        if(target_obj && target_obj->container)
//            obj_manager->list_add_obj(target_obj->container, obj);
        // FIXME: a method to prevent exploiting this would be to subtract the
        //        number of moves that are necessary to reach the object
    }

}


GUI_status MapWindow::Idle(void)
{
    return(GUI_Widget::Idle());
}


// single-click (press and release button)
GUI_status MapWindow::MouseClick(int x, int y, int button)
{
#if 0
    if(button == USE_BUTTON) // see MouseDelayed
        wait_for_mouseclick(button);
#endif
    return(MouseUp(x, y, button)); // do MouseUp so selected_obj is cleared
}

// single-click; waited for double-click
GUI_status MapWindow::MouseDelayed(int x, int y, int button)
{
#if 0 /* enable this once I can negotiate between Click,DoubleClick,Delayed and
         can get a MouseDelayed even if the cursor is moved */
    Event *event = game->get_event();
    int wx, wy;
    mouseToWorldCoords(x, y, wx, wy);
    if(event->newAction(LOOK_MODE))
    {
        moveCursor(wx - cur_x, wy - cur_y);
        event->look();
        centerCursor();
        event->endAction();
    }
#endif
    return(MouseUp(x, y, button)); // do MouseUp so selected_obj is cleared
}

// MouseDown; waited for MouseUp
GUI_status MapWindow::MouseHeld(int x, int y, int button)
{
    walking = true;
    return(GUI_PASS);
}

// double-click
GUI_status MapWindow::MouseDouble(int x, int y, int button)
{
    Event *event = game->get_event();

    // only USE if not doing anything in event
    if(event->get_mode() == MOVE_MODE && !is_wizard_eye_mode())
    {
        int wx, wy;
        mouseToWorldCoords(x, y, wx, wy);
        event->multiuse((uint16)wx, (uint16)wy);
    }
    return(MouseUp(x, y, button)); // do MouseUp so selected_obj is cleared
}

GUI_status MapWindow::MouseDown (int x, int y, int button)
{
	//DEBUG(0,LEVEL_DEBUGGING,"MapWindow::MouseDown, button = %i\n", button);
	Event *event = game->get_event();
	Actor *player = actor_manager->get_player();
	Obj	*obj = get_objAtMousePos (x, y);
	int wx, wy;

	if(is_wizard_eye_mode())
	{
		walking = true;
		return GUI_YUM;
	}

	if(button != USE_BUTTON && button != WALK_BUTTON && button != DRAG_BUTTON)
		return GUI_PASS;

	mouseToWorldCoords(x, y, wx, wy);

	if(event->get_mode() == MOVE_MODE) // PASS if Avatar is hit
	{
		if(wx == player->x && wy == player->y)
		{
			event->cancelAction(); // MOVE_MODE, so this should work
			return GUI_PASS;
		}
		if(button == WALK_BUTTON)
			walking = true;
		else if(button == USE_BUTTON) // you can also walk by holding the USE button
			wait_for_mousedown(button);
	}
	else if(event->get_mode() == INPUT_MODE || event->get_mode() == ATTACK_MODE) // finish whatever action is being done, with mouse coordinates
	{
		mouseToWorldCoords(x, y, wx, wy);   // some Event functions still use
		moveCursor(wx - cur_x, wy - cur_y); // the cursor location instead of
		event->select_target(uint16(wx), uint16(wy), cur_level); // the returned location
		return  GUI_PASS;
	}
	else
	{
		return GUI_PASS;
	}

	if (!obj)
	{
		return	GUI_PASS;
	}

	float weight = obj_manager->get_obj_weight (obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS);

	if (weight == 0 && !hackmove)
		return	GUI_PASS;

	if(button == DRAG_BUTTON)
		selected_obj = obj;

	return	GUI_PASS;
}

GUI_status MapWindow::MouseUp(int x, int y, int button)
{
	// cancel dragging and movement no matter what button is released
	if(selected_obj)
	{
		selected_obj = NULL;
	}
	walking = false;
	dragging = false;

	return	GUI_PASS;
}

GUI_status	MapWindow::MouseMotion (int x, int y, Uint8 state)
{
//	Event *event = game->get_event();
	Tile	*tile;

	update_mouse_cursor((uint32)x, (uint32)y);

	//	DEBUG(0,LEVEL_DEBUGGING,"MapWindow::MouseMotion\n");

//	if(selected_obj) // We don't want to walk if we are selecting an object to move.
//		walking = false;
	if(walking) // No, we don't want to select an object to move if we are walking.
	{
		selected_obj = NULL;
		dragging = false;
	}

	if (selected_obj && !dragging)
	{
		int wx, wy;
		// ensure that the player can reach the selected object before
		// letting them drag it
		//mouseToWorldCoords(x, y, wx, wy);
                wx = selected_obj->x; wy = selected_obj->y;
		LineTestResult result;
		Actor* player = actor_manager->get_player();

		if (map->lineTest(player->x, player->y, wx, wy, cur_level, LT_HitUnpassable, result)
                    && !(result.hitObj && result.hitObj->x == wx && result.hitObj->y == wy) && !hackmove)
			// something was in the way, so don't allow a drag
			return GUI_PASS;
		dragging = true;
		set_mousedown(0, DRAG_BUTTON); // cancel MouseHeld
		game->set_mouse_pointer(0); // arrow
		tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(selected_obj->obj_n)+selected_obj->frame_n);
		return gui_drag_manager->start_drag(this, GUI_DRAG_OBJ, selected_obj, tile->data, 16, 16, 8);
	}

	return	GUI_PASS;
}

void	MapWindow::drag_drop_success (int x, int y, int message, void *data)
{
	//DEBUG(0,LEVEL_DEBUGGING,"MapWindow::drag_drop_success\n");
	dragging = false;

// handled by drop target
//	if (selected_obj)
//		obj_manager->remove_obj (selected_obj);

	selected_obj = NULL;
	Redraw();
}

void	MapWindow::drag_drop_failed (int x, int y, int message, void *data)
{
	DEBUG(0,LEVEL_DEBUGGING,"MapWindow::drag_drop_failed\n");
	dragging = false;
	selected_obj = NULL;
}

// this does nothing
GUI_status MapWindow::KeyDown(SDL_keysym key)
{
	if(is_wizard_eye_mode())
	{
		if(key.sym == SDLK_ESCAPE)
		{
			wizard_eye_stop();
		}
		else if(key.sym == SDLK_LEFT)
		{
			moveMapRelative(-1,0);
			wizard_eye_update();
		}
		else if(key.sym == SDLK_RIGHT)
		{
			moveMapRelative(1,0);
			wizard_eye_update();
		}
		else if(key.sym == SDLK_UP)
		{
			moveMapRelative(0,-1);
			wizard_eye_update();
		}
		else if(key.sym == SDLK_DOWN)
		{
			moveMapRelative(0,1);
			wizard_eye_update();
		}
	}
/*    if(key.sym == SDLK_RETURN)
    {
		game->get_event()->select_target(cur_x+cursor_x, cur_y+cursor_y);
        return GUI_YUM;
    }*/
    return GUI_PASS;
}

Obj *MapWindow::get_objAtMousePos (int mx, int my)
{
	int wx, wy;
	mouseToWorldCoords (mx, my, wx, wy);

    return	obj_manager->get_obj (wx, wy, cur_level);
}


Actor *MapWindow::get_actorAtMousePos(int mx, int my)
{
	int wx, wy;
	mouseToWorldCoords (mx, my, wx, wy);

    return	actor_manager->get_actor (wx, wy, cur_level);
}


void MapWindow::mouseToWorldCoords (int mx, int my, int &wx, int &wy)
{
	int x = mx - area.x;
	int y = my - area.y;

    int	map_width = map->get_width(cur_level);

    wx = (cur_x + x / 16) % map_width;
    wy = (cur_y + y / 16) % map_width;
}

void MapWindow::drag_draw(int x, int y, int message, void* data)
{
	Tile* tile;

	if (!selected_obj)
		return;

	tile = tile_manager->get_tile(obj_manager->get_obj_tile_num (selected_obj->obj_n) + selected_obj->frame_n);

	int	nx = x - 8;
	int	ny = y - 8;

	if (nx + 16 >= screen->get_width())
		nx = screen->get_width()-17;
	else if (nx < 0)
		nx = 0;

	if (ny + 16 >= screen->get_height())
		ny = screen->get_height()-17;
	else if (ny < 0)
		ny = 0;

	screen->blit(nx, ny, tile->data, 8, 16, 16, 16, true);
	screen->update(nx, ny, 16, 16);
}


/* Display MapWindow animations. */
void MapWindow::drawAnims()
{
    if(!screen) // screen should be set early on
        return;
    else if(!anim_manager->get_surface()) // screen must be assigned to AnimManager
        anim_manager->set_surface(screen);
    anim_manager->display();
}


/* Set mouse pointer to a movement-arrow for walking, or a crosshair. */
void MapWindow::update_mouse_cursor(uint32 mx, uint32 my)
{
    Event *event = game->get_event();
    int wx, wy;
    sint16 rel_x, rel_y;
    uint8 mptr; // mouse-pointer is set here in get_movement_direction()

    if(event->get_mode() != MOVE_MODE && event->get_mode() != INPUT_MODE)
        return;

    // MousePos->WorldCoord->Direction&MousePointer
    mouseToWorldCoords((int)mx, (int)my, wx, wy);
    get_movement_direction((uint16)wx, (uint16)wy, rel_x, rel_y, &mptr);
    if(event->get_mode() == INPUT_MODE && mousecenter_x == (win_width/2) && mousecenter_y == (win_height/2))
        game->set_mouse_pointer(1); // crosshairs
    else if(dragging || wx == cur_x || wy == cur_y || wx == (cur_x+win_width-1) || wy == (cur_y+win_height-1))
        game->set_mouse_pointer(0); // arrow
    else
        game->set_mouse_pointer(mptr); // 1=crosshairs, 2to9=arrows
}

/* Get relative movement direction from the MouseCenter coordinates to the
 * world coordinates wx,wy, for walking with the mouse, etc. The mouse-pointer
 * number that should be used for that direction will be set to mptr.
 */
void MapWindow::get_movement_direction(uint16 wx, uint16 wy, sint16 &rel_x, sint16 &rel_y, uint8 *mptr)
{
    uint16 cent_x = cur_x + mousecenter_x, //+ (win_width / 2),
           cent_y = cur_y + mousecenter_y; //+ (win_height / 2);
    uint16 dist_x = abs(wx - cent_x), dist_y = abs(wy - cent_y);

    rel_x = rel_y = 0;
    if(dist_x <= 4 && dist_y <= 4)
    {   // use mapwindow coords (4,4 is center of mapwindow)
        uint8 cursor_num = movement_array[(9 * (4 + (wy - cent_y))) + (4 + (wx - cent_x))];
        if(mptr) // set mouse-pointer number
            *mptr = cursor_num;
        if(cursor_num == 1) // nowhere
            return;
        if(cursor_num == 2) // up
            rel_y = -1;
        else if(cursor_num == 6) // down
            rel_y = 1;
        else if(cursor_num == 8) // left
            rel_x = -1;
        else if(cursor_num == 4) // right
            rel_x = 1;
        else if(cursor_num == 3) // up-right
        {
            rel_x = 1; rel_y = -1;
        }
        else if(cursor_num == 5) // down-right
        {
            rel_x = 1; rel_y = 1;
        }
        else if(cursor_num == 7) // down-left
        {
            rel_x = -1; rel_y = 1;
        }
        else if(cursor_num == 9) // up-left
        {
            rel_x = -1; rel_y = -1;
        }
    }
    else // mapwindow is larger than the array; use 4 squares around center array
    {
        if(dist_x <= 4 && wy < cent_y) // up
        {
            rel_y = -1;
            if(mptr) *mptr = 2;
        }
        else if(dist_x <= 4 && wy > cent_y) // down
        {
            rel_y = 1;
            if(mptr) *mptr = 6;
        }
        else if(wx < cent_x && dist_y <= 4) // left
        {
            rel_x = -1;
            if(mptr) *mptr = 8;
        }
        else if(wx > cent_x && dist_y <= 4) // right
        {
            rel_x = 1;
            if(mptr) *mptr = 4;
        }
        else if(wx > cent_x && wy < cent_y) // up-right
        {
            rel_x = 1; rel_y = -1;
            if(mptr) *mptr = 3;
        }
        else if(wx > cent_x && wy > cent_y) // down-right
        {
            rel_x = 1; rel_y = 1;
            if(mptr) *mptr = 5;
        }
        else if(wx < cent_x && wy > cent_y) // down-left
        {
            rel_x = -1; rel_y = 1;
            if(mptr) *mptr = 7;
        }
        else if(wx < cent_x && wy < cent_y) // up-left
        {
            rel_x = -1; rel_y = -1;
            if(mptr) *mptr = 9;
        }
    }
}


/* Revert mouse cursor to normal arrow. Stop walking. */
GUI_status MapWindow::MouseLeave(Uint8 state)
{
    if(game_type == NUVIE_GAME_MD) // magnifying glass - pointer 0 should be used too for some areas
        game->set_mouse_pointer(1);
    else
        game->set_mouse_pointer(0);
    walking = false;
    dragging = false;
    // NOTE: Don't clear selected_obj here! It's used to remove the object after
    // dragging.
    return(GUI_PASS);
}

unsigned char *MapWindow::make_thumbnail()
{
 if(thumbnail)
   return false;

 new_thumbnail = true;

 GUI::get_gui()->Display(); // this calls MapWindow::display() which in turn calls create_thumbnail(). :-)

 return thumbnail;
}

void MapWindow::create_thumbnail()
{
 SDL_Rect src_rect;

 src_rect.w = MAPWINDOW_THUMBNAIL_SIZE * MAPWINDOW_THUMBNAIL_SCALE;
 src_rect.h = src_rect.w;

 src_rect.x = area.x + win_width * 8 - (src_rect.w/2);  // area.x + (win_width * 16) / 2 - 120 / 2
 src_rect.y = area.y + win_height * 8 - (src_rect.h/2); // area.y + (win_height * 16) / 2 - 120 / 2

 thumbnail = screen->copy_area(&src_rect, MAPWINDOW_THUMBNAIL_SCALE); //scale down x3

 new_thumbnail = false;
}

void MapWindow::free_thumbnail()
{
 if(thumbnail)
   {
    delete[] thumbnail;
    thumbnail = NULL;
   }

 return;
}


/* Returns a new 8bit copy of the mapwindow as displayed. Caller must free it. */
SDL_Surface *MapWindow::get_sdl_surface()
{
 return(get_sdl_surface(0, 0, area.w, area.h));
}

SDL_Surface *MapWindow::get_sdl_surface(uint16 x, uint16 y, uint16 w, uint16 h)
{
 SDL_Surface *new_surface = NULL;
 unsigned char *screen_area;
 SDL_Rect copy_area = { area.x + x, area.y + y, w, h };

 GUI::get_gui()->Display();
 screen_area = screen->copy_area(&copy_area);

 new_surface = screen->create_sdl_surface_8(screen_area, copy_area.w, copy_area.h);
// new_surface = screen->create_sdl_surface_from(screen_area, screen->get_bpp(),
//                                               copy_area.w, copy_area.h,
//                                               copy_area.w);
 delete [] screen_area;
 return(new_surface);
}

/* Returns the overlay surface. A new 8bit overlay is created if necessary. */
SDL_Surface *MapWindow::get_overlay()
{
    if(!overlay)
        overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, area.w, area.h,
                                       8, 0, 0, 0, 0);
    return(overlay);
}

/* Set the overlay surface. The current overlay is deleted if necessary. */
void MapWindow::set_overlay(SDL_Surface *surfpt)
{
    if(overlay && (overlay != surfpt))
        SDL_FreeSurface(overlay);
    overlay = surfpt;
}

/* Returns true if town tiles are visible in the MapWindow. */ 
bool MapWindow::in_town()
{
    for(std::vector<TileInfo>::iterator ti = m_ViewableTiles.begin();
        ti != m_ViewableTiles.end(); ti++)
        if((*ti).t->flags1&TILEFLAG_WALL)
            return true;
    return false;
}

void MapWindow::wizard_eye_start(MapCoord location, uint16 duration, CallBack *caller)
{
	wizard_eye_info.moves_left = duration;
	wizard_eye_info.caller = caller;

	wizard_eye_info.prev_x = cur_x;
	wizard_eye_info.prev_y = cur_y;

	set_x_ray_view(true);

	moveMap(location.x-(win_width/2), location.y-(win_height/2), cur_level);
	grab_focus();
}

void MapWindow::wizard_eye_stop()
{
	if(wizard_eye_info.moves_left > 0)
	{
		wizard_eye_info.moves_left = 0;
		wizard_eye_update();
	}
}

void MapWindow::wizard_eye_update()
{
	if(wizard_eye_info.moves_left > 0)
		wizard_eye_info.moves_left--;

	if(wizard_eye_info.moves_left == 0)
	{
		set_x_ray_view(false);
		moveMap(wizard_eye_info.prev_x, wizard_eye_info.prev_y, cur_level);
		wizard_eye_info.caller->callback(EFFECT_CB_COMPLETE, (CallBack *)this, NULL);
		release_focus();
	}
}

void MapWindow::loadRoofTiles()
{
	std::string datadir = GUI::get_gui()->get_data_dir();
	std::string imagefile;
	std::string path;

	build_path(datadir, "images", path);
	datadir = path;
	build_path(datadir, "roof_tiles.bmp", imagefile);
	roof_tiles = SDL_LoadBMP(imagefile.c_str());
	if(roof_tiles)
	{
		SDL_SetColorKey(roof_tiles, SDL_SRCCOLORKEY, SDL_MapRGB(roof_tiles->format, 0, 0x70, 0xfc));
	}
}
