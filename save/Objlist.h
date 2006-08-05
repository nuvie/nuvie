#ifndef __Objlist_h__
#define __Objlist_h__

/*
 *  Objlist.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Aug 05 2006.
 *  Copyright (c) 2006. All rights reserved.
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
 
//U6 specific offsets
#define OBJLIST_OFFSET_U6_WIND_DIR    0x1bfa
#define OBJLIST_OFFSET_U6_ECLIPSE     0x1c12
#define OBJLIST_OFFSET_U6_COMBAT_MODE 0x1c69

//u6 wind direction
#define OBJLIST_U6_WIND_DIR_N  0
#define OBJLIST_U6_WIND_DIR_NE 1
#define OBJLIST_U6_WIND_DIR_E  2
#define OBJLIST_U6_WIND_DIR_SE 3
#define OBJLIST_U6_WIND_DIR_S  4
#define OBJLIST_U6_WIND_DIR_SW 5
#define OBJLIST_U6_WIND_DIR_W  6
#define OBJLIST_U6_WIND_DIR_NW 7
#define OBJLIST_U6_WIND_DIR_C  0xff

#endif /* __Objlist_h__ */
