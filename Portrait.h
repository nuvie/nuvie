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

#include "U6Lib_n.h"

class Configuration;

class Portrait
{
 Configuration *config;
 
 uint8 avatar_portrait_num;
 
 U6Lib_n portrait_a;
 U6Lib_n portrait_b;
 U6Lib_n portrait_z;
 
 //unsigned char portrait_data[0xe00]; // 56x64 pixels
 
 public:
 
 Portrait(Configuration *cfg);
 
 bool init();
 unsigned char *get_portrait_data(uint16 num);
};

#endif /* __Portrait_h__ */

