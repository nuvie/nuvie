/*
 *  GameMenuDialog.cpp
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
#include "GUI_CallBack.h"
#include "GUI_area.h"

#include "GUI_Dialog.h"
#include "GameMenuDialog.h"
#include "VideoDialog.h"
#include "AudioDialog.h"
#include "GameplayDialog.h"
#include "CheatsDialog.h"
#include "event.h"

#define GMD_WIDTH 150
#define GMD_HEIGHT 96

GameMenuDialog::GameMenuDialog(GUI_CallBack *callback)
          : GUI_Dialog(Game::get_game()->get_game_x_offset() + (Game::get_game()->get_game_width() - GMD_WIDTH)/2,
                       Game::get_game()->get_game_y_offset() + (Game::get_game()->get_game_height() - GMD_HEIGHT)/2,
                       GMD_WIDTH, GMD_HEIGHT, 244, 216, 131, GUI_DIALOG_UNMOVABLE) {
	callback_object = callback;
	init();
	grab_focus();
}

bool GameMenuDialog::init() {
	int width = 132;
	int height = 12;
	int buttonX = 9;
	int buttonY[] = { 9, 22, 35, 48, 61, 74 };
	GUI *gui = GUI::get_gui();

	saveLoad_button = new GUI_Button(this, buttonX, buttonY[0], width, height, "Load/Save Game", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(saveLoad_button); 
	video_button = new GUI_Button(this, buttonX, buttonY[1], width, height, "Video Options", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(video_button);
	audio_button = new GUI_Button(this, buttonX, buttonY[2], width, height, "Audio Options", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(audio_button);
	gameplay_button = new GUI_Button(this, buttonX, buttonY[3], width, height, "Gameplay Options", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(gameplay_button);
	cheats_button = new GUI_Button(this, buttonX, buttonY[4], width, height, "Cheats", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(cheats_button);
	quit_button = new GUI_Button(this, buttonX, buttonY[5], width, height, "Quit", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(quit_button);
 
	return true;
}

GameMenuDialog::~GameMenuDialog() {
}

GUI_status GameMenuDialog::close_dialog() {
	Delete(); // mark dialog as deleted. it will be freed by the GUI object
	callback_object->callback(GAMEMENUDIALOG_CB_DELETE, this, this);
	GUI::get_gui()->unlock_input();
	return GUI_YUM;
}

GUI_status GameMenuDialog::KeyDown(SDL_keysym key) {
	if(key.sym == SDLK_ESCAPE)
		return close_dialog();
	return GUI_YUM;
}

GUI_status GameMenuDialog::callback(uint16 msg, GUI_CallBack *caller, void *data) {
	GUI *gui = GUI::get_gui();
	gui->lock_input(this);

	if(caller == this) {
		 close_dialog();
	} else if(caller == (GUI_CallBack *)saveLoad_button) {
		Game::get_game()->get_event()->saveDialog();
	} else if(caller == (GUI_CallBack *)video_button) {
		GUI_Widget *video_dialog;
		video_dialog = (GUI_Widget *) new VideoDialog((GUI_CallBack *)this);
		GUI::get_gui()->AddWidget(video_dialog);
		gui->lock_input(video_dialog);
	} else if(caller == (GUI_CallBack *)audio_button) {
		GUI_Widget *audio_dialog;
		audio_dialog = (GUI_Widget *) new AudioDialog((GUI_CallBack *)this);
		GUI::get_gui()->AddWidget(audio_dialog);
		gui->lock_input(audio_dialog);
	} else if(caller == (GUI_CallBack *)gameplay_button) {
		GUI_Widget *gameplay_dialog;
		gameplay_dialog = (GUI_Widget *) new GameplayDialog((GUI_CallBack *)this);
		GUI::get_gui()->AddWidget(gameplay_dialog);
		gui->lock_input(gameplay_dialog);
	} else if(caller == (GUI_CallBack *)cheats_button) {
		GUI_Widget *cheats_dialog;
		cheats_dialog = (GUI_Widget *) new CheatsDialog((GUI_CallBack *)this);
		GUI::get_gui()->AddWidget(cheats_dialog);
		gui->lock_input(cheats_dialog);
	} else if(caller == (GUI_CallBack *)quit_button) {
		Game::get_game()->get_event()->quitDialog();
	} else {
		return GUI_PASS;
	}
	return GUI_YUM;
}
