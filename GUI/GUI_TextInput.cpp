/*
 *  GUI_TextInput.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Jun 26 2004.
 *  Copyright (c) Nuvie Team 2004. All rights reserved.
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

#include "nuvieDefs.h"

#include "GUI_TextInput.h"
#include "GUI_font.h"

GUI_TextInput:: GUI_TextInput(int x, int y, Uint8 r, Uint8 g, Uint8 b, char *str,
                              GUI_Font *gui_font, uint16 width, uint16 height, GUI_CallBack *callback)
 : GUI_Text(x, y, r, g, b, gui_font, width)
{
 max_height = height;
 callback_object = callback;
 
 text = (char *)malloc(max_width * max_height + 1);

 if(text == NULL)
  {
   printf("Error: GUI_TextInput: allocating memory for text\n");
   return;
  }

 strncpy(text, str, max_width * max_height);
 
 pos = strlen(text);
 length = pos;
 
 area.w = max_width * font->CharWidth();
 area.h = max_height * font->CharHeight();
}

GUI_TextInput::~GUI_TextInput()
{
 return;
}

GUI_status GUI_TextInput::MouseUp(int x, int y, int button)
{
 //release focus if we click outside the text box.
 if(focused && !HitRect(x, y))
   release_focus();
 else 
  {
   if(!focused)  
     grab_focus();
  }
  
 return(GUI_PASS);
}

GUI_status GUI_TextInput::KeyDown(SDL_keysym key)
{
 if(!focused)
   return GUI_PASS;

 switch(key.sym)
   {
    case SDLK_RETURN : if(callback_object)
                         callback_object->callback(TEXTINPUT_CB_TEXT_READY, this, text);
    case SDLK_ESCAPE : release_focus(); break;
    
    case SDLK_KP4  : 
    case SDLK_LEFT : if(pos > 0)
                       pos--;
                     break;
    
    case SDLK_KP6   : 
    case SDLK_RIGHT : if(pos < length)
                       pos++;
                      break;

    case SDLK_BACKSPACE : remove_char(); break;
    
    default : add_char((char)key.sym); break;
   }
   

 
 return(GUI_YUM);
}

void GUI_TextInput::add_char(char c)
{
 uint16 i;
 
 if(length+1 > max_width * max_height)
   return;

 if(pos < length) //shuffle chars to the right if required.
  {
   for(i=length; i > pos; i--)
     text[i] = text[i-1];
  }

 length++;

 text[pos] = c;
 pos++;
  
 text[length] = '\0';

 return;
}

void GUI_TextInput::remove_char()
{
 uint16 i;
 
 if(pos == 0)
  return;

 for(i=pos-1;i<length;i++)
  text[i] = text[i+1];

 pos--;
 length--;
 
 return;
}

/* Map the color to the display */
void GUI_TextInput::SetDisplay(Screen *s)
{
	GUI_Widget::SetDisplay(s);
	cursor_color = SDL_MapRGB(surface->format, 0xff, 0, 0);
}


/* Show the widget  */
void GUI_TextInput:: Display(bool full_redraw)
{
 GUI_Text::Display(full_redraw);
 
 if(focused)
  display_cursor();

}

void GUI_TextInput::display_cursor()
{
 SDL_Rect r;
 uint16 x, y;
 uint16 cw, ch;
 
 x = pos % max_width;
 y = pos / max_width;
 
 cw = font->CharWidth();
 ch = font->CharHeight();
 
 r.x = area.x + x * cw;
 r.y = area.y + y * ch;
 r.w = 1;
 r.h = ch;
 
 SDL_FillRect(surface, &r, cursor_color);
 
 return;
}
