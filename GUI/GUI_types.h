
#ifndef _GUI_types_h
#define _GUI_types_h

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

class GUI_Color {
 public:

 uint8 r;
 uint8 g;
 uint8 b;
 //uint8 a;

 uint32 sdl_color;

 public:

 GUI_Color(uint8 red, uint8 green, uint8 blue) { r=red; g=green; b=blue; };
 GUI_Color() {};
 void map_color(SDL_Surface *surface) { sdl_color = SDL_MapRGB(surface->format, r, g, b); };

};

#endif /* _GUI_types_h */

