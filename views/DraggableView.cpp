/*
 *  DraggableView.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Mar 7 2012.
 *  Copyright (c) 2012. All rights reserved.
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
#include "U6misc.h"
#include "GUI.h"
#include "GUI_button.h"
#include "Configuration.h"
#include "DraggableView.h"

DraggableView::DraggableView(Configuration *config)
: View(config)
{
	drag = false;
}

DraggableView::~DraggableView()
{
}

GUI_status DraggableView::MouseDown(int x, int y, int button)
{
 drag = true;
 button_x = x;
 button_y = y;

 grab_focus();

 return GUI_YUM;
}

GUI_status DraggableView::MouseUp(int x, int y, int button)
{
 drag = false;

 release_focus();

 return GUI_YUM;
}

GUI_status DraggableView::MouseMotion(int x,int y,Uint8 state)
{
 int dx, dy;

 if(!drag || state == 0) //state is 0 if no button pressed
   return GUI_PASS;

 dx = x - button_x;
 dy = y - button_y;

 button_x = x;
 button_y = y;

 GUI::get_gui()->moveWidget(this,dx,dy);
// Redraw();

 return (GUI_YUM);
}

void DraggableView::MoveRelative(int dx,int dy)
{
 int new_x = area.x + dx;

 if(new_x < 0)
 {
	 dx = -area.x;
 }
 else if(new_x + area.w > screen->get_width())
 {
	 dx = screen->get_width() - (area.x + area.w);
 }

 int new_y = area.y + dy;

 if(new_y < 0)
 {
	 dy = -area.y;
 }
 else if(new_y + area.h > screen->get_height())
 {
	 dy = screen->get_height() - (area.y + area.h);
 }

 GUI_Widget::MoveRelative(dx, dy);

 return;
}

GUI_Button *DraggableView::loadButton(std::string dir, std::string name, uint16 x, uint16 y)
{
	GUI_Button *button;
	std::string imagefile;
		std::string path;

		SDL_Surface *image, *image1;
		build_path(dir, name + "_btn_up.bmp", imagefile);
			image = SDL_LoadBMP(imagefile.c_str());
			build_path(dir, name + "_btn_down.bmp", imagefile);
			image1 = SDL_LoadBMP(imagefile.c_str());

			button = new GUI_Button(NULL, x, y, image, image1, this);
			this->AddWidget(button);
	return button;
}
