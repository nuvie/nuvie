#ifndef __GameplayDialog_h__
#define __GameplayDialog_h__
/*
 *  GameplayDialog.h
 *  Nuvie
 *  Copyright (C) 2013 The Nuvie Team
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

#include "GUI_Dialog.h"

class GUI;
class GUI_CallBack;
class GUI_Button;
class GUI_TextToggleButton;

class GameplayDialog : public GUI_Dialog {
	protected:
		uint8 last_index;
		sint8 b_index_num;
		uint8 old_converse_gump_type;
		GUI_CallBack *callback_object;
		GUI_Button *save_button, *cancel_button;
		GUI_TextToggleButton *formation_button, *stealing_button, *text_gump_button,
		                     *converse_gump_button, *converse_solid_bg_button, 
		                     *startup_game_button, *skip_intro_button, *show_console_button,
		                     *cursor_button;
		GUI_Button *button_index[11]; // add to here when you add a button. Keep buttons in order by height

	public:
		GameplayDialog(GUI_CallBack *callback);
		~GameplayDialog();
		bool init();

		GUI_status close_dialog();
		GUI_status KeyDown(SDL_Keysym key);
		GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
};

#endif /* __GameplayDialog_h__ */
