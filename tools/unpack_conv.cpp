#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "misc/U6def.h"
#include "files/U6Lib_n.h"
#include "files/U6Lzw.h"
#include "conf/Configuration.h"

using std::string;

void pack_converse_file(U6Lib_n *converse, U6Lzw *lzw, const char *indexname);

uint16 get_actor_name_length(unsigned char *buf)
{
 uint16 i;
 
 for(i=0;buf[i] != 0xf1 && buf[i] != 0xf3;)
  i++;

 return i;
}


/* Compress each entry listed in the index file and recompile them, saving to
 * `converse'.
 */
void pack_converse_file(U6Lib_n *converse, U6Lzw *lzw, const char *indexname)
{
    FILE *src_f = 0, *index_f = 0;
    int num_items = 0, num_added = 0;
    unsigned char *buf = NULL, *comp_buf = NULL;
    uint32 buf_size = 0, comp_size = 0, b = 0;

    index_f = fopen(indexname, "rb");
    converse->load_index(index_f);
    num_items = converse->get_num_items();
    for(int i = 0; i < num_items; i++)
    {
        if(strlen(converse->item_name(i)))
        {
            std::cout << "<- `" << converse->item_name(i) << "'" << std::endl;
            src_f = fopen(converse->item_name(i), "rb");
            if(!src_f)
            {
                printf("Item %d: Error opening file!\n", i);
                break;
            }
            while(!feof(src_f))
            {
                buf = (unsigned char *)realloc(buf, buf_size + 1);
                fread(&buf[buf_size++], 1, 1, src_f);
            }
            fclose(src_f);
        }
        if(buf_size)
        {
#ifdef USE_LZW
#error LZW compression does not work yet
            comp_buf = lzw->compress_buffer(buf, buf_size, comp_size);
            if(!comp_buf)
            {
                cout << "Error: couldn't Lzw compress data, item " << i << "\n"
                     << endl;
                comp_size = 0;
            }
#else
            // write unencoded data (uncompressed size == 0)
            comp_size = buf_size + sizeof(uint32);
            comp_buf = (unsigned char *)malloc(comp_size);
            memset(comp_buf, 0, sizeof(uint32));
            memcpy(comp_buf + sizeof(uint32), buf, buf_size);
#endif
            converse->add_item(comp_buf, comp_size);
            ++num_added;
            free(buf); buf = NULL; buf_size = 0;
            if(comp_size)
            {
                comp_size = 0;
                free(comp_buf);
                comp_buf = NULL;
            }
        }
        else
        {
            std::cout << "<- (no data)" << std::endl;
            converse->add_item(NULL, 0);
        }
    }
    converse->calc_item_offsets();
    converse->write_index();
    if(num_added)
        converse->write_items();
    fclose(index_f);
}


void unpack_converse_file(U6Lib_n *converse, U6Lzw *lzw, const char *indexname)
{
 uint16 i;
 uint8 j;
 unsigned char *buf;
 uint32 buf_size, buf_loc = 0;
 unsigned char *uncomp_buf;
 uint32 uncomp_size;
 FILE *out_file, *indexfile = NULL;
 char outfilename[100];
 uint16 name_len;
 uint32 num_items;

 num_items = converse->get_num_items();
 if(indexname && strlen(indexname))
 {
  indexfile = fopen(indexname, "w");
  fprintf(indexfile, "# U6 Conversation library index\n"
                     "# %d item(s)\n"
                     "# Offset  Filename\n", num_items);
 }
 std::cout << "This file contains " << num_items << " scripts." << std::endl;
 for(i=0;i < num_items; i++)
  {
   buf_size = converse->get_item_size(i);
   buf_loc = converse->get_item_offset(i);
   fprintf(indexfile, "%08x  ", buf_loc);
   buf = converse->get_item(i);
    if(!buf)
    {
        buf = (unsigned char *)malloc(buf_size);
        memset(buf, 0, buf_size);
    }
    if(buf_size >= 4 && (buf[0] + buf[1] + buf[2] + buf[3]) == 0)
    {
        // data is not compressed (only seen in one item of converse.b)
        uncomp_size = buf_size - 4;
        uncomp_buf = (unsigned char *)malloc(uncomp_size);
        memcpy(uncomp_buf, buf + 4, uncomp_size);
    }
    else
        uncomp_buf = lzw->decompress_buffer(buf,buf_size,uncomp_size);
   if(uncomp_buf == NULL)
    {
     printf("Error: unpacking Lzw i = %d\n",i);
     printf("Error: (%s)\n", lzw->strerror());
     // write data that couldn't be decoded
     if(buf_size)
     {
        snprintf(outfilename, 100, "item%03d.unk", i, buf_loc);
        out_file = fopen(outfilename, "wb");
        fwrite(buf, 1, buf_size, out_file);
        printf("Wrote item data to %s\n", outfilename);
        free(buf);
        fclose(out_file);
        fprintf(indexfile, "%s", outfilename);
     }
     fputc('\n', indexfile);
     continue;
    }

   j = uncomp_buf[1];

   sprintf(outfilename,"%03d_",j);
   name_len = get_actor_name_length(&uncomp_buf[2]);
   strncpy(&outfilename[4], &((char *)uncomp_buf)[2], name_len);
   outfilename[4+name_len] = '\0';

   strcat(outfilename,".conv");

   std::cout << "-> " << outfilename << std::endl;
   out_file = fopen(outfilename,"wb");
   fwrite(uncomp_buf,1,uncomp_size,out_file);
   fclose(out_file);
   fprintf(indexfile, "%s\n", outfilename);
   free(buf);
   free(uncomp_buf);
  }
 fclose(indexfile);
 return;
}


int main(int argc, char **argv)
{
 Configuration config;
 U6Lib_n converse_a, converse_b, converse_a_out, converse_b_out;
 U6Lzw lzw;
 string filename;
 uint32 arg = 1;
 bool repack = false; // Repack files instead of Unpack?
 // get args
 while(arg < argc)
 {
    if(argv[arg][0] == '-' && strlen(argv[arg]) >= 2)
        if(argv[arg][1] == 'r') // re-pack
            repack = true;
    arg++;
 }

 std::cout << "Using Nuvie configuration `" << "nuvie.cfg" << "'." << std::endl;
 config.readConfigFile("nuvie.cfg","config");

 if(!repack)
 {
    std::cout << "Unpacking files converse.a and converse.b from the game"
            " directory..." << std::endl;
    config.pathFromValue("config/ultima6/gamedir", "converse.a", filename);
    std::cout << "--converse.a--" << std::endl;
    converse_a.open(filename, 4);
    unpack_converse_file(&converse_a, &lzw, "converse.a.index");
    std::cout << "--converse.b--" << std::endl;
    config.pathFromValue("config/ultima6/gamedir", "/converse.b", filename);
    converse_b.open(filename, 4);
    unpack_converse_file(&converse_b, &lzw, "converse.b.index");
 }
 else
 {
    std::cout << "Repacking scripts to files converse.a"
            " and converse.b in the current directory..." << std::endl;
    std::cout << "--converse.a--" << std::endl;
    filename = "converse.a";
    converse_a_out.create(filename, 4);
    pack_converse_file(&converse_a_out, &lzw, "converse.a.index");
    std::cout << "--converse.b--" << std::endl;
    filename = "converse.b";
    converse_b_out.create(filename, 4);
    pack_converse_file(&converse_b_out, &lzw, "converse.b.index");
 }
 return(0);
}

