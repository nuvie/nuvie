#include <string>

#include "misc/U6def.h"
#include "files/U6Lib_n.h"
#include "files/U6Lzw.h"

#include "conf/Configuration.h"

uint16 get_actor_name_length(unsigned char *buf)
{
 uint16 i;
 
 for(i=0;buf[i] != 0xf1 && buf[i] != 0xf3;)
  i++;

 return i;
}

void unpack_converse_file(U6Lib_n *converse, U6Lzw *lzw)
{
 uint16 i;
 uint8 j;
 unsigned char *buf;
 uint32 buf_size;
 unsigned char *uncomp_buf;
 uint32 uncomp_size;
 FILE *out_file;
 char outfilename[100];
 uint16 name_len;
 uint32 num_items;

 num_items = converse->get_num_items();

 for(i=0;i < num_items; i++)
  {
   buf = converse->get_item(i);
   if(buf == NULL)
     continue;
   buf_size = converse->get_item_size(i);
   uncomp_buf = lzw->decompress_buffer(buf,buf_size,uncomp_size);
   if(uncomp_buf == NULL)
    {
     printf("Error: unpacking Lzw i = %d\n",i);
     continue;
    }

   j = uncomp_buf[1];

   sprintf(outfilename,"%03d_",j);
   name_len = get_actor_name_length(&uncomp_buf[2]);
   strncpy(&outfilename[4],&uncomp_buf[2],name_len);
   outfilename[4+name_len] = '\0';

   strcat(outfilename,".conv");

   out_file = fopen(outfilename,"wb");
   fwrite(uncomp_buf,1,uncomp_size,out_file);
   fclose(out_file);
   free(buf);
   free(uncomp_buf);
  }

 return;
}


int main(void)
{
 Configuration config;
 U6Lib_n converse_a;
 U6Lib_n converse_b;
 U6Lzw lzw;
 std::string filename;
 FILE *out_file;
 char outfilename[100];
 unsigned char *buf;
 unsigned char *uncomp_buf;
 uint16 num_items;
 uint16 i;
 uint8 j;
 uint32 uncomp_size;
 uint32 buf_size;
 uint16 name_len;

 config.readConfigFile("nuvie.cfg","config");

 config.pathFromValue("config/ultima6/gamedir","converse.a",filename);
 converse_a.open(filename,4);
 unpack_converse_file(&converse_a, &lzw);

 config.pathFromValue("config/ultima6/gamedir","converse.b",filename);
 converse_b.open(filename,4);
 unpack_converse_file(&converse_b, &lzw);

 return 0;
}

