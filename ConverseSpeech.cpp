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
 uint16 *converted_audio;
 uint32 decomp_size;
#ifdef BIG_ENDIAN
 uint16 temp_sample;
#endif
 U6Lib_n sam_file;
 U6Lzw lzw;
// NuvieIOBuffer *wav_buffer;
 NuvieIOBuffer *wav_buffer = 0;
 uint32 j;
 
 sam_file.open(filename, 4);
 
 compressed_data = sam_file.get_item(sample_num, NULL);
 raw_audio = lzw.decompress_buffer(compressed_data, sam_file.get_item_size(sample_num), decomp_size);
 
 
 free(compressed_data);
 
 if(raw_audio != NULL)
  {
   wav_buffer = new NuvieIOBuffer();
   wav_data = (unsigned char *)malloc(decomp_size * sizeof(uint16) + 44); // 44 = size of wav header
   
   wav_buffer->open(wav_data, decomp_size * sizeof(uint16) + 44, false);
   wav_init_header(wav_buffer, decomp_size);
   
   converted_audio = (uint16 *)&wav_data[44];
   
   for(j=0;j<decomp_size;j++)
    {
     if(raw_audio[j] & 128)
       converted_audio[j] = ((sint16)(abs(128 - raw_audio[j]) * 256) ^ 0xffff)  + 1;
     else
       converted_audio[j] = (uint16)raw_audio[j] * 256;
#ifdef BIG_ENDIAN
     temp_sample = converted_audio[j] >> 8;
     temp_sample |= (converted_audio[j] & 0xff) << 8;
     converted_audio[j] = temp_sample;
#endif
    }
  }
 
 free(raw_audio);
 
 return wav_buffer;
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
 wav_buffer->write4(16000); // sample frequency 16KHz
 wav_buffer->write4(16000 * 2); // sample rate
 wav_buffer->write2(2); // BlockAlign 
 wav_buffer->write2(16); // Bits per sample 

 wav_buffer->writeBuf((unsigned char*)"data", 4);
 wav_buffer->write4(audio_length * 2); // length of data chunk 

 return;
}
