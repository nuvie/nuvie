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

/* This is a C++ class for handling a GUI, and associated widgets */

#ifndef _GUI_h
#define _GUI_h

#include "SDL.h"
#include "GUI_status.h"
#include "GUI_widget.h"
#include "GUI_font.h"

#include "Screen.h"

#define GUI_FULL_REDRAW true
class GUI {

public:
	GUI(Screen *s);
	~GUI();

	/* Add a widget to the GUI.
	   The widget will be automatically deleted when the GUI is deleted.
	   This function returns 0, or -1 if the function ran out of memory.
	 */
	int AddWidget(GUI_Widget *widget);

  /* remove widget from gui system but don't delete it */
  bool removeWidget(GUI_Widget *widget);

	/* Display the GUI manually */
	void Display(bool full_redraw=false);

	/* Returns will return true if the GUI is still ready to handle
	   events after a call to Run(), and false if a widget or idle
	   function requested a quit.
	 */
	int Running(void) {
		return(running);
	}

	/* Run the GUI.
	   This returns when either a widget requests a quit, the idle
	   function requests a quit, or the SDL window has been closed.
	   If 'once' is non-zero, you need to display the GUI yourself,
	   and the GUI event loop will run once and then return.
	   If 'multitaskfriendly' is non-zero AND idle is NULL,
	   a 'WaitEvent' will be used instead of the CPU time
	   consuming 'PollEvent'. CAVE: Any widget-'idle'-procs WON'T
	   be executed then.
	 */
	void Run(GUI_IdleProc idle = NULL, int once = 0, int multitaskfriendly = 0);
  
  GUI_Font *get_font();
  Screen *get_screen() {return screen;}
  
  /* Function to pass an event to the GUI widgets */
	GUI_status HandleEvent(SDL_Event *event);
  
protected:
	/* The display surface */
  Screen *screen;
	int screen_scale_factor;

  GUI_Font *gui_font;
  
	/* Pointers for an array of widgets */
	int maxwidgets;
	int numwidgets;
	GUI_Widget **widgets;

	/* Flag - whether or not the GUI is currently running */
	int running;

	/* Flag - whether or not the GUI needs to be displayed */
	int display;

	/* Function to handle a GUI status */
	void HandleStatus(GUI_status status);


};

#endif /* _GUI_h */
