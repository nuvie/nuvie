/*
 *  CheatsDialog.cpp
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
#include "GUI_TextToggleButton.h"
#include "GUI_CallBack.h"
#include "GUI_area.h"

#include "GUI_Dialog.h"
#include "CheatsDialog.h"
#include "U6misc.h"
#include "Converse.h"
#include "ObjManager.h"
#include "MapWindow.h"
#include "Configuration.h"

#define CD_WIDTH 212
#define CD_HEIGHT 101

CheatsDialog::CheatsDialog(GUI_CallBack *callback)
          : GUI_Dialog(Game::get_game()->get_game_x_offset() + (Game::get_game()->get_game_width() - CD_WIDTH)/2,
                       Game::get_game()->get_game_y_offset() + (Game::get_game()->get_game_height() - CD_HEIGHT)/2,
                       CD_WIDTH, CD_HEIGHT, 244, 216, 131, GUI_DIALOG_UNMOVABLE) {
	callback_object = callback;
	init();
	grab_focus();
}

bool CheatsDialog::init() {
	int textY[] = { 11, 24, 37, 50, 63 };
	int buttonY[] = { 9, 22, 35, 48, 61, 80 };
	int colX[] = { 9, 163 };
	int height = 12;
	GUI_Widget *widget;
	GUI *gui = GUI::get_gui();

	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[0], 0, 0, 0, "Cheats:", gui->get_font());
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[1], 0, 0, 0, "Show eggs:", gui->get_font());
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[2], 0, 0, 0, "Enable hackmove:", gui->get_font());
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[3], 0, 0, 0, "Anyone will join:", gui->get_font());
	AddWidget(widget); 
	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[4], 0, 0, 0, "Minimum brightness:", gui->get_font());
	AddWidget(widget);

	bool party_all_the_time;
	Game *game = Game::get_game();
	Configuration *config = game->get_config();
	config->value("config/cheats/party_all_the_time", party_all_the_time);
	const char* const enabled_text[] = { "Disabled", "Enabled" };
	const char* const yesno_text[] = { "no", "yes" };
	int brightness_selection;
	int num_of_brightness = 6;
	uint8 min_brightness = game->get_map_window()->get_min_brightness();

	if(min_brightness == 0) {
		brightness_selection = 0;
	} else if(min_brightness == 40) {
		brightness_selection = 1;
	} else if(min_brightness == 60) {
		brightness_selection = 2;
	} else if(min_brightness == 80) {
		brightness_selection = 3;
	} else if(min_brightness == 128) {
		brightness_selection = 4;
	} else if(min_brightness == 255) {
		brightness_selection = 5;
	} else {
		num_of_brightness = 7;
		brightness_selection = 6;
	}

	char buff[4];
	sprintf(buff, "%d", min_brightness);
	const char* const brightness_text[] = { "0", "40", "60", "80", "128", "255", buff };

	cheat_button = new GUI_TextToggleButton(this, colX[1] - 30, buttonY[0], 70, height, enabled_text, 2, game->are_cheats_enabled(), gui->get_font(), BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(cheat_button); 
	egg_button = new GUI_TextToggleButton(this, colX[1], buttonY[1], 40, height, yesno_text, 2, game->get_obj_manager()->is_showing_eggs(), gui->get_font(), BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(egg_button); 
	hackmove_button = new GUI_TextToggleButton(this, colX[1], buttonY[2], 40, height, yesno_text, 2, game->using_hackmove(), gui->get_font(), BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(hackmove_button);
	party_button = new GUI_TextToggleButton(this, colX[1], buttonY[3], 40, height, yesno_text, 2, party_all_the_time, gui->get_font(), BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(party_button);
	brightness_button = new GUI_TextToggleButton(this, colX[1], buttonY[4], 40, height, brightness_text, num_of_brightness, brightness_selection,  gui->get_font(), BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(brightness_button);

	cancel_button = new GUI_Button(this, 50, buttonY[5], 54, height, "Cancel", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(cancel_button);
	save_button = new GUI_Button(this, 121, buttonY[5], 40, height, "Save", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(save_button);

	return true;
}

CheatsDialog::~CheatsDialog() {
}

GUI_status CheatsDialog::close_dialog() {
	Delete(); // mark dialog as deleted. it will be freed by the GUI object
	callback_object->callback(0, this, this); // I don't think this does anything atm
	return GUI_YUM;
}

GUI_status CheatsDialog::KeyDown(SDL_keysym key) {
	if(key.sym == SDLK_ESCAPE)
		return close_dialog();
	return GUI_PASS;
}

GUI_status CheatsDialog::callback(uint16 msg, GUI_CallBack *caller, void *data) {
	if(caller == (GUI_CallBack *)cancel_button) {
		return close_dialog();
	} else if(caller == (GUI_CallBack *)save_button) {
		Game *game = Game::get_game();
		Configuration *config = game->get_config();

		std::string key = config_get_game_key(config);
		key.append("/show_eggs");
		config->set(key, egg_button->GetSelection() ? "yes" : "no");
		game->get_obj_manager()->show_egg_objs(egg_button->GetSelection());

		game->set_cheats_enabled(cheat_button->GetSelection());
		config->set("config/cheats/enabled", cheat_button->GetSelection() ? "yes" : "no");
		game->set_hackmove(hackmove_button->GetSelection());
		config->set("config/cheats/enable_hackmove", hackmove_button->GetSelection() ? "yes" : "no");
		game->get_converse()->set_party_all_the_time(party_button->GetSelection());
		config->set("config/cheats/party_all_the_time", party_button->GetSelection() ? "yes" : "no");

		int brightness = brightness_button->GetSelection();
		if(brightness < 6) {
			int min_brightness;
			if(brightness == 0)
				min_brightness = 0;
			else if(brightness == 1)
				min_brightness = 40;
			else if(brightness == 2)
				min_brightness = 60;
			else if(brightness == 3)
				min_brightness = 80;
			else if(brightness == 4)
				min_brightness = 128;
			else //if(brightness == 5)
				min_brightness = 255;
			config->set("config/cheats/min_brightness", min_brightness);
			game->get_map_window()->set_min_brightness(min_brightness);
			game->get_map_window()->updateAmbience();
		}

		config->write();
		return close_dialog();
	}

	return GUI_PASS;
}
