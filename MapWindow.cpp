/*
 *  MapWindow.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Fri Mar 21 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "MapWindow.h"

MapWindow::MapWindow(Configuration *cfg)
{
 config = cfg;
 screen = NULL;
 
 cur_x = 0;
 cur_y = 0;
 
 win_width = 11;
 win_height = 11;
 
 cur_level = 0;
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

 return true;
}
void MapWindow::set_windowSize(uint16 width, uint16 height)
{
 win_width = width;
 win_height = height;
}
 
void MapWindow::moveLevel(uint8 new_level)
{
 cur_level = new_level;
}

void MapWindow::move(uint16 new_x, uint16 new_y)
{
 moveRelative(new_x - cur_x, new_y - cur_y);
}

void MapWindow::moveRelative(sint16 rel_x, sint16 rel_y)
{
 uint16 map_side_length;
 
 if(cur_level == 0)
   map_side_length = 1024;
 else
   map_side_length = 256;
  
 if(cur_x + rel_x >= 0 && cur_x + rel_x <= map_side_length - win_width)
     cur_x += rel_x;
   
 if(cur_y + rel_y >= 0 && cur_y + rel_y <= map_side_length - win_height)
     cur_y += rel_y;
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
 
void MapWindow::drawMap()
{
 uint16 i,j;
 unsigned char *map_ptr;
 uint16 map_width;
 Tile *tile;
 
 map_ptr = map->get_map_data(cur_level);
 map_width = map->get_width(cur_level);
 
 map_ptr += cur_y * map_width + cur_x;
    
  for(i=0;i<win_height;i++)
  {
   for(j=0;j<win_width;j++)
     {
      tile = tile_manager->get_tile(map_ptr[j]); 
      screen->blit((unsigned char *)tile->data,8,(j*16),(i*16),16,16);
     }
   map_ptr += map_width;
  }

 drawObjs();
 
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
   
 actor_manager->drawActors(screen, cur_x, cur_y, win_width, win_height, cur_level);

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
 U6Link *link;
 Obj *obj;
 
 for(link=superblock->start();link != NULL;)
  {
   obj = (Obj *)link->data;
//   if(obj->y > cur_y + win_height)
//      break;

   if(obj->y >= cur_y && obj->y <= cur_y + win_height)
     {
      if(obj->x >= cur_x && obj->x <= cur_x + win_width)
        {
         drawObj(obj, toptile);
        }
     }
     
   link = superblock->next();
  }
 
}

inline void MapWindow::drawObj(Obj *obj, bool toptile)
{
 //Tile *tile;
 
  //tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n); 
  //screen->blit((char *)tile->data,8,((obj->x - cur_x)*16),((obj->y - cur_y)*16),16,16,tile->transparent);
  
  drawTile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n,obj->x - cur_x, obj->y - cur_y, toptile);
  
}

inline void MapWindow::drawTile(uint16 tile_num, uint16 x, uint16 y, bool toptile)
{
 Tile *tile;
 bool dbl_width, dbl_height;
 
 tile = tile_manager->get_tile(tile_num);

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
 if(toptile)
    {
     if(tile->toptile)
        screen->blit(tile->data,8,x*16,y*16,16,16,tile->transparent);
    }
 else
    {
     if(!tile->toptile)
        screen->blit(tile->data,8,x*16,y*16,16,16,tile->transparent);
    } 
}
