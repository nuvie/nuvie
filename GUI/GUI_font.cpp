
#include <stdlib.h>

#include "SDL.h"

#include "U6def.h"
#include "GUI_font.h"
#include "GUI_loadimage.h"

/* use default 8x8 font */
GUI_Font::GUI_Font()
{
  SDL_Surface *temp=GUI_DefaultFont();
  fontStore=SDL_ConvertSurface(temp,temp->format,SDL_SWSURFACE);
  charh = fontStore->h/16;
  charw = fontStore->w/16;
  freefont=1;
  SetTransparency(1);
}

/* open named BMP file */
GUI_Font::GUI_Font(char *name)
{
  fontStore=SDL_LoadBMP(name);
  if (fontStore!=NULL)
  {
    charh = fontStore->h/16;
    charw = fontStore->w/16;
    freefont=1;
  }
  else
  {
    freefont=0;
    fprintf(stderr,"Could not load font.\n");
    exit(1);
  }
  SetTransparency(1);
}

/* use given YxY surface */
GUI_Font::GUI_Font(SDL_Surface *bitmap)
{
  if (bitmap==NULL)
    fontStore=GUI_DefaultFont();
  else
    fontStore=bitmap;
  charh = fontStore->h/16;
  charw = fontStore->w/16;
  freefont=0;
  SetTransparency(1);
}

/* copy constructor */
GUI_Font::GUI_Font(GUI_Font& font)
{
  SDL_Surface *temp=font.fontStore;
  fontStore=SDL_ConvertSurface(temp,temp->format,SDL_SWSURFACE);
  charh = fontStore->h/16;
  charw = fontStore->w/16;
  freefont=1;
  SetTransparency(1);
}

GUI_Font::~GUI_Font()
{
  if (freefont)
    SDL_FreeSurface(fontStore);
}

/* determine drawing style */
void GUI_Font::SetTransparency(int on)
{
  if ( (transparent=on) )  // single "=" is correct
    SDL_SetColorKey(fontStore,SDL_SRCCOLORKEY,0);
  else
    SDL_SetColorKey(fontStore,0,0);
}

/* determine foreground and background color values RGB*/
void GUI_Font::SetColoring(Uint8 fr, Uint8 fg, Uint8 fb, Uint8 br, Uint8 bg, Uint8 bb)
{
  SDL_Color colors[3]={{br,bg,bb,0},{fr,fg,fb,0}};
  SDL_SetColors(fontStore,colors,0,2);
}

/* put the text onto the given surface using the preset mode and colors */
void GUI_Font::TextOut(SDL_Surface* context,int x, int y, char* text)
{
  int i;
  Uint8 ch;
  SDL_Rect src;
  SDL_Rect dst;

  src.w = charw;
  src.h = charh-1;
  dst.w = charw;
  dst.h = charh-1;
  i=0;
  while ( (ch=text[i]) )  // single "=" is correct!
  {
    src.x = (ch%16)*charw;
    src.y = (ch/16)*charh;
    dst.x = x+(i*charw);
    dst.y = y;
    SDL_BlitSurface(fontStore, &src, context, &dst);
    i++;
  }
}

void GUI_Font:: TextExtent(char *text, int *w, int *h)
{
 *w=strlen(text) * charw;
 *h=charh-1;

 return;
}
