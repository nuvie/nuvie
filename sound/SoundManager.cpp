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

#include "SoundManager.h"
#include "SongAdPlug.h"
#include "Sample.h"
#include <algorithm>

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
  g_MusicFinished = false;
  audio_enabled = false;
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
  m_Config = config;
  m_Config->value ("config/audio/enabled", audio_enabled, false);

  if(audio_enabled == false)
    return false;

  if(!initAudio ())
    return false;
  string scriptdirectory;
  config->pathFromValue ("config/ultima6/sounddir", "", scriptdirectory);

  if (scriptdirectory.length () == 0)
    return false;

  printf ("should load script from %s\n", scriptdirectory.c_str ());
  string filename;
  filename = scriptdirectory + "songs.cfg";
  LoadSongs (scriptdirectory, filename);
  filename = scriptdirectory + "obj_samples.cfg";
  LoadObjectSamples (scriptdirectory, filename);
  filename = scriptdirectory + "tile_samples.cfg";
  LoadTileSamples (scriptdirectory, filename);
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
  int audio_buffers = 734;        //4096;
  SDL_Init (SDL_INIT_AUDIO);
  ret = Mix_OpenAudio (audio_rate, audio_format, audio_channels, audio_buffers);
  if (ret)
    return false;
  Mix_HookMusicFinished (musicFinished);
  return true;
}

bool SoundManager::LoadSongs (string directory, string scriptname)
{
  NuvieIOFileRead niof;
  niof.open (scriptname);
  char * sz = (char *) niof.readAll ();
  char seps[] = ";\r\n";
  char *token1;
  char *token2;
//      char sz[] = "Castle;U6-BRIT.MID;\nCastle;ULTIMA_Ba.MOD;\nRandom;U6-BRIT.MID;\n";
  token1 = strtok (sz, seps);
  while ((token1 != NULL) && ((token2 = strtok (NULL, seps)) != NULL))
    {
      printf ("%s : %s\n", token1, token2);
      Sound *ps;
      ps = SoundExists (token2);
      if (ps == NULL)
        {
          Song *s;
          s = new SongAdPlug;
          string songname;
          songname = directory + token2;
          if (!s->Init (songname.c_str ()))
            {
              printf ("could not load %s\n", songname.c_str ());
            }
          ps = s;
          m_Songs.push_back (ps);       //add it to our global list 
        }
      if (ps != NULL)
        {                       //we have a valid sound
          SoundCollection *psc;
          std::map < string, SoundCollection * >::iterator it;
          it = m_MusicMap.find (token1);
          if (it == m_MusicMap.end ())
            {                   //is there already a collection for this entry?
              psc = new SoundCollection;        //no, create a new sound collection
              psc->m_Sounds.push_back (ps);     //add this sound to the collection
              m_MusicMap.insert (std::make_pair (token1, psc)); //insert this pair into the map
            }
          else
            {
              psc = (*it).second;       //yes, get the existing
              psc->m_Sounds.push_back (ps);     //add this sound to the collection
            }
        }
      token1 = strtok (NULL, seps);
    }
//      for (int i=0;i<10;i++)
//      printf("%s\n",RequestSong("Castle")->GetName().c_str());
  return true;
};


bool SoundManager::LoadObjectSamples (string directory, string scriptname)
{
  char seps[] = ";\r\n";
  char *token1;
  char *token2;
  NuvieIOFileRead niof;
  niof.open (scriptname);
  char *sz = (char *) niof.readAll ();
  
  token1 = strtok (sz, seps);
  
  while ((token1 != NULL) && ((token2 = strtok (NULL, seps)) != NULL))
    {
      int id = atoi (token1);
      printf ("%d : %s\n", id, token2);
      Sound *ps;
      ps = SoundExists (token2);
      if (ps == NULL)
        {
          Sample *s;
          s = new Sample;
          string samplename;
          samplename = directory + token2;
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

bool SoundManager::LoadTileSamples (string directory, string scriptname)
{
  char seps[] = ";\r\n";
  char *token1;
  char *token2;
  NuvieIOFileRead niof;
  niof.open (scriptname);
  char *sz = (char *) niof.readAll ();

  token1 = strtok (sz, seps);

  while ((token1 != NULL) && ((token2 = strtok (NULL, seps)) != NULL))
    {
      int id = atoi (token1);
      printf ("%d : %s\n", id, token2);
      Sound *ps;
      ps = SoundExists (token2);
      if (ps == NULL)
        {
          Sample *s;
          s = new Sample;
          string samplename;
          samplename = directory + token2;
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

void SoundManager::update ()
{
  int i;
  uint16 x, y;
  uint8 l;
  
  if(audio_enabled == false)
    return;

  string next_group = "";
  Player *p = Game::get_game ()->get_player ();
  MapWindow *mw = Game::get_game ()->get_map_window ();
  
  vector < Sound * >currentlyActiveSounds;
  map < Sound *, float >volumeLevels;
  
  p->get_location (&x, &y, &l);

  //m_ViewableTiles

  //get a list of all the sounds
  for (i = 0; i < mw->m_ViewableObjects.size (); i++)
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
  for (i = 0; i < mw->m_ViewableTiles.size (); i++)
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
  for (i = 0; i < currentlyActiveSounds.size (); i++)
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


  //decide song according to level first
//      printf("%d %d %d\n",x,y,l);
  switch (l)
    {
    case 0:                    //player in Britannia
      if ((x > 261) && (x < 418) && (y > 307) && (y < 459))     //player in britain
        next_group = "Castle";
      else
        next_group = "Random";
      break;
    case 5:                    //player in Gargoyle world
      next_group = "Gargoyle";
      break;
    default:
      next_group = "Dungeon";
      break;
    }
  if ((m_pCurrentSong == NULL) || (m_CurrentGroup != next_group)
      || (g_MusicFinished))
    {
      g_MusicFinished = false;
      if (m_pCurrentSong != NULL)
        {
          m_pCurrentSong->Stop ();
        }
      m_pCurrentSong = SoundManager::RequestSong (next_group);
      printf ("assinging new song! %x\n", m_pCurrentSong);
      if (!m_pCurrentSong->Play (false))
        {
          printf ("play failed!\n");
          m_pCurrentSong = NULL;
        }
      m_CurrentGroup = next_group;
    }

}


Sound *SoundManager::SoundExists (string name)
{
  std::list < Sound * >::iterator it;
  for (it = m_Songs.begin (); it != m_Songs.end (); ++it)
    {
      if ((*it)->GetName () == name)
        return *it;
    }
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

