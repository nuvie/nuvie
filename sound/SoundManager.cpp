/*
 *  SoundManager.cpp
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

#include "nuvieDefs.h"
#include "U6misc.h"
#include "U6objects.h"
#include "emuopl.h"
#include "SoundManager.h"
#include "SongAdPlug.h"
#include "Sample.h"
#include <algorithm>
#include <cmath>

#include "Game.h"
#include "Player.h"
#include "MapWindow.h"

#include "mixer.h"
#include "doublebuffersdl-mixer.h"
#include "decoder/FMtownsDecoderStream.h"

#include "TownsSfxManager.h"
#include "CustomSfxManager.h"

typedef struct // obj sfx lookup
{
    uint16 obj_n;
    SfxIdType sfx_id;
} ObjSfxLookup;

#define SOUNDMANANGER_OBJSFX_TBL_SIZE 5

static const ObjSfxLookup u6_obj_lookup_tbl[] = {
		{OBJ_U6_FOUNTAIN, NUVIE_SFX_FOUNTAIN},
		{OBJ_U6_FIREPLACE, NUVIE_SFX_FIRE},
		{OBJ_U6_CLOCK, NUVIE_SFX_CLOCK},
		{OBJ_U6_PROTECTION_FIELD, NUVIE_SFX_PROTECTION_FIELD},
		{OBJ_U6_WATER_WHEEL, NUVIE_SFX_WATER_WHEEL}
};



bool SoundManager::g_MusicFinished;

void musicFinished ()
{
  SoundManager::g_MusicFinished = true;
}

SoundManager::SoundManager ()
{
  m_pCurrentSong = NULL;
  g_MusicFinished = true;

  audio_enabled = false;
  music_enabled = false;
  sfx_enabled = false;

  m_SfxManager = NULL;

  opl = NULL;
}

// function object to delete map<T, SoundCollection *> items
template <typename T>
class SoundCollectionMapDeleter
{
public:
   void operator ()(const std::pair<T,SoundCollection *>& mapEntry)
   {
      delete mapEntry.second;
   }
};

SoundManager::~SoundManager ()
{
  //thanks to wjp for this one
  while (!m_Songs.empty ())
    {
      delete *(m_Songs.begin ());
      m_Songs.erase (m_Songs.begin ());
    }
  while (!m_Samples.empty ())
    {
      delete *(m_Samples.begin ());
      m_Samples.erase (m_Samples.begin ());
    }

  delete opl;

  std::for_each(m_ObjectSampleMap.begin(), m_ObjectSampleMap.end(), SoundCollectionMapDeleter<int>());
  std::for_each(m_TileSampleMap.begin(), m_TileSampleMap.end(), SoundCollectionMapDeleter<int>());
  std::for_each(m_MusicMap.begin(), m_MusicMap.end(), SoundCollectionMapDeleter<string>());

  if(audio_enabled)
	  mixer->suspendAudio();

}

bool SoundManager::nuvieStartup (Configuration * config)
{
  std::string config_key;
  std::string music_style;
  std::string music_cfg_file; //full path and filename to music.cfg
  std::string sound_dir;
  std::string sfx_style;

  m_Config = config;
  m_Config->value ("config/audio/enabled", audio_enabled, true);

  if(audio_enabled == false)
     {
      music_enabled = false;
      sfx_enabled = false;
      music_volume = 0;
      sfx_volume = 0;
      return false;
     }

  m_Config->value ("config/audio/enable_music", music_enabled, true);
  m_Config->value ("config/audio/enable_sfx", sfx_enabled, true);

  int volume;

  m_Config->value("config/audio/music_volume", volume, Audio::Mixer::kMaxChannelVolume);
  music_volume = clamp(volume, 0, 255);

  m_Config->value("config/audio/sfx_volume", volume, Audio::Mixer::kMaxChannelVolume);
  sfx_volume = clamp(volume, 0, 255);

  config_key = config_get_game_key(config);
  config_key.append("/music");
  config->value (config_key, music_style, "native");

  config_key = config_get_game_key(config);
  config_key.append("/sfx");
  config->value (config_key, sfx_style, "pcspeaker");

  config_key = config_get_game_key(config);
  config_key.append("/sounddir");
  config->value (config_key, sound_dir, "");

  if(!initAudio ())
    {
     return false;
    }

  if(music_enabled)
    {
     if(music_style == "native") 
       {
         int game_type; //FIXME there's a nuvie_game_t, but almost everything uses int game_type (or gametype)
         config->value("config/GameType",game_type);
         if (game_type == NUVIE_GAME_U6)
	   LoadNativeU6Songs(); //FIX need to handle MD & SE music too.
       }
     else if (music_style == "custom")
       LoadCustomSongs(sound_dir);
     else
       DEBUG(0,LEVEL_WARNING,"Unknown music style '%s'\n", music_style.c_str());

     musicPlayFrom("random");
    }

  if(sfx_enabled)
    {
     //LoadObjectSamples(sound_dir);
     //LoadTileSamples(sound_dir);
     LoadSfxManager(sfx_style);
    }

  return true;
}

bool SoundManager::initAudio()
{

#ifdef MACOSX
  mixer = new DoubleBufferSDLMixerManager();
#else
  mixer = new SdlMixerManager();
#endif

  mixer->init();

  opl = new CEmuopl(mixer->getOutputRate(), true, true); // 16bit stereo

  return true;
}

bool SoundManager::LoadNativeU6Songs()
{
 Song *song;

 string filename;

 config_get_path(m_Config, "brit.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
// loadSong(song, filename.c_str());
 loadSong(song, filename.c_str(), "Rule Britannia");
 groupAddSong("random", song);

 config_get_path(m_Config, "forest.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Wanderer (Forest)");
 groupAddSong("random", song);

 config_get_path(m_Config, "stones.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Stones");
 groupAddSong("random", song);

 config_get_path(m_Config, "ultima.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Ultima VI Theme");
 groupAddSong("random", song);

 config_get_path(m_Config, "engage.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Engagement and Melee");
 groupAddSong("combat", song);

 config_get_path(m_Config, "hornpipe.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Captain Johne's Hornpipe");
 groupAddSong("boat", song);

 config_get_path(m_Config, "gargoyle.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Audchar Gargl Zenmur");
 groupAddSong("gargoyle", song);

 config_get_path(m_Config, "dungeon.m", filename);
 song = new SongAdPlug(mixer->getMixer(), opl);
 loadSong(song, filename.c_str(), "Dungeon");
 groupAddSong("dungeon", song);

 return true;
}

bool SoundManager::LoadCustomSongs (string sound_dir)
{
  char seps[] = ";\r\n";
  char *token1;
  char *token2;
  char *sz;
  NuvieIOFileRead niof;
  Song *song;
  std::string scriptname;
  std::string filename;

  build_path(sound_dir, "music.cfg", scriptname);

  if(niof.open (scriptname) == false)
    return false;

  sz = (char *)niof.readAll();

  if(sz == NULL)
    return false;

  token1 = strtok (sz, seps);
  for( ; (token1 != NULL) && ((token2 = strtok(NULL, seps)) != NULL) ; token1 = strtok(NULL, seps))
    {
      build_path(sound_dir, token2, filename);

      song = (Song *)SongExists(token2);
      if(song == NULL)
        {
          song = new Song;
          if(!loadSong(song, filename.c_str()))
            continue; //error loading song
        }

      if(groupAddSong(token1, song))
        DEBUG(0,LEVEL_DEBUGGING,"%s : %s\n", token1, token2);
    }

  free(sz);

  return true;
}

bool SoundManager::loadSong(Song *song, const char *filename)
{
  if(song->Init (filename))
    {
      m_Songs.push_back(song);       //add it to our global list
      return true;
    }
  else
    {
      DEBUG(0,LEVEL_ERROR,"could not load %s\n", filename);
    }

 return false;
}

// (SB-X)
bool SoundManager::loadSong(Song *song, const char *filename, const char *title)
{
    if(loadSong(song, filename) == true)
    {
        song->SetName(title);
        return true;
    }
    return false;
}

bool SoundManager::groupAddSong (const char *group, Song *song)
{
  if(song != NULL)
    {                       //we have a valid song
      SoundCollection *psc;
      std::map < string, SoundCollection * >::iterator it;
      it = m_MusicMap.find(group);
      if(it == m_MusicMap.end())
        {                   //is there already a collection for this entry?
          psc = new SoundCollection;        //no, create a new sound collection
          psc->m_Sounds.push_back(song);     //add this sound to the collection
          m_MusicMap.insert(std::make_pair (group, psc)); //insert this pair into the map
        }
      else
        {
          psc = (*it).second;       //yes, get the existing
          psc->m_Sounds.push_back(song);     //add this sound to the collection
        }
    }

  return true;
};

/*
bool SoundManager::LoadObjectSamples (string sound_dir)
{
  char seps[] = ";\r\n";
  char *token1;
  char *token2;
  NuvieIOFileRead niof;
  char *sz;
  string samplename;
  string scriptname;

  build_path(sound_dir, "obj_samples.cfg", scriptname);

  if(niof.open (scriptname) == false)
    return false;

  sz = (char *) niof.readAll ();

  token1 = strtok (sz, seps);

  while ((token1 != NULL) && ((token2 = strtok (NULL, seps)) != NULL))
    {
      int id = atoi (token1);
      DEBUG(0,LEVEL_DEBUGGING,"%d : %s\n", id, token2);
      Sound *ps;
      ps = SampleExists (token2);
      if (ps == NULL)
        {
          Sample *s;
          s = new Sample;
          build_path(sound_dir, token2, samplename);
          if (!s->Init (samplename.c_str ()))
            {
              DEBUG(0,LEVEL_ERROR,"could not load %s\n", samplename.c_str ());
            }
          ps = s;
          m_Samples.push_back (ps);     //add it to our global list
        }
      if (ps != NULL)
        {                       //we have a valid sound
          SoundCollection *psc;
          std::map < int, SoundCollection * >::iterator it;
          it = m_ObjectSampleMap.find (id);
          if (it == m_ObjectSampleMap.end ())
            {                   //is there already a collection for this entry?
              psc = new SoundCollection;        //no, create a new sound collection
              psc->m_Sounds.push_back (ps);     //add this sound to the collection
              m_ObjectSampleMap.insert (std::make_pair (id, psc));      //insert this pair into the map
            }
          else
            {
              psc = (*it).second;       //yes, get the existing
              psc->m_Sounds.push_back (ps);     //add this sound to the collection
            }
        }
      token1 = strtok (NULL, seps);
    }
  return true;
};

bool SoundManager::LoadTileSamples (string sound_dir)
{
  char seps[] = ";\r\n";
  char *token1;
  char *token2;
  NuvieIOFileRead niof;
  char *sz;
  string samplename;
  string scriptname;

  build_path(sound_dir, "tile_samples.cfg", scriptname);

  if(niof.open (scriptname) == false)
    {
     DEBUG(0,LEVEL_ERROR,"opening %s\n",scriptname.c_str());
     return false;
    }

  sz = (char *) niof.readAll ();

  token1 = strtok (sz, seps);

  while ((token1 != NULL) && ((token2 = strtok (NULL, seps)) != NULL))
    {
      int id = atoi (token1);
      DEBUG(0,LEVEL_DEBUGGING,"%d : %s\n", id, token2);
      Sound *ps;
      ps = SampleExists (token2);
      if (ps == NULL)
        {
          Sample *s;
          s = new Sample;
          build_path(sound_dir, token2, samplename);
          if (!s->Init (samplename.c_str ()))
            {
              DEBUG(0,LEVEL_ERROR,"could not load %s\n", samplename.c_str ());
            }
          ps = s;
          m_Samples.push_back (ps);     //add it to our global list
        }
      if (ps != NULL)
        {                       //we have a valid sound
          SoundCollection *psc;
          std::map < int, SoundCollection * >::iterator it;
          it = m_TileSampleMap.find (id);
          if (it == m_TileSampleMap.end ())
            {                   //is there already a collection for this entry?
              psc = new SoundCollection;        //no, create a new sound collection
              psc->m_Sounds.push_back (ps);     //add this sound to the collection
              m_TileSampleMap.insert (std::make_pair (id, psc));        //insert this pair into the map
            }
          else
            {
              psc = (*it).second;       //yes, get the existing
              psc->m_Sounds.push_back (ps);     //add this sound to the collection
            }
        }
      token1 = strtok (NULL, seps);
    }
  return true;
};
*/
bool SoundManager::LoadSfxManager(string sfx_style)
{
	if(m_SfxManager != NULL)
	{
		return false;
	}

	if(sfx_style == "pcspeaker")
	{
		//m_SfxManager = new PCSpeakerSfxManager(m_Config, mixer->getMixer());
	}
	else if(sfx_style == "towns")
	{
		m_SfxManager = new TownsSfxManager(m_Config, mixer->getMixer());
	}
	else if(sfx_style == "custom")
	{
		m_SfxManager = new CustomSfxManager(m_Config, mixer->getMixer());
	}

	return true;
}

void SoundManager::musicPlayFrom(string group)
{
 if(m_CurrentGroup != group)
  {
   g_MusicFinished = true;
   m_CurrentGroup = group;
  }
}

void SoundManager::musicPause()
{
 //Mix_PauseMusic();
  if (m_pCurrentSong != NULL)
        {
          m_pCurrentSong->Stop();
        }
}

void SoundManager::musicPlay()
{
// Mix_ResumeMusic();

 // (SB-X) Get a new song if stopped.
 if(m_pCurrentSong == NULL)
        m_pCurrentSong = RequestSong(m_CurrentGroup);

 if (m_pCurrentSong != NULL)
        {
         m_pCurrentSong->Play();
         m_pCurrentSong->SetVolume(music_volume);
        }

}

// (SB-X) Stop the current song so a new song will play when resumed.
void SoundManager::musicStop()
{
    musicPause();
    m_pCurrentSong = NULL;
}

std::list < SoundManagerSfx >::iterator SoundManagerSfx_find( std::list < SoundManagerSfx >::iterator first, std::list < SoundManagerSfx >::iterator last, const SfxIdType &value )
  {
    for ( ;first!=last; first++)
    {
    	if ( (*first).sfx_id==value )
    		break;
    }
    return first;
  }

void SoundManager::update_map_sfx ()
{
  unsigned int i;
  uint16 x, y;
  uint8 l;

  if(sfx_enabled == false)
    return;

  string next_group = "";
  Player *p = Game::get_game ()->get_player ();
  MapWindow *mw = Game::get_game ()->get_map_window ();

  vector < SfxIdType >currentlyActiveSounds;
  map < SfxIdType, float >volumeLevels;

  p->get_location (&x, &y, &l);

  //m_ViewableTiles

  //get a list of all the sounds
  for (i = 0; i < mw->m_ViewableObjects.size(); i++)
    {
      //DEBUG(0,LEVEL_DEBUGGING,"%d %s",mw->m_ViewableObjects[i]->obj_n,Game::get_game()->get_obj_manager()->get_obj_name(mw->m_ViewableObjects[i]));
      SfxIdType sfx_id = RequestObjectSfxId(mw->m_ViewableObjects[i]->obj_n); //does this object have an associated sound?
      if (sfx_id != NUVIE_SFX_NONE)
        {
          //calculate the volume
          uint16 ox = mw->m_ViewableObjects[i]->x;
          uint16 oy = mw->m_ViewableObjects[i]->y;
          float dist = sqrtf ((float) (x - ox) * (x - ox) + (float) (y - oy) * (y - oy));
          float vol = (8.0f - dist) / 8.0f;
          if (vol < 0)
            vol = 0;
          //sp->SetVolume(vol);
          //need the map to adjust volume according to number of active elements
          std::map < SfxIdType, float >::iterator it;
          it = volumeLevels.find (sfx_id);
          if (it != volumeLevels.end ())
            {
              if (volumeLevels[sfx_id] < vol)
                volumeLevels[sfx_id] = vol;
            }
          else
            {
              volumeLevels.insert (std::make_pair (sfx_id, vol));
            }
          //add to currently active list
          currentlyActiveSounds.push_back (sfx_id);
        }
    }
  /*
  for (i = 0; i < mw->m_ViewableTiles.size(); i++)
    {
      Sound *sp = RequestTileSound (mw->m_ViewableTiles[i].t->tile_num);        //does this object have an associated sound?
      if (sp != NULL)
        {
          //calculate the volume
          short ox = mw->m_ViewableTiles[i].x - 5;
          short oy = mw->m_ViewableTiles[i].y - 5;
//                      DEBUG(0,LEVEL_DEBUGGING,"%d %d\n",ox,oy);
          float dist = sqrtf ((float) (ox) * (ox) + (float) (oy) * (oy));
//                      DEBUG(0,LEVEL_DEBUGGING,"%s %f\n",sp->GetName().c_str(),dist);
          float vol = (7.0f - (dist - 1)) / 7.0f;
          if (vol < 0)
            vol = 0;
          //sp->SetVolume(vol);
          //need the map to adjust volume according to number of active elements
          std::map < Sound *, float >::iterator it;
          it = volumeLevels.find (sp);
          if (it != volumeLevels.end ())
            {
              float old = volumeLevels[sp];
//                              DEBUG(0,LEVEL_DEBUGGING,"old:%f new:%f\n",old,vol);
              if (old < vol)
                {
                  volumeLevels[sp] = vol;
                }
            }
          else
            {
              volumeLevels.insert (std::make_pair (sp, vol));
            }
          //add to currently active list
          currentlyActiveSounds.push_back (sp);
        }
    }
    */
  //DEBUG(1,LEVEL_DEBUGGING,"\n");
  //is this sound new? - activate it.
  for (i = 0; i < currentlyActiveSounds.size(); i++)
    {
      std::list < SoundManagerSfx >::iterator it;
      it = SoundManagerSfx_find(m_ActiveSounds.begin (), m_ActiveSounds.end (), currentlyActiveSounds[i]);        //is the sound already active?
      if (it == m_ActiveSounds.end ())
        {                       //this is a new sound, add it to the active list
          //currentlyActiveSounds[i]->Play (true);
          //currentlyActiveSounds[i]->SetVolume (0);
          SoundManagerSfx sfx;
          sfx.sfx_id = currentlyActiveSounds[i];
          if(m_SfxManager->playSfxLooping(sfx.sfx_id, &sfx.handle, 0))
          {
        	  m_ActiveSounds.push_back(sfx);//currentlyActiveSounds[i]);
          }
        }
    }
  //is this sound old? - deactivate it
  std::list < SoundManagerSfx >::iterator it;
  it = m_ActiveSounds.begin ();
  while (it != m_ActiveSounds.end ())
    {
      std::vector < SfxIdType >::iterator fit;
      SoundManagerSfx sfx = (*it);
      fit = std::find (currentlyActiveSounds.begin (), currentlyActiveSounds.end (), sfx.sfx_id);       //is the sound in the new active list?
      if (fit == currentlyActiveSounds.end ())
        {                       //its not, stop this sound from playing.
          //sfx_id->Stop ();
    	  mixer->getMixer()->stopHandle(sfx.handle);
          it = m_ActiveSounds.erase (it);
        }
      else
        {
    	  mixer->getMixer()->setChannelVolume(sfx.handle, (uint8)(volumeLevels[sfx.sfx_id]*(sfx_volume/255.0f)*255.0f));
          it++;
        }
    }
}

void SoundManager::update()
{
  if (music_enabled && g_MusicFinished)
    {
      g_MusicFinished = false;
      if (m_pCurrentSong != NULL)
        {
          m_pCurrentSong->Stop();
        }
      m_pCurrentSong = SoundManager::RequestSong (m_CurrentGroup);
      if(m_pCurrentSong)
        {
          DEBUG(0,LEVEL_INFORMATIONAL,"assigning new song! '%s'\n", m_pCurrentSong->GetName().c_str());
          if(!m_pCurrentSong->Play (false))
            {
              DEBUG(0,LEVEL_ERROR,"play failed!\n");
            }
          m_pCurrentSong->SetVolume(music_volume);
        }
    }

}


Sound *SoundManager::SongExists (string name)
{
  std::list < Sound * >::iterator it;
  for (it = m_Songs.begin (); it != m_Songs.end (); ++it)
    {
      if ((*it)->GetName () == name)
        return *it;
    }

  return NULL;
}

Sound *SoundManager::SampleExists (string name)
{
  std::list < Sound * >::iterator it;
  for (it = m_Samples.begin (); it != m_Samples.end (); ++it)
    {
      if ((*it)->GetName () == name)
        return *it;
    }

  return NULL;
}

Sound *SoundManager::RequestTileSound (int id)
{
  std::map < int, SoundCollection * >::iterator it;
  it = m_TileSampleMap.find (id);
  if (it != m_TileSampleMap.end ())
    {
      SoundCollection *psc;
      psc = (*it).second;
      return psc->Select ();
    }
  return NULL;
}

Sound *SoundManager::RequestObjectSound (int id)
{
  std::map < int, SoundCollection * >::iterator it;
  it = m_ObjectSampleMap.find (id);
  if (it != m_ObjectSampleMap.end ())
    {
      SoundCollection *psc;
      psc = (*it).second;
      return psc->Select ();
    }
  return NULL;
}

uint16 SoundManager::RequestObjectSfxId(uint16 obj_n)
{
	uint16 i = 0;
	for(i=0;i<SOUNDMANANGER_OBJSFX_TBL_SIZE;i++)
	{
		if(u6_obj_lookup_tbl[i].obj_n == obj_n)
		{
			return u6_obj_lookup_tbl[i].sfx_id;
		}
	}

	return NUVIE_SFX_NONE;
}

Sound *SoundManager::RequestSong (string group)
{
  std::map < string, SoundCollection * >::iterator it;
  it = m_MusicMap.find (group);
  if (it != m_MusicMap.end ())
    {
      SoundCollection *psc;
      psc = (*it).second;
      return psc->Select ();
    }
  return NULL;
};

Audio::SoundHandle SoundManager::playTownsSound(std::string filename, uint16 sample_num)
{
	FMtownsDecoderStream *stream = new FMtownsDecoderStream(filename, sample_num);
	Audio::SoundHandle handle;
	mixer->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &handle, stream, -1, music_volume);

	return handle;
}

bool SoundManager::isSoundPLaying(Audio::SoundHandle handle)
{
	return mixer->getMixer()->isSoundHandleActive(handle);
}

bool SoundManager::playSfx(uint16 sfx_id)
{
	if(m_SfxManager == NULL || audio_enabled == false || sfx_enabled == false)
		return false;

	return m_SfxManager->playSfx(sfx_id, sfx_volume);
}
