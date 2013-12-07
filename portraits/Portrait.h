#ifndef __Portrait_h__
#define __Portrait_h__

/*
 *  Portrait.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue May 20 2003.
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


class Configuration;
class Actor;
class U6Lib_n;
class NuvieIO;

#define PORTRAIT_WIDTH 56
#define PORTRAIT_HEIGHT 64

#define NO_PORTRAIT_FOUND 255

Portrait *newPortrait(nuvie_game_t gametype, Configuration *cfg);

class Portrait
{
protected:
 Configuration *config;

 uint8 avatar_portrait_num;
 uint8 width;
 uint8 height;
 public:

 Portrait(Configuration *cfg);
 virtual ~Portrait() {};

 virtual bool init()=0;
 virtual bool load(NuvieIO *objlist)=0;
 virtual unsigned char *get_portrait_data(Actor *actor)=0;

 uint8 get_portrait_width() { return width; }
 uint8 get_portrait_height() { return height; }

 bool has_portrait(Actor *actor) { return (get_portrait_num(actor) != NO_PORTRAIT_FOUND); }

 uint8 get_avatar_portrait_num();

 protected:

 unsigned char *get_wou_portrait_data(U6Lib_n *lib, uint8 num);

 private:

 virtual uint8 get_portrait_num(Actor *actor)=0;

};

#endif /* __Portrait_h__ */

