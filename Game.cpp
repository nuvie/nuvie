/*
 *  Game.cpp
 *  Nuvie
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
#include "U6misc.h"

#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "Look.h"

#include "U6UseCode.h"
#include "MDUseCode.h"
#include "SEUseCode.h"

#include "Game.h"
#include "GUI_button.h"
#include "GUI_area.h"
#include "GUI_text.h"

Game *Game::game = NULL;

Game::Game(Configuration *cfg)
{
 game = this;
 config = cfg;
}

Game::~Game()
{
    delete tile_manager;
    delete obj_manager;
    delete palette;
    delete text;
    delete scroll;
    delete game_map;
    delete actor_manager;
    delete map_window;
    delete player;
    delete event;
    delete background;
    delete converse;
    delete clock;
    delete party;
    delete view_manager;
}
 
bool Game::loadGame(Screen *s, uint8 type)
{
 GUI_Button *test_button;
 GUI_Area *test_area;
 GUI_Text *test_text;
  
 screen = s;
 game_type = type;
 
 gui = new GUI(screen);

 try
  {
   tile_manager = new TileManager(config);
   tile_manager->loadTiles();

   obj_manager = new ObjManager(config);
   obj_manager->loadObjs(tile_manager);
 
   palette = new GamePalette(screen,config);
 
   clock = new GameClock(config);
   clock->init();
 
   loadBackground();

   text = new Text(config);
   text->loadFont();

   game_map = new Map(config);
   game_map->loadMap(tile_manager, obj_manager);
 
   actor_manager = new ActorManager(config, game_map, tile_manager, obj_manager, clock);
   actor_manager->loadActors();
   game_map->set_actor_manager(actor_manager);
  
   map_window = new MapWindow(config);
   map_window->init(game_map, tile_manager, obj_manager, actor_manager);
   gui->AddWidget(map_window);
   
   player = new Player(config);
   party = new Party(config);
   player->init(actor_manager, map_window, clock, party);
   party->init(this, actor_manager); // (reverse Game reference test-case)
 
   portrait = new Portrait(config);
   portrait->init();
 
   view_manager = new ViewManager(config);
   view_manager->init(screen, text, party, tile_manager, obj_manager, portrait);
 
   scroll = new MsgScroll(config);
   scroll->init(text, player->get_name());
   gui->AddWidget(scroll);

   map_window->set_windowSize(11,11);
   //map_window->move(0x12e,0x16b);
   map_window->centerMapOnActor(player->get_actor());

   converse = new Converse();
   converse->init(config, game_type, scroll, actor_manager, clock, player, view_manager, obj_manager);

   // Correct usecode class for each game
   switch (game_type)
     { 
      case NUVIE_GAME_U6 : usecode = (UseCode *) new U6UseCode(config); break;
      case NUVIE_GAME_MD : usecode = (UseCode *) new MDUseCode(config); break;
      case NUVIE_GAME_SE : usecode = (UseCode *) new SEUseCode(config); break;
     }

   usecode->init(obj_manager, game_map, player, scroll);
   obj_manager->set_usecode(usecode);

 
   event = new Event(config);
   event->init(obj_manager, map_window, scroll, player, clock, converse, view_manager, usecode, gui);
  }
 catch(const char *error_string)
  {
   printf("Error: %s\n",error_string);
   return false;
  }
 
 return true;
}

bool Game::loadBackground()
{
 std::string filename;
 U6Lib_n file;
 unsigned char *temp_buf;
  
 switch(game_type)
   {
    case NUVIE_GAME_U6 : config_get_path(config,"paper.bmp",filename);
                         background = (U6Shape *) new U6Bmp();
                         if(background->load(filename) == false)
                           return false;
                         break;

    case NUVIE_GAME_MD : 
                         background = new U6Shape();
                         config_get_path(config,"mdscreen.lzc",filename);
                         file.open(filename,4,game_type);
                         temp_buf = file.get_item(0);
                         background->load(temp_buf + 8);
                         free(temp_buf);
                         break;

    case NUVIE_GAME_SE : 
                         background = new U6Shape();
                         config_get_path(config,"screen.lzc",filename);
                         file.open(filename,4,game_type);
                         temp_buf = file.get_item(0);
                         background->load(temp_buf + 8);
                         free(temp_buf);
                         break;
   }

 return true;
}

void Game::drawBackground()
{
 unsigned char *data;
 uint16 width, height;

 data = background->get_data();
 background->get_size(&width,&height);

 //pixels = (unsigned char *)screen->get_pixels();

 screen->blit(0, 0, data, 8,  width, height, width,true);
 
 //memcpy(pixels,data,data_size);

 return;
}
  
void Game::play()
{
  bool game_play = true;

  drawBackground();

  scroll->display_prompt();
  
  view_manager->set_inventory_mode(1); //FIX
  
  screen->update();
  
  //map_window->drawMap();
  
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);

  for( ; game_play ; ) 
   {
     game_play = event->update();
     palette->rotatePalette();
     tile_manager->update();
     actor_manager->twitchActors();
     //map_window->drawMap();
     converse->continue_script();
     //scroll->updateScroll();

     gui->Display();

     screen->preformUpdate();
     event->wait();
   }
  return;
}


 
