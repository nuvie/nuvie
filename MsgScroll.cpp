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
#include <string>
#include <ctype.h>

#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "FontManager.h"
#include "Font.h"
#include "GUI.h"
#include "MsgScroll.h"

// MsgText Class
MsgText::MsgText()
{
 font = NULL;
}

MsgText::MsgText(std::string new_string, Font *f)
{
 s.assign(new_string);
 font = f;
}

MsgText::~MsgText()
{
}

void MsgText::append(std::string new_string)
{
 s.append(new_string);
}

void MsgText::copy(MsgText *msg_text)
{
 s.assign(msg_text->s);
 font = msg_text->font;
}

uint32 MsgText::length()
{
 return (uint32)s.length();
}

MsgLine::~MsgLine()
{
 std::list<MsgText *>::iterator iter;

 for(iter=text.begin(); iter != text.end(); iter++)
  {
   delete *iter;
  }
}

void MsgLine::append(MsgText *new_text)
{
 MsgText *msg_text = NULL;

 if(text.size() > 0)
    msg_text = text.back();

 if(msg_text && msg_text->font == new_text->font && new_text->s.length() == 1 && new_text->s[0] != ' ')
   msg_text->s.append(new_text->s);
 else
  {
   msg_text = new MsgText();
   msg_text->copy(new_text);
   text.push_back(msg_text);
  }

 total_length += new_text->s.length();

 return;
}

// remove the last char in the line.
void MsgLine::remove_char()
{
 MsgText *msg_text;

 if(total_length == 0)
   return;

 msg_text = text.back();
 msg_text->s.erase(msg_text->s.length() - 1, 1);

 if(msg_text->s.length() == 0)
   {
    text.pop_back();
    delete msg_text;
   }

 total_length--;

 return;
}

uint32 MsgLine::length()
{
 return total_length;
}

// gets the MsgText object that contains the text at line position pos
MsgText *MsgLine::get_text_at_pos(uint16 pos)
{
 uint16 i;
 std::list<MsgText *>::iterator iter;

 if(pos > total_length)
   return NULL;

 for(i=0,iter=text.begin(); iter != text.end(); iter++)
  {
   if(i + (*iter)->s.length() >= pos)
     return (*iter);

   i += (*iter)->s.length();
  }

 return NULL;
}

// MsgScroll Class

MsgScroll::MsgScroll(Configuration *cfg, Font *f) : GUI_Widget(NULL, 0, 0, 0, 0)
{
 uint16 x, y;

 font = f;

 config = cfg;
 config->value("config/GameType",game_type);

 callback_target = NULL;
 callback_user_data = NULL;

 switch(game_type)
   {
    case NUVIE_GAME_MD : scroll_width = MSGSCROLL_MD_WIDTH;
                         scroll_height = MSGSCROLL_MD_HEIGHT;
                         x = 184;
                         y = 128;
                         break;

    case NUVIE_GAME_SE : scroll_width = MSGSCROLL_SE_WIDTH;
                         scroll_height = MSGSCROLL_SE_HEIGHT;
                         x = 184;
                         y = 128;
                         break;
    case NUVIE_GAME_U6 :
	default :
						 scroll_width = MSGSCROLL_U6_WIDTH;
                         scroll_height = MSGSCROLL_U6_HEIGHT;
                         x = 176;
                         y = 112;
                         break;
   }

 GUI_Widget::Init(NULL, x, y, scroll_width * 8, scroll_height * 8);

 cursor_char = 0;
 cursor_x = 0;
 cursor_y = scroll_height-1;
 line_count = 0;

 cursor_wait = 0;

 scroll_updated = false;

 page_break = false;
 show_cursor = true;
 talking = false;

 add_new_line();
 display_pos = 0;
}

MsgScroll::~MsgScroll()
{
 std::list<MsgLine *>::iterator msg_line;
 std::list<MsgText *>::iterator msg_text;

 // delete the scroll buffer
 for(msg_line = msg_buf.begin(); msg_line != msg_buf.end(); msg_line++)
   delete *msg_line;

 // delete the holding buffer
 for(msg_text = holding_buffer.begin(); msg_text != holding_buffer.end(); msg_text++)
   delete *msg_text;

}

bool MsgScroll::init(char *player_name)
{
 std::string prompt_string;

 prompt_string.append(player_name);
 prompt_string.append(":\n>");

 if(set_prompt((char *)prompt_string.c_str(),font) == false)
   return false;

 set_input_mode(false);

 return true;
}

void MsgScroll::display_string(std::string s, uint16 length, uint8 lang_num)
{

}

void MsgScroll::display_string(std::string s, uint8 lang_num)
{
 display_string(s,font);
}

void MsgScroll::display_string(std::string s, Font *f)
{
 MsgText *msg_text;

 if(s.empty())
   return;

 if(f == NULL)
   f = font;

 msg_text = new MsgText(s, f);

 holding_buffer.push_back(msg_text);

 process_holding_buffer();

}

// process text tokens till we either run out or hit a page break.

 void MsgScroll::process_holding_buffer()
 {
  MsgText *token;

  if(!page_break)
    {
     token = holding_buffer_get_token();

     for( ; token != NULL && !page_break; )
       {
        add_token(token);
        delete token;
        scroll_updated = true;

        if(!page_break)
          token = holding_buffer_get_token();
       }
    }
 }

 MsgText *MsgScroll::holding_buffer_get_token()
 {
  MsgText *input, *token;
  int i;

  if(holding_buffer.empty())
    return NULL;

  input = holding_buffer.front();

  if(input->font == NULL)
    {
     line_count = 0;
     holding_buffer.pop_front();
     delete input;
     return NULL;
    }

  i = input->s.find_first_of(" \t\n*<>",0);
  if(i == 0) i++;

  if(i == -1)
    i = input->s.length();

  if(i > 0)
   {
    token = new MsgText(input->s.substr(0,i), font); // FIX maybe a format flag. // input->font);
	input->s.erase(0,i);
	if(input->s.length() == 0)
	  {
	   holding_buffer.pop_front();
	   delete input;
	  }
	return token;
   }

  return NULL;
 }

bool MsgScroll::add_token(MsgText *token)
{
 MsgLine *msg_line;

 msg_line = msg_buf.back(); // retrieve the last line from the scroll buffer.

 switch(token->s[0])
   {
    case '\n' :  add_new_line();
                 break;

    case '<'  :  font = Game::get_game()->get_font_manager()->get_font(1); // runic / gargoyle font
                 break;

    case '>'  :  if(font == Game::get_game()->get_font_manager()->get_font(1))
                    {
                     font = Game::get_game()->get_font_manager()->get_font(0); // english font
                     break;
                    }
                  // Note fall through. ;-) We fall through if we haven't already seen a '<' char

    default   :  if(msg_line->total_length + token->length() > scroll_width) // the token is to big for the current line
                   {
                    msg_line = add_new_line();
                   }

                 if(msg_line->total_length + token->length() == scroll_width) //we add a new line but write to the old line.
                    add_new_line();

                 if(msg_line->total_length == 0 && token->s[0] == ' ') // discard whitespace at the start of a line.
                    return true;

                 if(token->s[0] == '*')
                    set_page_break();
                 else
			        msg_line->append(token);
                 break;
   }

if(msg_buf.size() > scroll_height)
   display_pos = msg_buf.size() - scroll_height;

 return true;
}

bool MsgScroll::remove_char()
{
 MsgLine *msg_line;

 msg_line = msg_buf.back(); // retrieve the last line from the scroll buffer.
 msg_line->remove_char();

 if(msg_line->total_length == 0) // remove empty line from scroll buffer
   {
    msg_buf.pop_back();
    delete msg_line;
   }

 return true;
}

inline MsgLine *MsgScroll::add_new_line()
{
 MsgLine *msg_line = new MsgLine();
 msg_buf.push_back(msg_line);
 line_count++;

 if(msg_buf.size() > MSGSCROLL_SCROLLBACK_HEIGHT)
   msg_buf.pop_front();

 if(line_count > scroll_height - 1)
     set_page_break();

 return msg_line;
}

bool MsgScroll::set_prompt(const char *new_prompt, Font *f)
{

 prompt.s.assign(new_prompt);
 prompt.font = f;

 return true;
}

void MsgScroll::display_prompt()
{
 if(!talking)
  { 
 //line_count = 0;
   display_string(prompt.s.c_str(), prompt.font);
 //line_count = 0;

   clear_page_break();
  }
}

void MsgScroll::set_keyword_highlight(bool state)
{
 keyword_highlight = state;
}

void MsgScroll::set_input_mode(bool state, const char *allowed, bool can_escape)
{
 bool do_callback = false;

 input_mode = state;
 permit_input = NULL;
 permit_inputescape = can_escape;

 line_count = 0;

 clear_page_break();

 if(input_mode == true)
 {
   if(allowed && strlen(allowed))
     permit_input = allowed;
   SDL_EnableUNICODE(1); // allow character translation
   input_buf.erase(0,input_buf.length());
 }
 else
 {
   SDL_EnableUNICODE(0); // reduce translation overhead when not needed
   if(callback_target)
     do_callback = true; // **DELAY until end-of-method so callback can set_input_mode() again**
 }
 Game::get_game()->get_gui()->lock_input(input_mode ? this : NULL);

 // send whatever input was collected to target that requested it
 if(do_callback)
 {
   CallBack *requestor = callback_target; // copy to temp
   char *user_data = callback_user_data;
   cancel_input_request(); // clear originals (callback may request again)

   std::string input_str = input_buf;
   requestor->set_user_data(user_data); // use temp requestor/user_data
   requestor->callback(MSGSCROLL_CB_TEXT_READY, this, &input_str);
 }
}

/* Take input from the main event handler and do something with it
 * if necessary.
 */
GUI_status MsgScroll::KeyDown(SDL_keysym key)
{
    char ascii = 0;

    if(page_break == false && input_mode == false)
        return(GUI_PASS);

    switch(key.sym)
     {
      case SDLK_UP : if(display_pos > 0)
                       {
                        display_pos--;
                        scroll_updated = true;
                       }
                     return (GUI_YUM);
                     break;

      case SDLK_DOWN: if(msg_buf.size() > scroll_height && display_pos < msg_buf.size() - scroll_height)
                        {
                         display_pos++;
                         scroll_updated = true;
                        }
                      return (GUI_YUM);
                      break;
      default : break;
     }

    if(page_break)
      {
       page_break = false;

       if(!input_mode)
         Game::get_game()->get_gui()->unlock_input();

       process_holding_buffer(); // Process any text in the holding buffer.
       return(GUI_YUM);
      }

    switch(key.sym)
    {
        case SDLK_ESCAPE: if(permit_inputescape)
                          {
                            // reset input buffer
                            permit_input = NULL;
                            if(input_mode)
                              set_input_mode(false);
                          }
                          return(GUI_YUM);
        case SDLK_KP_ENTER:
        case SDLK_RETURN: if(permit_inputescape)
                          {
                            if(input_mode)
                              set_input_mode(false);
                          }
                          return(GUI_YUM);
        case SDLK_BACKSPACE :
                            if(input_mode)
                              input_buf_remove_char();
                            break;
        default: // alphanumeric characters
                 if((key.unicode & 0xFF80) == 0) // high 9bits 0 == ascii code
                   ascii = (char)(key.unicode & 0x7F); // (in low 7bits)
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

 return(GUI_YUM);
}

GUI_status MsgScroll::MouseUp(int x, int y, int button)
{
 uint16 i;
 MsgText *token;

 if(input_mode)
   {
    token = get_token_at_pos(x,y);
    if(token)
     {
      for(i=0;i < token->s.length(); i++)
        {
         if(isalpha(token->s[i]))
           input_buf_add_char(token->s[i]);
        }
     }
   }

    if(page_break) // any click == scroll-to-end
    {
        page_break = false;
        if(!input_mode)
            Game::get_game()->get_gui()->unlock_input();
        return(GUI_YUM);
    }
    else if(button == 3) // right click == send input
        if(permit_inputescape && input_mode)
        {
            set_input_mode(false);
            return(GUI_YUM);
        }
    return(GUI_PASS);
}

MsgText *MsgScroll::get_token_at_pos(uint16 x, uint16 y)
{
 uint16 i;
 sint32 buf_x, buf_y;
 MsgText *token = NULL;
 std::list<MsgLine *>::iterator iter;

 buf_x = (x - area.x) / 8;
 buf_y = (y - area.y) / 8;

 if(buf_x < 0 || buf_x >= scroll_width || // click not in MsgScroll area.
    buf_y < 0 || buf_y >= scroll_height)
     return NULL;

 if(msg_buf.size() <= scroll_height)
   {
    if((sint32)msg_buf.size() < buf_y + 1)
      return NULL;
   }
 else
   {
    buf_y = display_pos + buf_y;
   }

 for(i=0,iter=msg_buf.begin(); i < buf_y && iter != msg_buf.end();)
  {
   iter++;
   i++;
  }

 if(iter != msg_buf.end())
   {
    token = (*iter)->get_text_at_pos(buf_x);
    if(token)
       printf("Token at (%d,%d) = %s\n",buf_x, buf_y, token->s.c_str());
   }

 return token;
}

void MsgScroll::Display(bool full_redraw)
{
 uint16 i;
 std::list<MsgLine *>::iterator iter;
 MsgLine *msg_line = NULL;

 clearCursor(area.x + 8 * cursor_x, area.y + cursor_y * 8);

 if(scroll_updated || full_redraw)
  {
   screen->fill(0x31,area.x, area.y, scroll_width * 8, (scroll_height)*8); //clear whole scroll

   iter=msg_buf.begin();
   for(i=0;i < display_pos; i++)
	  iter++;

   for(i=0;i< scroll_height && iter != msg_buf.end();i++,iter++)
     {
	  msg_line = *iter;
	  drawLine(screen, msg_line, i);
     }
   scroll_updated = false;

   screen->update(area.x,area.y, scroll_width * 8, (scroll_height)*8);

   cursor_y = i-1;
   if (msg_line)
     cursor_x = msg_line->total_length;
   else
     cursor_x = area.x;
  }
else
 {
  if(show_cursor && (msg_buf.size() <= scroll_height || display_pos == msg_buf.size() - scroll_height)
     && widget_has_focus() ) // hide cursor when scroll loses input
    drawCursor(area.x + 8 * cursor_x, area.y + cursor_y * 8);
 }

}

inline void MsgScroll::drawLine(Screen *screen, MsgLine *msg_line, uint16 line_y)
{
 MsgText *token;
 std::list<MsgText *>::iterator iter;
 uint16 total_length = 0;

 for(iter=msg_line->text.begin();iter != msg_line->text.end() ; iter++)
   {
    token = *iter;
	token->font->drawString(screen, token->s.c_str(), area.x + total_length * 8, area.y+line_y*8); //FIX for hardcoded font height
	total_length += token->s.length();
   }
}

void MsgScroll::clearCursor(uint16 x, uint16 y)
{
 screen->fill(0x31, x, y, 8,8);
}

void MsgScroll::drawCursor(uint16 x, uint16 y)
{
 if(page_break)
    {
     if(cursor_wait <= 2) // flash arrow
	   font->drawChar(screen, 1, x, y); // down arrow
	}
 else
    font->drawChar(screen, cursor_char + 5, x, y); //spinning ankh

  screen->update(x, y, 8, 8);
  if(cursor_wait == MSGSCROLL_CURSOR_DELAY)
    {
     cursor_char = (cursor_char + 1) % 4;
     cursor_wait = 0;
    }
  else
     cursor_wait++;
}


void MsgScroll::set_page_break()
{
 line_count = 0;
 page_break = true;

 if(!input_mode)
   {
    Game::get_game()->get_gui()->lock_input(this);
   }

 return;
}

bool MsgScroll::input_buf_add_char(char c)
{
 MsgText token;
 input_buf.append(&c, 1);
 scroll_updated = true;

 // Add char to scroll buffer

 token.s.assign(&c, 1);
 token.font = font;

 add_token(&token);

 return true;
}

bool MsgScroll::input_buf_remove_char()
{
 if(input_buf.length())
   {
    input_buf.erase(input_buf.length() - 1, 1);
    scroll_updated = true;
    remove_char();

    return true;
   }

 return false;
}

bool MsgScroll::has_input()
{
 if(input_mode == false) //we only have input ready after the user presses enter.
   return true;

 return false;
}

std::string MsgScroll::get_input()
{
 // MsgScroll sets input_mode to false when it receives SDLK_ENTER
 std::string s;

 if(input_mode == false)
   {
    s.assign(input_buf);
   }

 return s;
}

void MsgScroll::clear_page_break()
{
  MsgText *msg_text = new MsgText("", NULL);
  holding_buffer.push_back(msg_text);

  process_holding_buffer();
}


/* Set callback & callback_user_data so that a message will be sent to the
 * caller when input has been gathered.
 */
void MsgScroll::request_input(CallBack *caller, void *user_data)
{
    callback_target = caller;
    callback_user_data = (char *)user_data;
}
