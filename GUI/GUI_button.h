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

/* This is a very generic button widget - NOT ANY LONGER*/

#ifndef _GUI_button_h
#define _GUI_button_h

#include "GUI_widget.h"
#include "GUI_font.h"

/* design constants */
#define BUTTON3D_UP 1
#define BUTTON3D_DOWN 2
#define BUTTON2D_UP 3
#define BUTTON2D_DOWN 4

/* alignment constants */
#define BUTTON_TEXTALIGN_LEFT 1
#define BUTTON_TEXTALIGN_CENTER 2
#define BUTTON_TEXTALIGN_RIGHT 3

/* color constants */

// Button face color
const Uint8 BF_R=180,BF_G=180,BF_B=180;
// Button light color
const Uint8 BL_R=220,BL_G=220,BL_B=220;
// Button shadow color
const Uint8 BS_R=140,BS_G=140,BS_B=140;
// 2D Button inverse text color
const Uint8 BI1_R=255,BI1_G=255,BI1_B=255;
// 2D Button inverse background color
const Uint8 BI2_R=0,BI2_G=0,BI2_B=0;


/* This is the definition of the "I've been activated" callback */
typedef GUI_status (*GUI_ActiveProc)(void *data);

class GUI_Button : public GUI_Widget {

public:
	/* Passed the button data, position, images (pressed/unpressed) and callback */
	GUI_Button(void *data, int x, int y, SDL_Surface *image,
		   SDL_Surface *image2, GUI_ActiveProc activeproc = NULL);

	/* I don't know what this one is for */
	GUI_Button(void *data, int x, int y, int w, int h,
		   GUI_ActiveProc activeproc = NULL);

	/* Passed the button data, position, width, height, a caption, a font,
	   an alignment (one of the constants above), if it should be a checkbutton (1/0),
	   the callback and a flag if it should be 2D (1) or 3D (0) */
	GUI_Button(void *data, int x, int y, int w, int h, char* text,
		   GUI_Font *font, int alignment, int is_checkbutton,
		   GUI_ActiveProc activeproc = NULL, int flat = 0);

	~GUI_Button();

	/* change features of a text button (if one of the dimensions is negativ, it's ignored) */
	virtual void ChangeTextButton(int x, int y, int w, int h, char* text, int alignment);

	/* Show the widget  */
	virtual void Display(void);

	/* Mouse hits activate us */
	virtual GUI_status MouseDown(int x, int y, int button);
	virtual GUI_status MouseUp(int x, int y, int button);
	virtual GUI_status MouseMotion(int x, int y, Uint8 state);

	/* Clickable or not ... */
	virtual void Disable();
	virtual void Enable(int flag = 1);

	/* yields current state */
	virtual int Enabled()
	  {return enabled;}

	/* yields flag if button is a checkbutton */
	virtual int IsCheckButton()
	  {return is_checkable;}

protected:
	/* yields an appropriate image */
	virtual SDL_Surface* CreateTextButtonImage(int style, char *text, int alignment);

	/* The button font */
	GUI_Font *buttonFont;

	/* The button images */
	SDL_Surface *button,*button2;

	/* The activation callback */
	GUI_ActiveProc ActiveProc;
	
	/* remember me! - flags */
	int enabled;
	int flatbutton;
	int freebutton,freefont;

	/* Checkbutton flags */
	int is_checkable;
	int checked;
};

#endif /* _GUI_button_h */
