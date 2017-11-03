#include <cstdlib>

#include "nuvieDefs.h"
#include "NuvieIOFile.h"
#include "U6Lzw.h"
#include "U6Lib_n.h"

bool write_wav_file(const char *filename, unsigned char *audio_data, uint32 length);


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,  LPSTR lpCmdLine, int iShowCmd) {
	//SDL_SetModuleHandle(GetModuleHandle(NULL));
	return main(__argc, __argv);
}
#endif

int main(int argc, char *argv[])
{
 U6Lib_n lib;
 U6Lzw lzw;
 string filename;
 uint16 i, num_items;
 uint32 item_size;
 uint32 decomp_size, j;
 unsigned char *item;
 unsigned char *raw_audio;
 uint16 *converted_audio;
#if BYTE_ORDER == BIG_ENDIAN
 uint16 temp_sample;
#endif
 char wave_file[16]; // "char000_000.wav"
 uint16 conv_number;

 if(argc < 2)
  {
   fprintf(stderr,"Usage: unpack inputfile index_number\n");
   exit(1);
  }

 filename.assign(argv[1]);
 conv_number = atoi(argv[2]);

 lib.open(filename,4);

 num_items = lib.get_num_items();

 for(i=0;i<num_items;i++)
  {
   item_size = lib.get_item_size(i);
   if(item_size == 0)
     continue;
   item = lib.get_item(i);
   raw_audio = lzw.decompress_buffer(item, item_size, decomp_size);

   if(raw_audio != NULL)
    {
     converted_audio = (uint16 *)malloc(decomp_size * sizeof(uint16));

//   fprintf(stderr,"uncompressed size = %d\n",decomp_size);

     for(j=0;j<decomp_size;j++)
      {
       if(raw_audio[j] & 128)
         converted_audio[j] = ((sint16)(abs(128 - raw_audio[j]) * 256) ^ 0xffff) + 1;
       else
         converted_audio[j] = (uint16)raw_audio[j] * 256;
#if BYTE_ORDER == BIG_ENDIAN
       temp_sample = converted_audio[j] >> 8;
       temp_sample |= (converted_audio[j] & 0xff) << 8;
       converted_audio[j] = temp_sample;
#endif
      }

     sprintf(wave_file,"char%03d_%03d.wav", conv_number, i);
     write_wav_file(wave_file, (unsigned char *)converted_audio, decomp_size);

     free(raw_audio);
     free(converted_audio);
    }
  }

 lib.close();

 return 0;
}


bool write_wav_file(const char *filename, unsigned char *audio_data, uint32 length)
{
 NuvieIOFileWrite file;
 file.open(filename);
 
 file.writeBuf((const unsigned char*)"RIFF", 4);
 file.write4(36 + length * 2); //length of RIFF chunk
 file.writeBuf((const unsigned char*)"WAVE", 4);
 file.writeBuf((const unsigned char*)"fmt ", 4);
 file.write4(16); // length of format chunk
 file.write2(1); // PCM encoding
 file.write2(1); // mono
 file.write4(16000); // sample frequency 16KHz
 file.write4(16000 * 2); // sample rate
 file.write2(2); // BlockAlign 
 file.write2(16); // Bits per sample 

 file.writeBuf((const unsigned char*)"data", 4);
 file.write4(length * 2); // length of data chunk 
 file.writeBuf(audio_data, length * 2);

 file.close();

 return true;
}

