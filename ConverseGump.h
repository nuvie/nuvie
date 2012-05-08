#ifndef __ConverseGump_h__
#define __ConverseGump_h__

/*
 *  ConverseGump.h
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
#include "CallBack.h"
#include "GUI_widget.h"
#include <stdarg.h>

#include <list>
#include <vector>
using std::list;


class Configuration;
class Font;
class MsgScroll;
class Actor;

class ConverseGump: public MsgScroll
{

	std::list<MsgText> display_text;

	unsigned char *npc_portrait;
	unsigned char *avatar_portrait;

 public:

 ConverseGump(Configuration *cfg, Font *f);
 ~ConverseGump();

 void set_actor_portrait(Actor *a);
 virtual void add_token(MsgText *token);

 void Display(bool full_redraw);
 GUI_status KeyDown(SDL_keysym key);
};


#endif /* __ConverseGump_h__ */

