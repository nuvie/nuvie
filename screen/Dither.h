#ifndef __Dither_h__
#define __Dither_h__

/*
 *  Dither.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Jul 20 2007.
 *  Copyright (c) 2007. All rights reserved.
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

#define DITHER_NO_TRANSPARENCY false

// Dither modes..

#define DITHER_NONE 0

#define DITHER_HRC  1 //FIXME add this mode.
#define DITHER_CGA  2
#define DITHER_EGA  3

class Dither
{
  Configuration *config;
  uint8 *dither;
  uint8 mode;
  
public:
    
  Dither(Configuration *cfg);
  ~Dither();
  uint8 get_mode() { return mode; }
  bool dither_bitmap(unsigned char *src_buf, uint16 src_w, uint16 src_h, bool has_transparency);
  
protected:
    
  bool load_data();
  void set_mode();
  
};


#endif /* __Dither_h__ */

