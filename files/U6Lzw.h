#ifndef __U6Lzw_h__
#define __U6Lzw_h__

#include <string>

class NuvieIOFileRead;

// LZW Stack

#define STACK_SIZE 10000

class U6LzwStack
{
protected:
   unsigned char stack[STACK_SIZE];
   int contains;

public:
   U6LzwStack();

   void reset(void);
   bool is_empty(void);
   bool is_full(void);
   void push(unsigned char element);
   unsigned char pop(void);
   unsigned char gettop(void);
};

// LZW dictionary

#define DICT_SIZE 10000

typedef struct {
      unsigned char root;
      int codeword;
      int contains;
   } dict_entry;

class U6LzwDict
{
   dict_entry dict[DICT_SIZE];
   int contains;

 public:

   U6LzwDict();

   void reset(void);
   void add(unsigned char root, int codeword);
   unsigned char get_root(int codeword);
   int get_codeword(int codeword);
};

class U6Lzw
{
 U6LzwStack *stack;
 U6LzwDict *dict;
    char *errstr; // error string
 public:

  U6Lzw(void);
  ~U6Lzw(void);

  unsigned char *decompress_buffer(unsigned char *source, uint32 source_length, uint32 &destination_length);
  bool decompress_buffer(unsigned char *source, uint32 source_length, unsigned char *destination, uint32 destination_length);
  unsigned char *decompress_file(std::string filename, uint32 &destination_length);
  unsigned char *compress_buffer(unsigned char *src, uint32 src_len,
                                 uint32 &dest_len);
  const char *strerror() { return (const char *)errstr; } // get error string
 protected:

  bool is_valid_lzw_file(NuvieIOFileRead *input_file);
  bool is_valid_lzw_buffer(unsigned char *buf, uint32 length);

  long get_uncompressed_file_size(NuvieIOFileRead *input_file);
  long get_uncompressed_buffer_size(unsigned char *buf, uint32 length);

  int get_next_codeword (long *bits_read, unsigned char *source,
                         int codeword_size);
  void output_root(unsigned char root, unsigned char *destination,
                   long *position);
  void get_string(int codeword);
};

#endif /* __U6Lzw_h__ */

