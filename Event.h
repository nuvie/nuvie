#ifndef __Event_h__
#define __Event_h__

/*
 *  Event.h
 *  Nuive
 *
 *  Created by Eric Fry on Wed Mar 26 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "U6def.h"

#include "Configuration.h"
#include "ObjManager.h"
#include "MapWindow.h"
#include "MsgScroll.h"
#include "Player.h"
#include "Book.h"
#include "Converse.h"

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
 Player *player;
 EventMode mode;
 Book *book;
 Converse *converse;
 
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
           Converse *c);
  
 bool update();
 bool move(sint16 rel_x, sint16 rel_y);
 bool use(sint16 rel_x, sint16 rel_y);
 bool look();
 bool talk();
 void alt_code(const char *cs);
 void alt_code_input(const char *in);
 void clear_alt_code() { alt_code_str[0] = '\0'; alt_code_len = 0; }

 void wait();
 
 protected:
 
 inline Uint32 TimeLeft();
};

#endif /* __Event_h__ */
