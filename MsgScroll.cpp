/*
 *  MsgScroll.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
 *  Copyright (c) 2003. All rights reserved.
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

#include "MsgScroll.h"

MsgScroll::MsgScroll(Text *txt)
{
 text = txt;
 
}

MsgScroll::~MsgScroll()
{
}

void MsgScroll::display_string(char *string, uint16 length)
{
}

void MsgScroll::set_keyword_highlight(bool state)
{
 keyword_highlight = state;
}
 
void MsgScroll::set_input_mode(bool state)
{
 input_mode = state;
}

char *MsgScroll::get_input()
{
 return NULL;
}

void MsgScroll::updateScroll()
{
 // spin cursor here.
}
 

