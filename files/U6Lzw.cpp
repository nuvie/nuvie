//
// This code is a modified version of the code from nodling's Ultima 6 website.
// http://www.geocities.com/nodling/
//

// =============================================================
// This program decompresses Ultima_6-style LZW-comrpessed files
// =============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "U6def.h"
#include "U6Lzw.h"

static U6LzwDict *dict = NULL;
static U6LzwStack *stack = NULL;

U6Lzw::U6Lzw()
{
 if(dict == NULL)
    dict = new U6LzwDict;
 if(stack == NULL)
    stack = new U6LzwStack;
}

U6Lzw::~U6Lzw()
{
 //delete dict;
 //delete stack;
}

// this function only checks a few *necessary* conditions
// returns "FALSE" if the file doesn't satisfy these conditions
// return "TRUE" otherwise

bool U6Lzw::is_valid_lzw_file(U6File *input_file)
{
    // file must contain 4-byte size header and space for the 9-bit value 0x100
    if (input_file->filesize() < 6) { return(false); }

   // the last byte of the size header must be 0 (U6's files aren't *that* big)
    input_file->seek(3);
    unsigned char byte3 = input_file->read1(); 
    if (byte3 != 0) { return(false); }
    // the 9 bits after the size header must be 0x100
    input_file->seek(4);
    unsigned char b0 = input_file->read1();
    unsigned char b1 = input_file->read1();
    input_file->seekStart();
    if ((b0 != 0) || ((b1 & 1) != 1)) { return(false); }

    return(true);
}

bool U6Lzw::is_valid_lzw_buffer(unsigned char *buf, uint32 length)
{
 if(length < 6)
   return false;

 if(buf[3] != 0)
   return false;

 if((buf[4] != 0) || ((buf[5] & 1) != 1))
   return false;

 return true;
}

long U6Lzw::get_uncompressed_file_size(U6File *input_file)
{ 
    long uncompressed_file_length;
 
    if (is_valid_lzw_file(input_file))
    {   
       input_file->seekStart();
       uncompressed_file_length = input_file->read4();
       input_file->seekStart();
       return(uncompressed_file_length);
    }
    else { return (-1); }
 }

long U6Lzw::get_uncompressed_buffer_size(unsigned char *buf, uint32 length)
{ 
    if (is_valid_lzw_buffer(buf,length))
    {   
     return (buf[0] + (buf[1]<<8) + (buf[2]<<16) + (buf[3]<<24));
    }
    else { return (-1); }
 } 
 
 // -----------------------------------------------------------------------------
 // LZW-decompress from buffer to buffer.
 // The parameters "source_length" and "destination_length" are currently unused.
 // They might be used to prevent reading/writing outside the buffers.
 // -----------------------------------------------------------------------------
unsigned char *U6Lzw::decompress_buffer(unsigned char *source, uint32 source_length, uint32 &destination_length)
{
    const int max_codeword_length = 12;
 
    bool end_marker_reached = false;
    int codeword_size = 9;
    long bits_read = 0; 
    int next_free_codeword = 0x102;
    int dictionary_size = 0x200;
 
    long bytes_written = 0;
 
    int cW;
    int pW;
    unsigned char C;
    unsigned char *destination;
    
    destination_length = this->get_uncompressed_buffer_size(source,source_length);
    
    destination = (unsigned char *)malloc(destination_length);
    
    source += 4; //skip the filesize dword.
    
    while (! end_marker_reached)
    {
       cW = get_next_codeword(&bits_read, source, codeword_size);
       switch (cW)
       {
       // re-init the dictionary
       case 0x100:
           codeword_size = 9;
           next_free_codeword = 0x102;
           dictionary_size = 0x200;
           dict->reset();
           cW = get_next_codeword(&bits_read, source, codeword_size);
           output_root((unsigned char)cW, destination, &bytes_written);
           break;
       // end of compressed file has been reached
       case 0x101:
           end_marker_reached = true;
           break;
       // (cW <> 0x100) && (cW <> 0x101)
       default:
           if (cW < next_free_codeword)  // codeword is already in the dictionary
           {
              // create the string associated with cW (on the stack)
              get_string(cW);
              C = stack->gettop();
              // output the string represented by cW
              while (!stack->is_empty())
              {
                 output_root(stack->pop(), destination, &bytes_written);
              }
              // add pW+C to the dictionary
              dict->add(C,pW);
 
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
           else  // codeword is not yet defined
           {
              // create the string associated with pW (on the stack)
              get_string(pW);
              C = stack->gettop();
              // output the string represented by pW
              while (!stack->is_empty())
              {
                 output_root(stack->pop(), destination, &bytes_written);
              }
              // output the char C
              output_root(C, destination, &bytes_written);
              // the new dictionary entry must correspond to cW
              // if it doesn't, something is wrong with the lzw-compressed data.
              if (cW != next_free_codeword)
              {
                 printf("cW != next_free_codeword!\n");
                 exit(-1);
              }
              // add pW+C to the dictionary
              dict->add(C,pW);
              
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
       // shift roles - the current cW becomes the new pW
       pW = cW;
    }
    
 return destination;
}
 
 // -----------------
 // from file to file
 // -----------------
unsigned char *U6Lzw::decompress_file(std::string filename, uint32 &destination_length)
{
    unsigned char *source_buffer;
    unsigned char *destination_buffer;
    uint32 source_buffer_size;
    U6File input_file;
    
    destination_length = 0;
    if(input_file.open(filename,"rb") == false)
      return NULL;
    
    if (this->is_valid_lzw_file(&input_file))
    {
       // determine the buffer sizes
       source_buffer_size = input_file.filesize();
//       destination_buffer_size = this->get_uncompressed_file_size(input_file);
 
       // create the buffers
       source_buffer = (unsigned char *)malloc(sizeof(unsigned char *) * source_buffer_size);
//       destination_buffer = (unsigned char *)malloc(sizeof(unsigned char *) * destination_buffer_size);
 
       // read the input file into the source buffer
       input_file.seekStart();
       input_file.readToBuf(source_buffer,source_buffer_size);
 
       // decompress the input file
       destination_buffer = this->decompress_buffer(source_buffer,source_buffer_size,destination_length);
 
       // write the destination buffer to the output file
       //fwrite(destination_buffer, 1, destination_buffer_size, output_file);
 
       // destroy the buffers
       free(source_buffer);
       //free(destination_buffer);
    }
  
 return destination_buffer;
}

 // ----------------------------------------------
 // Read the next code word from the source buffer
 // ----------------------------------------------
int U6Lzw::get_next_codeword (long *bits_read, unsigned char *source, int codeword_size)
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
         printf("U6Lzw Error: weird codeword size!\n");
         break;
    }
    *bits_read += codeword_size;
 
    return (codeword);
}
 
void U6Lzw::output_root(unsigned char root, unsigned char *destination, long *position)
{
    destination[*position] = root;
    *position = *position + 1;   
}
 
void U6Lzw::get_string(int codeword)
{
    unsigned char root;
    int current_codeword;
    
    current_codeword = codeword;
    stack->reset();
    while (current_codeword > 0xff)
    {
       root = dict->get_root(current_codeword);
       current_codeword = dict->get_codeword(current_codeword);
       stack->push(root);
    }
 
    // push the root at the leaf
    stack->push((unsigned char)current_codeword);
}


U6LzwStack::U6LzwStack()
{
 this->reset();
}

void U6LzwStack::reset(void)
{
 contains = 0;
}

bool U6LzwStack::is_empty(void)
{
 if(contains == 0)
   return true;
   
 return false; 
}

bool U6LzwStack::is_full(void)
{
 if(contains==STACK_SIZE)
    return true;

 return false;
}

void U6LzwStack::push(unsigned char element)
{
 if(!this->is_full())
   {
    stack[contains] = element;
    contains++;   
   }
}

unsigned char U6LzwStack::pop(void)
{
 unsigned char element;
     
 if(!this->is_empty())
 {
  element = stack[contains-1];
  contains--;
 }
  else
 {
  element = 0;
 }
 return(element);
}

unsigned char U6LzwStack::gettop(void)
{
 if(!this->is_empty())
   {
    return(stack[contains-1]);
   }

 return '\0'; /* what should we return here!? */
} 

/* 
   --------------------------------------------------
   a dictionary class
   --------------------------------------------------
*/

U6LzwDict::U6LzwDict()
{
 this->reset();
}
 
void U6LzwDict::reset(void)
{
 contains = 0x102;
}

void U6LzwDict::add(unsigned char root, int codeword)
{
 dict[contains].root = root;
 dict[contains].codeword = codeword;
 contains++;
}

unsigned char U6LzwDict::get_root(int codeword)
{
 return (dict[codeword].root);
}

int U6LzwDict::get_codeword(int codeword)
{
 return (dict[codeword].codeword);
}

