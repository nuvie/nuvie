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

MsgScroll::MsgScroll(Configuration *cfg)
{
 config = cfg;
 
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
 
 if(string == NULL)
   {
    display_string(prompt);
    return;
   }

 length = strlen(string);
 
 //if(msg_buf[buf_pos][0] != '\0')
 row_length = strlen(msg_buf[buf_pos]);
 
 for(i=0,j=0,word_start=0;i <= length;i++,row_length++)
   {
    if(string[i] == '\n' || string[i] == '*' || string[i] == ' ' || string[i] == '\0')
      {
       if(row_length > MSGSCROLL_WIDTH)
         {
          buf_addString(&string[j],word_start - j);
          j = word_start;
          buf_next();
        //  msg_buf[buf_pos][0] = '\0';
          row_length = i - j;
         }

      
       if(string[i] != ' ')
        {
         buf_addString(&string[j],i - j);
         if(string[i] != '\0')
          {
           buf_next();
          }
         row_length = 0;
         j = i+1;
        }
       else
        word_start = i+1;
      }
   }
   
 scroll_updated = true;
}

void MsgScroll::set_keyword_highlight(bool state)
{
 keyword_highlight = state;
}
 
void MsgScroll::set_input_mode(bool state)
{
 input_mode = state;
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

char *MsgScroll::get_input()
{
 return NULL;
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
  text->drawChar(screen, cursor_char + 5, x, y);
  
  screen->update(x, y, 8, 8);
  if(cursor_wait == 6)
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

