/* 
   --------------------------------------------------
   a dictionary
   --------------------------------------------------
*/
#include "dict.h"

static dict_entry dict[DICT_SIZE];
static int contains;

   void d_init(void)
   {
    contains = 0x102;
   }

   void d_add(unsigned char root, int codeword)
   {
      dict[contains].root = root;
      dict[contains].codeword = codeword;
      contains++;
   }

   unsigned char d_get_root(int codeword)
   {
      return (dict[codeword].root);
   }

   int d_get_codeword(int codeword)
   {
      return (dict[codeword].codeword);
   }
