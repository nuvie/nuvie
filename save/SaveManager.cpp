/*
 *  SaveManager.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Apr 28 2004.
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

#include "nuvieDefs.h"
#include "Configuration.h"
 
#include "Actor.h"
#include "ActorManager.h"
#include "U6misc.h"
#include "U6LList.h"
#include "SaveManager.h"
#include "NuvieIOFile.h"

#include "GUI.h"
#include "SaveDialog.h"


SaveManager::SaveManager(Configuration *cfg)
{
 config = cfg;
 dialog = NULL;
 
}

SaveManager::~SaveManager()
{
}

void SaveManager::create_dialog()
{
 GUI *gui = GUI::get_gui();
  
 if(dialog == NULL)
   {
    dialog = new SaveDialog((GUI_CallBack *)this);
    dialog->grab_focus();
    gui->AddWidget(dialog);
   }
   
 return;
}

GUI_status SaveManager::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
 if(caller == dialog)
  {
   switch(msg)
     {
       case SAVEDIALOG_CB_DELETE : dialog = NULL; break;
       default : break;
     }
  }

 return GUI_YUM;
}
