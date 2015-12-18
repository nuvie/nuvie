
#include <string.h>

#include "nuvieDefs.h"
#include "GUI_loadimage.h"

extern "C" {

/************************************************************************/
/* C functions for image loading                                        */
/*                                                                      */
/************************************************************************/

SDL_Surface *GUI_LoadImage(int w, int h, Uint8 *pal, Uint8 *data)
{
	SDL_Surface *image;

	image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
	if ( image ) {
		for ( int row=0; row<h; ++row ) {
			memcpy((Uint8 *)image->pixels + row * image->pitch,
								data, w);
			data += w;
		}
		for ( int i=0; i<256; ++i ) {
			image->format->palette->colors[i].r = *pal++;
			image->format->palette->colors[i].g = *pal++;
			image->format->palette->colors[i].b = *pal++;
		}
	}
	return(image);
}

/************************************************************************/
/* C functions for default font support                                 */
/*                                                                      */
/************************************************************************/

#include "the_font.h"

static SDL_Surface *the_font = NULL;
static SDL_Surface *the_font_6x8 = NULL;
static SDL_Surface *the_font_gump = NULL;

SDL_Surface *GUI_DefaultFont(void)
{
	if ( the_font == NULL ) {
		the_font = GUI_LoadImage(font_w, font_h, font_pal, font_data);
	}
	return(the_font);
}

SDL_Surface *GUI_Font6x8(void)
{
	if ( the_font_6x8 == NULL ) {
		the_font_6x8 = GUI_LoadImage(font_6x8_w, font_6x8_h, font_pal, font_6x8_data);
	}
	return(the_font_6x8);
}

SDL_Surface *GUI_FontGump(void)
{
	if ( the_font_gump == NULL ) {
		the_font_gump = GUI_LoadImage(font_gump_w, font_gump_h, font_pal, font_gump_data);
	}
	return(the_font_gump);
}

Uint8 *GUI_FontGumpWData(void)
{
	return font_gump_w_data;
}

}; /* Extern C */
