#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../nuvieDefs.h"
#include "../files/U6Lzw.h"
#include "../files/U6Lib_n.h"
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
 if (gametype[0] == 's') {
   name="savage.fnt";

   if(library.open(name, 4, NUVIE_GAME_SE) == false)
     exit(1);

   fprintf(stderr,"size: %ld\n",library.get_item_size(0));
   data = library.get_item(0, NULL);
   FILE * filed = fopen("item_savage.fnt","w");
   fwrite(data,1,library.get_item_size(0), filed);
   fclose(filed);
   free(data);

   library.close();

   exit(0);
 } 

 //martian dreams
 name="fonts.lzc";
 if(library.open(name, 4, NUVIE_GAME_MD) == false)
     exit(1);
 int i=library.get_num_items();
 fprintf(stderr,"items: %ld\n",i);

 fprintf(stderr,"size: %ld\n",library.get_item_size(0));
 data = library.get_item(0, NULL);
 FILE * filed = fopen("item_martian.fnt","w");
 fwrite(data,1,library.get_item_size(0), filed);
 fclose(filed);
 free(data);
   library.close();

   exit(0);
}

