/*
 *  SaveManager.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Apr 28 2004.
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

#include "nuvieDefs.h"
#include "Configuration.h"
 
#include "Actor.h"
#include "ActorManager.h"
#include "U6misc.h"
#include "U6LList.h"
#include "SaveManager.h"
#include "NuvieIOFile.h"
#include "NuvieFileList.h"

#include "GUI.h"
#include "SaveDialog.h"
#include "SaveSlot.h"
#include "SaveGame.h"


SaveManager::SaveManager(Configuration *cfg)
{
 config = cfg;
 dialog = NULL;
}

// setup the savedir variable.
// throws if the directory is not found or inaccessable
void SaveManager::init()
{
 std::string savedir_key;
 
 config->value("config/GameType",game_type);
 
 savegame = new SaveGame(config);

 savedir_key = config_get_game_key(config);
 
 savedir_key.append("/savedir");
 
 config->value(savedir_key, savedir);
 
 if(savedir.size() == 0)
   {
    printf("Warning: savedir config variable not found. Using current directory for saves!\n");
#ifdef WIN32    
    savedir.assign("");
#else
    savedir.assign(".");
#endif
    return;
   }
 
 if(directory_exists(savedir.c_str()) == false)
   {
    printf("Error: savedir '%s' either not found or not accessable!\n", savedir.c_str());
    throw "Setting Save Directory!";
   }

 return;
}

SaveManager::~SaveManager()
{
}

bool SaveManager::load_latest_save()
{
 if(savegame->load("nuvie01.sav") == false) //try to load the savegame nuvie01.sav
   return savegame->load_original();        // fall back to savegame/ if nuvie01.sav doesn't exist.

 return true;   
}
 
void SaveManager::create_dialog()
{
 GUI *gui = GUI::get_gui();
  
 if(dialog == NULL)
   {
    dialog = new SaveDialog((GUI_CallBack *)this);
    dialog->init(savedir.c_str(), get_game_tag(game_type));
    dialog->grab_focus();
    gui->AddWidget(dialog);
   }
   
 return;
}

bool SaveManager::load(SaveSlot *save_slot)
{
 std::string save_filename;

 if(save_slot->get_filename()->size() == 0)
   return savegame->load_new();

 build_path(savedir, save_slot->get_filename()->c_str(), save_filename);
 
 return savegame->load(save_filename.c_str());
}

bool SaveManager::save(SaveSlot *save_slot)
{
 std::string save_filename;
 std::string save_fullpath;
 
 save_filename.assign(save_slot->get_filename()->c_str());
 
 if(save_filename.size() == 0)
   save_filename = get_new_savefilename();
   
 build_path(savedir, save_filename, save_fullpath);
 
 return savegame->save(save_fullpath.c_str());
}

std::string SaveManager::get_new_savefilename()
{
 uint32 max_count;
 uint32 count;
 std::string *filename;
 std::string new_filename;
 std::string search_prefix;
 std::string num_str;
 char end_buf[8]; // 000.sav\0
 NuvieFileList filelist;
 
 max_count = 0;
 
 search_prefix = "nuvie";
 search_prefix.append(get_game_tag(game_type));
 
 new_filename = search_prefix;
 
 filelist.open(savedir.c_str(), search_prefix.c_str(), NUVIE_SORT_TIME_DESC);

 for(;(filename = filelist.next());)
   {
    // search for highest save number here.
    num_str = filename->substr(7,3); //extract the number.
    count = atoi(num_str.c_str());
    if(count > max_count)
      max_count = count;
   } 
 
 filelist.close();

 max_count++;
 
 snprintf(end_buf, 8, "%03d.sav",max_count);
 
 new_filename.append(end_buf);
 
 return new_filename;
}

GUI_status SaveManager::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
 SaveSlot *save_slot;
 
 if(caller == dialog)
  {
   switch(msg)
     {
       case SAVEDIALOG_CB_DELETE : dialog = NULL; break;

       case SAVEDIALOG_CB_LOAD : save_slot = (SaveSlot *)data;
                                 if(load(save_slot) == false)
                                    return GUI_PASS;
                                 break;

       case SAVEDIALOG_CB_SAVE : save_slot = (SaveSlot *)data;
                                 if(save(save_slot) == false)
                                    return GUI_PASS;
                                 break;
       default : break;
     }
  }

 return GUI_YUM;
}
