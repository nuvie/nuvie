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

#include <ctype.h>

#include "MsgScroll.h"

MsgScroll::MsgScroll(Configuration *cfg)
{
 config = cfg;
 converse = NULL; 
 prompt_buf_len = 0;

 memset(msg_buf,0,sizeof(msg_buf));

 prompt = NULL;
 
 buf_pos = 0;
 start_pos = 0;
 buf_full = false;
 
 cursor_char = 0;
 cursor_x = 0;
 cursor_y = MSGSCROLL_HEIGHT-1;
 
 cursor_wait = 0;
 
 scroll_updated = false;
 
 string_buf = NULL;
 string_buf_len = 0;
 string_buf_pos = 0;
 
 input_buf = NULL;
 input_buf_len = 0;
 input_buf_pos = 0;
 
 page_break = false;
}

MsgScroll::~MsgScroll()
{
}

bool MsgScroll::init(Screen *s, Text *txt)
{
 screen = s;
 text = txt;
 
 if(set_prompt("Avatar\n>") == false)
   return false;
 
 set_input_mode(false);
 
 return true;
}

void MsgScroll::display_string(char *string)
{
 uint16 length;
 uint16 i,j;
 uint16 word_start, row_length;
 uint16 num_rows;

    
 if(string_buf_pos == 0)
   {
    if(string == NULL)
     {
      return;
     }

    if(set_string_buf(string) == false)
       return;
   }
 else
   {
    if(string != NULL)
      {
       set_string_buf_append(string);
       return;
      }
   }

 length = strlen(string_buf);
 
 //if(msg_buf[buf_pos][0] != '\0')
 row_length = strlen(msg_buf[buf_pos]);
 
 i = string_buf_pos;
 string_buf_pos = 0;
 
 for(j=i,num_rows=0,word_start=i;i <= length;i++,row_length++)
   {
    if(string_buf[i] == '\n' || string_buf[i] == '*' || string_buf[i] == ' ' || string_buf[i] == '\0')
      {
       if(row_length >= MSGSCROLL_WIDTH)
         {
          buf_addString(&string_buf[j],word_start - j);
          j = word_start;
          buf_next();
          num_rows++;
        //  msg_buf[buf_pos][0] = '\0';
          row_length = i - j;
         }

      
       if(string_buf[i] != ' ')
        {
         buf_addString(&string_buf[j],i - j);
         if(string_buf[i] != '\0')
          {
           buf_next();
           num_rows++;
          }

         row_length = 0;
         j = i+1;
        }
       else
        word_start = i+1;
      if(num_rows == MSGSCROLL_HEIGHT-2 || string_buf[i] == '*')
        {
         set_page_break(j);
         break;
        }

      }
   }

 scroll_updated = true;
}


void MsgScroll::display_prompt()
{
 if(prompt != NULL)
    display_string(prompt);
}
 
void MsgScroll::set_keyword_highlight(bool state)
{
 keyword_highlight = state;
}
 
void MsgScroll::set_input_mode(bool state)
{
 input_mode = state;

 if(input_mode == true)
   input_buf_pos = 0;
}

bool MsgScroll::set_prompt(char *new_prompt)
{
 uint16 new_len;
 
 new_len = strlen(new_prompt);
 
 if(prompt_buf_len < new_len+1)
  {
   prompt = (char *)realloc(prompt,new_len+1);
   if(prompt == NULL)
     return false;
   prompt_buf_len = new_len+1;
  }

 strcpy(prompt,new_prompt);
 
 return true;
}


/* Take input from the main event handler and do something with it
 * if necessary.
 * Returns true if the input was used and false if not.
 */
bool MsgScroll::handle_input(SDLKey *input)
{
    if(page_break == false && input_mode == false)
        return(false);
        
    switch(*input)
    {
        case SDLK_ESCAPE:
        case SDLK_RETURN: if(page_break)
                            {
                             page_break = false;
                             display_string(NULL);
                            }
                          
                          if(input_mode)
                            set_input_mode(false);
                            
                          return(true);
        case SDLK_BACKSPACE :
                            if(input_mode)
                              input_buf_remove_char();
                            break;
        default: // alphanumeric characters
                 if(input_mode && isascii(*input)) //(isalnum(*input) || *input == ' '))
                  {
                   input_buf_add_char((char)*input);
                  }
            break;
    }
    
 return(true);
}


void MsgScroll::updateScroll()
{
 uint16 i,j;

 clearCursor(176 + 8 * cursor_x, 112 + cursor_y * 8);

 if(scroll_updated)
  {
   if(buf_full == true)
     {
      j = (buf_pos + 1) % MSGSCROLL_HEIGHT;
      screen->fill(0x31,176, 112, MSGSCROLL_WIDTH * 8, (MSGSCROLL_HEIGHT)*8);
     }
   else
      j = 0;
         
   for(i=0;i< MSGSCROLL_HEIGHT;i++)
     {
      if(msg_buf[j][0] != '\0')
         text->drawString(screen, msg_buf[j], 176, 112+i*8, 0);
      j = (j + 1) % MSGSCROLL_HEIGHT;
     }
   scroll_updated = false;
   
   screen->update(176,112, MSGSCROLL_WIDTH * 8, (MSGSCROLL_HEIGHT)*8);
  }
else
 {
  if(buf_full)
   cursor_y = MSGSCROLL_HEIGHT-1;
  else
   cursor_y = buf_pos;
  
  cursor_x = strlen(msg_buf[buf_pos]);
  
  drawCursor(176 + 8 * cursor_x, 112 + cursor_y * 8); 
 }
 
 // spin cursor here.
}

void MsgScroll::clearCursor(uint16 x, uint16 y)
{
 screen->fill(0x31, x, y, 8, 8);
}

void MsgScroll::drawCursor(uint16 x, uint16 y)
{
 if(page_break)
    text->drawChar(screen, 1, x, y); // down arrow
 else
    text->drawChar(screen, cursor_char + 5, x, y); //spinning ankh
  
  screen->update(x, y, 8, 8);
  if(cursor_wait == MSGSCROLL_CURSOR_DELAY)
    {
     cursor_char = (cursor_char + 1) % 4;
     cursor_wait = 0;
    }
  else 
     cursor_wait++;
}

bool MsgScroll::buf_addString(char *string, uint8 length)
{
 uint16 buf_len;
 
 buf_len = strlen(msg_buf[buf_pos]);
 strncpy(&msg_buf[buf_pos][buf_len], string, length);
 msg_buf[buf_pos][buf_len+length] = '\0';

 //buf_pos++;

 return true;
}

bool MsgScroll::buf_next()
{
 if(buf_pos == MSGSCROLL_HEIGHT-1)
   {
    buf_full = true;
    buf_pos = 0;
   }
 else
   buf_pos++;

 msg_buf[buf_pos][0] = '\0';
 
 return true;
}

bool MsgScroll::buf_prev()
{
 if(buf_pos == 0)
  {
   buf_pos = MSGSCROLL_HEIGHT-1;
  }
 else
  {
   buf_pos--;
  }
  
 return true;
}

bool MsgScroll::set_string_buf(char *string)
{
 uint16 len;
 
 len = strlen(string);
 
 if(string_buf_len < len+1)
   {
    string_buf = (char *)realloc(string_buf,len+1);
    string_buf_len = len+1;
   }

 strcpy(string_buf,string);
 
 return true;
}

bool MsgScroll::set_string_buf_append(char *string)
{
 uint16 len;
 
 if(string == NULL)
   return false;

 len = strlen(string);
 len += strlen(string_buf);
 
 if(string_buf_len < len+1)
   {
    string_buf = (char *)realloc(string_buf, len+1);
    string_buf_len = len+1;
   }

 strcat(string_buf,string);
 
 return true;
}

bool MsgScroll::set_string_buf_pos(uint16 pos)
{
 if(pos < string_buf_len)
 { 
  string_buf_pos = pos;
  return true;
 }
 
 return false;
}

void MsgScroll::set_page_break(uint16 pos)
{
 if(set_string_buf_pos(pos) == false)
  return;

 page_break = true;

 return;
}

bool MsgScroll::input_buf_add_char(char c)
{
 uint16 buf_len;
 
 if(input_buf_len < input_buf_pos + 2)
   {
    input_buf = (char *)realloc(input_buf,input_buf_len + 16);
    if(input_buf == NULL)
      return false;
   }
 
 input_buf[input_buf_pos] = c;
 
 input_buf_pos++;
 input_buf[input_buf_pos] = '\0';

 buf_len = strlen(msg_buf[buf_pos]);
 
 if(buf_len + 1 >= MSGSCROLL_WIDTH)
   {
    buf_next();
    buf_len = 0;
   }

 msg_buf[buf_pos][buf_len] = c;
 msg_buf[buf_pos][buf_len+1] = '\0';

 scroll_updated = true;
 
 return true;
}

bool MsgScroll::input_buf_remove_char()
{
 uint16 buf_len;
 
 if(input_buf_pos == 0)
  return false;

 input_buf_pos--;
 input_buf[input_buf_pos] = '\0';
 
 buf_len = strlen(msg_buf[buf_pos]);
 
 if(buf_len == 0)
   {
    buf_prev();
    buf_len = strlen(msg_buf[buf_pos]);
   }

 msg_buf[buf_pos][buf_len-1] = '\0';

 scroll_updated = true;
 
 return true;
}
