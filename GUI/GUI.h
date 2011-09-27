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
#include "GUI_DragManager.h"
#include "GUI_widget.h"
#include "GUI_font.h"


class Configuration;
class Screen;
class GUI_Color;

#define GUI_FULL_REDRAW true

class GUI {

protected:

  static GUI *gui;
   Configuration *config;

	/* The display surface */
  Screen *screen;

	int screen_scale_factor;

  GUI_Font *gui_font;
  GUI_DragManager *gui_drag_manager;

	/* Pointers for an array of widgets */
	int maxwidgets;
	int numwidgets;
	GUI_Widget **widgets;

	/* All input will go to this widget first. */
	GUI_Widget *focused_widget; // SB-X
	/* All input will go to this widget ONLY. */
	GUI_Widget *locked_widget; // SB-X
        /* All input will be ignored. */
        bool block_input; // SB-X

	/* Flag - whether or not the GUI is currently running */
	int running;

	/* Flag - whether or not the GUI needs to be displayed */
	int display;

  /* Flag - wether we are preforming a drag and drop */
  bool dragging;

  bool full_redraw; //this forces all widgets to redraw on the next call to Display()

  // some default colours
  GUI_Color *selected_color;

public:
  static const int mouseclick_delay; /* SB-X */

	GUI(Configuration *c, Screen *s);
	~GUI();

	/* Add a widget to the GUI.
	   The widget will be automatically deleted when the GUI is deleted.
	   This function returns 0, or -1 if the function ran out of memory.
	 */
	int AddWidget(GUI_Widget *widget);

  /* remove widget from gui system but don't delete it */
  bool removeWidget(GUI_Widget *widget);

  bool moveWidget(GUI_Widget *widget, uint32 dx, uint32 dy);

  /* force everything to redraw */
  void force_full_redraw();

	/* Display the GUI manually */
	void Display();

	/* Returns will return true if the GUI is still ready to handle
	   events after a call to Run(), and false if a widget or idle
	   function requested a quit.
	 */
	int Running(void) { return(running); }

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

	/* Run Idle() on all widgets. */
	void Idle(); // SB-X

  static GUI *get_gui() { return gui; }
  GUI_Font *get_font();
  Screen *get_screen()             { return screen; }
  GUI_Widget *get_focused_widget() { return focused_widget; }
  GUI_Widget *get_locked_widget()  { return locked_widget; }
  bool get_block_input()           { return block_input; }

  //colors
  GUI_Color *get_selected_color() { return selected_color; }

  /* Function to pass an event to the GUI widgets */
	GUI_status HandleEvent(SDL_Event *event);

	bool set_focus(GUI_Widget *widget);
	void clear_focus()  { set_focus(NULL); }
	void lock_input(GUI_Widget *widget);
	void unlock_input() { lock_input(NULL); unblock(); }
        void block()   { block_input = true; }
        void unblock() { block_input = false; }
    std::string get_data_dir();

    uint16 get_width() { return screen->get_width(); }
    uint16 get_height() { return screen->get_height(); }
protected:

	/* Function to handle a GUI status */
	void HandleStatus(GUI_status status);

  void CleanupDeletedWidgets(bool redraw=false);

};

#endif /* _GUI_h */
