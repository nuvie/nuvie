/*
 *  Copyright (C) 2001-2011 The Exult Team
 *  Copyright (C) 2012 The Nuvie Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef KEYS_H
#define KEYS_H

#include "SDL_version.h"
//#include "sdl-compat.h" // Nuvie doesn't have this file but likely would whenever compatibility is added
#if SDL_VERSION_ATLEAST(1, 3, 0)
  #include "SDL_stdinc.h"
  #include "SDL_scancode.h"
#endif

#include "SDL_events.h"

#include <vector>
#include <map>
#include <string>

struct str_int_pair
{
	const char *str;
	int  num;
};

const int c_maxparams = 4;

struct Action;
struct ActionType {
	const Action* action;
	int params[c_maxparams];
};

struct ltSDLkeysym
{
	bool operator()(SDL_keysym k1, SDL_keysym k2) const
	{
		if (k1.sym == k2.sym)
			return k1.mod < k2.mod;
		else
			return k1.sym < k2.sym;
	}
};

typedef std::map<SDL_keysym, ActionType, ltSDLkeysym>   KeyMap;

class KeyBinder {
 private:
	KeyMap bindings;
	
	std::vector<std::string> keyhelp;
	std::vector<std::string> cheathelp;

 	void LoadFromFileInternal(const char* filename);
public:
	KeyBinder();
	~KeyBinder();
	/* Add keybinding */
	void AddKeyBinding(SDLKey sym, int mod, const Action* action,
					   int nparams, int* params);
	
	/* Delete keybinding */
//	void DelKeyBinding(SDLKey sym, int mod); // unused
	
	/* Other methods */
	void Flush() {
		bindings.clear(); keyhelp.clear(); cheathelp.clear();
	}
	bool DoAction(ActionType action);
	bool HandleEvent(const SDL_Event *event);
	
	void LoadFromFile(const char* filename);
	void LoadFromPatch();
	void handle_wrong_key_pressed();

	void ShowKeys();
 private:
	void ParseText(char *text, int len);
	void ParseLine(char *line);
	void FillParseMaps();
};

#endif /* KEYS_H */
