/*
 *  SongAdPlug.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Feb 11 2004.
 *  Copyright (c) 2004. All rights reserved.
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

//Mix_HookMusicFinished

#include "nuvieDefs.h"
#include "adplug/emuopl.h"
#include "adplug/u6m.h"

#include "SongAdPlug.h"
#include "SoundManager.h"

#define DEFAULT_BUF_LEN 512

void adplug_mixer_callback(void *udata, Uint8 *stream, int len)
{
 sint32 i, j;
 SongAdPlug *song = (SongAdPlug *)udata;
 short *data = (short *)stream;
 CEmuopl *opl = song->get_opl();
 CPlayer *player = song->get_player();

 len /= 4;

 if(song->samples_left)
    {
      opl->update(data, song->samples_left);
      data += song->samples_left * 2;
    }

 len -= song->samples_left;

 for(i=len;i > 0; i -= 735)
   {
     if(!player->update())
       {
         player->rewind();
         SoundManager::g_MusicFinished = true;
         printf("Music Finished!\n");
       }

	 j = i;
	 if (j > 735) j = 735;
	 song->samples_left = 735 - j;

     opl->update(data, j);

     data += j * 2;
   }

 return;
}


SongAdPlug::SongAdPlug(CEmuopl *o) {
 opl = o;
 samples_left = 0;
}

SongAdPlug::~SongAdPlug() {
 delete player;
}

bool SongAdPlug::Init(const char *filename) {
    player = new Cu6mPlayer(opl);
    player->load(filename);
    if(!player)
      printf("Argh!!\n");

	return true;
}

bool SongAdPlug::Play(bool looping) {
    Mix_HookMusic(adplug_mixer_callback, this);
	return true;
}

bool SongAdPlug::Stop() {

	//if (!Mix_PlayingMusic()) return false;

    Mix_HookMusic(NULL,NULL);

	return true;
}

/*
bool SongAdPlug::Pause() {
	if (!Mix_PlayingMusic()) return false;
	return true;
}

bool SongAdPlug::Resume() {
	if (Mix_PlayingMusic()) return false;
	return true;
}
*/
