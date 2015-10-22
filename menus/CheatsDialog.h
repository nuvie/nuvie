#ifndef __CheatsDialog_h__
#define __CheatsDialog_h__
/*
 *  CheatsDialog.h
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

class CheatsDialog : public GUI_Dialog {
	protected:
		uint8 last_index;
		sint8 b_index_num;
		GUI_CallBack *callback_object;
		GUI_Button *save_button, *cancel_button;
		GUI_TextToggleButton *brightness_button, *cheat_button, *egg_button, *hackmove_button, *party_button;
		GUI_Button *button_index[7]; // add to here when you add a button. Keep buttons in order by height

	public:
		CheatsDialog(GUI_CallBack *callback);
		~CheatsDialog();
		bool init();

		GUI_status close_dialog();
		GUI_status KeyDown(SDL_Keysym key);
		GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
};

#endif /* __CheatsDialog_h__ */
