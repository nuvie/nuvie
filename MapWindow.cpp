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
 
#include "nuvieDefs.h"

#include "Configuration.h"
#include "U6LList.h"
#include "Actor.h"
#include "TileManager.h"
#include "ActorManager.h"
#include "MapWindow.h"
#include "Map.h"
#include "MsgScroll.h"

#include "GUI_widget.h"
#include "Game.h"
#include "GameClock.h"

MapWindow::MapWindow(Configuration *cfg): GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
 config->value("config/GameType",game_type);
 screen = NULL;
 //surface = NULL;
 
 cur_x = 0;
 cur_y = 0;
 
 cursor_x = 0;
 cursor_y = 0;
 show_cursor = false;
 show_use_cursor = false;
  
 cur_level = 0;

 tmp_buf = NULL;

 selected_obj = NULL;
   
}

MapWindow::~MapWindow()
{
 free(tmp_buf);
}

bool MapWindow::init(Map *m, TileManager *tm, ObjManager *om, ActorManager *am)
{
 int game_type;

 map = m;
 tile_manager = tm;
 obj_manager = om;
 actor_manager = am;

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
 
 return true;
}

bool MapWindow::set_windowSize(uint16 width, uint16 height)
{
 win_width = width;
 win_height = height;
 
 area.w = win_width * 16;
 area.h = win_height * 16;
 
 //we make the map +1 bigger all around for the boundary fill function
 //this enables edges of the map window to display correctly.
 
 tmp_buf = (uint16 *)realloc(tmp_buf, (win_width + 2) * (win_height + 2) * sizeof(uint16)); 
 if(tmp_buf == NULL)
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

void MapWindow::moveMap(sint16 new_x, sint16 new_y, sint8 new_level)
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
         updateBlacking();
 //       }
 //    } 
}

void MapWindow::moveMapRelative(sint16 rel_x, sint16 rel_y)
{
 moveMap(cur_x + rel_x, cur_y + rel_y, cur_level); 
}

void MapWindow::centerMapOnActor(Actor *actor)
{
 uint16 x;
 uint16 y;
 uint8 z;
 
 actor->get_location(&x,&y,&z);
 
 moveMap(x - ((win_width - 1) / 2), y - ((win_height - 1) / 2), z);
 
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


const char *MapWindow::lookAtCursor(bool show_prefix)
{
 Actor *actor;
 
 if(tmp_buf[(cursor_y+1) * (win_width+2) + (cursor_x+1)] == 0) //black area
   return tile_manager->lookAtTile(0,0,true); // nothing to see here. ;)

 actor = actor_manager->get_actor(cur_x + cursor_x, cur_y + cursor_y, cur_level);
 if(actor != NULL)
   {
    return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(actor->get_tile_num()),0,show_prefix);
   }
   
 return map->look(cur_x + cursor_x, cur_y + cursor_y, cur_level);
}
 

Obj *MapWindow::get_objAtCursor()
{
 
 if(tmp_buf[(cursor_y+1) * (win_width+2) + (cursor_x+1)] == 0) //black area
   return NULL; // nothing to see here. ;)
   
 return obj_manager->get_obj(cur_x + cursor_x, cur_y + cursor_y, cur_level);
}

Actor *MapWindow::get_actorAtCursor()
{
 //Actor *actor;
 
 if(tmp_buf[(cursor_y+1) * (win_width+2) + (cursor_x+1)] == 0) //black area
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

void MapWindow::get_pos(uint16 *x, uint16 *y)
{
 *x = cur_x;
 *y = cur_y;
}


/* Returns true if the location at the coordinates is visible on the map window.
 */
bool MapWindow::in_window(uint16 x, uint16 y, uint8 z)
{
    return( (z == cur_level && x >= cur_x && x <= (cur_x + win_width)
             && y >= cur_y && y <= (cur_y + win_height)) );
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
if(screen)
{
     if( h == 19 ) //Dusk
         screen->set_ambient( 255*(float)(60-clock->get_minute())/60.0 );
     else if( h == 5 ) //Dawn
         screen->set_ambient( 255*(float)clock->get_minute()/60.0 );
     else if( h > 5 && h < 19 ) //Day
         screen->set_ambient( 0xFF );
     else //Night
         screen->set_ambient( 0x00 );
         
 //Clear the opacity map
 screen->clearalphamap8( 8, 8, 160, 160, screen->get_ambient() );
 
 //Light globe around the avatar
 screen->drawalphamap8globe( 88, 88, 64 );
}

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
  map_ptr = tmp_buf;
  map_ptr += (1 * (win_width + 2) + 1);// * sizeof(uint16); //remember our tmp map is 1 bigger all around.
  
  for(i=0;i<win_height;i++)
  {
   for(j=0;j<win_width;j++)
     {
      if(map_ptr[j] == 0)
        screen->clear((j*16),(i*16),16,16,&clip_rect); //blackout tile.
      else
        {
         if(map_ptr[j] >= 16 && map_ptr[j] < 48) //lay down the base tile for shoreline tiles
           {
            tile = tile_manager->get_anim_base_tile(map_ptr[j]);
            screen->blit((j*16),(i*16),(unsigned char *)tile->data,8,16,16,16,tile->transparent,&clip_rect);
           }

         tile = tile_manager->get_tile(map_ptr[j]);
         screen->blit((j*16),(i*16),(unsigned char *)tile->data,8,16,16,16,tile->transparent,&clip_rect);
        }

     }
   //map_ptr += map_width;
   map_ptr += (win_width + 2) ;//* sizeof(uint16);
  }

 drawObjs();
 
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
 drawBorder();
 
// ptr = (unsigned char *)screen->get_pixels();
// ptr += 8 * screen->get_pitch() + 8;
 
// screen->blit(8,8,ptr,8,(win_width-1) * 16,(win_height-1) * 16, win_width * 16, false);
 
 screen->update(8,8,win_width*16-16,win_height*16-16);
 
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
             if(tmp_buf[(actor->y - cur_y + 1) * (win_width+2) + (actor->x - cur_x + 1)] != 0 && 
                actor->is_visible() && actor->obj_n != 0)
               {
                tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->obj_n)+actor->frame_n);
                //FIX need a function for multi-tile actors.
                drawTile(tile,actor->x - cur_x, actor->y - cur_y, false);                
                drawTile(tile,actor->x - cur_x, actor->y - cur_y, true);
                
                //screen->blit((actor->x - cur_x)*16,(actor->y - cur_y)*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
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
 uint16 x,y;

    for(y=cur_y+win_height; y >= cur_y; y--)
      {
       for(x=cur_x+win_width;x >= cur_x; x--)
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
 uint16 x,y;
 Tile *tile;
 
 y = obj->y - cur_y;
 x = obj->x - cur_x;
 
 tile = tile_manager->get_original_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);

 //Draw a lightglobe in the middle of the 16x16 tile.
 if((tile->flags2 & 0x3) > 0 && screen->should_update_alphamap())
	 screen->drawalphamap8globe( 8 + (obj->x - cur_x)*16, 8 + (obj->y - cur_y)*16 , (tile->flags2 & 0x3)*16 );

 if(draw_lowertiles == false && (tile->flags3 & 0x4) && toptile == false) //don't display force lower tiles.
   return;
 
 if(draw_lowertiles == true && !(tile->flags3 & 0x4))
   return;

 if(tmp_buf[(y+1)*(win_width+2)+(x+1)] == 0) //don't draw object if area is in darkness.
    return;
 else
    {
     if(tmp_buf[(y+1)*(win_width+2)+(x+2)] == 0 && !(tile->flags1 & TILEFLAG_WALL))
        return;
     else
      {
       if(tmp_buf[(y+2)*(win_width+2)+(x+1)] == 0 && !(tile->flags1 & TILEFLAG_WALL))//(obj->obj_n < 290 || obj->obj_n > 302))
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
   
 if(toptile)
    {
     if(tile->toptile)
        screen->blit(x*16,y*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
    }
 else
    {
     if(!tile->toptile)
        screen->blit(x*16,y*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
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
 
 map_ptr = map->get_map_data(cur_level);
 pitch = map->get_width(cur_level);
 
 memset(tmp_buf, 0, (win_width+2) * (win_height+2) * sizeof(uint16));

 boundaryFill(map_ptr, pitch, cur_x + ((win_width - 1) / 2), cur_y + ((win_height - 1) / 2));
 
 reshapeBoundary();
}

void MapWindow::boundaryFill(unsigned char *map_ptr, uint16 pitch, uint16 x, uint16 y) 
{
 unsigned char current;
 uint16 *ptr;
 uint16 pos;
 
 if((x < cur_x - 1) || (x >= (cur_x-1) + win_width + 2) || x >= pitch)
   return;

 if((y < cur_y - 1) || (y >= (cur_y-1) + win_height + 2) || y >= pitch)
   return;
   
 pos = (y - (cur_y-1)) * (win_width+2) + (x - (cur_x-1));

 ptr = &tmp_buf[pos];

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
          tile = tile_manager->get_tile(tmp_buf[y*(win_width + 2) + x]);
      
          if((tile->tile_num >= 140 && tile->tile_num <= 187)) //main U6 wall tiles FIX for WOU games
            {
             flag = 0;
             original_flag = tile->flags1 & TILEFLAG_WALL_MASK;
            }
          else
            continue;

          //generate the required wall flags                 
          if(tmpBufTileIsWall(x,y-1))
            flag |= TILEFLAG_WALL_NORTH;
          if(tmpBufTileIsWall(x+1,y))
            flag |= TILEFLAG_WALL_EAST;
          if(tmpBufTileIsWall(x,y+1))
            flag |= TILEFLAG_WALL_SOUTH;
          if(tmpBufTileIsWall(x-1,y))
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
                tmp_buf[y*(win_width + 2) + x] = 266 + 2 * (((tile->tile_num - tile->tile_num % 16) - 140) / 16);
                continue;
               }
            }

          if(flag == 192) // 1100 bottom left corner
            {
             if(tmpBufTileIsBlack(x,y+1) && tmpBufTileIsBlack(x-1,y)) //replace with blacked corner tile
               {
                //Oh dear! this is evil. FIX
                tmp_buf[y*(win_width + 2) + x] = 266 + 1 + 2 * (((tile->tile_num - tile->tile_num % 16) - 140) / 16);
                continue;
               }
            }
            
          if((tile->flags1 & TILEFLAG_WALL_MASK) == flag) // complete match no work needed
            continue;

          // Look for a suitable tile to transform into
              
          flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;

          if(((tile->flags1) & TILEFLAG_WALL_MASK) > flag && flag != 144) // 1001 _| corner
            {
             flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
             for(;((tile->flags1 ) & TILEFLAG_WALL_MASK) != flag;)
               tile = tile_manager->get_tile(tile->tile_num - 1);
            }
          else
            {
             flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
             for(;((tile->flags1 ) & TILEFLAG_WALL_MASK) != flag;)
               tile = tile_manager->get_tile(tile->tile_num + 1);
            }

          tmp_buf[y*(win_width + 2) + x] = tile->tile_num;
         }
      }
   }
}

inline bool MapWindow::tmpBufTileIsBlack(uint16 x, uint16 y)
{
 if(tmp_buf[y*(win_width + 2) + x] == 0)
   return true;

 return false;
}

bool MapWindow::tmpBufTileIsBoundary(uint16 x, uint16 y)
{
 uint16 tile_num;
 Tile *tile;

 tile_num = tmp_buf[y*(win_width + 2) + x];
 
 if(tile_num == 0)
   return false;
   
 tile = tile_manager->get_tile(tile_num);

 if(tile->boundary)
   return true;

 if(obj_manager->is_boundary(cur_x-1+x, cur_y-1+y, cur_level))
   return true;

 return false;
}

bool MapWindow::tmpBufTileIsWall(uint16 x, uint16 y)
{
 uint16 tile_num;
 Tile *tile;

 tile_num = tmp_buf[y*(win_width + 2) + x];
 
 if(tile_num == 0)
   return false;
   
 tile = tile_manager->get_tile(tile_num);

 if(tile->flags1 & TILEFLAG_WALL)
   return true;

// if(obj_manager->is_boundary(cur_x-1+x, cur_y-1+y, cur_level))
//  return true;

 tile = obj_manager->get_obj_tile(cur_x-1+x, cur_y-1+y, cur_level, false);
 if(tile != NULL)
   {
    if(tile->flags2 & TILEFLAG_BOUNDARY)
      return true;
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
    if(tmpBufTileIsBlack(x,y))
      {
       printf("Cannot drop onto nothing!");
       return false;
      }

    map_width = map->get_width(cur_level);
 
    x = (cur_x + x) % map_width;
    y = (cur_y + y) % map_width;

    if(map->is_passable(x,y,cur_level))
	{
	  LineTestResult result;

      // make sure the player can reach the drop point
	  Actor* player = actor_manager->get_player();
	  if (!map->lineTest(player->x, player->y, x, y, cur_level, LT_HitUnpassable, result))
	  {
        return true;
	  }
	}

	if (Game::get_game ()->get_scroll ())
		Game::get_game ()->get_scroll ()->display_string("\n\nNot Possible\n\n>");
  }

 return false;
}

void MapWindow::drag_perform_drop(int x, int y, int message, void *data)
{
 uint16 map_width;
 Obj *obj, *target_obj;
 
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
    if(target_obj && target_obj->container) //drop object into a container. FIX for locked chests.
      {
       target_obj->container->addAtPos(0,obj);
      }
    else //drop object onto map
      {   
       obj->x = x;
       obj->y = y;
       obj->z = cur_level;
    
       obj_manager->add_obj(obj,true);
      }
   }
   
 return;
}

GUI_status	MapWindow::MouseDown (int x, int y, int button)
{
	//printf ("MapWindow::MouseDown, button = %i\n", button);

	Obj	*obj = get_objAtMousePos (x, y);

	if (!obj)
		return	GUI_PASS;

	float weight = obj_manager->get_obj_weight (obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS);

	if (weight == 0)
		return	GUI_PASS;

	selected_obj = obj;

	return	GUI_PASS;
}

GUI_status	MapWindow::MouseUp (int x, int y, int button)
{
	//printf ("MapWindow::MouseUp\n");
	if (selected_obj)
	{
		selected_obj = NULL;
	}

	return	GUI_PASS;
}

GUI_status	MapWindow::MouseMotion (int x, int y, Uint8 state)
{
	Tile	*tile;

	//	printf ("MapWindow::MouseMotion\n");
	if (selected_obj && !dragging)
	{
		// ensure that the player can reach the selected object before
		// letting them drag it
		int wx, wy;
		mouseToWorldCoords (x, y, wx, wy);

		LineTestResult result;
		Actor* player = actor_manager->get_player();

		if (map->lineTest(player->x, player->y, wx, wy, cur_level, LT_HitUnpassable, result))
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
