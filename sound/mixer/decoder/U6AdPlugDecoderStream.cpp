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
#include "U6misc.h"
#include "NuvieIO.h"
#include "U6Lib_n.h"
#include "U6Lzw.h"

#include "U6AdPlugDecoderStream.h"


U6AdPlugDecoderStream::U6AdPlugDecoderStream(CEmuopl *o, std::string filename, uint16 song_num)
{
  opl = o;
  samples_left = 0;
  if(has_file_extension(filename.c_str(), ".lzc"))
  {
	  player = new CmidPlayer(opl);
	  ((CmidPlayer *)player)->load(filename, song_num);
  }
  else
  {
	  player = new Cu6mPlayer(opl);
	  player->load(filename.c_str());
  }
  player_refresh_count = (int)(opl->getRate() / player->getrefresh());
}

U6AdPlugDecoderStream::~U6AdPlugDecoderStream()
{
}

int U6AdPlugDecoderStream::readBuffer(sint16 *buffer, const int numSamples)
{
 sint32 i, j;
 short *data = (short *)buffer;

 int len = numSamples / 2;

 //DEBUG(0, LEVEL_INFORMATIONAL, "Get here. numSamples = %d player refreshrate = %f refresh_count = %d\n", numSamples, player->getrefresh(), (int)(opl->getRate() / player->getrefresh()));

 if(samples_left>0)
    {
	  if(samples_left > len)
	  {
		  opl->update(data, len);
		  samples_left -= len;
		  return numSamples;
	  }

      opl->update(data, samples_left);
      data += samples_left * 2;
      len -= samples_left;
      samples_left = 0;
    }

 for(i=len;i > 0;)
   {
     if(!player->update())
       {
         player->rewind();
         //SoundManager::g_MusicFinished = true;
         DEBUG(0,LEVEL_DEBUGGING,"Music Finished!\n");
       }

     j = (int)(opl->getRate() / player->getrefresh());
     if(j > i)
     {
    	 samples_left = j - i;
    	 j = i;
     }
     opl->update(data, j);

     data += j * 2;
     i -= j;
   }

 return numSamples;
}

