/*
 *  GUI_Dialog.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Apr 03 2004.
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

#include <cmath>
#include <string>
#include "nuvieDefs.h"
#include "U6misc.h"
#include "GUI.h"
#include "GUI_Dialog.h"

GUI_Dialog::GUI_Dialog(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, bool is_moveable)
 : GUI_Widget(NULL, x, y, w, h)
{

	R = r;
	G = g;
	B = b;
	bg_color = 0;
    drag = false;
    old_x = old_y = 0;

    loadBorderImages();
}


void GUI_Dialog::loadBorderImages()
{
  uint8 i;
  char filename[15]; // BorderU6_x.bmp\0
  std::string datadir = GUI::get_gui()->get_data_dir();
  std::string imagefile;

  for(i=0;i<8;i++)
   {
     sprintf(filename, "Border%s_%d.bmp", "U6", i + 1);
     build_path(datadir, filename, imagefile);
     border[i] = SDL_LoadBMP(imagefile.c_str());
     if(border[i] == NULL)
       {
        printf("Error: Loading %s from '%s' directory\n", filename, datadir.c_str());
       }
   }
}

/* Map the color to the display */
void GUI_Dialog::SetDisplay(Screen *s)
{
	GUI_Widget::SetDisplay(s);
	bg_color = SDL_MapRGB(surface->format, R, G, B);
}

/* Show the widget  */
void
GUI_Dialog:: Display(bool full_redraw)
{
 int i;
 SDL_Rect framerect;
 SDL_Rect src, dst;

 framerect.x = area.x + 8;
 framerect.y = area.y + 8;
 framerect.w = area.w - 16;
 framerect.h = area.h - 16;
 SDL_FillRect(surface, &framerect, bg_color);

 // Draw border corners

 dst = area;
 dst.w = 8;
 dst.h = 8;
 SDL_BlitSurface(border[0], NULL, surface, &dst);

 dst.x = area.x + area.w - 8;
 dst.y = area.y;
 dst.w = 8;
 dst.h = 8;
 SDL_BlitSurface(border[2], NULL, surface, &dst);

 dst.x = area.x + area.w - 8;
 dst.y = area.y + area.h - 8;
 dst.w = 8;
 dst.h = 8;
 SDL_BlitSurface(border[4], NULL, surface, &dst);

 dst.x = area.x;
 dst.y = area.y + area.h - 8;
 dst.w = 8;
 dst.h = 8;
 SDL_BlitSurface(border[6], NULL, surface, &dst);

 // Draw top and bottom border lines

 for(i=area.x + 8; i < area.x + area.w - 24; i += 16)
   {
    dst.x = i;
    dst.y = area.y;
    dst.w = 16;
    dst.h = 8;
    SDL_BlitSurface(border[1], NULL, surface, &dst);

    dst.x = i;
    dst.y = area.y + area.h - 8;
    dst.w = 16;
    dst.h = 8;
    SDL_BlitSurface(border[5], NULL, surface, &dst);
   }

 if(i < area.x + area.w - 8) // draw partial border images
   {
    src.x = 0;
    src.y = 0;
    src.w = area.x + area.w - 8 - i;
    src.h = 8;

    dst.x = i;
    dst.y = area.y;
    dst.w = src.w;
    dst.h = 8;
    SDL_BlitSurface(border[1], &src, surface, &dst);

    dst.x = i;
    dst.y = area.y + area.h - 8;
    dst.w = src.w;
    dst.h = 8;
    SDL_BlitSurface(border[5], &src, surface, &dst);
   }

 // Draw left and right sides


 for(i=area.y + 8; i < area.y + area.h - 24; i += 16)
   {
    dst.x = area.x;
    dst.y = i;
    dst.w = 8;
    dst.h = 16;
    SDL_BlitSurface(border[7], NULL, surface, &dst);

    dst.x = area.x + area.w - 8;
    dst.y = i;
    dst.w = 8;
    dst.h = 16;
    SDL_BlitSurface(border[3], NULL, surface, &dst);
   }

 if(i < area.y + area.h - 8) // draw partial border images
   {
    src.x = 0;
    src.y = 0;
    src.w = 8;
    src.h = area.y + area.h - 8 - i;

    dst.x = area.x;
    dst.y = i;
    dst.w = 8;
    dst.h = src.h;
    SDL_BlitSurface(border[7], &src, surface, &dst);

    dst.x = area.x + area.w - 8;
    dst.y = i;
    dst.w = 8;
    dst.h = src.h;
    SDL_BlitSurface(border[3], &src, surface, &dst);
   }

 DisplayChildren();

 screen->update(area.x,area.y,area.w,area.h);

 return;
}

GUI_status GUI_Dialog::MouseDown(int x, int y, int button)
{
 drag = true;
 old_x = x;
 old_y = y;

 grab_focus();

 return GUI_YUM;
}

GUI_status GUI_Dialog::MouseUp(int x, int y, int button)
{
 drag = false;

 release_focus();

 return GUI_YUM;
}

GUI_status GUI_Dialog::MouseMotion(int x,int y,Uint8 state)
{
 int dx, dy;

 if(!drag)
   return GUI_PASS;

 dx = x - old_x;
 dy = y - old_y;

 old_x = x;
 old_y = y;

 GUI::get_gui()->moveWidget(this,dx,dy);
// Redraw();

 return (GUI_YUM);
}

