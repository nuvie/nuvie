#ifndef __GameMenuDialog_h__
#define __GameMenuDialog_h__
/*
 *  GameMenuDialog.h
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

#define GAMEMENUDIALOG_CB_DELETE 3

class GUI;
class GUI_CallBack;
class GUI_Button;

class GameMenuDialog : public GUI_Dialog {
	protected:
		GUI_CallBack *callback_object;
		GUI_Button *saveLoad_button, *video_button, *audio_button, *gameplay_button, *cheats_button, *quit_button;

	public:
		GameMenuDialog(GUI_CallBack *callback);
		~GameMenuDialog();
		bool init();

		GUI_status close_dialog();
		GUI_status KeyDown(SDL_keysym key);
		GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
};

#endif /* __GameMenuDialog_h__ */
