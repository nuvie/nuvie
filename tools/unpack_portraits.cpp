#include <stdio.h>
#include "../nuvieDefs.h"
#include "../misc/U6misc.h"
#include "../misc/SDLUtils.h"
#include "../files/U6Lzw.h"
#include "../files/U6Lib_n.h"
#include "../files/U6Shape.h"
#include "../files/NuvieIOFile.h"

uint8 *loadpal(char *gametype)
{
 uint8 *palette;
 uint16 i,j;
 char filename[6] = ""; // 'xxpal\0'
 NuvieIOFileRead file;
 unsigned char *buf;
 uint8 *pal_ptr;

 strcpy(filename, gametype);
 strcat(filename, "pal");

 if(file.open(filename) == false)
  return NULL;

 buf = file.readAll();
 file.close();

 palette = (uint8 *)malloc(768);
 memset(palette, 0xff, 768);

 pal_ptr = palette;

 for(i=0,j=0;i<256;i++,j+=3)
  {
   pal_ptr[0] = buf[j]<<2;
   pal_ptr[1] = buf[j+1]<<2;
   pal_ptr[2] = buf[j+2]<<2;
   pal_ptr += 3;
  }

 free(buf);

 return palette;
}

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,  LPSTR lpCmdLine, int iShowCmd) {
	SDL_SetModuleHandle(GetModuleHandle(NULL));
	return main(__argc, __argv);
}
#endif

int main(int argc, char **argv)
{
 U6Lib_n faces;
 U6Shape *shp;
 unsigned char *shp_data;
 NuvieIOBuffer shp_buf;
 U6Lib_n shp_lib;
 SDL_Surface *s;
 uint16 w, h;
 string name;
 uint8 *pal;
 SDL_Color c[256];
 uint16 i;
 uint16 num_portraits;
 char bmp_file[8]; // 'xxx.bmp\0'
 char gametype[3]; // 'xx\0'
 uint8 game=0;
 if(argc != 2 || (strcmp(argv[1], "md") != 0 && strcmp(argv[1], "se") != 0))
 {
  fprintf(stderr,"Usage: %s gametype\n\ngametype = either md or se\n\n",argv[0]);
  exit(1);
 }

 strcpy(gametype, argv[1]);

 if(strcmp(gametype, "md") == 0)
 {
  name = "mdfaces.lzc";
  game = NUVIE_GAME_MD;
 }
 else
 {
  name = "faces.lzc";
  game = NUVIE_GAME_MD;
 }

 SDL_Init(SDL_INIT_VIDEO);

 pal = loadpal(gametype);
 if(!pal)
  exit(1);

 for(i=0;i<256;i++)
 {
  c[i].r=pal[i*3];
  c[i].g=pal[i*3+1];
  c[i].b=pal[i*3+2];
 
  fprintf(stderr,"pal[%u] = [%u,%u,%u]\n", i, c[i].r, c[i].g, c[i].b);
 }

 free(pal);

 if(faces.open(name, 4, game) == false)
  exit(1);

 num_portraits = faces.get_num_items();

 for(i=0;i<num_portraits;i++)
 {
  if(faces.get_item_size(i) == 0)
   continue;

  shp = new U6Shape();

  shp_data = faces.get_item(i, NULL);
  shp_buf.open(shp_data, faces.get_item_size(i), NUVIE_BUF_NOCOPY); 
  shp_lib.open(&shp_buf, 4, game);

  shp->load(&shp_lib, 0);
  shp->get_size(&w,&h);
  s = shp->get_shape_surface();
  SDL_SetColors(s, c, 0, 256);

  sprintf(bmp_file, "%03u.bmp", i);
  SDL_SaveBMP(s, bmp_file);
 
  fprintf(stderr,"Portrait %03u: width = %u, height = %u\n", i, w, h); 

  SDL_FreeSurface(s);
  delete shp;
  shp_lib.close();
  free(shp_data);

 }

 faces.close();

 exit(0);
}

