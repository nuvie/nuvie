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

#include "GUI_area.h"

class GUI;

class GUI_YesNoDialog : public GUI_Area {
protected:

GUI_CallbackProc yesCallback;
GUI_CallbackProc noCallback;

public:

GUI_YesNoDialog(GUI *gui, int x, int y, int w, int h, char *msg, GUI_CallbackProc yesCallbackProc, GUI_CallbackProc noCallbackProc);

~GUI_YesNoDialog();

GUI_status KeyDown(SDL_keysym key);

};

#endif /* __GUI_YesNoDialog_h__ */
