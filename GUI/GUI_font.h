#ifndef _GUI_font_h
#define _GUI_font_h

#include <string.h>

#include "SDL.h"

class GUI_Font
{

public:
  /* use default 8x8 font */
  GUI_Font();

  /* open named BMP file */
  GUI_Font(char *name);

  /* use given YxY surface */
  GUI_Font(SDL_Surface *bitmap);

  /* copy constructor */
  GUI_Font(GUI_Font& font);

  virtual ~GUI_Font();

  /* determine drawing style */
  virtual void SetTransparency(int on);

  /* determine foreground and background color values RGB*/
  virtual void SetColoring(Uint8 fr, Uint8 fg, Uint8 fb, Uint8 br=255, Uint8 bg=255, Uint8 bb=255);

  /* yields the pixel height of a single character */
  inline virtual int CharHeight()
    {return charh-1;}

  /* yields the pixel width of a single character */
  inline virtual int CharWidth()
    {return charw;}

  /* put the text onto the given surface using the preset mode and colors */
  virtual void TextOut(SDL_Surface *context, int x, int y, char* text, int line_wrap=0);
  
  /* yields pixel width and height of a string when printed with this font */
  void TextExtent(char *text, int *w, int *h, int line_wrap=0);
  
protected:
  /* the font source surface */
  SDL_Surface *fontStore;

  /* flags */
  int transparent;
  int freefont;

  /* dimensions */
  int charh,charw;
};

#endif /* _GUI_font_h */
