/*
    GUILIB:  An example GUI framework library for use with SDL
    Copyright (C) 1997  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

#ifndef _GUI_loadimage_h
#define _GUI_loadimage_h

#include "SDL.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* C functions for C image loading support:
   Use 'genimage' to turn an 8-bit BMP file into a C image, and include
   the output in your application, either directly or as a header file.
   Create an SDL surface in your program:
	surface = GUI_LoadImage(image_w, image_h, image_pal, image_data);
 */
/************************************************************************/

extern SDL_Surface *GUI_LoadImage(int w, int h, Uint8 *pal, Uint8 *data);

/* Load the internal 8x8 font and return the associated font surface */
extern SDL_Surface *GUI_DefaultFont(void);

#ifdef __cplusplus
};
#endif
#include "close_code.h"

#endif /* _GUI_loadimage_h */
