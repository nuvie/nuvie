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

#define DBLCLICK_USE_BUTTON 1 /* FIXME: this should be in a common location */

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


MapWindow::MapWindow(Configuration *cfg): GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
 config->value("config/GameType",game_type);
 screen = NULL;
 //surface = NULL;
 anim_manager = NULL;

 cur_x = 0; mousecenter_x = 0;
 cur_y = 0; mousecenter_y = 0;
 cur_x_add = cur_y_add = 0;
 vel_x = vel_y = 0;

 cursor_x = 0;
 cursor_y = 0;
 show_cursor = false;
 show_use_cursor = false;

 new_thumbnail = false;
 thumbnail = NULL;
 overlay = NULL;
 overlay_level = MAP_OVERLAY_DEFAULT;

 cur_level = 0;

 tmp_map_buf = NULL;

 selected_obj = NULL;
 selected_actor = NULL;
 config->value("config/enable_hackmove", hackmove);
 walking = false;
 walk_start_delay = 0;

 window_updated = true;
}

MapWindow::~MapWindow()
{
 set_overlay(NULL); // free
 free(tmp_map_buf);
 delete anim_manager;
}

bool MapWindow::init(Map *m, TileManager *tm, ObjManager *om, ActorManager *am)
{
 int game_type;

 map = m;
 tile_manager = tm;
 obj_manager = om;
 actor_manager = am;
 anim_manager = new AnimManager();

 config->value("config/GameType",game_type);

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

 area.x = 0;
 area.y = 0;

 set_windowSize(11,11);

 // hide the window until game is fully loaded and does fade-in
 get_overlay(); // this allocates `overlay`
 overlay_level = MAP_OVERLAY_ONTOP;
 assert(SDL_FillRect(overlay, NULL, 0x31) == 0);

 set_accept_mouseclick(true, DBLCLICK_USE_BUTTON); // allow double-clicks

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

 tmp_map_buf = (uint16 *)realloc(tmp_map_buf, tmp_map_width * tmp_map_height * sizeof(uint16));
 if(tmp_map_buf == NULL)
   return false;

// if(surface != NULL)
//   delete surface;
// surface = new Surface;

// if(surface->init(win_width*16,win_height*16) == false)
//   return false;

 clip_rect.x = 8;
 clip_rect.w = (win_width - 1) * 16;
 clip_rect.h = (win_height - 1) * 16;

 if(game_type == NUVIE_GAME_U6)
   clip_rect.y = 8;
 else
   {
    clip_rect.y = 16;
    clip_rect.h -= 16;
   }

 anim_manager->set_area(&clip_rect);

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

 return obj_manager->get_obj(cur_x + cursor_x, cur_y + cursor_y, cur_level);
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
    GameClock *clock = Game::get_game()->get_clock();
    Event *event = Game::get_game()->get_event();
    static bool game_started = false; // set to true on the first update()
    static uint32 last_update_time = clock->get_ticks();
    uint32 update_time = clock->get_ticks();

    // do fade-in on the first update (game has loaded now)
    if(game_started == false)
    {
//        new FadeEffect(FADE_PIXELATED_ONTOP, FADE_IN, 0x31);
        new GameFadeInEffect(0x31);
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

    if(walking)
    {
        int mx, my;
#if 0
        // wait for possible double-click before starting
        uint32 time_passed = update_time - last_mousedown_time;
        walk_start_delay -= ((sint32)(walk_start_delay - time_passed) > 0) ? time_passed : walk_start_delay;
        if(walk_start_delay)
            return;
#endif
        if(SDL_GetMouseState(&mx, &my) & (SDL_BUTTON(1) | SDL_BUTTON(3)))
            event->walk_to_mouse_cursor((uint32)mx / screen->get_scale_factor(),
                                        (uint32)my / screen->get_scale_factor());
    }

}


void MapWindow::updateBlacking()
{
 generateTmpMap();
      //Dusk starts at 19:00
     //It's completely dark by 20:00
     //Dawn starts at 5:00
     //It's completely bright by 6:00
     //Dusk and dawn operate by changing the ambient light, not by changing the radius of the avatar's light globe

     GameClock *clock = Game::get_game()->get_clock();
     int h = clock->get_hour();
if(!screen)
 return;

     if(in_dungeon_level())
	     screen->set_ambient( 0x00 );
     else if( h == 19 ) //Dusk
         screen->set_ambient( (uint8)(255*(float)(60-clock->get_minute())/60.0) );
     else if( h == 5 ) //Dawn
         screen->set_ambient( (uint8)(255*(float)clock->get_minute()/60.0) );
     else if( h > 5 && h < 19 ) //Day
         screen->set_ambient( 0xFF );
     else //Night
         screen->set_ambient( 0x00 );

 //Clear the opacity map
 screen->clearalphamap8( 8, 8, 160, 160, screen->get_ambient() );

 m_ViewableObjects.clear();
 m_ViewableTiles.clear();


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
      uint16 draw_x = (j*16), draw_y = (i*16);
      draw_x -= (cur_x_add <= draw_x) ? cur_x_add : draw_x;
      draw_y -= (cur_y_add <= draw_y) ? cur_y_add : draw_y;

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
        }

     }
   //map_ptr += map_width;
   map_ptr += tmp_map_width ;//* sizeof(uint16);
  }

 drawObjs();

 drawAnims();

 if(show_cursor)
  {
   screen->blit(cursor_x*16,cursor_y*16,(unsigned char *)cursor_tile->data,8,16,16,16,true,&clip_rect);
  }

 if(show_use_cursor)
  {
   screen->blit(cursor_x*16,cursor_y*16,(unsigned char *)use_tile->data,8,16,16,16,true,&clip_rect);
  }

// screen->fill(0,8,8,win_height*16-16,win_height*16-16);

 screen->blitalphamap8();

 if(overlay && overlay_level == MAP_OVERLAY_DEFAULT)
   screen->blit(area.x, area.y, (unsigned char *)(overlay->pixels), overlay->format->BitsPerPixel, overlay->w, overlay->h, overlay->pitch, true, &clip_rect);

 if(new_thumbnail)
   create_thumbnail();

 drawBorder();

 if(overlay && overlay_level == MAP_OVERLAY_ONTOP)
   screen->blit(area.x, area.y, (unsigned char *)(overlay->pixels), overlay->format->BitsPerPixel, overlay->w, overlay->h, overlay->pitch, true, &clip_rect);

// ptr = (unsigned char *)screen->get_pixels();
// ptr += 8 * screen->get_pitch() + 8;

// screen->blit(8,8,ptr,8,(win_width-1) * 16,(win_height-1) * 16, win_width * 16, false);

 screen->update(8,8,win_width*16-16,win_height*16-16);

 if(window_updated)
  {
   window_updated = false;
   Game::get_game()->get_sound_manager()->update_map_sfx();
  }

}

void MapWindow::drawActors()
{
 uint16 i;
 Tile *tile;
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
             if(tmp_map_buf[(actor->y - cur_y + 1) * tmp_map_width + (actor->x - cur_x + 1)] != 0 &&
                actor->is_visible() && actor->obj_n != 0)
               {
                tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->obj_n)+actor->frame_n);
                //FIX need a function for multi-tile actors.
                drawTile(tile,actor->x - cur_x, actor->y - cur_y, false);
                drawTile(tile,actor->x - cur_x, actor->y - cur_y, true);
                //screen->blit((actor->x - cur_x)*16,(actor->y - cur_y)*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
 
                // draw light coming from actor
                if(actor->light > 0 && screen->updatingalphamap)
                   screen->drawalphamap8globe(actor->x-cur_x, actor->y-cur_y, actor->light>5?5:actor->light);

               }
            }
         }
      }
   }
}

void MapWindow::drawObjs()
{
 //FIX we need to make this more efficent.

 drawObjSuperBlock(true,false); //draw force lower objects
 drawObjSuperBlock(false,false); //draw lower objects

 drawActors();

 drawObjSuperBlock(false,true); //draw top objects

 return;
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
   m_ViewableObjects.push_back(obj);

 tile = tile_manager->get_original_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);

   //Draw a lightglobe in the middle of the 16x16 tile.
   if( !draw_lowertiles &&
       toptile &&
       tile->flags2 & 0x3 &&
       Game::get_game()->get_screen()->updatingalphamap )
   screen->drawalphamap8globe( obj->x - cur_x, obj->y - cur_y, (tile->flags2 & 0x3) );


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


  drawTile(tile,obj->x - cur_x, obj->y - cur_y, toptile);

}

inline void MapWindow::drawTile(Tile *tile, uint16 x, uint16 y, bool toptile)
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
   drawTopTile(tile_manager->get_tile(tile_num),x,y,toptile);

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
        screen->blit((x*16)-cur_x_add,(y*16)-cur_y_add,tile->data,8,16,16,16,tile->transparent,&clip_rect);
    }
 else
    {
     if(!tile->toptile)
//        screen->blit(x*16,y*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
        screen->blit((x*16)-cur_x_add,(y*16)-cur_y_add,tile->data,8,16,16,16,tile->transparent,&clip_rect);
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
 screen->blit(0,0,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(434);
 screen->blit((win_width-1)*16,0,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(435);
 screen->blit(0,(win_height-1)*16,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(437);
 screen->blit((win_width-1)*16,(win_height-1)*16,tile->data,8,16,16,16,true,&clip_rect);

 tile = tile_manager->get_tile(433);
 tile1 = tile_manager->get_tile(436);

 for(i=1;i < win_width-1;i++)
   {
    screen->blit(i*16,0,tile->data,8,16,16,16,true,&clip_rect);
    screen->blit(i*16,(win_height-1)*16,tile1->data,8,16,16,16,true,&clip_rect);
   }

 tile = tile_manager->get_tile(438);
 tile1 = tile_manager->get_tile(439);

  for(i=1;i < win_height-1;i++)
   {
    screen->blit(0,i*16,tile->data,8,16,16,16,true,&clip_rect);
    screen->blit((win_width-1)*16,i*16,tile1->data,8,16,16,16,true,&clip_rect);
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

 memset(tmp_map_buf, 0, tmp_map_width * tmp_map_height * sizeof(uint16));

 x = cur_x + ((win_width - 1) / 2);
 y = cur_y + ((win_height - 1) / 2);
 
 //This is for U6. Sherry needs to pass through walls
 //We shift the boundary fill start location off the wall tile so it flood
 //fills correctly. We move east for vertical wall tiles and south for
 //horizontal wall tiles. 
 if(game_type == NUVIE_GAME_U6 && obj_manager->is_boundary(x,y,cur_level))
  {
   tile = obj_manager->get_obj_tile(x, y, cur_level, false);
   if((tile->flags1 & TILEFLAG_WALL_MASK) == (TILEFLAG_WALL_NORTH | TILEFLAG_WALL_SOUTH))
     x++;
   else
     y++;
  }
  
 boundaryFill(map_ptr, pitch, x, y);

 reshapeBoundary();
}

void MapWindow::boundaryFill(unsigned char *map_ptr, uint16 pitch, uint16 x, uint16 y)
{
 unsigned char current;
 uint16 *ptr;
 uint16 pos;

 if((x < cur_x - 1) || (x >= (cur_x-1) + tmp_map_width) || x >= pitch)
   return;

 if((y < cur_y - 1) || (y >= (cur_y-1) + tmp_map_height) || y >= pitch)
   return;

 pos = (y - (cur_y-1)) * tmp_map_width + (x - (cur_x-1));

 ptr = &tmp_map_buf[pos];

 if(*ptr != 0)
   return;

 current = map_ptr[y * pitch + x];

 *ptr = (uint16)current;

 if(map->is_boundary(x,y,cur_level)) //hit the boundary wall tiles
  {
   if(boundaryLookThroughWindow(*ptr, x, y) == false)
      return;
  }

 boundaryFill(map_ptr, pitch, x+1, y);
 boundaryFill(map_ptr, pitch, x, y+1);
 boundaryFill(map_ptr, pitch, x+1, y+1);
 boundaryFill(map_ptr, pitch, x-1, y-1);
 boundaryFill(map_ptr, pitch, x-1, y);
 boundaryFill(map_ptr, pitch, x, y-1);
 boundaryFill(map_ptr, pitch, x+1, y-1);
 boundaryFill(map_ptr, pitch, x-1, y+1);


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
    if(a_y == y - 1 || a_y == y + 1)
      return true;
   }

 if(a_y == y)
   {
    if(a_x == x - 1 || a_x == x + 1)
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

 if(obj_manager->is_boundary(cur_x-1+x, cur_y-1+y, cur_level))
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

 tile = obj_manager->get_obj_tile(cur_x-1+x, cur_y-1+y, cur_level, false);
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

bool MapWindow::drag_accept_drop(int x, int y, int message, void *data)
{
 uint16 map_width;

 x -= area.x;
 y -= area.y;

 x /= 16;
 y /= 16;

 if(message == GUI_DRAG_OBJ)
   {
/*
    if(tmpBufTileIsBlack(x,y) && !hackmove)
      {
       printf("Cannot drop onto nothing!\n");
       return false;
      }
*/
    map_width = map->get_width(cur_level);

    x = (cur_x + x) % map_width;
    y = (cur_y + y) % map_width;
/*
    Obj *block_obj = NULL; // blocking object
    if(map->is_passable(x,y,cur_level) || hackmove)
	{
	  LineTestResult result;
	// make sure the player can reach the drop point
	  Actor* player = actor_manager->get_player();
	  if (!map->lineTest(player->x, player->y, x, y, cur_level, LT_HitUnpassable, result) || hackmove)
	  {
		return true;
	  }
          else if(result.hitObj)
		block_obj = result.hitObj;
	}
    else
	block_obj = obj_manager->get_obj(x,y,cur_level);

    if(block_obj)
    {
        // We can place an object on a bench or table. Or on any other object if
        // the object is passable and not on a boundary.
        Tile *obj_tile = obj_manager->get_obj_tile(block_obj->obj_n, block_obj->frame_n);
        if(obj_tile->flags3 & TILEFLAG_CAN_PLACE_ONTOP ||
           (obj_tile->passable && !map->is_boundary(block_obj->x, block_obj->y, cur_level)) )
            return true;
    }
*/
    if(can_drop_obj(x, y, actor_manager->get_player()))
      return true;

    Game::get_game()->get_scroll()->display_string("\n\nNot Possible\n\n");
    Game::get_game()->get_scroll()->display_prompt();
  }

 return false;
}

void MapWindow::drag_perform_drop(int x, int y, int message, void *data)
{
 uint16 map_width;
 Obj *obj = NULL, *target_obj;

 x -= area.x;
 y -= area.y;

 if(message == GUI_DRAG_OBJ)
   {
    map_width = map->get_width(cur_level);

    x = (cur_x + x / 16) % map_width;
    y = (cur_y + y / 16) % map_width;

    //printf("Drop (%x,%x,%x)\n", x, y, cur_level);
    obj = (Obj *)data;
    target_obj = obj_manager->get_obj(x,y, cur_level);
    if(obj && target_obj && target_obj->container) //drop object into a container. FIX for locked chests.
      {
       target_obj->container->addAtPos(0,obj);
      }
    else //drop object onto map
      {
//       obj->x = x;
//       obj->y = y;
//       obj->z = cur_level;

//       obj_manager->add_obj(obj,true);
         Event *event = Game::get_game()->get_event();
// if(dropping)
         event->newAction(DROP_MODE); // FIXME: drops no matter what the mode is
         event->drop_select(obj, obj->qty);
         event->drop(x, y);
// else move
      }
   }

 updateBlacking();

 return;
}


GUI_status MapWindow::Idle(void)
{
    return(GUI_PASS);
}


// single-click (press and release button)
GUI_status MapWindow::MouseClick(int x, int y, int button)
{
    printf("MouseClick(%d,%d,%d): Thou dost see the console.\n",x,y,button);
// FIXME: LOOK here
    return(MouseUp(x, y, button)); // do MouseUp so selected_obj is cleared
}


// double-click
GUI_status MapWindow::MouseDouble(int x, int y, int button)
{
    Event *event = Game::get_game()->get_event();

    // only USE if not doing anything in event
    if(event->get_mode() == MOVE_MODE)
    {
        int wx, wy;
        mouseToWorldCoords(x, y, wx, wy);
        event->multiuse((uint16)wx, (uint16)wy);
    }
    return(MouseUp(x, y, button)); // do MouseUp so selected_obj is cleared
}

GUI_status MapWindow::MouseDown (int x, int y, int button)
{
	//printf ("MapWindow::MouseDown, button = %i\n", button);
	Event *event = Game::get_game()->get_event();
	Actor *player = actor_manager->get_player();
	Obj	*obj = get_objAtMousePos (x, y);
	int wx, wy;

	mouseToWorldCoords(x, y, wx, wy);
	if(event->get_mode() == MOVE_MODE) // PASS if Avatar is hit
	{
		if(wx == player->x && wy == player->y)
		{
			event->cancelAction(); // MOVE_MODE, so this should work
			return GUI_PASS;
		}
		walking = true;
		walk_start_delay = (obj || get_actorAtMousePos(x, y))
					? GUI::mouseclick_delay : 0;
	}
	else // finish whatever action is being done, with mouse coordinates
	{
		mouseToWorldCoords(x, y, wx, wy);
		moveCursor(wx - cur_x, wy - cur_y);
		event->doAction(sint16(wx - player->x), sint16(wy - player->y));
		return  GUI_PASS;
	}

	if (!obj)
	{
		return	GUI_PASS;
	}

	float weight = obj_manager->get_obj_weight (obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS);

	if (weight == 0 && !hackmove)
		return	GUI_PASS;

	selected_obj = obj;

	return	GUI_PASS;
}

GUI_status MapWindow::MouseUp(int x, int y, int button)
{
	//printf ("MapWindow::MouseUp\n");
	if (selected_obj)
	{
		selected_obj = NULL;
	}
	walking = false;

	return	GUI_PASS;
}

GUI_status	MapWindow::MouseMotion (int x, int y, Uint8 state)
{
//	Event *event = Game::get_game()->get_event();
	Tile	*tile;

	update_mouse_cursor((uint32)x, (uint32)y);

	//	printf ("MapWindow::MouseMotion\n");

    if(selected_obj) // We don't want to walk if we are selecting an object to move.
      walking = false;

	if (selected_obj && !dragging)
	{
		int wx, wy;
		// ensure that the player can reach the selected object before
		// letting them drag it
		mouseToWorldCoords(x, y, wx, wy);
		LineTestResult result;
		Actor* player = actor_manager->get_player();

		if (map->lineTest(player->x, player->y, wx, wy, cur_level, LT_HitUnpassable, result) && !hackmove)
			// something was in the way, so don't allow a drag
			return GUI_PASS;
		dragging = true;
		tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(selected_obj->obj_n)+selected_obj->frame_n);
		return gui_drag_manager->start_drag(this, GUI_DRAG_OBJ, selected_obj, tile->data, 16, 16, 8);
	}

	return	GUI_PASS;
}

void	MapWindow::drag_drop_success (int x, int y, int message, void *data)
{
	//printf ("MapWindow::drag_drop_success\n");
	dragging = false;

	if (selected_obj)
		obj_manager->remove_obj (selected_obj);

	selected_obj = NULL;
	Redraw();
}

void	MapWindow::drag_drop_failed (int x, int y, int message, void *data)
{
	printf ("MapWindow::drag_drop_failed\n");
	dragging = false;
	selected_obj = NULL;
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

	if (nx + 16 >= 320)
		nx = 303;
	else if (nx < 0)
		nx = 0;

	if (ny + 16 >= 200)
		ny = 183;
	else if (ny < 0)
		ny = 0;

	screen->blit(nx, ny, tile->data, 8, 16, 16, 16, true);
	screen->update(nx, ny, 16, 16);
}


/* Display MapWindow animations.
 */
void MapWindow::drawAnims()
{
    if(!screen) // screen should be set early on
        return;
    else if(!anim_manager->get_surface()) // screen must be assigned to AnimManager
        anim_manager->set_surface(screen);
    anim_manager->display();
}


/* Set mouse pointer to a movement-arrow for walking, or a crosshair.
 */
void MapWindow::update_mouse_cursor(uint32 mx, uint32 my)
{
    Game *game = Game::get_game();
    Event *event = game->get_event();
    int wx, wy;
    sint16 rel_x, rel_y;
    uint8 mptr; // mouse-pointer is set here in get_movement_direction()

    if(event->get_mode() != MOVE_MODE && event->get_mode() != PUSH_MODE)
        return;

    // MousePos->WorldCoord->Direction&MousePointer
    mouseToWorldCoords((int)mx, (int)my, wx, wy);
    get_movement_direction((uint16)wx, (uint16)wy, rel_x, rel_y, &mptr);
    if(dragging || wx == cur_x || wy == cur_y || wx == (cur_x+win_width-1) || wy == (cur_y+win_height-1))
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


/* Revert mouse cursor to normal arrow. Stop walking.
 */
GUI_status MapWindow::MouseLeave(Uint8 state)
{
    Game::get_game()->set_mouse_pointer(0);
    walking = false;
    return(GUI_PASS);
}


/* Returns true if any object could be placed at world coordinates x,y.
 * If actor is set a line-of-site check must pass. (z is always cur_level)
 */
bool MapWindow::can_drop_obj(uint16 x, uint16 y, Actor *actor)
{
    LineTestResult lt;
    MapCoord actor_loc;
    if(actor)
    {
        actor_loc = actor->get_location();
        // can't ever drop at the actor location
        if(actor_loc.x == x && actor_loc.y == y)
            return(false);
    }

    if(tmpBufTileIsBlack(x - cur_x, y - cur_y)
       || !map->is_passable(x, y, cur_level)
       || (actor && map->lineTest(actor_loc.x, actor_loc.y, x, y, cur_level, LT_HitUnpassable, lt)))
    {
        // We can place an object on a bench or table. Or on any other object if
        // the object is passable and not on a boundary.
        Obj *obj = lt.hitObj ? lt.hitObj : obj_manager->get_obj(x, y, cur_level);
        if(!obj) // (but if unpassable isn't an object, nothing we can do)
            return(false);
        Tile *obj_tile = obj_manager->get_obj_tile(obj->obj_n, obj->frame_n);
        if(!(obj_tile->flags3 & TILEFLAG_CAN_PLACE_ONTOP ||
            (obj_tile->passable && !map->is_boundary(obj->x, obj->y, cur_level))))
            return(false);
    }
    return(true);
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
