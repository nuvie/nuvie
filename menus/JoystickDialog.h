#ifndef __JoystickDialog_h__
#define __JoystickDialog_h__
#ifdef HAVE_JOYSTICK_SUPPORT
/*
 *  JoystickDialog.h
 *  Nuvie
 *  Copyright (C) 2014 The Nuvie Team
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

class JoystickDialog : public GUI_Dialog {
	protected:
		uint8 last_index;
		sint8 b_index_num;
		GUI_CallBack *callback_object;
		GUI_Button *save_button, *cancel_button;
		GUI_TextToggleButton *enable_button, *hat_repeating_b, *axes_index[8];
		GUI_Button *button_index[12]; // add to here when you add a button. Keep buttons in order by height
		uint8 get_axis_index(uint8 axis);

	public:
		JoystickDialog(GUI_CallBack *callback);
		~JoystickDialog();
		bool init();

		GUI_status close_dialog();
		GUI_status KeyDown(SDL_Keysym key);
		GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
};
#endif /* HAVE_JOYSTICK_SUPPORT */
#endif /* __JoystickDialog_h__ */
