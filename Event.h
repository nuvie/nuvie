#ifndef __Event_h__
#define __Event_h__

/*
 *  Event.h
 *  Nuvie
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
#include <list>
#include "U6def.h"

#include "Configuration.h"
#include "ObjManager.h"
#include "MapWindow.h"
#include "MsgScroll.h"
#include "GameClock.h"
#include "Player.h"
#include "Converse.h"
#include "ViewManager.h"
#include "UseCode.h"
#include "GUI.h"

class Book;
class Game;
class TimeQueue;

#define NUVIE_INTERVAL    65

typedef enum {
 LOOK_MODE,
 USE_MODE,
 CAST_MODE,
 GET_MODE,
 MOVE_MODE,
 DROP_MODE,
 TALK_MODE,
 USESELECT_MODE,
 FREESELECT_MODE /*...or use a single free-move mode for get,talk,select?*/
} EventMode;

extern uint32 nuvieGameCounter;


class Event
{
 Configuration *config;
 GUI *gui;
 ObjManager *obj_manager;
 MapWindow *map_window;
 MsgScroll *scroll;
 GameClock *clock;
 Player *player;
 EventMode mode;
 Book *book;
 Converse *converse;
 ViewManager *view_manager;
 UseCode *usecode;
 
 SDL_Event event;
 int ts; //timestamp for TimeLeft() method.
 char alt_code_str[4]; // string representation of alt-code input
 uint8 alt_code_len; // how many characters have been input for alt-code
 uint16 active_alt_code; // alt-code that needs more input
 uint8 alt_code_input_num; // alt-code can get multiple inputs

 TimeQueue *time_queue;
 Obj *use_obj;

 public:
 Event(Configuration *cfg);
 ~Event();

 bool init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p,
           GameClock *gc, Converse *c, ViewManager *vm, UseCode *uc, GUI *g);
 Book *get_book() { return(book); }
 TimeQueue *get_time_queue() { return(time_queue); }

 bool update();
 bool handleEvent(const SDL_Event *event);
 void useselect_mode(Obj *src, const char *prompt = NULL);
 void freeselect_mode(Obj *src, const char *prompt = NULL);

 bool move(sint16 rel_x, sint16 rel_y);
 bool use(sint16 rel_x, sint16 rel_y);
 bool get(sint16 rel_x, sint16 rel_y);
 bool look();
 bool talk();
 bool select_obj(Obj *obj = NULL, Actor *actor = NULL);
 bool select_obj(sint16 rel_x, sint16 rel_y);

 void alt_code(const char *cs);
 void alt_code_input(const char *in);
 void clear_alt_code() { alt_code_str[0] = '\0'; alt_code_len = 0; }
 bool alt_code_teleport(const char *location_string);
 void alt_code_infostring();

 void wait();

 protected:

 inline Uint32 TimeLeft();
 
 void quitDialog();
 bool handleSDL_KEYDOWN (const SDL_Event *event);
 
};


#endif /* __Event_h__ */
