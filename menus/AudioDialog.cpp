/*
 *  AudioDialog.cpp
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
#include "AudioDialog.h"
#include "SoundManager.h"
#include "Configuration.h"
#include <math.h>

#define AD_WIDTH 200
#define AD_HEIGHT 101

AudioDialog::AudioDialog(GUI_CallBack *callback)
          : GUI_Dialog(Game::get_game()->get_game_x_offset() + (Game::get_game()->get_game_width() - AD_WIDTH)/2,
                       Game::get_game()->get_game_y_offset() + (Game::get_game()->get_game_height() - AD_HEIGHT)/2,
                       AD_WIDTH, AD_HEIGHT, 244, 216, 131, GUI_DIALOG_UNMOVABLE) {
	callback_object = callback;
	init();
	grab_focus();
}

bool AudioDialog::init() {
	int height = 12;
	int colX[] = { 121, 151 };
	int textX[] = { 9, 19, 29 };
	int textY[] = { 11, 24, 37, 50, 63};
	int buttonY[] = { 9, 22, 35, 48, 61, 80 };

	GUI_Widget *widget;
	GUI_Font * font = GUI::get_gui()->get_font();

	widget = (GUI_Widget *) new GUI_Text(textX[0], textY[0], 0, 0, 0, "Audio:", font);
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(textX[1], textY[1], 0, 0, 0, "Enable music:", font);
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(textX[2], textY[2], 0, 0, 0, "Music volume:", font);
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(textX[1], textY[3], 0, 0, 0, "Enable sfx:", font);
	AddWidget(widget);
	widget = (GUI_Widget *) new GUI_Text(textX[2], textY[4], 0, 0, 0, "Sfx volume:", font);
	AddWidget(widget);
 
	char musicBuff[6], sfxBuff[6];
	int sfxVol_selection, musicVol_selection, num_of_sfxVol, num_of_musicVol;
	SoundManager *sm = Game::get_game()->get_sound_manager();
	const char* const enabled_text[] = { "Disabled", "Enabled" };

	uint8 music_percent = round(sm->get_music_volume() / 2.55); // round needed for 10%, 30%, etc. 
	sprintf(musicBuff, "~%u%%", music_percent);
	const char* const musicVol_text[] = { "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%", musicBuff };

	if(music_percent % 10 == 0) {
		num_of_musicVol = 11;
		musicVol_selection = music_percent/10;
	} else {
		num_of_musicVol = 12;
		musicVol_selection = 11;
	}

	uint8 sfx_percent = round(sm->get_sfx_volume() / 2.55); // round needed for 10%, 30%, etc.
	sprintf(sfxBuff, "~%u%%", sfx_percent);
	const char* const sfxVol_text[] = { "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%", sfxBuff };

	if(sfx_percent % 10 == 0) {
		num_of_sfxVol = 11;
		sfxVol_selection = sfx_percent/10;
	} else {
		num_of_sfxVol = 12;
		sfxVol_selection = 11;
	}

	audio_button = new GUI_TextToggleButton(this, colX[0], buttonY[0], 70, height, enabled_text, 2, sm->is_audio_enabled(), font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(audio_button); 
	music_button = new GUI_TextToggleButton(this, colX[0], buttonY[1], 70, height, enabled_text, 2, sm->is_music_enabled(), font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(music_button);
	musicVol_button = new GUI_TextToggleButton(this, colX[1], buttonY[2], 40, height, musicVol_text, num_of_musicVol, musicVol_selection, font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(musicVol_button);
	sfx_button = new GUI_TextToggleButton(this, colX[0], buttonY[3], 70, height, enabled_text, 2, sm->is_sfx_enabled(), font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(sfx_button);
	sfxVol_button = new GUI_TextToggleButton(this, colX[1], buttonY[4], 40, height, sfxVol_text, num_of_sfxVol, sfxVol_selection, font, BUTTON_TEXTALIGN_CENTER, this, 0);
	AddWidget(sfxVol_button); 

	cancel_button = new GUI_Button(this, 34, buttonY[5], 54, height, "Cancel", font, BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(cancel_button);
	save_button = new GUI_Button(this, 105, buttonY[5], 60, height, "Save", font, BUTTON_TEXTALIGN_CENTER, 0, this, 0);
	AddWidget(save_button);
 
 return true;
}

AudioDialog::~AudioDialog() {
}

GUI_status AudioDialog::close_dialog() {
	Delete(); // mark dialog as deleted. it will be freed by the GUI object
	callback_object->callback(0, this, this);
	return GUI_YUM;
}

GUI_status AudioDialog::KeyDown(SDL_keysym key) {
	if(key.sym == SDLK_ESCAPE)
		return close_dialog();
	return GUI_PASS;
}

GUI_status AudioDialog::callback(uint16 msg, GUI_CallBack *caller, void *data) {
	if(caller == (GUI_CallBack *)cancel_button) {
		return close_dialog();
	} else if(caller == (GUI_CallBack *)save_button) {
		Configuration *config = Game::get_game()->get_config();
		SoundManager *sm = Game::get_game()->get_sound_manager();

		int music_selection = musicVol_button->GetSelection();
		if(music_selection != 11) {
			uint8 musicVol = music_selection * 25.5;
			sm->set_music_volume(musicVol);
			if(sm->get_m_pCurrentSong() != NULL)
				sm->get_m_pCurrentSong()->SetVolume(musicVol);
			config->set("config/audio/music_volume", musicVol);
		}

		int sfx_selection = sfxVol_button->GetSelection();
		if(sfx_selection != 11) {
			uint8 sfxVol = sfx_selection * 25.5;
			sm->set_sfx_volume(sfxVol);
// probably need to update sfx volume if we have background sfx implemented
			config->set("config/audio/sfx_volume", sfxVol);
		}

		if(music_button->GetSelection() != sm->is_music_enabled())
			sm->set_music_enabled(music_button->GetSelection());
		config->set("config/audio/enable_music", music_button->GetSelection() ? "yes" : "no");
		if(sfx_button->GetSelection() != sm->is_sfx_enabled())
			sm->set_sfx_enabled(sfx_button->GetSelection());
		config->set("config/audio/enable_sfx", sfx_button->GetSelection() ? "yes" : "no");
		if(audio_button->GetSelection() != sm->is_audio_enabled())
			sm->set_audio_enabled(audio_button->GetSelection());
		config->set("config/audio/enabled", audio_button->GetSelection() ? "yes" : "no");

		config->write();
		return close_dialog();
	}

	return GUI_PASS;
}
