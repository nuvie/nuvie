#ifndef __U6Bmp_h__
#define __U6Bmp_h__

/*
 *  U6Bmp.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Mar 09 2003.
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

#include "U6def.h"
#include "U6Shape.h"

class U6Bmp: public U6Shape
{
 private:
 unsigned char *data;
 
 public:
 
   U6Bmp();
   ~U6Bmp();
    
   bool load(std::string filename);
 
};

#endif /* __U6Bmp_h__ */
