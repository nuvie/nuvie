/*
 *  SaveDialog.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Mon May 10 2004.
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
#include "GUI_types.h"
#include "GUI_button.h"
#include "GUI_text.h"
#include "GUI_Scroller.h"
#include "GUI_Callback.h"
#include "GUI_area.h"

#include "GUI_Dialog.h"
#include "SaveSlot.h"
#include "SaveDialog.h"


SaveDialog::SaveDialog(GUI_CallBack *callback) : GUI_Dialog(10,10, 300, 180, 244, 216, 131, GUI_DIALOG_MOVABLE)
{
 GUI_Widget *widget;
 GUI *gui = GUI::get_gui();
 GUI_Color bg_color = GUI_Color(162,144,87);
 GUI_Color bg_color1 = GUI_Color(147,131,74);
 
 callback_object = callback;
 selected_slot = NULL;
 
 scroller = new GUI_Scroller(10,25, 280, 120, 135,119,76, 20 );
 widget = (GUI_Widget *) new GUI_Text(10, 12, 0, 0, 0, "Nuvie Load/Save Manager", gui->get_font());
 AddWidget(widget);
 
 widget = new SaveSlot(this, bg_color);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color1);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color1);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color1);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color1);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color1);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color);
 scroller->AddWidget(widget);
 widget = new SaveSlot(this, bg_color1);
 scroller->AddWidget(widget);

 AddWidget(scroller);

 load_button = new GUI_Button(this, 135, 152, 40, 18, "Load", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, NULL, 0);
 AddWidget(load_button);
 
 save_button = new GUI_Button(this, 185, 152, 40, 18, "Save", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, NULL, 0);
 AddWidget(save_button);

 cancel_button = new GUI_Button(this, 235, 152, 55, 18, "Cancel", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
 AddWidget(cancel_button);

 return;
}


SaveDialog::~SaveDialog()
{
}

GUI_status SaveDialog::close_dialog()
{
 Delete(); // mark dialog as deleted. it will be freed by the GUI object
 return callback_object->callback(SAVEDIALOG_CB_DELETE, this, this);
}

GUI_status SaveDialog::KeyDown(SDL_keysym key)
{

 if(key.sym == SDLK_ESCAPE)
   return close_dialog();

 /*
 return no_callback_object->callback(YESNODIALOG_CB_NO, this, this);
 */
 return GUI_PASS;
}

GUI_status SaveDialog::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
 if(caller == (GUI_CallBack *)cancel_button)
    return close_dialog();
 
 if(dynamic_cast<SaveSlot*>(caller))
   {
    if(msg == SAVESLOT_CB_SELECTED)
      {
       if(selected_slot != NULL)
         selected_slot->deselect();

       selected_slot = (SaveSlot *)caller;
      }
   }
/*
 if(caller == (GUI_CallBack *)no_button)
   return no_callback_object->callback(YESNODIALOG_CB_NO, this, this);
*/
 return GUI_PASS;
}
