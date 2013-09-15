/*
 *  GameplayDialog.cpp
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

#include "SDL.h"
#include "nuvieDefs.h"

#include "GUI.h"
#include "GUI_types.h"
#include "GUI_button.h"
#include "GUI_text.h"
#include "GUI_CallBack.h"
#include "GUI_area.h"

#include "GUI_Dialog.h"
#include "GameplayDialog.h"

#define GD_WIDTH 212
#define GD_HEIGHT 180

GameplayDialog::GameplayDialog(GUI_CallBack *callback)
          : GUI_Dialog(Game::get_game()->get_game_x_offset() + (Game::get_game()->get_game_width() - GD_WIDTH)/2,
                       Game::get_game()->get_game_y_offset() + (Game::get_game()->get_game_height() - GD_HEIGHT)/2,
                       GD_WIDTH, GD_HEIGHT, 244, 216, 131, GUI_DIALOG_UNMOVABLE) {
	callback_object = callback;
	init();
	grab_focus();
}

bool GameplayDialog::init() {
	int height = 12;
	GUI *gui = GUI::get_gui();

	cancel_button = new GUI_Button(this, 50, 158, 54, height, "Cancel", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(cancel_button);
	save_button = new GUI_Button(this, 121, 158, 40, height, "Save", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(save_button);
 
 return true;
}

GameplayDialog::~GameplayDialog() {
}

GUI_status GameplayDialog::close_dialog() {
	Delete(); // mark dialog as deleted. it will be freed by the GUI object
	callback_object->callback(0, this, this);
	return GUI_YUM;
}

GUI_status GameplayDialog::KeyDown(SDL_keysym key) {
	if(key.sym == SDLK_ESCAPE)
		return close_dialog();
	return GUI_YUM;
}

GUI_status GameplayDialog::callback(uint16 msg, GUI_CallBack *caller, void *data) {
	if(caller == (GUI_CallBack *)cancel_button) {
		return close_dialog();
	} else if(caller == (GUI_CallBack *)save_button) {
		close_dialog();
		return GUI_YUM;
	}

 return GUI_PASS;
}
