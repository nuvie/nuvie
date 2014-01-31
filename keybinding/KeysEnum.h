#ifndef __KeysEnum_h__
#define __KeysEnum_h__

// FIXME - I needed to reduce includes. Maybe use C++11 enum class in the future
typedef enum {
	WEST_KEY = 0,
	EAST_KEY,
	NORTH_KEY,
	SOUTH_KEY,
	NORTH_EAST_KEY,
	SOUTH_EAST_KEY,
	NORTH_WEST_KEY,
	SOUTH_WEST_KEY,
	TOGGLE_CURSOR_KEY,
	DO_ACTION_KEY, // don't change the order before this without checking MsgScroll.cpp, MapEditorView.cpp and MapWindow.cpp
	CANCEL_ACTION_KEY,
	NEW_COMMAND_BAR_KEY,
	NEXT_PARTY_MEMBER_KEY,
	PREVIOUS_PARTY_MEMBER_KEY,
	MSGSCROLL_UP_KEY,
	MSGSCROLL_DOWN_KEY,
	TOGGLE_AUDIO_KEY,
	TOGGLE_MUSIC_KEY,
	TOGGLE_SFX_KEY,
	TOGGLE_FPS_KEY,
	TOGGLE_FULLSCREEN_KEY,
	DECREASE_DEBUG_KEY,
	INCREASE_DEBUG_KEY,
	QUIT_KEY,
	HOME_KEY,
	END_KEY,
	OTHER_KEY
} ActionKeyType;

#endif /* __KeysEnum_h__ */
