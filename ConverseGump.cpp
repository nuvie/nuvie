/*
 *  ConverseGump.cpp
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
#include <cctype>
#include <iostream>

#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "FontManager.h"
#include "Font.h"
#include "GamePalette.h"
#include "GUI.h"
#include "MsgScroll.h"
#include "Portrait.h"
#include "Player.h"
#include "ConvFont.h"
#include "ConverseGump.h"



// ConverseGump Class

ConverseGump::ConverseGump(Configuration *cfg, Font *f)
{
 uint16 x, y;

 init(cfg, f);

 scroll_width = 30;
 scroll_height = 8;

 x = 8;
 y = 8;

 uint16 x_off = config_get_video_x_offset(config);
 uint16 y_off = config_get_video_y_offset(config);

 GUI_Widget::Init(NULL, x+x_off, y+y_off, PORTRAIT_WIDTH + 8 + scroll_width * 8, 168);
 npc_portrait = NULL;
 avatar_portrait = NULL;

 font = new ConvFont();
 font->init(NULL, 256, 0);

 found_break_char = false;
}

ConverseGump::~ConverseGump()
{
	if(npc_portrait)
		free(npc_portrait);
	if(avatar_portrait)
		free(avatar_portrait);
	conv_keywords.clear();
	permitted_input_keywords.clear();
	delete font;
}

void ConverseGump::set_talking(bool state)
{
	if(state == true)
	{
		found_break_char = true;
		conv_keywords.clear();
		permitted_input_keywords.clear();
    	Show();
    	set_input_mode(false);
    	clear_scroll();
    	set_found_break_char(true);
    	add_keyword("name");
    	add_keyword("job");
    	add_keyword("bye");

    	keyword_list = &conv_keywords;
	}

	MsgScroll::set_talking(state);
}

void ConverseGump::set_actor_portrait(Actor *a)
{
	if(npc_portrait)
		free(npc_portrait);

	Portrait *p = Game::get_game()->get_portrait();
	npc_portrait = p->get_portrait_data(a);

	if(avatar_portrait == NULL)
	{
		avatar_portrait = p->get_portrait_data(Game::get_game()->get_player()->get_actor());
	}
}

void ConverseGump::set_permitted_input(const char *allowed)
{
	permitted_input_keywords.clear();
	keyword_list = &permitted_input_keywords;

	if(allowed && strcmp(allowed, "yn") == 0)
	{
		add_keyword("yes");
		add_keyword("no");
	}

	MsgScroll::set_permitted_input(allowed);
}

void ConverseGump::clear_permitted_input()
{
	keyword_list = &conv_keywords;
	MsgScroll::clear_permitted_input();
}

/*
void ConverseGump::add_token(MsgText *token)
{
	DEBUG(0,LEVEL_ALERT, "TOKEN: %s\n", token->s.c_str());

	display_text.push_back(*token);
}
*/

void ConverseGump::display_string(std::string s, Font *f)
{
	if(s.empty())
		return;

	MsgScroll::display_string(strip_whitespace_after_break(s), f);
}

std::string ConverseGump::strip_whitespace_after_break(std::string s)
{
	std::string::iterator iter;
	for(iter=s.begin();iter != s.end();)
	{
		if(found_break_char == true)
		{
			char c = *iter;
			if(c == ' ' || c == '\t' || c == '\n' || c == '*')
			{
				iter = s.erase(iter);
			}
			else
			{
				found_break_char = false;
				iter++;
			}
		}
		else
		{
			char c = *iter;
			if(c == '*')
			{
				found_break_char = true;
			}
			iter++;
		}
	}

	return s;
}

bool ConverseGump::parse_token(MsgText *token)
{
	int at_idx = token->s.find_first_of('@', 0);
	int i=0;
	int len = (int)token->s.length();
	while(at_idx != -1 && i < len)
	{
		std::string keyword = "";
		for(i=at_idx+1;i < len;i++)
		{
			char c = token->s[i];
			if(isalpha(c))
			{
				keyword.push_back(c);
			}

			if(!isalpha(c) || i == len - 1)
			{
				token->s.erase(at_idx, 1);
				i--;
				at_idx = token->s.find_first_of('@', i);
				break;
			}
		}
		DEBUG(0,LEVEL_WARNING, "%s", keyword.c_str());
		add_keyword(keyword);
	}

	return MsgScroll::parse_token(token);
}

void ConverseGump::add_keyword(std::string keyword)
{
	keyword = " *" + keyword;

	std::list<MsgText>::iterator iter;
	for(iter=keyword_list->begin();iter!=keyword_list->end();iter++)
	{
		if(string_i_compare((*iter).s, keyword))
		{
			return;
		}
	}

	MsgText m_text;
	m_text.s = keyword;
	m_text.font = font;
	keyword_list->push_back(m_text);
}

std::string ConverseGump::get_token_string_at_pos(uint16 x, uint16 y)
{
	uint16 total_length = 0;
	uint16 tmp_y = area.y + PORTRAIT_HEIGHT + 8;
	std::list<MsgText>::iterator iter;
	for(iter=keyword_list->begin();iter!=keyword_list->end();iter++)
	{
		MsgText t = *iter;
		if(t.s.length() + total_length >= 26)
		{
			total_length = 0;
			tmp_y += 8;
		}
		//t.font->drawString(screen, t.s.c_str(), area.x + PORTRAIT_WIDTH / 2 + PORTRAIT_WIDTH + 8 + total_length * 8, y + PORTRAIT_HEIGHT + 8, 0);
		if( x > area.x + PORTRAIT_WIDTH / 2 + PORTRAIT_WIDTH + 8 + total_length * 8 && x < area.x + PORTRAIT_WIDTH / 2 + PORTRAIT_WIDTH + 8 + total_length * 8 + t.s.length() * 8)
		{
			if(y > tmp_y && y < tmp_y + 8)
			{
				keyword_list->erase(iter);
				return t.s;
			}
		}
		total_length += t.s.length();
	}
	return "";
}

void ConverseGump::Display(bool full_redraw)
{
	MsgText *token;
	 //std::list<MsgText>::iterator iter;
	 uint16 total_length = 0;
	 uint16 y = area.y;

	 if(npc_portrait)
	 {
		 screen->blit(area.x,area.y,npc_portrait,8,PORTRAIT_WIDTH,PORTRAIT_HEIGHT,PORTRAIT_WIDTH,false);
	 }

	 if(!page_break && input_mode && avatar_portrait)
	 {
		 screen->blit(area.x + PORTRAIT_WIDTH / 2,area.y + PORTRAIT_HEIGHT + 8,avatar_portrait,8,PORTRAIT_WIDTH,PORTRAIT_HEIGHT,PORTRAIT_WIDTH,false);
		 std::list<MsgText>::iterator iter;
		 for(iter=keyword_list->begin();iter!=keyword_list->end();iter++)
		 {
			 MsgText t = *iter;
			 if(t.s.length() + total_length >= 26)
			 {
				 total_length = 0;
				 y += 8;
			 }
			 t.font->drawString(screen, t.s.c_str(), area.x + PORTRAIT_WIDTH / 2 + PORTRAIT_WIDTH + 8 + total_length * 8, y + PORTRAIT_HEIGHT + 8, 0);
			 total_length += t.s.length();
		 }
	 }

	 y = area.y;
	 total_length = 0;
	 std::list<MsgLine *>::iterator iter;
	 for(iter=msg_buf.begin();iter != msg_buf.end();iter++)
	     {
		  MsgLine *msg_line = *iter;
		  std::list<MsgText *>::iterator iter1;

		  for(iter1=msg_line->text.begin();iter1 != msg_line->text.end() ; iter1++)
		  {
			  token = *iter1;

			  token->font->drawString(screen, token->s.c_str(), area.x + PORTRAIT_WIDTH + 8 + total_length * 8, y, 0); //FIX for hardcoded font height
			  total_length += token->s.length();
			  //token->font->drawChar(screen, ' ', area.x + PORTRAIT_WIDTH + 8 + total_length * 8, y, 0);
			  //total_length += 1;

		  }
		  y+=8;
		  total_length = 0;
	     }

	//font->drawString(screen, conv_str.c_str(), area.x, area.y);
	screen->update(area.x,area.y, area.w, area.h);
}


GUI_status ConverseGump::KeyDown(SDL_keysym key)
{
    char ascii = 0;

    if(page_break)
      {
       page_break = false;
       just_finished_page_break = true;
       if(!input_mode)
         Game::get_game()->get_gui()->unlock_input();
       if(!is_holding_buffer_empty() || !input_mode)
       {
    	   clear_scroll();
    	   process_holding_buffer(); // Process any text in the holding buffer.
       }
       return(GUI_YUM);
      }

	switch(key.sym)
	    {
	        case SDLK_ESCAPE:
	                            // reset input buffer
	                            permit_input = NULL;
	                            if(input_mode)
	                              set_input_mode(false);

	                          return(GUI_YUM);
	        case SDLK_KP_ENTER:
	        case SDLK_RETURN:
	                            if(input_mode)
	                              set_input_mode(false);
	                            clear_scroll();
	                            found_break_char = true; //strip leading whitespace.


	                          return(GUI_YUM);
	        case SDLK_BACKSPACE :
	                            if(input_mode)
	                              input_buf_remove_char();
	                            break;
	        default: // alphanumeric characters
	                 if((key.unicode & 0xFF80) == 0) // high 9bits 0 == ascii code
	                   ascii = (char)(key.unicode & 0x7F); // (in low 7bits)
	                 else DEBUG(0,LEVEL_WARNING,"unhandled unicode value (%d)\n",key.unicode);
	                 if(input_mode && isprint(ascii))
	                  {
	                   if(permit_input == NULL)
	                    input_buf_add_char(ascii);
	                   else if(strchr(permit_input, ascii) || strchr(permit_input, tolower(ascii)))
	                   {
	                    input_buf_add_char(toupper(ascii));
	                    set_input_mode(false);
	                    clear_scroll();
	                    found_break_char = true;
	                   }
	                  }
	            break;
	    }
	return GUI_YUM;
}

GUI_status ConverseGump::MouseUp(int x, int y, int button)
{
 uint16 i;
 std::string token_str;

    if(page_break) // any click == scroll-to-end
    {
        page_break = false;
        just_finished_page_break = true;
        if(!input_mode)
            Game::get_game()->get_gui()->unlock_input();

        if(!is_holding_buffer_empty() || !input_mode)
        {
        	clear_scroll();
        	process_holding_buffer(); // Process any text in the holding buffer.
        }
        return(GUI_YUM);
    }
    else if(button == 1) // left click == select word
    {
     if(input_mode)
       {
        token_str = get_token_string_at_pos(x,y);
        if(token_str.length() > 0)
        {
        	for(i=0;i < token_str.length(); i++)
        	{
        		if(isalpha(token_str[i]))
        			input_buf_add_char(token_str[i]);
        	}
        	set_input_mode(false);
        	clear_scroll();
        	found_break_char = true; //strip leading whitespace.
        }
       }
    }
    /*
    else if(button == 3) // right click == send input
        if(permit_inputescape && input_mode)
        {
            set_input_mode(false);
            return(GUI_YUM);
        }
     */
    return(GUI_YUM);
}
