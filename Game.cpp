/*
 *  Game.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
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
#include "Configuration.h"
#include "U6Bmp.h"

#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "Look.h"

#include "Game.h"

Game::Game(Configuration *cfg)
{
 config = cfg;
 
}

Game::~Game()
{
}
 
bool Game::loadGame(Screen *s, uint8 game_type)
{
 screen = s;

 tile_manager = new TileManager(config);
 tile_manager->loadTiles();

 obj_manager = new ObjManager(config);
 obj_manager->loadObjs(tile_manager);
 
 palette = new GamePalette(screen,config);
 
 loadBackground();

 text = new Text(config);
 text->loadFont();

 scroll = new MsgScroll(text);
 
 game_map = new Map(config);
 game_map->loadMap(tile_manager, obj_manager);
 
 actor_manager = new ActorManager(config, game_map, tile_manager, obj_manager);
 actor_manager->loadActors();
  
 map_window = new MapWindow(config);
 map_window->init(screen, game_map, tile_manager, obj_manager, actor_manager);

 player = new Player(config);
 player->init(actor_manager->get_actor(1),actor_manager, map_window);
 
 event = new Event(config);
 event->init(obj_manager, map_window, player);
 
 map_window->set_windowSize(11,11);
 //map_window->move(0x12e,0x16b);
 map_window->centerMapOnActor(player->get_actor());

 
 return true;
}

bool Game::loadBackground()
{
 std::string filename;
 
 background = new U6Bmp();
 
 config->pathFromValue("config/ultima6/gamedir", "paper.bmp", filename);
 
 if(background->open(filename) == false)
   return false;

 return true;
}

void Game::drawBackground()
{
 //char *pixels;
 unsigned char *data;
 uint16 width, height;

 //pixels = (unsigned char *)screen->get_pixels();
 data = background->get_data();
 width = background->get_width();
 height = background->get_height();

 screen->blit(0, 0, data, 8,  width, height, width,false);
 
 //memcpy(pixels,data,data_size);

 return;
}
  
void Game::play()
{
 bool game_play = true;
 
  drawBackground();
  screen->update();
  
  map_window->drawMap();
  
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  
	for( ; game_play ; ) 
    {
     game_play = event->update();
     palette->rotatePalette();
     tile_manager->update();
     actor_manager->updateActors();
     map_window->drawMap();
     scroll->updateScroll();
     //screen->update();
     event->wait();
	  }

 return;
}
