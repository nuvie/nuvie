/*
 * =============================================================
 * This program decompresses Ultima_6-style LZW-comrpessed files
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "stack.h"
#include "dict.h"

unsigned char read1(FILE *f)
{
    return(fgetc(f));
}

long read4(FILE *f)
{
    unsigned char b0, b1, b2, b3;
    b0 = fgetc(f);
    b1 = fgetc(f);
    b2 = fgetc(f);
    b3 = fgetc(f);

    return (b0 + (b1<<8) + (b2<<16) + (b3<<24));
}

long get_filesize(FILE *input_file)
{
   long file_length;
   fseek(input_file, 0, SEEK_END);
   file_length = ftell(input_file);
   fseek(input_file, 0, SEEK_SET);
   return(file_length);
}

/*
 * this function only checks a few *necessary* conditions
 * returns "FALSE" if the file doesn't satisfy these conditions
 * return "TRUE" otherwise
 */
BOOL is_valid_lzw_file(FILE *input_file)
{
 unsigned char byte3, b0, b1;

    /* file must contain 4-byte size header and space for the 9-bit value 0x100 */
    if (get_filesize(input_file) < 6) { return(FALSE); }
    /* the last byte of the size header must be 0 (U6's files aren't *that* big)*/
    fseek(input_file, 3, SEEK_SET);
    byte3 = fgetc(input_file); 
    fseek(input_file, 0, SEEK_SET);
    if (byte3 != 0) { return(FALSE); }
    /* the 9 bits after the size header must be 0x100 */
    fseek(input_file, 4, SEEK_SET);
    b0 = fgetc(input_file);
    b1 = fgetc(input_file);
    fseek(input_file, 0, SEEK_SET);
    if ((b0 != 0) || ((b1 & 1) != 1)) { return(FALSE); }

    return(TRUE);
}

long get_uncompressed_size(FILE *input_file)
{
   long uncompressed_file_length;

   if (is_valid_lzw_file(input_file))
   {   
      fseek(input_file, 0, SEEK_SET);
      uncompressed_file_length = read4(input_file);
      fseek(input_file, 0, SEEK_SET);
      return(uncompressed_file_length);
   }
   else { return (-1); }
}

/*
 * ----------------------------------------------
 * Read the next code word from the source buffer
 * ----------------------------------------------
 */
int get_next_codeword (long *bits_read, unsigned char *source, int codeword_size)
{
   unsigned char b0,b1,b2;
   int codeword;
   
   b0 = source[*bits_read/8];
   b1 = source[*bits_read/8+1];
   b2 = source[*bits_read/8+2];

   codeword = ((b2 << 16) + (b1 << 8) + b0);
   codeword = codeword >> (*bits_read % 8);
   switch (codeword_size)
   {
    case 0x9:
        codeword = codeword & 0x1ff;
        break;
    case 0xa:
        codeword = codeword & 0x3ff;
        break;
    case 0xb:
        codeword = codeword & 0x7ff;
        break;
    case 0xc:
        codeword = codeword & 0xfff;
        break;
    default:
        printf("Error: weird codeword size!\n");
        break;
   }
   *bits_read += codeword_size;

   return (codeword);
}

void output_root(unsigned char root, unsigned char *destination, long *position)
{
   destination[*position] = root;
   *position = *position + 1;   
}

void get_string(int codeword)
{
   unsigned char root;
   int current_codeword;
   
   current_codeword = codeword;
   s_init();
   while (current_codeword > 0xff)
   {
      root = d_get_root(current_codeword);
      current_codeword = d_get_codeword(current_codeword);
      s_push(root);
   }

   /* push the root at the leaf */
   s_push((unsigned char)current_codeword);
}

/*
 * -----------------------------------------------------------------------------
 * LZW-decompress from buffer to buffer.
 * The parameters "source_length" and "destination_length" are currently unused.
 * They might be used to prevent reading/writing outside the buffers.
 * -----------------------------------------------------------------------------
 */
void lzw_decompress_b(unsigned char *source, long source_length, unsigned char *destination, long destination_length)
{
   const int max_codeword_length = 12;

   BOOL end_marker_reached = FALSE;
   int codeword_size = 9;
   long bits_read = 0; 
   int next_free_codeword = 0x102;
   int dictionary_size = 0x200;

   long bytes_written = 0;

   int cW;
   int pW;
   unsigned char C;

   while (! end_marker_reached)
   {
      cW = get_next_codeword(&bits_read, source, codeword_size);
      switch (cW)
      {
      /* re-init the dictionary */
      case 0x100:
          codeword_size = 9;
          next_free_codeword = 0x102;
          dictionary_size = 0x200;
          d_init();
          cW = get_next_codeword(&bits_read, source, codeword_size);
          output_root((unsigned char)cW, destination, &bytes_written);
          break;
      /* end of compressed file has been reached */
      case 0x101:
          end_marker_reached = TRUE;
          break;
      /* (cW <> 0x100) && (cW <> 0x101) */
      default:
          if (cW < next_free_codeword)  /* codeword is already in the dictionary*/
          {
             /* create the string associated with cW (on the stack) */
             get_string(cW);
             C = s_gettop();
             /* output the string represented by cW */
             while (!s_is_empty())
             {
                output_root(s_pop(), destination, &bytes_written);
             }
             /* add pW+C to the dictionary */
             d_add(C,pW);

             next_free_codeword++;
             if (next_free_codeword >= dictionary_size)
             {
                if (codeword_size < max_codeword_length)
                {
                   codeword_size += 1;
                   dictionary_size *= 2;
                }
             }
          }
          else  /* codeword is not yet defined */
          {
             /* create the string associated with pW (on the stack) */
             get_string(pW);
             C = s_gettop();
             /* output the string represented by pW */
             while (!s_is_empty())
             {
                output_root(s_pop(), destination, &bytes_written);
             }
             /* output the char C */
             output_root(C, destination, &bytes_written);
             /* the new dictionary entry must correspond to cW */
             /* if it doesn't, something is wrong with the lzw-compressed data.*/
             if (cW != next_free_codeword)
             {
                printf("cW != next_free_codeword!\n");
                exit(-1);
             }
             /* add pW+C to the dictionary */
             d_add(C,pW);
             
             next_free_codeword++;
             if (next_free_codeword >= dictionary_size)
             {
                if (codeword_size < max_codeword_length)
                {                   
                   codeword_size += 1;
                   dictionary_size *= 2;
                } 
             }
          };
          break;
      }
      /* shift roles - the current cW becomes the new pW */
      pW = cW;
   }
}

/*
 * -----------------
 * from file to file
 * -----------------
 */
void lzw_decompress(FILE *input_file, FILE* output_file)
{
   unsigned char *source_buffer;
   unsigned char *destination_buffer;
   long source_buffer_size;
   long destination_buffer_size;

   if (is_valid_lzw_file(input_file))
   {
      /* determine the buffer sizes */
      source_buffer_size = get_filesize(input_file)-4;
      destination_buffer_size = get_uncompressed_size(input_file);

      /* create the buffers */
      source_buffer = (unsigned char *)malloc(sizeof(unsigned char *) * source_buffer_size);
      destination_buffer = (unsigned char *)malloc(sizeof(unsigned char *) * destination_buffer_size);

      /*  read the input file into the source buffer */
      fseek(input_file, 4, SEEK_SET);  
      fread(source_buffer, 1, source_buffer_size, input_file);

      /* decompress the input file */
      lzw_decompress_b(source_buffer,source_buffer_size,destination_buffer,destination_buffer_size);

      /* write the destination buffer to the output file */
      fwrite(destination_buffer, 1, destination_buffer_size, output_file);

      /* destroy the buffers */
      free(source_buffer);
      free(destination_buffer);
   }
}

/*
 * ----------------------------------------------------------
 * called if the program is run with 1 command line parameter
 * display uncompressed file size
 * ----------------------------------------------------------
 */
void one_argument(char *file_name) {
   FILE *compressed_file;
   long uncompressed_size;

   compressed_file = fopen(file_name,"rb");
   if (compressed_file==NULL) {
      printf("Couldn't open the file.\n");
   }
   else {
      uncompressed_size = get_uncompressed_size(compressed_file);
      if (uncompressed_size==(-1)) {
         printf("The input file is not a valid LZW-compressed file.\n");
      }
      else {     
         printf("The uncompressed file '%s' would be %ld bytes long.\n",file_name,get_uncompressed_size(compressed_file));
         fclose(compressed_file);
      }
   }
}
   
/*
 * -----------------------------------------------------------
 * called if the program is run with 2 command line parameters
 * decompress arg1 into arg2
 * -----------------------------------------------------------
 */
void two_arguments(char *source_file_name, char *destination_file_name)
{
   FILE *source;
   FILE *destination;

   if (strcmp (source_file_name, destination_file_name) == 0)
   {
      printf("Source and destination must not be identical.\n");
   }
   else
   {
      if (!(source=fopen(source_file_name,"rb"))||!(destination=fopen(destination_file_name,"wb")))
      {
         printf("Couldn't open '%s' or '%s' or both.\n.", source_file_name, destination_file_name);   
      }
      else
      {
         if (is_valid_lzw_file(source)) { lzw_decompress(source,destination); }
         else { printf("The input file is not a valid LZW-compressed file.\n"); }
      }
   }
}


int main (int argc, char *argv[]) {

/*
 * 0 args => print help message
 * 1 arg ==> display uncompressed file size, but don't decompress
 * 2 args => decompress arg1 into arg2
 */
 switch (argc)
 {
 case 1:
     printf("Usage:\n");
     printf("0 parameters - this message.\n");
     printf("1 parameter  - display uncompressed size.\n");
     printf("2 parameters - extract arg1 into arg2.\n");
     break;
 case 2:
     one_argument(argv[1]);
     break;
 case 3:
     two_arguments(argv[1],argv[2]);
     break;
 default:
     printf("Too many command line parameters.\n");
 }
 return 0;
}

