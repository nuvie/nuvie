#ifndef __Look_h__
#define __Look_h__
/*
 *  Look.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Mar 24 2003.
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

#include "U6def.h"
#include "Configuration.h"

class Look
{
 Configuration *config;
 const char *look_tbl[2048];
 uint16 max_len;
 unsigned char *look_data;
 char *desc_buf;
 
 public:
 
 Look(Configuration *cfg);
 ~Look();
 
 bool init();
 
 // if description has a plural form, true is returned in plural
 const char *get_description(uint16 tile_num, bool *plural);
 uint16 get_max_len();
 
 void print();
 
 protected:
};

#endif /* __Look_h__ */
