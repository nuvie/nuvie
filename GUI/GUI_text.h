#ifndef __GUI_text_h__
#define __GUI_text_h__

/*
 *  GUI_text.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thr Aug 14 2003.
 *  Copyright (c) Nuvie Team 2003. All rights reserved.
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

#include "GUI_widget.h"
#include "GUI_font.h"

class GUI_Text : public GUI_Widget 
{
protected:
	Uint8 R, G, B;
	char *text;
	GUI_Font *font;

public:

GUI_Text(int x, int y, Uint8 r, Uint8 g, Uint8 b, char *str, GUI_Font *gui_font);
~GUI_Text();

	/* Show the widget  */
	virtual void Display(bool full_redraw);

};

#endif /* __GUI_text_h__ */

