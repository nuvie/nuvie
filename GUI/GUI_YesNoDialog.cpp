/*
 *  GUI_YesNoDialog.cpp
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

#include "SDL.h"
#include "nuvieDefs.h"

#include "GUI.h"
#include "GUI_button.h"
#include "GUI_text.h"

#include "GUI_area.h"
#include "GUI_YesNoDialog.h"


GUI_YesNoDialog::GUI_YesNoDialog(GUI *gui, int x, int y, int w, int h, char *msg, GUI_CallbackProc yesCallbackProc, GUI_CallbackProc noCallbackProc) : 
   GUI_Area(x, y, w, h, 212, 208, 131, 72, 69, 29, 2, AREA_ANGULAR)
{
  GUI_Widget *widget;
  
  yesCallback = yesCallbackProc;
  noCallback = noCallbackProc;

  widget = (GUI_Widget *) new GUI_Button(this, 100, 50, 40, 18, "Yes", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, yesCallback, 0);
  AddWidget(widget);

  widget = (GUI_Widget *) new GUI_Button(this, 30, 50, 40, 18, "No", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, noCallback, 0);
  AddWidget(widget);
 
  widget = (GUI_Widget *) new GUI_Text(10, 25, 0, 0, 0, msg, gui->get_font());
  AddWidget(widget);
}


GUI_YesNoDialog::~GUI_YesNoDialog()
{
}

GUI_status GUI_YesNoDialog::KeyDown(SDL_keysym key)
{
 if(key.sym == SDLK_y)
   return yesCallback(this);

 return noCallback(this);
}
