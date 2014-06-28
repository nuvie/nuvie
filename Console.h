#ifndef __CONSOLE_H__
#define __CONSOLE_H__
/*
 *  Console.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Apr 5 2011.
 *  Copyright (c) 2011. All rights reserved.
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

#include <stdarg.h>

#include "SDL.h"

#include "GUI_Console.h"
#include "Screen.h"
#include "Configuration.h"

class Console : public GUI_Console {

protected:
	GUI *gui;
	Screen *screen;
	Configuration *config;

	bool displayConsole;

public:
	Console(Configuration *c, Screen *s, GUI *g, uint16 x, uint16 y, uint16 w, uint16 h);
	~Console();

	void AddLine(std::string line);

protected:

};

void ConsoleInit(Configuration *c, Screen *s, GUI *gui, uint16 w, uint16 h);
void ConsoleDelete();
void ConsoleAddInfo(const char *s, ...);
void ConsoleAddError(std::string s);
void ConsoleAddWarning(std::string s);
void ConsolePause();
void ConsoleShow();
void ConsoleHide();

#endif /* __CONSOLE_H__ */
