#ifndef __ScriptActor_h__
#define __ScriptActor_h__

/*
 *  Script.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jun 7 2009.
 *  Copyright (c) 2009. All rights reserved.
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

extern "C"
{
#include "lauxlib.h"
}

void nscript_init_actor(lua_State *L);
bool nscript_new_actor_var(lua_State *L, uint16 actor_num);

#endif /* __ScriptActor_h__ */