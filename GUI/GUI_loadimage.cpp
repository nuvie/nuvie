
#include <string.h>
#include "GUI_loadimage.h"

extern "C" {

/************************************************************************/
/* C functions for image loading                                        */
/*                                                                      */
/************************************************************************/

SDL_Surface *GUI_LoadImage(int w, int h, Uint8 *pal, Uint8 *data)
{
	SDL_Surface *image;

	image = SDL_AllocSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
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

SDL_Surface *GUI_DefaultFont(void)
{
	if ( the_font == NULL ) {
		the_font = GUI_LoadImage(font_w, font_h, font_pal, font_data);
	}
	return(the_font);
}


}; /* Extern C */
