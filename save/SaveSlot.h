#ifndef __SaveSlot_h__
#define __SaveSlot_h__
/*
 *  SaveSlot.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed May 12 2004.
 *  Copyright (c) 2004. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "GUI_widget.h"
#include "GUI_types.h"
using std::string;

class GUI;
class GUI_CallBack;
class GUI_TextInput;

#define NUVIE_SAVESLOT_HEIGHT 52

// Callback message types

#define SAVESLOT_CB_SAVE     0x1
#define SAVESLOT_CB_CANCEL   0x2
#define SAVESLOT_CB_SELECTED 0x3

class SaveSlot : public GUI_Widget {
protected:

GUI_CallBack *callback_object;
GUI_Color background_color;
bool selected;
bool new_save;

std::string filename;
std::string save_description;

SDL_Surface *thumbnail;

GUI_TextInput *textinput_widget;

public:

SaveSlot(GUI_CallBack *callback, GUI_Color bg_color);

~SaveSlot();

bool init(const char *directory, std::string *filename);

bool is_new_save() { return new_save; }
std::string get_save_description();
std::string *get_filename();

void deselect();

void SetDisplay(Screen *s);

void Display(bool full_redraw);

GUI_status KeyDown(SDL_Keysym key);
GUI_status MouseDown(int x, int y, int button);
GUI_status MouseUp(int x, int y, int button);
GUI_status MouseWheel(sint32 x, sint32 y);

    GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);

protected:

bool load_info(const char *directory);

};

#endif /* __SaveSlot_h__ */
