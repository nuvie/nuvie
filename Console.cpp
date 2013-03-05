/*
 *  Console.cpp
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

#include <assert.h>
#include <cmath>
#include <string>
#include "nuvieDefs.h"
#include "U6misc.h"
#include "GUI.h"
#include "Console.h"

static Console *g_console = NULL;

Console::Console(Configuration *c, Screen *s, GUI *g, uint16 x, uint16 y, uint16 w, uint16 h)
: GUI_Console(x, y, w, h)
{
	config = c;
	screen = s;
	gui = g;
	displayConsole = true;

	config->value("config/general/show_console", displayConsole, true);

	if(displayConsole == false)
		Hide();

	gui->AddWidget(this);
}

Console::~Console()
{

}

void Console::AddLine(std::string line)
{
	GUI_Console::AddLine(line);

	if(status == WIDGET_VISIBLE)
	{
		gui->Display();
		screen->preformUpdate();
	}
}

void ConsoleInit(Configuration *c, Screen *s, GUI *gui, uint16 w, uint16 h)
{
	assert(g_console == NULL);

	//uint16 x_off = config_get_video_x_offset(c);
	//uint16 y_off = config_get_video_y_offset(c);

	g_console = new Console(c, s, gui, 0, 0, s->get_width(), s->get_height());//x_off, y_off, w, h);
}

void ConsoleDelete()
{
	if(g_console != NULL)
	{
		g_console->Delete();
		g_console = NULL;
	}
}

void ConsoleAddInfo(const char *format, ...)
{
	char buf[1024];
	memset(buf, 0, 1024);
	va_list args;
	va_start(args, format);
	vsnprintf(buf, 1024, format, args);
	va_end(args);

	if(g_console != NULL)
	{
		DEBUG(0,LEVEL_INFORMATIONAL, buf);
		g_console->AddLine(buf);
	}
}

void ConsoleAddError(std::string s)
{
	if(g_console != NULL)
	{
		DEBUG(0,LEVEL_ERROR, s.c_str());
		g_console->Show();
		g_console->AddLine("Error: " + s);
	}
}

void ConsolePause()
{
	if(g_console == NULL)
		return;

	//pause here.
	 SDL_Event event;
	 bool waiting=true;
	 for(;waiting;)
	 {
		 while(!SDL_PollEvent(&event))
		 {
			 if(event.type == SDL_KEYDOWN || event.type == SDL_QUIT)
			 {
				 waiting = false;
			 	 break;
			 }
		 }
	 }
}

void ConsoleShow()
{
	if(g_console)
		g_console->Show();
}

void ConsoleHide()
{
	if(g_console)
		g_console->Hide();
}
