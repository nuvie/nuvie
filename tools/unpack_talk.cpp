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
 U6Lib_n scripts;
 uint16 i;
 uint16 num_scripts;
 string name;
 char script_file[8]; // 'xxx.bmp\0'
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
  name = "talk.lzc";
  game = NUVIE_GAME_MD;
 }
 else
 {
  name = "talk.lzc";
  game = NUVIE_GAME_MD;
 }

 if(scripts.open(name, 4, game) == false)
  exit(1);

 num_scripts= scripts.get_num_items();

 for(i=0;i<num_scripts;i++)
 {
   int len=scripts.get_item_size(i);
  if(len == 0)
   continue;


  sprintf(script_file, "%03u.dat", i);
  FILE * scr;
  scr=fopen(script_file,"wb");
  if (scr) {
    fwrite(scripts.get_item(i, NULL),1,len,scr);
    fclose(scr);
  }
 }

 scripts.close();

 exit(0);
}

