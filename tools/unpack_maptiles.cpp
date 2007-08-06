#include <stdio.h>
#include "../nuvieDefs.h"
#include "../files/U6Lzw.h"
#include "../files/U6Lib_n.h"
#include "../files/U6Shape.h"
#include "../files/NuvieIOFile.h"

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

 if(library.open(name, 4, NUVIE_GAME_SE) == false)
  exit(1);

  data = library.get_item(0, NULL);
  FILE * filed = fopen("item_masktype.vga","w");
  fprintf(stderr,"size: %ld\n",library.get_item_size(0));
  fwrite(data,1,library.get_item_size(0), filed);
  fclose(filed);
  free(data);

 library.close();
 
 name="maptiles.vga";
 if(library.open(name, 4, NUVIE_GAME_SE) == false)
  exit(1);

  data = library.get_item(0, NULL);
  filed = fopen("item_maptiles.vga","w");
  fprintf(stderr,"size: %ld\n",library.get_item_size(0));
  fwrite(data,1,library.get_item_size(0), filed);
  fclose(filed);
  free(data);

 library.close();

 exit(0);
}

