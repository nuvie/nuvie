/*
 *  Background.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Aug 24 2003.
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
#include <string>

#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "U6Lib_n.h"
#include "U6Bmp.h"
#include "Background.h"

Background::Background(Configuration *cfg) : GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
 config->value("config/GameType",game_type);

 background = NULL;
}

Background::~Background()
{
 if(background)
  delete background;
}

bool Background::init()
{
 std::string filename;
 U6Lib_n file;
 unsigned char *temp_buf;

 switch(game_type)
   {
    case NUVIE_GAME_U6 : config_get_path(config,"paper.bmp",filename);
                         background = (U6Shape *) new U6Bmp();
                         if(background->load(filename) == false)
                           return false;
                         break;

    case NUVIE_GAME_MD :
                         background = new U6Shape();
                         config_get_path(config,"mdscreen.lzc",filename);
                         file.open(filename,4,game_type);
                         temp_buf = file.get_item(0);
                         background->load(temp_buf + 8);
                         free(temp_buf);
                         break;

    case NUVIE_GAME_SE :
                         background = new U6Shape();
                         config_get_path(config,"screen.lzc",filename);
                         file.open(filename,4,game_type);
                         temp_buf = file.get_item(0);
                         background->load(temp_buf + 8);
                         free(temp_buf);
                         break;
   }

 area.x = 0;
 area.y = 0;

 background->get_size(&area.w,&area.h);

 return true;
}

void Background::Display(bool full_redraw)
{
 if(full_redraw || update_display)
   {
    update_display = false;

    if(game_type != NUVIE_GAME_U6)
       screen->clear(area.x,area.y,area.w,area.h,NULL);

    screen->blit(area.x, area.y, background->get_data(), 8,  area.w, area.h, area.w, true);
    screen->update(area.x,area.y,area.w,area.h);
   }

 return;
}

