
#ifndef _GUI_types_h
#define _GUI_types_h
#error FIXME:  Not done yet

/* Basic GUI data types and classes */

#include "SDL.h"

class GUI_image {

public:
	/* Load an image from a BMP file */
	GUI_image(char *file);

	/* Use a SDL_Surface as the image
	   The surface shouldn't be freed while the miage object exists.
	 */
	GUI_image(SDL_Surface *picture, int shouldfree = 0);

private:
	SDL_Surface *image;
};

#endif _GUI_types_h

