#ifndef __Event_h__
#define __Event_h__

/*
 *  Event.h
 *  Nuive
 *
 *  Created by Eric Fry on Wed Mar 26 2003.
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
#include "ObjManager.h"
#include "MapWindow.h"
#include "MsgScroll.h"
#include "GameClock.h"
#include "Player.h"
#include "Book.h"
#include "Converse.h"
#include "ViewManager.h"

#define NUVIE_INTERVAL    50

typedef enum {
 LOOK_MODE,
 USE_MODE,
 CAST_MODE,
 GET_MODE,
 MOVE_MODE,
 DROP_MODE,
 TALK_MODE
} EventMode;

extern uint32 nuvieGameCounter;

class Event
{
 Configuration *config;
 ObjManager *obj_manager;
 MapWindow *map_window;
 MsgScroll *scroll;
 GameClock *clock;
 Player *player;
 EventMode mode;
 Book *book;
 Converse *converse;
 ViewManager *view_manager;
 
 SDL_Event event;
 int ts; //timestamp for TimeLeft() method.
 char alt_code_str[4]; // string representation of alt-code input
 uint8 alt_code_len; // how many characters have been input for alt-code
 uint16 active_alt_code; // alt-code that needs more input
 uint8 alt_code_input_num; // alt-code can get multiple inputs
  
 public:
 Event(Configuration *cfg);
 ~Event();
 
 bool init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p,
           GameClock *gc, Converse *c, ViewManager *vm);
  
 bool update();
 bool move(sint16 rel_x, sint16 rel_y);
 bool use(sint16 rel_x, sint16 rel_y);
 bool look();
 bool talk();
 void alt_code(const char *cs);
 void alt_code_input(const char *in);
 void clear_alt_code() { alt_code_str[0] = '\0'; alt_code_len = 0; }
 bool alt_code_teleport(const char *location_string);
 void alt_code_infostring();
 
 void wait();
 
 protected:
 
 inline Uint32 TimeLeft();
};

#endif /* __Event_h__ */
