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
#include "GUI_CallBack.h"
#include "GUI_area.h"

#include "GUI_Dialog.h"
#include "SaveSlot.h"
#include "SaveDialog.h"
#include "NuvieFileList.h"

#define NUVIE_SAVE_SCROLLER_ROWS   3
#define NUVIE_SAVE_SCROLLER_HEIGHT NUVIE_SAVE_SCROLLER_ROWS * NUVIE_SAVESLOT_HEIGHT

SaveDialog::SaveDialog(GUI_CallBack *callback)
          : GUI_Dialog(0,0, 320, 200, 244, 216, 131, false)
{
 callback_object = callback;
 selected_slot = NULL;
 scroller = NULL;
 save_button = NULL;
 load_button = NULL;
 cancel_button = NULL;
}

bool SaveDialog::init(const char *save_directory, const char *search_prefix)
{
 uint32 num_saves, i;
 NuvieFileList filelist;
 std::string *filename;
 GUI_Widget *widget;
 GUI *gui = GUI::get_gui();
 GUI_Color bg_color = GUI_Color(162,144,87);
 GUI_Color bg_color1 = GUI_Color(147,131,74);
 GUI_Color *color_ptr;

 if(filelist.open(save_directory, search_prefix, NUVIE_SORT_TIME_DESC) == false)
   return false;


 scroller = new GUI_Scroller(20,26, 280, NUVIE_SAVE_SCROLLER_HEIGHT, 135,119,76, NUVIE_SAVESLOT_HEIGHT );
 widget = (GUI_Widget *) new GUI_Text(20, 12, 0, 0, 0, "Load/Save", gui->get_font());
 AddWidget(widget);

 num_saves = filelist.get_num_files();



// Add an empty slot at the top.
 widget = new SaveSlot(this, bg_color1);
 ((SaveSlot *)widget)->init(NULL, NULL);

 scroller->AddWidget(widget);

 color_ptr = &bg_color;

 for(i=0; i < num_saves; i++)
   {
    filename = filelist.next();
    widget = new SaveSlot(this, *color_ptr);
    if(((SaveSlot *)widget)->init(save_directory, filename) == true)
     {
      scroller->AddWidget(widget);

      if(color_ptr == &bg_color)
        color_ptr = &bg_color1;
      else
        color_ptr = &bg_color;
     }
    else
      delete (SaveSlot *)widget;
   }

 // pad out empty slots if required
/*
 if(num_saves < NUVIE_SAVE_SCROLLER_ROWS-1)
   {
    for(i=0; i < NUVIE_SAVE_SCROLLER_ROWS - num_saves - 1; i++)
      {
       widget = new SaveSlot(this, *color_ptr);
       ((SaveSlot *)widget)->init(NULL);

       scroller->AddWidget(widget);

       if(color_ptr == &bg_color)
         color_ptr = &bg_color1;
       else
         color_ptr = &bg_color;
      }
   }
*/
 AddWidget(scroller);

 load_button = new GUI_Button(this, 145, 8, 40, 16, "Load", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
 AddWidget(load_button);

 save_button = new GUI_Button(this, 195, 8, 40, 16, "Save", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0);
 AddWidget(save_button);

 cancel_button = new GUI_Button(this, 245, 8, 55, 16, "Cancel", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, this, 0); //154
 AddWidget(cancel_button);

 filelist.close();

 if(Game::get_game()->is_armageddon())
     save_button->Hide();

 return true;
}


SaveDialog::~SaveDialog()
{
}

GUI_status SaveDialog::close_dialog()
{
 Delete(); // mark dialog as deleted. it will be freed by the GUI object
 return callback_object->callback(SAVEDIALOG_CB_DELETE, this, this);
}

GUI_status SaveDialog::MouseDown(int x, int y, int button)
{
 if(button == SDL_BUTTON_WHEELUP)
	scroller->move_up();
 else if(button == SDL_BUTTON_WHEELDOWN)
	scroller->move_down();
 return GUI_YUM;
}

GUI_status SaveDialog::KeyDown(SDL_keysym key)
{

 switch(key.sym)
 {
	case SDLK_ESCAPE :
		return close_dialog();
	case SDLK_UP :
	case SDLK_KP8 :
		scroller->move_up();
		break;
	case SDLK_DOWN :
	case SDLK_KP2 :
		scroller->move_down();
		break;
	case SDLK_PAGEUP:
	case SDLK_LEFT:
	case SDLK_KP4:
		scroller->page_up();
		break;
	case SDLK_PAGEDOWN :
	case SDLK_RIGHT :
	case SDLK_KP6 :
		scroller->page_down();
		break;
	case SDLK_HOME :
		scroller->page_up(true);
		break;
	case SDLK_END :
		scroller->page_down(true);
		break;
	default:
		break;
 }
 return GUI_YUM;
}

GUI_status SaveDialog::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
 if(caller == (GUI_CallBack *)cancel_button)
    return close_dialog();

 if(caller == (GUI_CallBack *)load_button)
    {
     if(selected_slot != NULL && callback_object->callback(SAVEDIALOG_CB_LOAD, this, selected_slot) == GUI_YUM)
        close_dialog();
     return GUI_YUM;
    }

 if(caller == (GUI_CallBack *)save_button)
    {
     if(selected_slot != NULL && callback_object->callback(SAVEDIALOG_CB_SAVE, this, selected_slot) == GUI_YUM)
        close_dialog();
     return GUI_YUM;
    }

 if(dynamic_cast<SaveSlot*>(caller))
   {
    if(msg == SAVESLOT_CB_SELECTED)
      {
       if(selected_slot != NULL)
         selected_slot->deselect();

       selected_slot = (SaveSlot *)caller;
      }

    if(msg == SAVESLOT_CB_SAVE)
      {
       if(callback_object->callback(SAVEDIALOG_CB_SAVE, this, caller) == GUI_YUM) //caller = slot to save in
         close_dialog();
      }
   }
/*
 if(caller == (GUI_CallBack *)no_button)
   return no_callback_object->callback(YESNODIALOG_CB_NO, this, this);
*/
 return GUI_PASS;
}
