/*
 *  SDLUtils.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Feb 19 2017.
 *  Copyright (c) 2017. All rights reserved.
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

#include "SDLUtils.h"

#if SDL_VERSION_ATLEAST(2,0,0)

int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{
    return SDL_SetPaletteColors(surface->format->palette, colors, firstcolor, ncolors);
}

#endif