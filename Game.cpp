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
 obj_manager->loadObjs();
 
 palette = new GamePalette(screen,config);
 
 loadBackground();

 text = new Text(config);
 text->loadFont();

 scroll = new MsgScroll(text);
 
 game_map = new Map(config);
 game_map->loadMap(screen, tile_manager, obj_manager);
 
 game_map->moveWindow(0x12e,0x16b);

 game_map->set_windowSize(11,11);
 
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
 char *pixels;
 char *data;
 uint32 data_size;

 pixels = (char *)screen->get_pixels();
 data = (char *)background->get_data();
 data_size = background->get_width() * background->get_height();
 
 memcpy(pixels,data,data_size);

 return;
}
  
void Game::play()
{

  drawBackground();
  
  game_map->drawMap();
  
  screen->update();
  
  game_stop = false;
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  
	for( ; !game_stop ; ) 
    {
     updateEvents();
     tile_manager->update();
     game_map->drawMap();
     scroll->updateScroll();
     screen->update();
     wait();
	  }

 return;
}

void Game::updateEvents()
{
 
  while ( SDL_PollEvent(&event) ) {
			switch (event.type) {

				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYDOWN:
					if(event.key.keysym.sym==SDLK_UP)
            { game_map->moveWindowRelative(0,-1); break; }
 					if(event.key.keysym.sym==SDLK_DOWN)
            { game_map->moveWindowRelative(0,1); break; }
					if(event.key.keysym.sym==SDLK_LEFT)
            { game_map->moveWindowRelative(-1,0); break; }
					if(event.key.keysym.sym==SDLK_RIGHT)
            { game_map->moveWindowRelative(1,0); break; }
       
				case SDL_QUIT:
					game_stop = true;
					break;
				default:
					break;
			}
		}

}

inline Uint32 Game::TimeLeft()
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+NUVIE_INTERVAL;
        return(0);
    }
    return(next_time-now);
}

void Game::wait()
{
 SDL_Delay(TimeLeft());
}
