/*
 *  MsgScroll.cpp
 *  Nuvie
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
#include <string.h>
#include <ctype.h>

#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "Text.h"

#include "MsgScroll.h"

MsgScroll::MsgScroll(Configuration *cfg) : GUI_Widget(NULL, 0, 0, 0, 0)
{
 uint16 i;
 
 config = cfg;
 config->value("config/GameType",game_type);

 prompt_buf_len = 0;

 switch(game_type)
   {
    case NUVIE_GAME_U6 : scroll_width = MSGSCROLL_U6_WIDTH;
                         scroll_height = MSGSCROLL_U6_HEIGHT;
                         area.x = 176;
                         area.y = 112;
                         break;
                         
    case NUVIE_GAME_MD : scroll_width = MSGSCROLL_MD_WIDTH;
                         scroll_height = MSGSCROLL_MD_HEIGHT;
                         area.x = 184;
                         area.y = 128;
                         break;
                         
    case NUVIE_GAME_SE : scroll_width = MSGSCROLL_SE_WIDTH;
                         scroll_height = MSGSCROLL_SE_HEIGHT;
                         area.x = 184;
                         area.y = 128;
                         break;
   }

 area.w = scroll_width * 8;
 area.h = scroll_height * 8;
 
 msg_buf = (char **)malloc(scroll_height * sizeof(char *));
 for(i=0;i<scroll_height;i++)
  {
   msg_buf[i] = (char *)malloc(scroll_width+1);
   memset(msg_buf[i],0,scroll_width+1);
  }
 
 msg_buf_languages = (uint8 *)malloc(sizeof(uint8) * scroll_height);
 memset(msg_buf_languages,0,scroll_height * sizeof(uint8));
 
 prompt = NULL;
 
 buf_pos = 0;
 start_pos = 0;
 buf_full = false;
 
 cursor_char = 0;
 cursor_x = 0;
 cursor_y = scroll_height-1;
 
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
 uint16 i;
 
 for(i=0;i<scroll_height;i++)
  free(msg_buf[i]);
 free(msg_buf); 
 free(msg_buf_languages);
 
 free(string_buf);
 free(prompt);
}

bool MsgScroll::init(Text *txt, char *player_name)
{
 std::string prompt_string;
 text = txt;
 
 prompt_string.append(player_name);
 prompt_string.append(":\n>");
 
 if(set_prompt((char *)prompt_string.c_str()) == false)
   return false;
 
 set_input_mode(false);
 
 return true;
}
 
void MsgScroll::display_string(const char *string, uint8 lang_num)
{
 if(string)
   display_string(string, strlen(string), lang_num);
 else
   display_string(NULL, 0, lang_num);
}

void MsgScroll::display_string(const char *string, uint16 string_len, uint8 lang_num)
{
 uint16 string_buf_length;
 uint16 i;
 uint16 word_start, row_start;
 sint16 row_length;
 uint16 num_rows;

    
 if(string_buf_pos == 0)
   {
    if(string == NULL)
     {
      return;
     }

    if(set_string_buf(string, string_len) == false)
       return;
   }
 else
   {
    if(string != NULL)
      {
       set_string_buf_append(string, string_len);
       return;
      }
   }

 string_buf_length = strlen(string_buf);
  
 //if(msg_buf[buf_pos][0] != '\0')
 row_length = strlen(msg_buf[buf_pos]);
 
 i = string_buf_pos;
 string_buf_pos = 0;
 
 for(row_start = i, num_rows = 0, word_start = i; i <= string_buf_length; i++, row_length++)
   {
    if(string_buf[i] == '\n' || string_buf[i] == '*' || string_buf[i] == ' ' || string_buf[i] == '\0')
      {
       if(row_length > scroll_width)
         {
          buf_addString(&string_buf[row_start],word_start - row_start,lang_num);
          row_start = word_start;
          buf_next();
          num_rows++;
        //  msg_buf[buf_pos][0] = '\0';
          row_length = i - row_start;
         }

      
       if(string_buf[i] != ' ')
        {
         buf_addString(&string_buf[row_start],i - row_start,lang_num);
         if(string_buf[i] == '\n' || row_length == scroll_width)
          {
           buf_next();
           num_rows++;
          }

         row_length = -1; // this will increment back to 0 which is what we really want.
         row_start = i+1;
        }
       else
        word_start = i+1;
        // == 
      if(num_rows >= scroll_height-2 || string_buf[i] == '*')
        {
         set_page_break(row_start);
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
 
void MsgScroll::set_input_mode(bool state, const char *allowed, bool can_escape)
{
 input_mode = state;
 permit_input = NULL;
 permit_inputescape = can_escape;

 if(input_mode == true)
 {
   input_buf_pos = 0;
   if(allowed && strlen(allowed))
     permit_input = allowed;
   SDL_EnableUNICODE(1); // allow character translation
 }
 else
   SDL_EnableUNICODE(0); // reduce translation overhead when not needed
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
bool MsgScroll::handle_input(const SDL_keysym *input)
{
    char ascii = 0;
    if(page_break == false && input_mode == false)
        return(false);
    if(page_break)
      {
       page_break = false;
       display_string(NULL);
       return(true);
      }
    switch(input->sym)
    {
        case SDLK_ESCAPE: if(permit_inputescape)
                          {
                            // reset input buffer
                            permit_input = NULL;
                            input_buf_pos = 0;
                            if(input_mode)
                              set_input_mode(false);
                          }
                          return(true);
		case SDLK_KP_ENTER:
        case SDLK_RETURN: if(permit_inputescape)
                          {
                            if(input_mode)
                              set_input_mode(false);
                          }
                          return(true);
        case SDLK_BACKSPACE :
                            if(input_mode)
                              input_buf_remove_char();
                            break;
        default: // alphanumeric characters
                 if((input->unicode & 0xFF80) == 0) // high 9bits 0 == ascii code
                   ascii = (char)(input->unicode & 0x7F); // (in low 7bits)
                 if(input_mode && isprint(ascii))
                  {
                   if(permit_input == NULL)
                    input_buf_add_char(ascii);
                   else if(strchr(permit_input, ascii) || strchr(permit_input, tolower(ascii)))
                   {
                    input_buf_add_char(toupper(ascii));
                    set_input_mode(false);
                   }
                  }
            break;
    }
    
 return(true);
}


void MsgScroll::Display(bool full_redraw)
{
 uint16 i,j;
 
 clearCursor(area.x + 8 * cursor_x, area.y + cursor_y * 8);

 if(scroll_updated || full_redraw)
  {
   if(buf_full == true)
     {
      j = (buf_pos + 1) % scroll_height;      
     }
   else
      j = 0;

   if(buf_full || full_redraw)
     screen->fill(0x31,area.x, area.y, scroll_width * 8, (scroll_height)*8); //clear whole scroll
     
   for(i=0;i< scroll_height;i++)
     {
      if(msg_buf[j][0] != '\0')
        {
         text->drawString(screen, msg_buf[j], area.x, area.y+i*8, msg_buf_languages[j]);         
        }
      j = (j + 1) % scroll_height;
     }
   scroll_updated = false;
   
   screen->update(area.x,area.y, scroll_width * 8, (scroll_height)*8);
   
   if(buf_full)
    cursor_y = scroll_height-1;
   else
    cursor_y = buf_pos;
  
   cursor_x = strlen(msg_buf[buf_pos]);
  }
else
 {
  
  drawCursor(area.x + 8 * cursor_x, area.y + cursor_y * 8); 
 }
 
 // spin cursor here.
}

void MsgScroll::clearCursor(uint16 x, uint16 y)
{
 screen->fill(0x31, x, y, 8,8);
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

bool MsgScroll::buf_addString(char *string, uint8 length, uint8 lang_num)
{
 uint16 buf_len;
 
 buf_len = strlen(msg_buf[buf_pos]);
 strncpy(&msg_buf[buf_pos][buf_len], string, length);
 msg_buf[buf_pos][buf_len+length] = '\0';

 msg_buf_languages[buf_pos] = lang_num; //set the language for this line.
 
 //buf_pos++;

 return true;
}

bool MsgScroll::buf_next()
{
 if(buf_pos == scroll_height-1)
   {
    buf_full = true;
    buf_pos = 0;
   }
 else
   buf_pos++;

 msg_buf[buf_pos][0] = '\0';
 
 msg_buf_languages[buf_pos] = 0; //clear the language for this line.
 
 return true;
}

bool MsgScroll::buf_prev()
{
 if(buf_pos == 0)
  {
   buf_pos = scroll_height-1;
  }
 else
  {
   buf_pos--;
  }
  
 return true;
}

bool MsgScroll::set_string_buf(const char *string, uint16 len)
{
 
 if(string_buf_len < len+1)
   {
    string_buf = (char *)realloc(string_buf,len+1);
    string_buf_len = len+1;
   }

 strncpy(string_buf,string,len);
 string_buf[len] = '\0';
 
 return true;
}

bool MsgScroll::set_string_buf_append(const char *string, uint16 len)
{
 uint16 new_len;
 
 if(string == NULL)
   return false;

 new_len = len + strlen(string_buf);
 
 if(string_buf_len < new_len+1)
   {
    string_buf = (char *)realloc(string_buf, new_len+1);
    string_buf_len = new_len+1;
   }

 strncat(string_buf,string,len);
 
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
    input_buf_len += 16;
   }
 
 input_buf[input_buf_pos] = c;
 
 input_buf_pos++;
 input_buf[input_buf_pos] = '\0';

 buf_len = strlen(msg_buf[buf_pos]);
 
 if(buf_len + 1 >= scroll_width)
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


char *MsgScroll::get_input()
{
 // MsgScroll sets input_mode to false when it receives SDLK_ENTER 
 if(input_mode == false)
   {
    if(input_buf_pos > 0)
      return input_buf;
    else
      return "";
   }
  
 return NULL;
}

char *MsgScroll::peek_at_input()
{
 if(input_buf_pos > 0)
   return input_buf;
 return NULL;
}
 
