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
 char outfile[8]; // 'nnn.dat\0'

 if(argc != 2)
 {
  fprintf(stderr,"Usage: %s file.lzc\n",argv[0]);
  exit(1);
 }

 std::string path(argv[1]);

   if(!library.open(path, 4, NUVIE_GAME_SE))
     exit(1);

 for(uint32 i=0;i<library.get_num_items();i++)
{
   fprintf(stderr,"size: %d\n",library.get_item_size(i));
   data = library.get_item(i, NULL);
   snprintf(outfile, 8, "%d.dat", i);
   FILE * filed = fopen(outfile,"wb");
   fwrite(data,1,library.get_item_size(i), filed);
   fclose(filed);
   free(data);
}
   library.close();

   exit(0);

}

