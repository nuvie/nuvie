// --------------------------------------------------
// a simple implementation of an equally simple stack
// --------------------------------------------------
#include "def.h"
#include "stack.h"

   static unsigned char stack[STACK_SIZE];
   static int contains;

   void s_init(void)
   {
      contains = 0;
   }

   BOOL s_is_empty(void)
   {
      return (contains==0);
   }

   BOOL s_is_full(void)
   {
      return(contains==STACK_SIZE);
   }

   void s_push(unsigned char element)
   {
      if (!s_is_full())
      {
         stack[contains] = element;
         contains++;   
      }
   }

   unsigned char s_pop(void)
   {
      unsigned char element;
      
      if (!s_is_empty())
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

   unsigned char s_gettop(void)
   {
      if (!s_is_empty())
      {
         return(stack[contains-1]);
      }
    return '\0'; /* what should we return here!? */
   }



