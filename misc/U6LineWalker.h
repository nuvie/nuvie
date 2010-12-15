#ifndef __U6LineWalker_h__
#define __U6LineWalker_h__

/*
 *  U6LineWalker.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Nov 11 2010.
 *  Copyright (c) 2010. All rights reserved.
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

#include "nuvieDefs.h"

class U6LineWalker {
protected:
	uint32 start_x, start_y;
	uint32 end_x, end_y;

	sint32 xoffset[2], yoffset[2];
	sint32 line_inc[2];
	sint32 line_counter;

	uint32 cur_x, cur_y;
	uint32 max_length;

	uint32 cur_step;

public:
	U6LineWalker(uint32 sx, uint32 sy, uint32 ex, uint32 ey);
	virtual ~U6LineWalker();

	bool step();
	bool next(uint32 *x, uint32 *y);

};

#endif /* __U6LineWalker_h__ */
