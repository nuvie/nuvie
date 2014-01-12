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
#include "GUI_TextToggleButton.h"
#include "GUI_CallBack.h"
#include "GUI_area.h"

#include "GUI_Dialog.h"
#include "GameplayDialog.h"
#include "Party.h"
#include "Script.h"
#include "U6misc.h"
#include "Converse.h"
#include "ConverseGump.h"
#include "Configuration.h"

#define GD_WIDTH 274
#define GD_HEIGHT 153

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
	int yesno_width = 32;
	int colX[] = { 9, 50, 233 };
	int buttonY[] = { 9, 22, 35, 48, 61, 74, 87, 100, 113, 126 };
	int textY[] = { 11, 24, 37, 50, 63 , 76, 89, 102, 115 };

	GUI_Widget *widget;
	GUI *gui = GUI::get_gui();
	GUI_Font *font = gui->get_font();
	Game *game = Game::get_game();
	Configuration *config = Game::get_game()->get_config();
	const char* const yesno_text[] = { "no", "yes" };
	const char* const formation_text[] = { "standard", "column", "row", "delta" };

	bool is_u6 = (game->get_game_type() == NUVIE_GAME_U6);
	bool show_stealing, skip_intro, show_console, use_original_cursor, solid_bg;
	std::string key = config_get_game_key(config);
	config->value(key + "/skip_intro", skip_intro, false);
	config->value("config/general/show_console", show_console, false);
	config->value("config/general/enable_cursors", use_original_cursor, false);
// party formation
	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[0], 0, 0, 0, "Party formation:", font);
	AddWidget(widget);
	formation_button = new GUI_TextToggleButton(this, 197, buttonY[0], 68, height, formation_text, 4, game->get_party()->get_formation(), font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(formation_button);
	if(is_u6) {
// show stealing
		widget = (GUI_Widget *) new GUI_Text(colX[0], textY[1], 0, 0, 0, "Look shows private property:", font);
		AddWidget(widget);
		config->value("config/ultima6/show_stealing", show_stealing, false);
		stealing_button = new GUI_TextToggleButton(this, colX[2], buttonY[1], yesno_width, height, yesno_text, 2, show_stealing, font, BUTTON_TEXTALIGN_CENTER, this, 0);
		AddWidget(stealing_button);
	} else {
		stealing_button = NULL;
	}
	if(!Game::get_game()->is_new_style()) {
// Use text gump
		widget = (GUI_Widget *) new GUI_Text(colX[0], textY[1 + is_u6], 0, 0, 0, "Use text gump:", font);
		AddWidget(widget);
		text_gump_button = new GUI_TextToggleButton(this, colX[2], buttonY[1 + is_u6], yesno_width, height, yesno_text, 2, game->is_using_text_gumps(), font, BUTTON_TEXTALIGN_CENTER, this, 0);
		AddWidget(text_gump_button);
		converse_solid_bg_button = NULL;
	} else {
// converse solid bg
		widget = (GUI_Widget *) new GUI_Text(colX[0], textY[1 + is_u6], 0, 0, 0, "Converse gump has solid bg:", font);
		AddWidget(widget);
		solid_bg = game->get_converse_gump()->get_solid_bg();
		converse_solid_bg_button = new GUI_TextToggleButton(this, colX[2], buttonY[1 + is_u6], yesno_width, height, yesno_text, 2, solid_bg, font, BUTTON_TEXTALIGN_CENTER, this, 0);
		AddWidget(converse_solid_bg_button);
		text_gump_button = converse_gump_button = NULL;
	}
	widget = (GUI_Widget *) new GUI_Text(colX[0], textY[4], 0, 0, 0, "The following require a restart:", font);
	AddWidget(widget);
// skip intro
	widget = (GUI_Widget *) new GUI_Text(colX[1], textY[5], 0, 0, 0, "Skip intro:", font);
	AddWidget(widget);
	skip_intro_button = new GUI_TextToggleButton(this, colX[2], buttonY[5], yesno_width, height, yesno_text, 2, skip_intro,  font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(skip_intro_button);
// show console
	widget = (GUI_Widget *) new GUI_Text(colX[1], textY[6], 0, 0, 0, "Show console:", font);
	AddWidget(widget);
	show_console_button = new GUI_TextToggleButton(this, colX[2], buttonY[6], yesno_width, height, yesno_text, 2, show_console, font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(show_console_button);
// original cursor
	widget = (GUI_Widget *) new GUI_Text(colX[1], textY[7], 0, 0, 0, "Use original cursors:", font);
	AddWidget(widget);
	cursor_button = new GUI_TextToggleButton(this, colX[2], buttonY[7], yesno_width, height, yesno_text, 2, use_original_cursor, font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(cursor_button);

	if(!Game::get_game()->is_new_style()) {
// use converse gump
		widget = (GUI_Widget *) new GUI_Text(colX[1], textY[8], 0, 0, 0, "Use converse gump:", font);
		AddWidget(widget);
		converse_gump_button = new GUI_TextToggleButton(this, colX[2], buttonY[8], yesno_width, height, yesno_text, 2, game->using_new_converse_gump(), font, BUTTON_TEXTALIGN_CENTER, this, 0);
		AddWidget(converse_gump_button);
	}
	cancel_button = new GUI_Button(this, 77, buttonY[9] + 6, 54, height, "Cancel", font, BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(cancel_button);
	save_button = new GUI_Button(this, 158, buttonY[9] + 6, 40, height, "Save", font, BUTTON_TEXTALIGN_CENTER, 0, this, 0);
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
		Game *game  = Game::get_game();
		Configuration *config = game->get_config();
		std::string key = config_get_game_key(config);

		game->get_party()->set_formation(formation_button->GetSelection());
		config->set("config/general/party_formation", formation_button->GetSelection() ? "yes" : "no");
		if(game->get_game_type() == NUVIE_GAME_U6) {
			game->get_script()->call_set_g_show_stealing(stealing_button->GetSelection());
			config->set("config/ultima6/show_stealing", stealing_button->GetSelection() ? "yes" : "no");
		}
		if(!Game::get_game()->is_new_style()) {
			game->set_using_text_gumps(text_gump_button->GetSelection());
			config->set("config/general/use_text_gumps", text_gump_button->GetSelection() ? "yes" : "no");
			config->set("config/general/converse_gump", converse_gump_button->GetSelection() ? "yes" : "no"); // need restart
		} else {
			game->get_converse_gump()->set_solid_bg(converse_solid_bg_button->GetSelection());
			config->set(key + "/converse_solid_bg", converse_solid_bg_button->GetSelection() ? "yes" : "no");
		}
		config->set(key + "/skip_intro", skip_intro_button->GetSelection() ? "yes" : "no"); // need restart
		config->set("config/general/show_console", show_console_button->GetSelection() ? "yes" : "no"); // need restart
		config->set("config/general/enable_cursors", cursor_button->GetSelection() ? "yes" : "no"); // need restart

		config->write();
		close_dialog();
		return GUI_YUM;
	}

 return GUI_PASS;
}
