#ifndef __PortraitU6_h__
#define __PortraitU6_h__

/*
 *  PortraitU6.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Dec 2 2013.
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
class NuvieIO;

class PortraitU6 : public Portrait
{
 U6Lib_n portrait_a;
 U6Lib_n portrait_b;
 U6Lib_n portrait_z;

 public:

 PortraitU6(Configuration *cfg) : Portrait(cfg) {};
 ~PortraitU6() {};

 bool init();
 bool load(NuvieIO *objlist);
 unsigned char *get_portrait_data(Actor *actor);

 private:

 uint8 get_portrait_num(Actor *actor);

};

#endif /* __PortraitU6_h__ */

