#ifndef	__vc7inc_h__
#define __vc7inc_h__

/*
 *  vc7inc.h
 *  Nuvie
 *
 *  Created by Nelno the Amoeba on Fri Nov 14 13 2003.
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

#define snprintf	_snprintf
#define strcasecmp	_stricmp

#include <locale>
#include <math.h>
#include <assert.h>

#pragma	warning (disable : 4244)	//	conversion from 'x' to 'x', possible loss of data
#pragma	warning (disable : 4100)	//	unreferenced formal parameter
#pragma warning (disable : 4800)	//	forcing value to bool 'true' or 'false' (performance warning)
#pragma warning (disable : 4121)	//	alignment of member was sensitive to packing

//	for pre-compiled headers when I get all the #include issues fixed
/*
#include <iostream>

#include "sys/types.h"
#include "sys/stat.h"
#include "SDL.h"
#include "misc.h"
*/

// .NET has no roundf function (C99)
__inline roundf (float val) { return floorf (val); }

#define M_PI	3.1415926535897932384626433832795

#endif	/* __vc7inc_h__ */