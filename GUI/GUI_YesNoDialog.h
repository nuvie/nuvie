#ifndef __GUI_YesNoDialog_h__
#define __GUI_YesNoDialog_h__
/*
 *  GUI_YesNoDialog.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Feb 07 2004.
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

#include "GUI_Dialog.h"

class GUI;
class GUI_CallBack;
class GUI_Button;

// Callback message types

#define YESNODIALOG_CB_YES 0x1
#define YESNODIALOG_CB_NO  0x2

class GUI_YesNoDialog : public GUI_Dialog {
protected:

sint8 b_index_num;
GUI_Button *yes_button, *no_button;
GUI_CallBack *yes_callback_object, *no_callback_object;
GUI_Button *button_index[2];

public:

GUI_YesNoDialog(GUI *gui, int x, int y, int w, int h, const char *msg, GUI_CallBack *yesCallback, GUI_CallBack *noCallback);

~GUI_YesNoDialog();

GUI_status KeyDown(SDL_Keysym key);

GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
};

#endif /* __GUI_YesNoDialog_h__ */
