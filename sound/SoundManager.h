/*
 *  SoundManager.h
 *  Nuvie
 *
 *  Created by Adrian Boeing on Wed Jan 21 2004.
 *  Copyright (c) 2003. All rights reserved.
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

//notes: this file should be moved to the nuvie main directory instead of sound.

//priorities:
//todo: 
//-sample loading partialy implemented, will do later (now is 21/01/04)
//-make songs fade in & out - add query/callback for end of song so that they can cycle 
//-make samples sound from mapwindow
//-make samples fade in & out according to distance
//-try and use original .m files

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
#include "Sound.h"
#include "Song.h"
#include "nuvieDefs.h"
#include "Configuration.h"
#include "NuvieIOFile.h"

class CEmuopl;

class SoundManager {
public:
	SoundManager();
	~SoundManager();
	
	bool nuvieStartup(Configuration *config);
	bool initAudio();
	void update_map_sfx(); //updates the active sounds
    void update(); // at the moment this just changes songs if required
    
    void musicPlayFrom(string group);
    
    void musicPause();
    void musicPlay();
    
private:
	bool LoadCustomSongs(string scriptname);
    bool LoadNativeU6Songs();
    bool loadSong(Song *song, const char *filename);
    bool groupAddSong(char *group, Song *song);
    
	bool LoadObjectSamples(string sound_dir);
	bool LoadTileSamples(string sound_dir);

	Sound* SongExists(string name); //have we loaded this sound before?
	Sound* SampleExists(string name); //have we loaded this sound before?
    

	Sound* RequestTileSound(int id);
	Sound* RequestObjectSound(int id);
	Sound* RequestSong(string group); //request a song from this group
	
	map<int,SoundCollection *> m_TileSampleMap;
	map<int,SoundCollection *> m_ObjectSampleMap;
	map<string,SoundCollection *> m_MusicMap;
	list<Sound *> m_Songs;
	list<Sound *> m_Samples;
	Configuration *m_Config;

	//state info:
	string m_CurrentGroup;
	Sound *m_pCurrentSong;
	list<Sound *> m_ActiveSounds;
    bool audio_enabled;
    bool music_enabled;
    bool sfx_enabled;
    
    CEmuopl *opl;
public:
	static bool g_MusicFinished;
};

#endif
