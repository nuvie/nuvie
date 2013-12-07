#ifndef __PortraitMD_h__
#define __PortraitMD_h__

/*
 *  PortraitMD.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Dec 4 2013.
 *  Copyright (c) 2013. All rights reserved.
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

#include "Portrait.h"
#include "U6Lib_n.h"

class Configuration;
class Actor;
class U6Shape;

class PortraitMD : public Portrait
{
 U6Lib_n faces;

 public:
 PortraitMD(Configuration *cfg): Portrait(cfg) {};

 bool init();
 bool load(NuvieIO *objlist);
 unsigned char *get_portrait_data(Actor *actor);

 protected:

 uint8 get_portrait_num(Actor *actor);

 private:
 U6Shape *get_background_shape(uint8 actor_num);
 uint8 get_background_shape_num(uint8 actor_num);
};

#endif /* __PortraitMD_h__ */

