/* Created by Eric Fry 
 * Copyright (C) 2004 The Nuvie Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cmath>

#include "nuvieDefs.h"
#include "Game.h"
#include "Configuration.h"
#include "NuvieIO.h"
#include "U6Lib_n.h"
#include "U6Lzw.h"

#include "ConverseSpeech.h"

ConverseSpeech::ConverseSpeech()
{
    config = NULL;
}


/* Initialize global classes from the game.
 */
void ConverseSpeech::init(Configuration *cfg)
{
    config = cfg;
    config->value ("config/audio/enabled", audio_enabled, true);

}


ConverseSpeech::~ConverseSpeech()
{
}

void ConverseSpeech::update()
{
 Mix_Chunk *sample;
 
 if(!audio_enabled)
   return;

 if(!list.empty())
   {
    sample = list.front();
    if(!Mix_Playing(channel_num) || Mix_GetChunk(channel_num) != sample)
     {
      list.pop_front();
      Mix_FreeChunk(sample);
      if(!list.empty())
       {
        sample = list.front();
        channel_num = Mix_PlayChannel(-1, sample, 0);
       }
     }
   }
}

void ConverseSpeech::play_speech(uint16 actor_num, uint16 sample_num)
{
 std::string sample_file;
 char filename[20]; // "/speech/charxxx.sam"
 SDL_RWops *rw;
 NuvieIOBuffer *wav_buffer;
 unsigned char *raw_data;
 Mix_Chunk *sample;
 
 if(!audio_enabled)
   return;

 sample_num--;
 
 sprintf(filename, "speech%cchar%d.sam", U6PATH_DELIMITER, actor_num);
 
 config->pathFromValue("config/ultima6/townsdir", filename, sample_file);
 
 printf("Loading Speech Sample %s:%d\n", sample_file.c_str(), sample_num);
 
 wav_buffer = load_speech(sample_file, sample_num);

 raw_data = wav_buffer->get_raw_data();
 
 rw = SDL_RWFromMem(raw_data, wav_buffer->get_size());

 sample = Mix_LoadWAV_RW(rw, false);
 
 wav_buffer->close();
 free(raw_data);
 
 if(list.empty())
   channel_num = Mix_PlayChannel(0, sample, 0);
    
 list.push_back(sample);

 return;
}

NuvieIOBuffer *ConverseSpeech::load_speech(std::string filename, uint16 sample_num)
{
 unsigned char *compressed_data, *raw_audio, *wav_data;
 sint16 *converted_audio;
 uint32 decomp_size;
 uint32 upsampled_size;
 sint16 sample, prev_sample;
 U6Lib_n sam_file;
 U6Lzw lzw;
 NuvieIOBuffer *wav_buffer = 0;
 uint32 j, k;
 
 sam_file.open(filename, 4);
 
 compressed_data = sam_file.get_item(sample_num, NULL);
 raw_audio = lzw.decompress_buffer(compressed_data, sam_file.get_item_size(sample_num), decomp_size);
 
 free(compressed_data);
 
 if(raw_audio != NULL)
  {
   wav_buffer = new NuvieIOBuffer();
   upsampled_size = decomp_size + floor((decomp_size - 1) / 4) * (2 + 2 + 2 + 1);

   switch((decomp_size - 1) % 4)
    {
     case 1 : upsampled_size += 2; break;
     case 2 : upsampled_size += 4; break;
     case 3 : upsampled_size += 6; break;
    }
   
   printf("decomp_size %d, upsampled_size %d\n", decomp_size, upsampled_size);
   
   wav_data = (unsigned char *)malloc(upsampled_size * sizeof(sint16) + 44); // 44 = size of wav header
   
   wav_buffer->open(wav_data, upsampled_size * sizeof(sint16) + 44, false);
   wav_init_header(wav_buffer, upsampled_size);
   
   converted_audio = (sint16 *)&wav_data[44];
   
   prev_sample = convert_sample(raw_audio[0]); 
   
   for(j=1,k=0;j<decomp_size;j++,k++)
    {
     converted_audio[k] = prev_sample;

     sample = convert_sample(raw_audio[j]);

     if(j < decomp_size - 1)
      {
       switch(j % 4) // calculate the in-between samples using linear interpolation.
       {
        case 0 : 
        case 1 : 
        case 2 : 
                 converted_audio[k+1] = (sint16)(0.666 * (float)prev_sample + 0.333 * (float)sample);
                 converted_audio[k+2] = (sint16)(0.333 * (float)prev_sample + 0.666 * (float)sample);             
                 k += 2;
                 break;
        case 3 : converted_audio[k+1] = (sint16)(0.5 * (float)(prev_sample + sample));
                 k += 1;
                 break;
       }
      }
     prev_sample = sample;
    }
  }
 
 free(raw_audio);
  
 return wav_buffer;
}

inline sint16 ConverseSpeech::convert_sample(uint16 raw_sample)
{
 sint16 sample;
#ifdef BIG_ENDIAN
 sint16 temp_sample;
#endif
 
 if(raw_sample & 128)
   sample = ((sint16)(abs(128 - raw_sample) * 256) ^ 0xffff)  + 1;
 else
   sample = raw_sample * 256;

#ifdef BIG_ENDIAN
   temp_sample = sample >> 8;
   temp_sample |= (sample & 0xff) << 8;
   sample = temp_sample;
#endif

 return sample;
}

void ConverseSpeech::wav_init_header(NuvieIOBuffer *wav_buffer, uint32 audio_length)
{
 wav_buffer->writeBuf((unsigned char*)"RIFF", 4);
 wav_buffer->write4(36 + audio_length * 2); //length of RIFF chunk
 wav_buffer->writeBuf((unsigned char*)"WAVE", 4);
 wav_buffer->writeBuf((unsigned char*)"fmt ", 4);
 wav_buffer->write4(16); // length of format chunk
 wav_buffer->write2(1); // PCM encoding
 wav_buffer->write2(1); // mono
 wav_buffer->write4(44100); // sample frequency 16KHz
 wav_buffer->write4(44100 * 2); // sample rate
 wav_buffer->write2(2); // BlockAlign 
 wav_buffer->write2(16); // Bits per sample 

 wav_buffer->writeBuf((unsigned char*)"data", 4);
 wav_buffer->write4(audio_length * 2); // length of data chunk 

 return;
}
