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

/* Base class for all widgets -- the GUI operates on this class */

#ifndef _GUI_widget_h
#define _GUI_widget_h

#include <list>
#include <stdio.h>
#include <stdarg.h>

#include "SDL.h"
#include "GUI_status.h"


class GUI_Widget {

public:
	GUI_Widget(void *data);
	GUI_Widget(void *data, int x, int y, int w, int h);
	virtual ~GUI_Widget() { }

  int AddWidget(GUI_Widget *widget);
  
	/* Mark the widget as visible -- this is the default state */
	virtual void Show(void);

	/* Mark the widget as hidden;  no display, no events */
	virtual void Hide(void);

	/* Mark the widget as free, so it will be deleted by the GUI */
	virtual void Delete(void);
  
  void PlaceOnScreen(SDL_Surface *display, int x, int y);
  
	virtual int  Status(void);	/* Reports status to GUI */

	/* Set the bounds of the widget.
	   If 'w' or 'h' is -1, that parameter will not be changed.
	 */
	virtual void SetRect(int x, int y, int w, int h);
	virtual void SetRect(SDL_Rect **bounds);

	/* Return the whole area */
	virtual SDL_Rect GetRect()
	  {return area;}

	/* Return the bounds of the widget */
	virtual int X() { return area.x; }
	virtual int Y() { return area.y; }
	virtual int W() { return area.w; }
	virtual int H() { return area.h; }

	/* Check to see if a point intersects the bounds of the widget.
	 */
	virtual int HitRect(int x, int y);
	virtual int HitRect(int x, int y, SDL_Rect &rect);

	/* Set the display surface for this widget */
	virtual void SetDisplay(SDL_Surface *display);

	/* Show the widget.
	   If the surface needs to be locked, it will be locked
	   before this call, and unlocked after it returns.
	 */
	virtual void Display(void);
  void DisplayChildren(void);

	/* Redraw the widget and only the widget */
	virtual void Redraw(void);

	/* GUI idle function -- run when no events pending */
	virtual GUI_status Idle(void);

	/* Widget event handlers.
	   These functions should return a status telling the GUI whether
	   or not the event should be passed on to other widgets.
	   These are called by the default HandleEvent function.
	*/
	virtual GUI_status KeyDown(SDL_keysym key);
	virtual GUI_status KeyUp(SDL_keysym key);
	virtual GUI_status MouseDown(int x, int y, int button);
	virtual GUI_status MouseUp(int x, int y, int button);
	virtual GUI_status MouseMotion(int x, int y, Uint8 state);

	/* Main event handler function.
	   This function gets raw SDL events from the GUI.
	 */
	virtual GUI_status HandleEvent(const SDL_Event *event);

	/* Returns NULL if everything is okay, or an error message if not */
	char *Error(void) {
		return(error);
	}

	/* yields click state: none, pressed, intermediate */
	inline virtual int ClickState(int button)
	{ return pressed[button-1]; }

	/* set click state from remote */
	inline virtual void SetClickState(int button, int value)
	{ if ((button>0) && (button<=3)) pressed[button-1]=value; }

protected:
	/* The constructor, separated out for both access constructors */
	void Init(void *data, int x, int y, int w, int h);

	/* A generic pointer to user-specified data for the widget.
	 */
	void *widget_data;

	/* The display surface for the widget */
	SDL_Surface *screen;

	/* The area covered by the widget */
	SDL_Rect area;

  int real_x, real_y; /* x,y in screen coords */
  
	/* Flag -- whether or not the widget should be freed */
	int status;

	/* the button states for theoretically 3 buttons */
	int pressed[3];
  
  std::list<GUI_Widget *>children;

	/* Useful for getting error feedback */
	void SetError(char *fmt, ...) {
		va_list ap;

		va_start(ap, fmt);
		vsprintf(errbuf, fmt, ap);
		va_end(ap);
		error = errbuf;
	}
	char *error;
	char  errbuf[BUFSIZ];

};

#endif /* _GUI_widget_h */
