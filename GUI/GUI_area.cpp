/*
    GUILIB:  An example GUI framework library for use with SDL
    Copyright (C) 1997  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

#include <cmath>
#include "nuvieDefs.h"
#include "GUI.h"
#include "GUI_area.h"

GUI_Area:: GUI_Area(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int aShape)
 : GUI_Widget(NULL, x, y, w, h)
{
	R = r;
	G = g;
	B = b;
	color = 0;
	useFrame=0;
	shape=aShape;
}

GUI_Area:: GUI_Area(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b,
			Uint8 fr, Uint8 fg, Uint8 fb, int fthick, int aShape)
 : GUI_Widget(NULL, x, y, w, h)
{
	R = r;
	G = g;
	B = b;
	color = 0;
	useFrame=1;
	fR = fr;
	fG = fg;
	fB = fb;
	frameColor=0;
	frameThickness=fthick;
	shape=aShape;
}

/* Map the color to the display */
void
GUI_Area:: SetDisplay(Screen *s)
{
	GUI_Widget::SetDisplay(s);
	color = SDL_MapRGB(surface->format, R, G, B);
	if (useFrame)
	  frameColor = SDL_MapRGB(surface->format, fR, fG, fB);
}

/* Show the widget  */
void
GUI_Area:: Display(bool full_redraw)
{
	SDL_Rect framerect;
	int x,dy,r1,r2,x0,y0;

	switch (shape)
	{
	case AREA_ROUND:

	  r1=area.w >> 1;
	  r2=area.h >> 1;
	  x0=area.x+r1;
	  y0=area.y+r2;
	  for (x=area.x;x<area.x+area.w;x++)
	  {
	    dy=(int)((double) r2*sin(acos((double) (x-x0)/(double) r1)));
	    framerect.x=x; framerect.y=y0-dy;
	    framerect.w=1; framerect.h=dy << 1;
	    SDL_FillRect(surface,&framerect,color);
	    if (useFrame)
	    {
	      if ((x==area.x) || (x==area.x+area.w-1))
	      {
	        SDL_FillRect(surface,&framerect,frameColor);
	      }
	      framerect.h=frameThickness;
	      SDL_FillRect(surface,&framerect,frameColor);
	      framerect.y=y0+dy-frameThickness;
	      SDL_FillRect(surface,&framerect,frameColor);
	    }
	  }
/*
******** GUI_FillEllipse is not ready yet, GUI_BoundaryFill either *****
	    framerect=area;
	    if (useFrame)
	    {
	      GUI_FillEllipse(screen,&framerect,frameColor);
	      area.x+=frameThickness; area.w-=frameThickness << 1;
	      area.y+=frameThickness; area.h-=frameThickness << 1;
	    }
	    GUI_FillEllipse(screen,&framerect,color);
*/
	  break;

	case AREA_ANGULAR:
      framerect=area;
	  SDL_FillRect(surface, &framerect, color);

	  /* draw frame */
	  if (useFrame)
	  {
        framerect=area;
	    framerect.h=frameThickness;
	    SDL_FillRect(surface, &framerect, frameColor);
        framerect=area;
        framerect.h=frameThickness;
	    framerect.y+=area.h-frameThickness;
	    SDL_FillRect(surface, &framerect, frameColor);
        framerect=area;
	    framerect.w=frameThickness;
	    SDL_FillRect(surface, &framerect, frameColor);
        framerect=area;
        framerect.w=frameThickness;
	    framerect.x+=area.w-frameThickness;
	    SDL_FillRect(surface, &framerect, frameColor);
	  }
	  break;
	}

 DisplayChildren();
 
 screen->update(area.x,area.y,area.w,area.h);

 return;
}