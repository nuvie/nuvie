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
#include "GameClock.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "Player.h"
#include "Converse.h"

#include "MsgScroll.h"
#include "Map.h"
#include "MapWindow.h"
#include "Event.h"

#define NUVIE_GAME_ULTIMA6 1
#define NUVIE_GAME_MARTIAN 2
#define NUVIE_GAME_SAVAGE  4

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
 ActorManager *actor_manager;
 Map *game_map;
 MapWindow *map_window;
 MsgScroll *scroll;
 Player *player;
 Converse *converse;

 GameClock *clock;
 
 Event *event;
 
 public:
 
 Game(Configuration *cfg);
 ~Game();
 
 bool loadGame(Screen *screen, uint8 game_type);
 
 bool loadBackground();
 void drawBackground();
 
 void play();
 
};

#endif /* __Game_h__ */

