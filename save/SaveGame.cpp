/*
 *  SaveGame.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sat May 22 2004.
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

#include <list>
#include <cassert>
#include "nuvieDefs.h"
#include "U6misc.h"
#include "NuvieIO.h"
#include "NuvieIOFile.h"
#include "U6Lzw.h"
#include "SaveGame.h"
#include "Configuration.h"
#include "Game.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "ViewManager.h"
#include "MapWindow.h"
#include "MsgScroll.h"
#include "Party.h"
#include "Player.h"
#include "GameClock.h"

SaveGame::SaveGame(Configuration *cfg)
{
 config = cfg;
 init(NULL); //we don't need ObjManager here as there will be nothing to clean at this stage. :-)
}

SaveGame::~SaveGame()
{
}

void SaveGame::init(ObjManager *obj_manager)
{
 num_saves = 0;
 actual_play_time = 0;
 game_play_time = 0; 
 
 save_description.assign("");
 
 if(objlist.get_size() > 0)
   objlist.close();

 if(obj_manager)
   obj_manager->clean();
  
 return;
}

bool SaveGame::load_new()
{
 std::string filename;
 U6Lzw lzw;
 NuvieIOBuffer buf;
 unsigned char *data;
 uint32 decomp_size;
 ObjManager *obj_manager;
 uint8 i;
 uint32 pos;

 obj_manager = Game::get_game()->get_obj_manager();

 init(obj_manager);
 
 // load surface chunks
  
 config_get_path(config,"lzobjblk",filename);
 data = lzw.decompress_file(filename, decomp_size);
 
 buf.open(data, decomp_size, NUVIE_BUF_NOCOPY);

 for(i=0;i<64;i++)
   obj_manager->load_super_chunk(&buf, 0, i);

 buf.close();
 free(data);
 
 // load dungeon chunks
  
 config_get_path(config,"lzdngblk",filename);
 data = lzw.decompress_file(filename,decomp_size);
 
 buf.open(data, decomp_size, NUVIE_BUF_NOCOPY);

 for(i=0;i<5;i++)
   obj_manager->load_super_chunk(&buf, i, 0);

 pos = buf.position();
 buf.close();
 
 // load objlist
  
 objlist.open(&data[pos], decomp_size - pos, NUVIE_BUF_COPY);

 load_objlist();
 
 free(data);

 return true;
}

bool SaveGame::load_original()
{
 std::string path, key, objlist_filename;
 unsigned char *data;
 char *filename;
 char x,y;
 uint16 len;
 uint8 i;
 NuvieIOFileRead *objblk_file;
 NuvieIOFileRead objlist_file;
 ObjManager *obj_manager;
 
 objblk_file = new NuvieIOFileRead();

 obj_manager = Game::get_game()->get_obj_manager();
 
 init(obj_manager);
  
 key = config_get_game_key(config);
 key.append("/gamedir");
 
 config->value(key,path);

 printf("Loading Original Game: %s/savegame/\n", path.c_str());
 
 filename = get_objblk_path((char *)path.c_str());

 len = strlen(filename);
 
 i = 0;

 for(y = 'a';y < 'i'; y++)
  {
   for(x = 'a';x < 'i'; x++)
    {
     filename[len-1] = y;
     filename[len-2] = x;
     
     objblk_file->open(filename);
     
     if(obj_manager->load_super_chunk((NuvieIO *)objblk_file,0,i) == false)
       {
        delete[] filename;
        delete objblk_file;
        return false;
       }
     i++;
     objblk_file->close();
    }
  }

 filename[len-1] = 'i';

 for(i=0,x = 'a';x < 'f';x++,i++) //Load dungeons
  {
   filename[len-2] = x;
   objblk_file->open(filename);
     
   if(obj_manager->load_super_chunk((NuvieIO *)objblk_file, i, 0) == false)
     {
      delete[] filename;
      delete objblk_file;
      return false;
     }

   objblk_file->close();
  }

 delete[] filename;

 //print_egg_list();
 config_get_path(config, "/savegame/objlist", objlist_filename);
 if(objlist_file.open(objlist_filename)==false)
   return false;

 data = objlist_file.readAll();

 objlist.open(data, objlist_file.get_size(), NUVIE_BUF_COPY);
 free(data);
 
 load_objlist();
 
 return true;
}


bool SaveGame::load_objlist()
{
 Game *game;
 GameClock *clock;
 ActorManager *actor_manager;
 ObjManager *obj_manager; 
 ViewManager *view_manager;
 MapWindow *map_window;
 MsgScroll *scroll;
 Player *player;
 Party *party;
 uint16 px, py;
 uint8 pz;
 
 game = Game::get_game();
 
 clock = game->get_clock();
 actor_manager = game->get_actor_manager();
 obj_manager = game->get_obj_manager();
 scroll = game->get_scroll();
 map_window = game->get_map_window();
 
 player = game->get_player();
 party = game->get_party();
 view_manager = game->get_view_manager();
 
 clock->load(&objlist);
 actor_manager->load(&objlist);
 
 party->load(&objlist);
 player->load(&objlist);
  
 view_manager->reload();

 
 player->get_location(&px, &py, &pz);
 obj_manager->update(px, py, pz); // spawn eggs. 
   
 map_window->centerMapOnActor(player->get_actor());

 scroll->display_string("\nGame Loaded\n\n");
 
 scroll->init(player->get_name());
 
 scroll->display_prompt();
 
 return true;
}

bool SaveGame::load(const char *filename)
{
 uint8 i;
 uint32 objlist_size;
 uint32 bytes_read;
 NuvieIOFileRead *loadfile;
 unsigned char *data;
 int game_type;
 //char game_tag[3];
 ObjManager *obj_manager = Game::get_game()->get_obj_manager();
 
 init(obj_manager);
 
 config->value("config/GameType",game_type);

 loadfile = new NuvieIOFileRead();
 
 if(loadfile->open(filename) == false)
  {
   delete loadfile;
   return false;
  }
 
 printf("Loading Game: %s\n", filename);
 
 loadfile->seek(15);
 
 num_saves = loadfile->read2();

 // load actor inventories
 obj_manager->load_super_chunk((NuvieIO *)loadfile, 0, 0);
 
 // load eggs
 obj_manager->load_super_chunk((NuvieIO *)loadfile, 0, 0);

 // load surface objects
 for(i=0;i<64;i++)
   {
    obj_manager->load_super_chunk((NuvieIO *)loadfile, 0, i);
   }

 // load dungeon objects
 for(i=0;i<5;i++)
   {
    obj_manager->load_super_chunk((NuvieIO *)loadfile, i+1, 0);
   }
 
 objlist_size = loadfile->get_size() - loadfile->position();
 
 data = loadfile->readBuf(objlist_size, &bytes_read);
 
 objlist.open(data, objlist_size, NUVIE_BUF_COPY);
 
 free(data);
 loadfile->close();
 delete loadfile;
  
 load_objlist();
 
 return true;
}

bool SaveGame::save(const char *filename)
{
 uint8 i;
 NuvieIOFileWrite *savefile;
 int game_type;
 char game_tag[3];
 ObjManager *obj_manager = Game::get_game()->get_obj_manager();
 
 config->value("config/GameType",game_type);

 savefile = new NuvieIOFileWrite();
 
 savefile->open(filename);
 
 savefile->write2(NUVIE_SAVE_VERSION);
 savefile->writeBuf((const unsigned char *)"Nuvie Save", 11);


 switch(game_type)
   {
    case NUVIE_GAME_U6 : strcpy(game_tag, "U6");
                         break;
 
    case NUVIE_GAME_MD : strcpy(game_tag, "MD");
                         break;

    case NUVIE_GAME_SE : strcpy(game_tag, "SE");
                         break;
   }
 
 savefile->writeBuf((const unsigned char *)game_tag, 2);
 
 num_saves++;
 savefile->write2(num_saves);
 
 obj_manager->save_inventories(savefile);
 
 obj_manager->save_eggs(savefile);

 // save surface objects
 for(i=0;i<64;i++)
   obj_manager->save_super_chunk(savefile, 0, i);

 // save dungeon objects
 for(i=0;i<5;i++)
   obj_manager->save_super_chunk(savefile, i+1, 0);
 
 save_objlist();
 
 savefile->writeBuf(objlist.get_raw_data(), objlist.get_size());

 savefile->close();
 
 return true;
}

bool SaveGame::save_objlist()
{
 Game *game;
 GameClock *clock;
 ActorManager *actor_manager;
 Player *player;
 Party *party;
 MsgScroll *scroll;
 
 game = Game::get_game();
 
 clock = game->get_clock();
 actor_manager = game->get_actor_manager();
 
 player = game->get_player();
 party = game->get_party();
 scroll = game->get_scroll();
 
 clock->save(&objlist);
 actor_manager->save(&objlist);
 
 player->save(&objlist);
 party->save(&objlist);

 scroll->display_string("\nGame Saved\n\n");
 scroll->display_prompt();

 return true;
}

char *SaveGame::get_objblk_path(char *path)
{
 char *filename;
 uint16 len;

 if(path == NULL)
   return NULL;

 len = strlen(path);

 if(len == 0)
   return NULL;

 filename = new char [len+19]; // + room for /savegame/objblkxx\0

 strcpy(filename,path);
 if(filename[len-1] != U6PATH_DELIMITER)
   {
    filename[len] = U6PATH_DELIMITER;
    filename[len+1] = '\0';
   }

 strcat(filename,"savegame/objblkxx");

 return filename;
}

