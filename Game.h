#ifndef __Game_h__
#define __Game_h__

/*
 *  Game.h
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
#include <SDL.h>

#include "Configuration.h"

#include "U6Bmp.h"

#include "Screen.h"
#include "GamePalette.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"

#include "MsgScroll.h"
#include "Map.h"

#define NUVIE_GAME_ULTIMA6 1
#define NUVIE_GAME_MARTIAN 2
#define NUVIE_GAME_SAVAGE  4

#define NUVIE_INTERVAL    50

#define NUVIE_CMD_MODE  1
#define NUVIE_TALK_MODE 2
#define NUVIE_CAST_MODE 4

class Game
{
 uint8 type;
 
 Configuration *config;
 Screen *screen;
 U6Bmp *background;
 GamePalette *palette;
 Text *text;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 Map *game_map;
 MsgScroll *scroll;
 
 uint8 game_mode;
 
 int ts;
 SDL_Event event;
 bool game_stop;
 
 public:
 
 Game(Configuration *cfg);
 ~Game();
 
 bool loadGame(Screen *screen, uint8 game_type);
 
 bool loadBackground();
 void drawBackground();
 
 void play();
 
 protected:
 
 void updateEvents();
 
 inline Uint32 TimeLeft();
 void wait();
};

#endif /* __Game_h__ */