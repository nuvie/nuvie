/*
 *  Event.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Wed Mar 26 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include <SDL.h>


#include "U6def.h"
#include "Configuration.h"

#include "MapWindow.h"
#include "Player.h"

#include "Event.h"

uint32 nuvieGameCounter;

Event::Event(Configuration *cfg)
{
 config = cfg;
}

bool Event::init(ObjManager *om, MapWindow *mw, Player *p)
{
 obj_manager = om;
 map_window = mw;
 player = p;
 
 mode = MOVE_MODE;
 
 return true;
}

bool Event::update()
{
 
  while ( SDL_PollEvent(&event) ) {
			switch (event.type) {

				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYDOWN:
           switch(event.key.keysym.sym)
            {
             case SDLK_UP    :
                               move(0,-1);
                               break;
             case SDLK_DOWN  :
                               move(0,1);
                               break;
             case SDLK_LEFT  :
                               move(-1,0);
                               break;
             case SDLK_RIGHT :
                               move(1,0);
                               break; 
					   case SDLK_q     :
						                   return false;
					                     break;
             case SDLK_l     :
                               mode = LOOK_MODE;
                               map_window->centerCursor();
                               map_window->set_show_cursor(true);
                               break;
             case SDLK_u     : 
                               mode = USE_MODE;
                               map_window->centerCursor();
                               map_window->set_show_use_cursor(true);
                               break;
             case SDLK_RETURN  :
                               if(mode == LOOK_MODE)
                                 {
                                  printf("%s\n",map_window->lookAtCursor());
                                  mode = MOVE_MODE;
                                  map_window->set_show_cursor(false);
                                 }
                               if(mode == USE_MODE)
                                 {
                                  mode = MOVE_MODE;
                                  use(0,0);
                                  map_window->set_show_use_cursor(false);
                                 }
                               break;
             default : break;                    
            }
          break;
             
        case SDL_QUIT :
                       return false; //time to quit.
                       break;
        default:
        break;
			}
		}

 return true;
}

bool Event::move(sint16 rel_x, sint16 rel_y)
{
 if(mode == LOOK_MODE)
    map_window->moveCursorRelative(rel_x,rel_y);
 else
  {
   if(mode == USE_MODE)
     {
      use(rel_x,rel_y);
     }
   else
      player->moveRelative(rel_x,rel_y);
  }
 return true;
}

bool Event::use(sint16 rel_x, sint16 rel_y)
{
 uint16 x,y;
 uint8 level;
 
 player->get_location(&x,&y,&level);
 
 obj_manager->use_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level);
 map_window->set_show_use_cursor(false);
 map_window->updateBlacking();
 mode = MOVE_MODE;

 return true;
}

void Event::wait()
{
 SDL_Delay(TimeLeft());
}

//Protected

inline Uint32 Event::TimeLeft()
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