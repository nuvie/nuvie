typedef struct {
      unsigned char root;
      int codeword;
      int contains;
   } dict_entry;

#define DICT_SIZE 10000

   void d_init(void);
   void d_add(unsigned char root, int codeword);
   unsigned char d_get_root(int codeword);
   int d_get_codeword(int codeword);

