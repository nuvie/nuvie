#ifndef __MsgScroll_h__
#define __MsgScroll_h__

/*
 *  MsgScroll.h
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

#include "Text.h"


class MsgScroll
{
 Text *text;
 bool keyword_highlight;
 bool input_mode;
 
 public:
 
 MsgScroll(Text *txt);
 ~MsgScroll();
 
 void display_string(char *string, uint16 length);
 void set_keyword_highlight(bool state);
 
 void set_input_mode(bool state);
 
 char *get_input();
 
 void updateScroll();
 
};


#endif /* __MsgScroll_h__ */

