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

#include <iostream>

#include "SDL_keyboard.h"
#include "Keys.h"
#include "KeyActions.h"
#include "nuvieDefs.h"
#include "Game.h"
#include "XMLTree.h"
#include "Player.h"
#include "Event.h"
#include "Utils.h"
#include "MsgScroll.h"
#include "Configuration.h"
#include "U6misc.h"
#include "Console.h"
#include "Effect.h"

#ifndef UNDER_EMBEDDED_CE
using std::atoi;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::isspace;
using std::strchr;
using std::string;
using std::strlen;
#endif

static	class Chardata	// ctype-like character lists
	{
	public:
	string	whitespace;
	Chardata()
		{
		for(size_t i=0;i<256;i++)
			if(isspace(i))
				whitespace+=static_cast<char>(i);
		}
	} chardata;

typedef void(*ActionFunc)(int const*);

const struct Action {
	const char *s;
	ActionFunc func; // called on keydown
	const char* desc;
	enum {
		dont_show = 0,
		normal_keys,
		cheat_keys
	} key_type;
	bool allow_in_vehicle;
	ActionKeyType action_key_type;
} NuvieActions[] = {
	{ "WALK_WEST", ActionWalkWest, "Walk west", Action::normal_keys, true, WEST_KEY  },
	{ "WALK_EAST", ActionWalkEast, "Walk east", Action::normal_keys, true, EAST_KEY },
	{ "WALK_NORTH", ActionWalkNorth, "Walk north", Action::normal_keys, true, NORTH_KEY },
	{ "WALK_SOUTH", ActionWalkSouth, "Walk south", Action::normal_keys, true, SOUTH_KEY },
	{ "WALK_NORTH_EAST", ActionWalkNorthEast, "Walk north-east", Action::normal_keys, true, NORTH_EAST_KEY },
	{ "WALK_SOUTH_EAST", ActionWalkSouthEast, "Walk south-east", Action::normal_keys, true, SOUTH_EAST_KEY },
	{ "WALK_NORTH_WEST", ActionWalkNorthWest, "Walk north-west", Action::normal_keys, true, NORTH_WEST_KEY },
	{ "WALK_SOUTH_WEST", ActionWalkSouthWest, "Walk south-west", Action::normal_keys, true, SOUTH_WEST_KEY },
	{ "CAST", ActionCast, "Cast", Action::normal_keys, true, OTHER_KEY }, // allow_in_vehicle is true, so the right message or cancel is done for WOU games
	{ "LOOK", ActionLook, "Look", Action::normal_keys, true, OTHER_KEY },
	{ "TALK", ActionTalk, "Talk", Action::normal_keys, true, OTHER_KEY },
	{ "USE", ActionUse, "Use", Action::normal_keys, true, OTHER_KEY },
	{ "GET", ActionGet, "Get", Action::normal_keys, false, OTHER_KEY },
	{ "MOVE", ActionMove, "Move", Action::normal_keys, false, OTHER_KEY },
	{ "DROP", ActionDrop, "Drop", Action::normal_keys, false, OTHER_KEY },
	{ "TOGGLE_COMBAT", ActionToggleCombat, "Toggle combat", Action::normal_keys, false, OTHER_KEY },
	{ "ATTACK", ActionAttack, "Attack", Action::normal_keys, true, OTHER_KEY },
	{ "REST", ActionRest, "Rest", Action::normal_keys, true, OTHER_KEY },
	{ "MULTI_USE", ActionMultiUse, "Multi-use", Action::normal_keys, true, OTHER_KEY },
	{ "SELECT_COMMAND_BAR", ActionSelectCommandBar, "Select Command Bar", Action::normal_keys, true, OTHER_KEY },
	{ "NEW_COMMAND_BAR", ActionSelectNewCommandBar, "Select New Command Bar", Action::normal_keys, true, NEW_COMMAND_BAR_KEY },
	{ "DOLL_GUMP", ActionDollGump, "Doll gump", Action::normal_keys, true, OTHER_KEY },
	{ "SHOW_STATS", ActionShowStats, "Shows the party member's stats", Action::normal_keys, true, OTHER_KEY },
	{ "INVENTORY", ActionInventory, "inventory", Action::normal_keys, true, OTHER_KEY },
	{ "PREVIOUS_PARTY_MEMBER", ActionPreviousPartyMember, "Previous party member", Action::normal_keys, true, PREVIOUS_PARTY_MEMBER_KEY },
	{ "NEXT_PARTY_MEMBER", ActionNextPartyMember, "Next party member", Action::normal_keys, true, NEXT_PARTY_MEMBER_KEY },
	{ "HOME_KEY", ActionHome, "Home key", Action::normal_keys, true, HOME_KEY },
	{ "END_KEY", ActionEnd, "End key", Action::normal_keys, true, END_KEY },
	{ "TOGGLE_VIEW", ActionToggleView, "Toggle between inventory and actor view", Action::normal_keys, true, OTHER_KEY },
	{ "PARTY_VIEW", ActionPartyView, "Party view", Action::normal_keys, true, OTHER_KEY },
	{ "SOLO_MODE", ActionSoloMode, "Solo mode", Action::normal_keys, true, OTHER_KEY },
	{ "PARTY_MODE", ActionPartyMode, "Party mode", Action::normal_keys, true, OTHER_KEY },
	{ "SAVE_MENU", ActionSaveDialog, "Save menu", Action::normal_keys, true, OTHER_KEY },
	{ "LOAD_LATEST_SAVE", ActionLoadLatestSave, "Load latest save", Action::normal_keys, true, OTHER_KEY },
	{ "QUICK_SAVE", ActionQuickSave, "Quick save", Action::normal_keys, true, OTHER_KEY },
	{ "QUICK_LOAD", ActionQuickLoad, "Quick load", Action::normal_keys, true, OTHER_KEY },
	{ "QUIT", ActionQuitDialog, "Quit", Action::normal_keys, true, QUIT_KEY },
	{ "QUIT_NO_DIALOG", ActionQuitNODialog, "Quit without confirmation", Action::normal_keys, true, QUIT_KEY },
	{ "GAME_MENU_DIALOG", ActionGameMenuDialog, "Show game menu; Cancel action if in middle of action", Action::normal_keys, true, CANCEL_ACTION_KEY },
	{ "TOGGLE_FULLSCREEN", ActionToggleFullscreen, "Toggle Fullscreen", Action::normal_keys, true, TOGGLE_FULLSCREEN_KEY },
	{ "TOGGLE_CURSOR", ActionToggleCursor, "Toggle Cursor", Action::normal_keys, true, TOGGLE_CURSOR_KEY },
	{ "TOGGLE_COMBAT_STRATEGY", ActionToggleCombatStrategy, "Toggle combat strategy", Action::normal_keys, true, OTHER_KEY },
	{ "TOGGLE_FPS_DISPLAY", ActionToggleFps, "Toggle frames per second display", Action::normal_keys, true, TOGGLE_FPS_KEY },
	{ "TOGGLE_AUDIO", ActionToggleAudio, "Toggle audio", Action::normal_keys, true, TOGGLE_AUDIO_KEY },
	{ "TOGGLE_MUSIC", ActionToggleMusic, "Toggle music", Action::normal_keys, true, TOGGLE_MUSIC_KEY },
	{ "TOGGLE_SFX", ActionToggleSFX, "Toggle sfx", Action::normal_keys, true, TOGGLE_SFX_KEY },
	{ "TOGGLE_ORIGINAL_PLUS_COMMAND_BAR", ActionToggleOriginalPlusCommandBar, "Show/hide original plus command bar", Action::normal_keys, true,  },
	{ "DO_ACTION", ActionDoAction, "Do action", Action::normal_keys, true, DO_ACTION_KEY },
	{ "CANCEL_ACTION", ActionCancelAction, "Cancel action", Action::normal_keys, true, CANCEL_ACTION_KEY },
	{ "MSG_SCROLL_UP", ActionMsgScrollUP, "Msg scroll up", Action::normal_keys, true, MSGSCROLL_UP_KEY },
	{ "MSG_SCROLL_DOWN", ActionMsgScrollDown, "Msg scroll down", Action::normal_keys, true, MSGSCROLL_DOWN_KEY },
	{ "SHOW_KEYS", ActionShowKeys, "Show keys", Action::normal_keys, true, OTHER_KEY },
	{ "DECREASE_DEBUG", ActionDecreaseDebug, "Decrease debug", Action::normal_keys, true, DECREASE_DEBUG_KEY },
	{ "INCREASE_DEBUG", ActionIncreaseDebug, "Increase debug", Action::normal_keys, true, INCREASE_DEBUG_KEY },
	{ "CLOSE_GUMPS", ActionCloseGumps, "Close gumps", Action::normal_keys, true, OTHER_KEY },
	{ "USE_ITEM", ActionUseItem, "Use item", Action::normal_keys, true, OTHER_KEY },
	{ "SHOW_EGGS", ActionShowEggs, "Show eggs", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_HACKMOVE", ActionToggleHackmove, "Toggle hack move", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_EGG_SPAWN", ActionToggleEggSpawn, "Toggle egg spawn", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_UNLIMITED_CASTING", ActionToggleUnlimitedCasting, "Toggle unlimited casting", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_NO_DARKNESS", ActionToggleNoDarkness, "Toggle no darkness", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_PICKPOCKET_MODE", ActionTogglePickpocket, "Toggle pickpocket mode", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_GOD_MODE",  ActionToggleGodMode, "Toggle god mode", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_ETHEREAL",  ActionToggleEthereal, "Toggle ethereal mode", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_X_RAY",  ActionToggleX_Ray, "Toggle X-ray mode", Action::cheat_keys, true, OTHER_KEY },
	{ "HEAL_PARTY", ActionHealParty, "Heal party", Action::cheat_keys, true, OTHER_KEY },
	{ "TELEPORT_TO_CURSOR", ActionTeleportToCursor, "Teleport to cursor", Action::cheat_keys, true, OTHER_KEY },
	{ "TOGGLE_CHEATS", ActionToggleCheats, "Toggle cheats", Action::normal_keys, true, OTHER_KEY },
	{ "TEST", ActionTest, "Test", Action::dont_show, true, OTHER_KEY },
	{ "", 0, "", Action::dont_show, false, OTHER_KEY } //terminator
};

const struct {
	const char *s;
	SDLKey k;
} SDLKeyStringTable[] = {
	{"LCTRL",     SDLK_LCTRL},
	{"RCTRL",     SDLK_RCTRL},
	{"LALT",      SDLK_LALT},
	{"RALT",      SDLK_RALT},
	{"LSHIFT",    SDLK_LSHIFT},
	{"RSHIFT",    SDLK_RSHIFT},
	{"BACKSPACE", SDLK_BACKSPACE},
	{"TAB",       SDLK_TAB}, 
	{"ENTER",     SDLK_RETURN}, 
	{"PAUSE",     SDLK_PAUSE}, 
	{"ESC",       SDLK_ESCAPE}, 
	{"SPACE",     SDLK_SPACE}, 
	{"DEL",       SDLK_DELETE}, 
	{"KP0",       SDLK_KP0}, 
	{"KP1",       SDLK_KP1}, 
	{"KP2",       SDLK_KP2}, 
	{"KP3",       SDLK_KP3}, 
	{"KP4",       SDLK_KP4}, 
	{"KP5",       SDLK_KP5}, 
	{"KP6",       SDLK_KP6}, 
	{"KP7",       SDLK_KP7}, 
	{"KP8",       SDLK_KP8}, 
	{"KP9",       SDLK_KP9}, 
	{"KP.",       SDLK_KP_PERIOD}, 
	{"KP/",       SDLK_KP_DIVIDE}, 
	{"KP*",       SDLK_KP_MULTIPLY}, 
	{"KP-",       SDLK_KP_MINUS}, 
	{"KP+",       SDLK_KP_PLUS}, 
	{"KP_ENTER",  SDLK_KP_ENTER}, 
	{"UP",        SDLK_UP}, 
	{"DOWN",      SDLK_DOWN}, 
	{"RIGHT",     SDLK_RIGHT}, 
	{"LEFT",      SDLK_LEFT}, 
	{"INSERT",    SDLK_INSERT}, 
	{"HOME",      SDLK_HOME}, 
	{"END",       SDLK_END}, 
	{"PAGEUP",    SDLK_PAGEUP}, 
	{"PAGEDOWN",  SDLK_PAGEDOWN}, 
	{"F1",        SDLK_F1}, 
	{"F2",        SDLK_F2}, 
	{"F3",        SDLK_F3}, 
	{"F4",        SDLK_F4}, 
	{"F5",        SDLK_F5}, 
	{"F6",        SDLK_F6}, 
	{"F7",        SDLK_F7}, 
	{"F8",        SDLK_F8}, 
	{"F9",        SDLK_F9}, 
	{"F10",       SDLK_F10}, 
	{"F11",       SDLK_F11}, 
	{"F12",       SDLK_F12}, 
	{"F13",       SDLK_F13}, 
	{"F14",       SDLK_F14}, 
	{"F15",       SDLK_F15},
	{"", SDLK_UNKNOWN} // terminator
};


typedef std::map<std::string, SDLKey> ParseKeyMap;
typedef std::map<std::string, const Action*> ParseActionMap;

static ParseKeyMap keys;
static ParseActionMap actions;


KeyBinder::KeyBinder()
{
	FillParseMaps();
}

KeyBinder::~KeyBinder()
{
}

void KeyBinder::AddKeyBinding( SDLKey key, int mod, const Action* action, 
				 int nparams, int* params)
{
	SDL_keysym k;
	ActionType a;

	#if SDL_VERSION_ATLEAST(1, 3, 0)
	k.scancode = (SDL_Scancode)0;	
	#else
	k.scancode = 0;
	#endif
	k.sym      = key;
	k.mod      = (SDLMod) mod;
	k.unicode  = 0;
	a.action    = action;
	int i;	// For MSVC
	for (i = 0; i < c_maxparams && i < nparams; i++)
		a.params[i] = params[i];
	for (i = nparams; i < c_maxparams; i++)
		a.params[i] = -1;
	
	bindings[k] = a;
}

ActionType KeyBinder::get_ActionType(SDL_keysym key)
{
	KeyMap::iterator sdlkey_index = get_sdlkey_index(key);
	return (*sdlkey_index).second;
}

ActionKeyType KeyBinder::GetActionKeyType(ActionType a)
{
	return a.action->action_key_type;
}

bool KeyBinder::DoAction(ActionType const& a) const
{
	if (!a.action->allow_in_vehicle && Game::get_game()->get_player()->is_in_vehicle())
	{
		Game::get_game()->get_event()->display_not_aboard_vehicle();
		return true;
	}
	else if(a.action->key_type == Action::cheat_keys && !Game::get_game()->are_cheats_enabled())
	{
		new TextEffect("Cheats are disabled");
		return true;
	}
	a.action->func(a.params);

	return true;
}

KeyMap::iterator KeyBinder::get_sdlkey_index(SDL_keysym keysym)
{
	SDL_keysym key = keysym;
	key.mod = KMOD_NONE;
	if (keysym.mod & KMOD_SHIFT)
		key.mod = (SDLMod)(key.mod | KMOD_SHIFT);
	if (keysym.mod & KMOD_CTRL)
		key.mod = (SDLMod)(key.mod | KMOD_CTRL);
#if defined(MACOS) || defined(MACOSX)
	// map Meta to Alt on MacOS
	if (keysym.mod & KMOD_META)
		key.mod = (SDLMod)(key.mod | KMOD_ALT);
#else
	if (keysym.mod & KMOD_ALT)
		key.mod = (SDLMod)(key.mod | KMOD_ALT);
#endif

	return bindings.find(key);
}

bool KeyBinder::HandleEvent(const SDL_Event *ev)
{
	SDL_keysym key = ev->key.keysym;
	KeyMap::iterator sdlkey_index;

	if (ev->type != SDL_KEYDOWN)
		return false;

	sdlkey_index = get_sdlkey_index(key);
	if (sdlkey_index != bindings.end())
		return DoAction((*sdlkey_index).second);

	if (ev->key.keysym.sym != SDLK_LALT && ev->key.keysym.sym != SDLK_RALT
	    && ev->key.keysym.sym != SDLK_LCTRL && ev->key.keysym.sym != SDLK_RCTRL)
	{
		handle_wrong_key_pressed();
	}
	return false;
}

void KeyBinder::handle_wrong_key_pressed()
{
	if(Game::get_game()->get_event()->get_mode() != MOVE_MODE)
		Game::get_game()->get_event()->cancelAction();
	else
	{
		Game::get_game()->get_scroll()->display_string("what?\n\n");
		Game::get_game()->get_scroll()->display_prompt();
	}
}

bool KeyBinder::handle_always_available_keys(ActionType a)
{
	switch(a.action->action_key_type)
	{
		case TOGGLE_AUDIO_KEY: // FIXME - Shutting off the music in cutscenes will not have any music play when
		case TOGGLE_MUSIC_KEY: //         you toggle it back on. It has to wait for the engine to play another song
		case TOGGLE_SFX_KEY:
		case TOGGLE_FPS_KEY: // fps is not available in intro or when viewing ending with command line cheat
		case TOGGLE_FULLSCREEN_KEY:
		case DECREASE_DEBUG_KEY:
		case INCREASE_DEBUG_KEY:
		case QUIT_KEY: // FIXME - doesn't currently work in intro or when viewing ending with command line cheat
			a.action->func(a.params);
			return true;
		default: return false;
	}
}

void KeyBinder::ShowKeys() const // FIXME This doesn't look very good, the font is missing
{                                // some characters, and it is longer than msgscroll can hold
//	if(Game::get_game()->is_orig_style())
	{
		std::vector<string>::const_iterator iter;
		string keys;
		MsgScroll *scroll = Game::get_game()->get_scroll();
		scroll->set_autobreak(true);

		for (iter = keyhelp.begin(); iter != keyhelp.end(); ++iter)
		{
			keys = "\n";
			keys.append(iter->c_str());
			scroll->display_string(keys, 1);
		}
		scroll->message("\n\n\t");
	}
}

void KeyBinder::ParseText(char *text, int len)
{
	char *ptr, *end;
	const char LF = '\n';
	
	ptr = text;
	
	// last (useful) line must end with LF
	while ((ptr - text) < len && (end = strchr(ptr, LF)) != 0) {
		*end = '\0';
		ParseLine(ptr);
		ptr = end + 1;
	}  
}

static void skipspace(string &s) {
	size_t i=s.find_first_not_of(chardata.whitespace);
	if(i&&i!=string::npos)
		s.erase(0,i);
}


void KeyBinder::ParseLine(char *line)
{
	size_t i;
	SDL_keysym k;
	ActionType a;
	k.sym      = SDLK_UNKNOWN;
	k.mod      = KMOD_NONE;
	string s = line, u;
	string d, desc, keycode;
	bool show;
	
	skipspace(s);
	
	// comments and empty lines
	if (s.length() == 0 || s[0] == '#')
		return;
	
	u = s;
	u = to_uppercase(u);
	
	// get key
	while (s.length() && !isspace(s[0])) {
		// check modifiers
		if (u.substr(0,4) == "ALT-") {
			k.mod = (SDLMod)(k.mod | KMOD_ALT);
			s.erase(0,4); u.erase(0,4);
		} else if (u.substr(0,5) == "CTRL-") {
			k.mod = (SDLMod)(k.mod | KMOD_CTRL);
			s.erase(0,5); u.erase(0,5);
		} else if (u.substr(0,6) == "SHIFT-") {
			k.mod = (SDLMod)(k.mod | KMOD_SHIFT);
			s.erase(0,6); u.erase(0,6);
		} else {
			
			i=s.find_first_of(chardata.whitespace);

			keycode = s.substr(0, i); s.erase(0, i);
			string t = to_uppercase(keycode);
			
			if (t.length() == 0) {
				cerr << "Keybinder: parse error in line: " << s << endl;
				return;
			} else if (t.length() == 1) {
				// translate 1-letter keys straight to SDLKey
				char c = t[0];
				if (c >= 33 && c <= 122 && c != 37) {
					if (c >= 'A' && c <= 'Z')
						c += 32; // need lowercase
					k.sym = static_cast<SDLKey>(c);
				} else {
					cerr << "Keybinder: unsupported key: " << keycode << endl;
				}
			} else {
				// lookup in table
				ParseKeyMap::iterator key_index;
				key_index = keys.find(t);
				if (key_index != keys.end()) {
					k.sym = (*key_index).second;
				} else {
					cerr << "Keybinder: unsupported key: " << keycode << endl;
					return;
				}
			}
		}
	}
	
	if (k.sym == SDLK_UNKNOWN) {
		cerr << "Keybinder: parse error in line: " << s << endl;
		return;
	}
	
	// get function
	skipspace(s);
	
	i=s.find_first_of(chardata.whitespace);
	string t = s.substr(0, i); s.erase(0, i);
	t = to_uppercase(t);
	
	ParseActionMap::iterator action_index;
	action_index = actions.find(t);
	if (action_index != actions.end()) {
		a.action = (*action_index).second;
	} else {
		cerr << "Keybinder: unsupported action: " << t << endl;
		return;
	}
	
	// get params
	skipspace(s);
	
	int np = 0;
	while (s.length() && s[0] != '#' && np < c_maxparams) {
		i=s.find_first_of(chardata.whitespace);
		string t = s.substr(0, i);
		s.erase(0, i);
		skipspace(s);
		
		int p = atoi(t.c_str());
		a.params[np++] = p;
	}
	
	// read optional help comment
	if (s.length() >= 1 && s[0] == '#') {
		if (s.length() >= 2 && s[1] == '-') {
			show = false;
		} else {
			s.erase(0,1);
			skipspace(s);
			d = s;
			show = true;
		}
	} else {
		d = a.action->desc;
		show = a.action->key_type != Action::dont_show;
	}
	
	if (show) {
		desc = "";
		if (k.mod & KMOD_CTRL)
			desc += "Ctrl-";
#if defined(MACOS) || defined(MACOSX)
		if (k.mod & KMOD_ALT)
			desc += "Cmd-";
#else
		if (k.mod & KMOD_ALT)
			desc += "Alt-";
#endif
		if (k.mod & KMOD_SHIFT)
			desc += "Shift-";
		if(keycode == "`")
			desc += "grave";
		else
			desc += keycode;
		desc += " - " + d;
		
		// add to help list
		if (a.action->key_type == Action::normal_keys)
			keyhelp.push_back(desc);
		else if (a.action->key_type == Action::cheat_keys)
			cheathelp.push_back(desc);
	}
	
	// bind key
	AddKeyBinding(k.sym, k.mod, a.action, np, a.params);
}

void KeyBinder::LoadFromFileInternal(const char* filename)
{
	ifstream keyfile;

	openFile(keyfile, filename);
	char temp[1024]; // 1024 should be long enough
	while(!keyfile.eof()) {
		keyfile.getline(temp, 1024);
		if (keyfile.gcount() >= 1023) {
			fprintf(stderr, "Keybinder: parse error: line too long. Skipping rest of file.\n");
			return;
		}
		ParseLine(temp);
	}
	keyfile.close();
}

void KeyBinder::LoadFromFile(const char* filename)
{
	
	Flush();
	
	ConsoleAddInfo("Loading keybindings from file %s\n", filename);
	LoadFromFileInternal(filename);
}

void KeyBinder::LoadGameSpecificKeys()
{
	string key_path_str;
	string default_key_path;
	Configuration *config = Game::get_game()->get_config();
	config->value("config/datadir", default_key_path, "./data");
	nuvie_game_t game_type = get_game_type(config);

	if(game_type == NUVIE_GAME_U6)
		default_key_path += "/u6keys.txt";
	else if(game_type ==  NUVIE_GAME_MD)
		default_key_path += "/mdkeys.txt";
	else // SE
		default_key_path += "/sekeys.txt";

	config->value(config_get_game_key(config) + "/game_specific_keys", key_path_str, default_key_path.c_str());
	const char *key_path;
	if(key_path_str == "(default)")
		key_path = default_key_path.c_str();
	else
		key_path = key_path_str.c_str();
	if (fileExists(key_path)) {
		ConsoleAddInfo("Loading %s\n", key_path);
		LoadFromFileInternal(key_path);
	}
	else // These aren't critical so failing to load doesn't matter much
		ConsoleAddInfo("Couldn't find $s\n", key_path);
}

void KeyBinder::LoadFromPatch() // FIXME default should probably be system specific
{
	string PATCH_KEYS;
	Configuration *config = Game::get_game()->get_config();

	config->value(config_get_game_key(config) + "/patch_keys", PATCH_KEYS, "./patchkeys.txt");
	if (fileExists(PATCH_KEYS.c_str())) {
		ConsoleAddInfo("Loading patch keybindings\n");
		LoadFromFileInternal(PATCH_KEYS.c_str());
	}
}

// codes used in keybindings-files. (use uppercase here)
void KeyBinder::FillParseMaps()
{
	int i;	// For MSVC
	for (i = 0; strlen(SDLKeyStringTable[i].s) > 0; i++)
		keys[SDLKeyStringTable[i].s] = SDLKeyStringTable[i].k;
	
	for (i = 0; strlen(NuvieActions[i].s) > 0; i++)
		actions[NuvieActions[i].s] = &(NuvieActions[i]);
}
