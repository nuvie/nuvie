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
#include "emuopl.h"
#include "SoundManager.h"
#include "SongAdPlug.h"
#include "Sample.h"
#include <algorithm>
#include <cmath>

#include "Game.h"
#include "Player.h"
#include "MapWindow.h"


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
  
  opl = NULL;
}

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

  if(audio_enabled)
     Mix_CloseAudio ();
}

bool SoundManager::nuvieStartup (Configuration * config)
{
  std::string music_key;
  std::string music_style;
  std::string music_cfg_file; //full path and filename to music.cfg
  std::string sound_dir_key;
  std::string sound_dir;

  m_Config = config;
  m_Config->value ("config/audio/enabled", audio_enabled, true);

  if(audio_enabled == false)
     {
      music_enabled = false;
      sfx_enabled = false;
      return false;
     }
  
  m_Config->value ("config/audio/enable_music", music_enabled, true);
  m_Config->value ("config/audio/enable_sfx", sfx_enabled, true);

  music_key = config_get_game_key(config);
  music_key.append("/music");
  config->value (music_key, music_style, "native");

  sound_dir_key = config_get_game_key(config);
  sound_dir_key.append("/sounddir");
  config->value (sound_dir_key, sound_dir, "");
  
  if(!initAudio ())
    {
     return false;
    }

  if(music_enabled)
    {
     if(music_style == "native")
       LoadNativeU6Songs(); //FIX need to handle MD & SE music too.
     else if (music_style == "custom")
       LoadCustomSongs(sound_dir);
     else
       printf("Warning: Unknown music style '%s'\n", music_style.c_str());

     musicPlayFrom("random");
    }

  if(sfx_enabled)
    {
     LoadObjectSamples (sound_dir);
     LoadTileSamples (sound_dir);
    }

  return true;
}

bool SoundManager::initAudio ()
{
  int ret;
  int audio_rate = 44100;
#ifdef BIG_ENDIAN
  Uint16 audio_format = AUDIO_S16MSB;        //AUDIO_S16; /* 16-bit stereo */
#else
  Uint16 audio_format = AUDIO_S16LSB;        //AUDIO_S16; /* 16-bit stereo */
#endif

  int audio_channels = 2;
  int audio_buffers = 1024;        //4096; // Note: must be a power of two
  SDL_InitSubSystem (SDL_INIT_AUDIO);
  ret = Mix_OpenAudio (audio_rate, audio_format, audio_channels, audio_buffers);
  if (ret) {
    printf("Error: Failed to initialize audio: %s\n", Mix_GetError());
    return false;
  }
  Mix_HookMusicFinished (musicFinished);
  
  opl = new CEmuopl(audio_rate, true, true); // 16bit stereo
  
  return true;
}

bool SoundManager::LoadNativeU6Songs()
{
 Song *song;
 
 string filename;
 
 config_get_path(m_Config, "brit.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("random", song);

 config_get_path(m_Config, "forest.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("random", song);

 config_get_path(m_Config, "stones.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("random", song);

 config_get_path(m_Config, "ultima.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("random", song);

 config_get_path(m_Config, "engage.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("combat", song);

 config_get_path(m_Config, "hornpipe.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("boat", song);

 config_get_path(m_Config, "gargoyle.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
 groupAddSong("gargoyle", song);

 config_get_path(m_Config, "dungeon.m", filename);
 song = new SongAdPlug(opl);
 loadSong(song, filename.c_str());
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
        printf ("%s : %s\n", token1, token2);
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
      printf ("could not load %s\n", filename);
    }

 return false;
}

bool SoundManager::groupAddSong (char *group, Song *song)
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
      printf ("%d : %s\n", id, token2);
      Sound *ps;
      ps = SampleExists (token2);
      if (ps == NULL)
        {
          Sample *s;
          s = new Sample;
          build_path(sound_dir, token2, samplename);
          if (!s->Init (samplename.c_str ()))
            {
              printf ("could not load %s\n", samplename.c_str ());
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
    return false;

  sz = (char *) niof.readAll ();

  token1 = strtok (sz, seps);

  while ((token1 != NULL) && ((token2 = strtok (NULL, seps)) != NULL))
    {
      int id = atoi (token1);
      printf ("%d : %s\n", id, token2);
      Sound *ps;
      ps = SampleExists (token2);
      if (ps == NULL)
        {
          Sample *s;
          s = new Sample;
          build_path(sound_dir, token2, samplename);
          if (!s->Init (samplename.c_str ()))
            {
              printf ("could not load %s\n", samplename.c_str ());
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
 if (m_pCurrentSong != NULL)
        {
          m_pCurrentSong->Play();
        }

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
  
  vector < Sound * >currentlyActiveSounds;
  map < Sound *, float >volumeLevels;
  
  p->get_location (&x, &y, &l);

  //m_ViewableTiles

  //get a list of all the sounds
  for (i = 0; i < mw->m_ViewableObjects.size(); i++)
    {
      //printf("%d %s",mw->m_ViewableObjects[i]->obj_n,Game::get_game()->get_obj_manager()->get_obj_name(mw->m_ViewableObjects[i]));
      Sound *sp = RequestObjectSound (mw->m_ViewableObjects[i]->obj_n); //does this object have an associated sound?
      if (sp != NULL)
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
          std::map < Sound *, float >::iterator it;
          it = volumeLevels.find (sp);
          if (it != volumeLevels.end ())
            {
              if (volumeLevels[sp] < vol)
                volumeLevels[sp] = vol;
            }
          else
            {
              volumeLevels.insert (std::make_pair (sp, vol));
            }
          //add to currently active list
          currentlyActiveSounds.push_back (sp);
        }
    }
  for (i = 0; i < mw->m_ViewableTiles.size(); i++)
    {
      Sound *sp = RequestTileSound (mw->m_ViewableTiles[i].t->tile_num);        //does this object have an associated sound?
      if (sp != NULL)
        {
          //calculate the volume
          short ox = mw->m_ViewableTiles[i].x - 5;
          short oy = mw->m_ViewableTiles[i].y - 5;
//                      printf("%d %d\n",ox,oy);
          float dist = sqrtf ((float) (ox) * (ox) + (float) (oy) * (oy));
//                      printf("%s %f\n",sp->GetName().c_str(),dist);
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
//                              printf("old:%f new:%f\n",old,vol);
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
  //printf("\n");
  //is this sound new? - activate it.
  for (i = 0; i < currentlyActiveSounds.size(); i++)
    {
      std::list < Sound * >::iterator it;
      it = std::find (m_ActiveSounds.begin (), m_ActiveSounds.end (), currentlyActiveSounds[i]);        //is the sound already active?
      if (it == m_ActiveSounds.end ())
        {                       //this is a new sound, add it to the active list
          currentlyActiveSounds[i]->Play (true);
          currentlyActiveSounds[i]->SetVolume (0);
          m_ActiveSounds.push_back (currentlyActiveSounds[i]);
        }
    }
  //is this sound old? - deactivate it
  std::list < Sound * >::iterator it;
  it = m_ActiveSounds.begin ();
  while (it != m_ActiveSounds.end ())
    {
      std::vector < Sound * >::iterator fit;
      Sound *sp = (*it);
      fit = std::find (currentlyActiveSounds.begin (), currentlyActiveSounds.end (), sp);       //is the sound in the new active list?
      if (fit == currentlyActiveSounds.end ())
        {                       //its not, stop this sound from playing.
          sp->Stop ();
          it = m_ActiveSounds.erase (it);
        }
      else
        {
          sp->SetVolume (volumeLevels[sp]);
          it++;
        }
    }
}

void SoundManager::update ()
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
          printf ("assigning new song! %x\n", m_pCurrentSong);
          if(!m_pCurrentSong->Play (false))
            {
              printf ("play failed!\n");
            }
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

