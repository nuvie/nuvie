
#include <stdlib.h>
#include <cmath>

#include "SDL.h"

#include "nuvieDefs.h"
#include "GUI_font.h"
#include "GUI_loadimage.h"

/* use default 8x8 font */
GUI_Font::GUI_Font(Uint8 fontType)
{
  SDL_Surface *temp;

  if(fontType == GUI_FONT_6X8)
	  temp = GUI_Font6x8();
  else
	  temp = GUI_DefaultFont();


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
    DEBUG(0,LEVEL_EMERGENCY,"Could not load font.\n");
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
void GUI_Font::TextOut(SDL_Surface* context,int x, int y, const char* text, int line_wrap)
{
  int i;
  int j;
  Uint8 ch;
  SDL_Rect src;
  SDL_Rect dst;

  src.w = charw;
  src.h = charh-1;
  dst.w = charw;
  dst.h = charh-1;
  i=0;
  j=0;
  while ( (ch=text[i]) )  // single "=" is correct!
  {
    if(line_wrap && j == line_wrap)
       {
        j=0;
        y += charh;
       }

    src.x = (ch%16)*charw;
    src.y = (ch/16)*charh;
    dst.x = x+(j*charw);
    dst.y = y;
    SDL_BlitSurface(fontStore, &src, context, &dst);
    i++;
    j++;
  }
}

void GUI_Font:: TextExtent(const char *text, int *w, int *h, int line_wrap)
{
 int len = strlen(text);

 if(line_wrap && len > line_wrap)
   *w = line_wrap * charw;
 else
   *w=len * charw;

 if(line_wrap && len > line_wrap)
   {
    *h = (int)ceil((float)len / (float)line_wrap);
    *h *= charh-1;
   }
 else
    *h=charh-1;

 return;
}
