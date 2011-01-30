/* Created by Eric Fry 
 * Copyright (C) 2011 The Nuvie Team
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

#include "nuvieDefs.h"
#include "NuvieIO.h"
#include "U6Lib_n.h"
#include "U6Lzw.h"

#include "FMtownsDecoderStream.h"

inline sint16 convert_sample(uint16 raw_sample);

FMtownsDecoderStream::FMtownsDecoderStream(std::string filename, uint16 sample_num)
{
	 unsigned char *compressed_data;
	 uint32 decomp_size;
	 U6Lib_n sam_file;
	 U6Lzw lzw;

	 sam_file.open(filename, 4);

	 compressed_data = sam_file.get_item(sample_num, NULL);
	 //raw_audio_buf = compressed_data;
	 //buf_len = sam_file.get_item_size(sample_num);
	 raw_audio_buf = lzw.decompress_buffer(compressed_data, sam_file.get_item_size(sample_num), decomp_size);

	 free(compressed_data);

	 buf_len = decomp_size;
	 buf_pos = 0;
}

FMtownsDecoderStream::~FMtownsDecoderStream()
{
	if(raw_audio_buf)
		free(raw_audio_buf);
}

int FMtownsDecoderStream::readBuffer(sint16 *buffer, const int numSamples)
{
	int j=0;
	uint32 i=buf_pos;
	//DEBUG(0,LEVEL_INFORMATIONAL, "numSamples = %d. buf_pos = %d, buf_len = %d\n", numSamples, buf_pos, buf_len);

	for(;j < numSamples && i < buf_len;)
	{
		buffer[j] = convert_sample(raw_audio_buf[i]);
		j++;
		i++;
	}

	buf_pos += j;
	//DEBUG(0,LEVEL_INFORMATIONAL, "read %d samples.\n", j);

	return j;
}

inline sint16 convert_sample(uint16 raw_sample)
{
 sint16 sample;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
 sint16 temp_sample;
#endif

 if(raw_sample & 128)
   sample = ((sint16)(abs(128 - raw_sample) * 256) ^ 0xffff)  + 1;
 else
   sample = raw_sample * 256;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
   temp_sample = sample >> 8;
   temp_sample |= (sample & 0xff) << 8;
   sample = temp_sample;
#endif

 return sample;
}
