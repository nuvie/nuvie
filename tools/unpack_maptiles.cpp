#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../nuvieDefs.h"
#include "../files/U6Lzw.h"
#include "../files/U6Lib_n.h"
#include "../files/NuvieIOFile.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,  LPSTR lpCmdLine, int iShowCmd) {
	//SDL_SetModuleHandle(GetModuleHandle(NULL));
	return main(__argc, __argv);
}
#endif

int main(int argc, char **argv)
{
 U6Lib_n library;
 unsigned char *data;
 string name;
 char gametype[3]; // 'xx\0'

 if(argc != 2 || (strcmp(argv[1], "md") != 0 && strcmp(argv[1], "se") != 0))
 {
  fprintf(stderr,"Usage: %s gametype\n\ngametype = either md or se\n\n",argv[0]);
  exit(1);
 }

 strcpy(gametype, argv[1]);

 name="masktype.vga";

 if(!library.open(name, 4, NUVIE_GAME_SE))
  exit(1);

  data = library.get_item(0, NULL);
  FILE * filed = fopen("item_masktype.vga","wb");
  fprintf(stderr,"size: %d\n",library.get_item_size(0));
  fwrite(data,1,library.get_item_size(0), filed);
  fclose(filed);
  free(data);

 library.close();
 
 name="maptiles.vga";
 if(!library.open(name, 4, NUVIE_GAME_SE))
  exit(1);

  data = library.get_item(0, NULL);
  filed = fopen("item_maptiles.vga","wb");
  fprintf(stderr,"size: %d\n",library.get_item_size(0));
  fwrite(data,1,library.get_item_size(0), filed);
  fclose(filed);
  free(data);

 library.close();

 exit(0);
}

