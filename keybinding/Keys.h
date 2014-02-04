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
#include "KeysEnum.h"

#ifdef HAVE_JOYSTICK_SUPPORT
#include "nuvieDefs.h"

typedef enum { AXES_PAIR1, AXES_PAIR2, AXES_PAIR3, AXES_PAIR4, UNHANDLED_AXES_PAIR } joy_axes_pairs;
#endif

#include <vector>
#include <map>
#include <string>

struct str_int_pair
{
	const char *str;
	int  num;
};

const int c_maxparams = 1;

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

class Configuration;

class KeyBinder {
 private:
	KeyMap bindings;
	
	std::vector<std::string> keyhelp;
	std::vector<std::string> cheathelp;
#ifdef HAVE_JOYSTICK_SUPPORT
	SDL_Joystick *joystick;
	bool repeat_hat, joy_repeat_enabled; // repeat hat instead of axis when hat is found
	uint32 next_axes_pair_update, next_axes_pair2_update, next_axes_pair3_update,
	       next_axes_pair4_update, next_joy_repeat_time;
	uint16 pair1_delay, pair2_delay, pair3_delay, pair4_delay, joy_repeat_delay, x_axis_deadzone,
           y_axis_deadzone, x_axis2_deadzone, y_axis2_deadzone, x_axis3_deadzone, y_axis3_deadzone,
           x_axis4_deadzone, y_axis4_deadzone;
	uint8 x_axis, y_axis, x_axis2, y_axis2, x_axis3, y_axis3, x_axis4, y_axis4;
#endif
 	void LoadFromFileInternal(const char* filename);
public:
	KeyBinder(Configuration *config);
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
	ActionType get_ActionType(SDL_keysym key);
	ActionKeyType GetActionKeyType(ActionType a);
	bool DoAction(ActionType const& a) const;
	KeyMap::iterator get_sdlkey_index(SDL_keysym key);
	bool HandleEvent(const SDL_Event *event);
	
	void LoadFromFile(const char* filename);
	void LoadGameSpecificKeys();
	void LoadFromPatch();
	void handle_wrong_key_pressed();
	bool handle_always_available_keys(ActionType a);

	void ShowKeys() const;
#ifdef HAVE_JOYSTICK_SUPPORT
	SDLKey get_key_from_joy_walk_axes() { return get_key_from_joy_axis_motion(x_axis, true); }
	SDLKey get_key_from_joy_axis_motion(int axis, bool repeating);
	SDLKey get_key_from_joy_hat_button(uint8 hat_button);
	SDLKey get_key_from_joy_events(SDL_Event *event);
	void init_joystick(Configuration *config);
	SDL_Joystick *get_joystick() { return joystick; }
	uint32 get_next_joy_repeat_time() { return next_joy_repeat_time; }
	void set_enable_joy_repeat(bool val) { if (joy_repeat_delay == 10000) return; joy_repeat_enabled = val; }
	bool is_joy_repeat_enabled() { return joy_repeat_enabled; }
	bool is_hat_repeating() { return repeat_hat; }
#endif

 private:
	void ParseText(char *text, int len);
	void ParseLine(char *line);
	void FillParseMaps();
#ifdef HAVE_JOYSTICK_SUPPORT
	joy_axes_pairs get_axes_pair(int axis);
	uint16 get_x_axis_deadzone(joy_axes_pairs axes_pair);
	uint16 get_y_axis_deadzone(joy_axes_pairs axes_pair);
	SDLKey get_key_from_joy_button(uint8 button);
	SDLKey get_key_from_joy_hat(SDL_JoyHatEvent);
#endif
};

#ifdef HAVE_JOYSTICK_SUPPORT

static const SDLKey FIRST_JOY = (SDLKey)400;
const SDLKey JOY_UP            = FIRST_JOY;               // PS d-pad when analog is disabled. left stick when enabled
const SDLKey JOY_DOWN          = (SDLKey)(FIRST_JOY + 1);
const SDLKey JOY_LEFT          = (SDLKey)(FIRST_JOY + 2);
const SDLKey JOY_RIGHT         = (SDLKey)(FIRST_JOY + 3);
const SDLKey JOY_RIGHTUP       = (SDLKey)(FIRST_JOY + 4);
const SDLKey JOY_RIGHTDOWN     = (SDLKey)(FIRST_JOY + 5);
const SDLKey JOY_LEFTUP        = (SDLKey)(FIRST_JOY + 6);
const SDLKey JOY_LEFTDOWN      = (SDLKey)(FIRST_JOY + 7);
const SDLKey JOY_UP2           = (SDLKey)(FIRST_JOY + 8); // PS right stick when analog is enabled
const SDLKey JOY_DOWN2         = (SDLKey)(FIRST_JOY + 9);
const SDLKey JOY_LEFT2         = (SDLKey)(FIRST_JOY + 10);
const SDLKey JOY_RIGHT2        = (SDLKey)(FIRST_JOY + 11);
const SDLKey JOY_RIGHTUP2      = (SDLKey)(FIRST_JOY + 12);
const SDLKey JOY_RIGHTDOWN2    = (SDLKey)(FIRST_JOY + 13);
const SDLKey JOY_LEFTUP2       = (SDLKey)(FIRST_JOY + 14);
const SDLKey JOY_LEFTDOWN2     = (SDLKey)(FIRST_JOY + 15);
const SDLKey JOY_UP3           = (SDLKey)(FIRST_JOY + 16);
const SDLKey JOY_DOWN3         = (SDLKey)(FIRST_JOY + 17);
const SDLKey JOY_LEFT3         = (SDLKey)(FIRST_JOY + 18);
const SDLKey JOY_RIGHT3        = (SDLKey)(FIRST_JOY + 19);
const SDLKey JOY_RIGHTUP3      = (SDLKey)(FIRST_JOY + 20);
const SDLKey JOY_RIGHTDOWN3    = (SDLKey)(FIRST_JOY + 21);
const SDLKey JOY_LEFTUP3       = (SDLKey)(FIRST_JOY + 22);
const SDLKey JOY_LEFTDOWN3     = (SDLKey)(FIRST_JOY + 23);
const SDLKey JOY_UP4           = (SDLKey)(FIRST_JOY + 24);
const SDLKey JOY_DOWN4         = (SDLKey)(FIRST_JOY + 25);
const SDLKey JOY_LEFT4         = (SDLKey)(FIRST_JOY + 26);
const SDLKey JOY_RIGHT4        = (SDLKey)(FIRST_JOY + 27);
const SDLKey JOY_RIGHTUP4      = (SDLKey)(FIRST_JOY + 28);
const SDLKey JOY_RIGHTDOWN4    = (SDLKey)(FIRST_JOY + 29);
const SDLKey JOY_LEFTUP4       = (SDLKey)(FIRST_JOY + 30);
const SDLKey JOY_LEFTDOWN4     = (SDLKey)(FIRST_JOY + 31);
const SDLKey JOY_HAT_UP        = (SDLKey)(FIRST_JOY + 32); // PS d-pad when analog is enabled
const SDLKey JOY_HAT_DOWN      = (SDLKey)(FIRST_JOY + 33);
const SDLKey JOY_HAT_LEFT      = (SDLKey)(FIRST_JOY + 34);
const SDLKey JOY_HAT_RIGHT     = (SDLKey)(FIRST_JOY + 35);
const SDLKey JOY_HAT_RIGHTUP   = (SDLKey)(FIRST_JOY + 36);
const SDLKey JOY_HAT_RIGHTDOWN = (SDLKey)(FIRST_JOY + 37);
const SDLKey JOY_HAT_LEFTUP    = (SDLKey)(FIRST_JOY + 38);
const SDLKey JOY_HAT_LEFTDOWN  = (SDLKey)(FIRST_JOY + 39);
const SDLKey JOY0              = (SDLKey)(FIRST_JOY + 40); // PS triangle
const SDLKey JOY1              = (SDLKey)(FIRST_JOY + 41); // PS circle
const SDLKey JOY2              = (SDLKey)(FIRST_JOY + 42); // PS x
const SDLKey JOY3              = (SDLKey)(FIRST_JOY + 43); // PS square
const SDLKey JOY4              = (SDLKey)(FIRST_JOY + 44); // PS L2
const SDLKey JOY5              = (SDLKey)(FIRST_JOY + 45); // PS R2
const SDLKey JOY6              = (SDLKey)(FIRST_JOY + 46); // PS L1
const SDLKey JOY7              = (SDLKey)(FIRST_JOY + 47); // PS R1
const SDLKey JOY8              = (SDLKey)(FIRST_JOY + 48); // PS select
const SDLKey JOY9              = (SDLKey)(FIRST_JOY + 49); // PS start
const SDLKey JOY10             = (SDLKey)(FIRST_JOY + 50); // PS L3 (analog must be enabled)
const SDLKey JOY11             = (SDLKey)(FIRST_JOY + 51); // PS R3 (analog must be enabled)
const SDLKey JOY12             = (SDLKey)(FIRST_JOY + 52);
const SDLKey JOY13             = (SDLKey)(FIRST_JOY + 53);
const SDLKey JOY14             = (SDLKey)(FIRST_JOY + 54);
const SDLKey JOY15             = (SDLKey)(FIRST_JOY + 55);
const SDLKey JOY16             = (SDLKey)(FIRST_JOY + 56);
const SDLKey JOY17             = (SDLKey)(FIRST_JOY + 57);
const SDLKey JOY18             = (SDLKey)(FIRST_JOY + 58);
const SDLKey JOY19             = (SDLKey)(FIRST_JOY + 59);
#endif /* HAVE_JOYSTICK_SUPPORT */

#endif /* KEYS_H */
