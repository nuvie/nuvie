/*
 *  MapWindow.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Fri Mar 21 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "MapWindow.h"

MapWindow::MapWindow(Configuration *cfg)
{
 config = cfg;
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
   
}

MapWindow::~MapWindow()
{
 
}

bool MapWindow::init(Screen *s, Map *m, TileManager *tm, ObjManager *om, ActorManager *am)
{
 screen = s;
 map = m;
 tile_manager = tm;
 obj_manager = om;
 actor_manager = am;

 cursor_tile = tile_manager->get_tile(365); // might change in MD, SE
 use_tile = tile_manager->get_tile(364); // might change in MD, SE

 set_windowSize(11,11);
 
 return true;
}

bool MapWindow::set_windowSize(uint16 width, uint16 height)
{
 win_width = width;
 win_height = height;
 
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
 clip_rect.y = 8;
 clip_rect.w = (win_width - 1) * 16;
 clip_rect.h = (win_height - 1) * 16;
 
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
  
 if(new_x >= 0 && new_x <= map_side_length - win_width)
    {
     if(new_y >= 0 && new_y <= map_side_length - win_height)
        {
         cur_x = new_x;
         cur_y = new_y;
         cur_level = new_level;
         updateBlacking();
        }
     } 
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

char *MapWindow::lookAtCursor()
{
 Actor *actor;
 
 if(tmp_buf[(cursor_y+1) * (win_width+2) + (cursor_x+1)] == 0) //black area
   return tile_manager->lookAtTile(0,0); // nothing to see here. ;)

 actor = actor_manager->get_actor(cur_x + cursor_x, cur_y + cursor_y, cur_level);
 if(actor != NULL)
   {
    return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(actor->get_tile_num()),0);
   }
   
 return map->look(cur_x + cursor_x, cur_y + cursor_y, cur_level);
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
 
void MapWindow::updateBlacking()
{
 generateTmpMap();
}

void MapWindow::drawMap()
{
 uint16 i,j;
 uint16 *map_ptr;
 uint16 map_width;
 Tile *tile;
 unsigned char *ptr;
 
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

 drawBorder();
 
// ptr = (unsigned char *)screen->get_pixels();
// ptr += 8 * screen->get_pitch() + 8;
 
// screen->blit(8,8,ptr,8,(win_width-1) * 16,(win_height-1) * 16, win_width * 16, false);
 
 screen->update(0,0,win_width*16,win_height*16);
 
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
             if(tmp_buf[(actor->y - cur_y + 1) * (win_width+2) + (actor->x - cur_x + 1)] != 0)
               {
                tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->a_num)+actor->frame_n);
                screen->blit((actor->x - cur_x)*16,(actor->y - cur_y)*16,tile->data,8,16,16,16,tile->transparent,&clip_rect);
               }
            }
         }
      }
   }
}

void MapWindow::drawObjs()
{
 uint16 sc_x, sc_y;
 uint16 sc_w, sc_h;
 uint16 x,y;
 
 if(cur_level == 0)
   {
    sc_y = cur_y / 128;
    sc_x = cur_x / 128;
    
    if(cur_x % 128 + win_width > 128)
       sc_w = 2;
    else
       sc_w = 1;

    if(cur_y % 128 + win_height > 128)
       sc_h = 2;
    else
       sc_h = 1;

    for(y=sc_y; y < sc_y + sc_h; y++)
      {
       for(x=sc_x; x < sc_x + sc_w; x++)
         drawObjSuperBlock(obj_manager->get_obj_superchunk(x,y,0),false);
      }      
   }
 else
   {
    drawObjSuperBlock(obj_manager->get_obj_superchunk(0,0,cur_level),false); //draw objects for dungeon level
   }
   
 drawActors();

 if(cur_level == 0)
   {
    for(y=sc_y; y < sc_y + sc_h; y++)
      {
       for(x=sc_x; x < sc_x + sc_w; x++)
         drawObjSuperBlock(obj_manager->get_obj_superchunk(x,y,0),true);
      }      
   }
 else
   {
    drawObjSuperBlock(obj_manager->get_obj_superchunk(0,0,cur_level),true); //draw objects for dungeon level
   }
 
 return;
}

void MapWindow::drawObjSuperBlock(U6LList *superblock, bool toptile)
{
 U6Link *link, *link1;
 ObjList *obj_list;
 Obj *obj;
 
 for(link=superblock->start();link != NULL;)
  {
   obj_list = (ObjList *)link->data;
//   if(obj->y > cur_y + win_height)
//      break;

   if(obj_list->y >= cur_y && obj_list->y <= cur_y + win_height)
     {
      if(obj_list->x >= cur_x && obj_list->x <= cur_x + win_width)
        {
         for(link1=obj_list->objs->start();link1 != NULL;link1=link1->next)
           {
            obj = (Obj *)link1->data;
            drawObj(obj, toptile);
           }
        }
     }
     
   link = superblock->next();
  }
 
}

inline void MapWindow::drawObj(Obj *obj, bool toptile)
{
 uint16 x,y;
 Tile *tile;
 
 y = obj->y - cur_y;
 x = obj->x - cur_x;
 
 tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
 
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
   drawTopTile(tile,x,y,toptile);
       
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
 
 if((x < cur_x - 1) || (x >= (cur_x-1) + win_width + 2))
   return;

 if((y < cur_y - 1) || (y >= (cur_y-1) + win_height + 2))
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
 
 tile = tile_manager->get_tile(tile_num);
 if(!(tile->flags2 & TILEFLAG_WINDOW))
   return false;

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

void MapWindow::reshapeBoundary()
{
 uint16 x,y;
 uint8 flag;
 Tile *tile;
 
 for(y=1;y <= win_height;y++)
   {
    for(x=1;x <= win_width;x++)
      {
       if(tmpBufTileIsBoundary(x,y))
         {
          tile = tile_manager->get_tile(tmp_buf[y*(win_width + 2) + x]);
        
          if((tile->tile_num >= 144 && tile->tile_num <= 146) ||
             (tile->tile_num >= 160 && tile->tile_num <= 162) ||
             (tile->tile_num >= 176 && tile->tile_num <= 178))
            {
             flag = 0;
            }
          else
            continue;
         /*
         flag = 0;
         if(!tile->boundary)
           continue;
         if(tile->flags2 & TILEFLAG_WINDOW)
           continue;
           

          if(tile->tile_num < 144 || tile->tile_num > 187)
            continue;

          flag = 0;
*/                 
          if(tmpBufTileIsWall(x,y-1))
            flag |= TILEFLAG_WALL_NORTH;
          if(tmpBufTileIsWall(x+1,y))
            flag |= TILEFLAG_WALL_EAST;
          if(tmpBufTileIsWall(x,y+1))
            flag |= TILEFLAG_WALL_SOUTH;
          if(tmpBufTileIsWall(x-1,y))
            flag |= TILEFLAG_WALL_WEST;
            
          if(flag == 0) //isolated border tiles
            continue; 
            
                       if(tile->flags1 & 0xf0 == flag)
              continue;

          if(flag == 192)
            {
             tmp_buf[y*(win_width + 2) + x] = 266 + 1 + 2 * (((tile->tile_num - tile->tile_num % 16) - 144) / 16);
             continue;
            }

          if(flag == 48)
            {
             tmp_buf[y*(win_width + 2) + x] = 266 + 2 * (((tile->tile_num - tile->tile_num % 16) - 144) / 16);
             continue;
            }
           if(tile->tile_num <= 187)
            {
              
             flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
          
   //       for(;(flag & tile->flags1) != flag;)
    //        {
    //      if(((tile->flags1  & 0xf0) & flag))
    //        {
             //tile = tile_manager->get_tile(tile->tile_num + 1);

             if(((tile->flags1) & 0xf0) > flag && flag != 144)
              {
               flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
               for(;((tile->flags1 ) & 0xf0) != flag;)
                 tile = tile_manager->get_tile(tile->tile_num - 1);
              }
             else
              {
               flag |= TILEFLAG_WALL_NORTH | TILEFLAG_WALL_WEST;
               for(;((tile->flags1 ) & 0xf0) != flag;)
                 tile = tile_manager->get_tile(tile->tile_num + 1);
              }
             }  
          tmp_buf[y*(win_width + 2) + x] = tile->tile_num;
         }
      }
   }
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

 tile = obj_manager->get_obj_tile(cur_x-1+x, cur_y-1+y, cur_level, false);
 if(tile != NULL)
   {
    if(tile->flags1 & TILEFLAG_WALL)
      return true;
   }
   
 return false;
}
