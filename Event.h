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
 
 SDL_Event event;
 int ts; //timestamp for TimeLeft() method.
  
 public:
 Event(Configuration *cfg);
 
 bool init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p);
  
 bool update();
 bool move(sint16 rel_x, sint16 rel_y);
 bool use(sint16 rel_x, sint16 rel_y);
 bool look();
 
 void wait();
 
 protected:
 
 inline Uint32 TimeLeft();
};

#endif /* __Event_h__ */
