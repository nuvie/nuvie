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
#include "GUI_Area.h"
#include "GUI_Text.h"
#include "GUI_Scroller.h"
#include "GUI_Dialog.h"
#include "GUI_Button.h"

SaveManager::SaveManager(Configuration *cfg, ActorManager *am, ObjManager *om)
{
 config = cfg;
 actor_manager = am;
 obj_manager = om;
 
 create_dialog();
}

SaveManager::~SaveManager()
{
}

void SaveManager::create_dialog()
{
 GUI_Widget *widget;
 GUI *gui = GUI::get_gui();
 
 dialog = new GUI_Dialog(10,10, 300, 180, 244, 216, 131, GUI_DIALOG_MOVABLE);
 scroller = new GUI_Scroller(10,25, 280, 120, 135,119,76, 20 );
 widget = (GUI_Widget *) new GUI_Text(10, 12, 0, 0, 0, "Nuvie Load/Save Manager", gui->get_font());
 dialog->AddWidget(widget);
 
 widget = new GUI_Area(0, 0, 266, 20, 225, 225, 225, AREA_ANGULAR);
 scroller->AddWidget(widget);
 widget = new GUI_Area(0, 0, 266, 20, 50, 50, 50, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 25, 25, 25, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 50, 50, 50, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 25, 25, 25, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 50, 50, 50, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 25, 25, 25, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 50, 50, 50, AREA_ANGULAR);
 scroller->AddWidget(widget);
  widget = new GUI_Area(0, 0, 266, 20, 25, 25, 25, AREA_ANGULAR);
 scroller->AddWidget(widget);
  
 dialog->AddWidget(scroller);

 widget = (GUI_Widget *) new GUI_Button(this, 135, 152, 40, 18, "Load", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, NULL, 0);
 dialog->AddWidget(widget);
 
 widget = (GUI_Widget *) new GUI_Button(this, 185, 152, 40, 18, "Save", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, NULL, 0);
 dialog->AddWidget(widget);

 widget = (GUI_Widget *) new GUI_Button(this, 235, 152, 55, 18, "Cancel", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, NULL, 0);
 dialog->AddWidget(widget);

 gui->AddWidget(dialog);
}
