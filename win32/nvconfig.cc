/*
 *	Modified 2015 by Joseph Applegate / The Nuvie Team
 *  Copyright (C) 2000-2011  Ryan Nunn
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Original comments from EXConfig:
 *  What is this?
 *
 *  EXCONFIG is a dll that is used by the InstallShield Wizard to get and set the
 *  Black Gate and Serpent Isle paths.
 *
 */

#include "nvconfig.h"
#include "Configuration.h"
//#include "utils.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
//#include "fnames.h"
#include "nuvieDefs.h"  // getConfigPathWin32

#define MAX_STRLEN  512

#ifdef _DEBUF
#define MessageBoxDebug(a,b,c,d) MessageBox(a,b,c,d);
#else
#define MessageBoxDebug(a,b,c,d)
#endif

const std::string c_empty_string;

const char *CONFIG_FILENAME = "nuvie.cfg";

// Not setting these in Nuvie yet.
const char *config_defaults[] = {
	"config/disk/game/blackgate/keys",  "(default)",
	"config/disk/game/serpentisle/keys",    "(default)",
	"config/audio/enabled",         "yes",
	"config/audio/effects/enabled",     "yes",
	"config/audio/effects/convert",     "gs",
	"config/audio/midi/enabled",        "yes",
	"config/audio/midi/convert",        "gm",
	"config/audio/midi/reverb/enabled", "no",
	"config/audio/midi/reverb/level",   "0",
	"config/audio/midi/chorus/enabled", "no",
	"config/audio/midi/chorus/level",   "0",
	"config/audio/midi/volume_curve",   "1.000000",
	"config/audio/speech/enabled",      "yes",
	"config/gameplay/cheat",        "yes",
	"config/gameplay/skip_intro",       "no",
	"config/gameplay/skip_splash",      "no",
	"config/video/width",           "320",
	"config/video/height",          "200",
	"config/video/scale",           "2",
	"config/video/scale_method",        "2xSaI",
	"config/video/fullscreen",      "no",
	"config/video/disable_fades",       "no",
	"config/video/gamma/red",       "1.00",
	"config/video/gamma/green",     "1.00",
	"config/video/gamma/blue",      "1.00",
	0
};

const char *BASE_Files[] = {
	"MAP",
	0
};

const char *U6_Files[] = {
	"CONVERSE.A",
	0
};

const char *SE_Files[] = {
	"SE.1",
	0
};

const char *MD_Files[] = {
	"MARS.LZC",
	0
};

//
// Path Helper class
//
class Path {
	struct Directory {
		char        name[256];
		Directory   *next;

		Directory() : next(0) {
			name[0] = 0;
		}
	};

	bool        network;// Networked?
	char        drive;  // Drive letter (if set)
	Directory   *dirs;  // Directories

	void RemoveAll();
	int Addit(const char *p);
public:

	Path() : network(false), drive(0), dirs(0) { }
	Path(const char *p) : network(false), drive(0), dirs(0) {
		AddString(p);
	}
	~Path();

	void RemoveLast() {
		Addit("..");
	}
	void AddString(const char *p);
	void GetString(char *p, int max_strlen = MAX_STRLEN);

};

// Destructor
Path::~Path() {
	RemoveAll();
	network = false;
	drive = 0;
}

void Path::RemoveAll() {
	Directory *d = dirs;
	while (d) {
		Directory *next = d->next;
		delete d;
		d = next;
	}
	dirs = 0;
}

int Path::Addit(const char *p) {
	Directory *d = dirs;
	Directory *prev = 0;
	Directory *prevprev = 0;
	int i;

	// Check for . and ..

	// Create new
	if (!d) {
		dirs = d = new Directory;
	} else {
		while (d->next) {
			prevprev = d;
			d = d->next;
		}
		d->next = new Directory;
		prev = d;
		d = d->next;
	}

	for (i = 0; p[i] != 0 && p[i] != '\\' && p[i] != '/'; i++)
		d->name[i] = p[i];

	d->name[i] = 0;

	// Skip all 'slashes'
	while (p[i] && (p[i] == '\\' || p[i] == '/')) i++;

	// Check for . and ..
	if (!std::strcmp(d->name, ".")) {
		delete d;
		prev->next = 0;
	} else if (!std::strcmp(d->name, "..")) {
		delete d;
		delete prev;
		prevprev->next = 0;

	}

	return i;
}

// Add path
void Path::AddString(const char *p) {
	int len = std::strlen(p);

	// Root dir of this drive
	if (*p == '\\' || *p == '/') {
		// Remove all the entires
		RemoveAll();

		p++;

		// Could be network drive
		if (*p == '\\') {
			network = true;
			p++;
		}

	} else if (p[0] && p[1] == ':') { // Drive
		RemoveAll();
		drive = *p;
		network = false;
		p += 2;
	}

	// Skip all slashes
	while (*p && (*p == '\\' || *p == '/')) p++;

	while (*p) {
		p += Addit(p);
	}
}

void Path::GetString(char *p, int max_strlen) {
	p[0] = 0;

	if (network) std::strncat(p, "\\\\", max_strlen);
	else if (drive) _snprintf(p, max_strlen, "%c:\\", drive);
	else std::strncat(p, "\\", max_strlen);

	Directory *d = dirs;
	while (d) {
		std::strncat(p, d->name, max_strlen);
		d = d->next;
		if (d) std::strncat(p, "\\", max_strlen);
	}

}



//             //
// The exports //
//             //

#ifdef __cplusplus
extern "C" {
#endif

// For Nuvie we aren't using these utility methods. Stubs have been added for compiling.
void setup_program_paths() {}
std::string get_system_path(const char *config_key) { return "."; }
bool add_system_path(char *config_key, std::string path) {	return false; }
bool config_read_config_file(char *fname) {	return false; }
void config_write_back() { }
////

// Check if a file exists.
// Modified version of U7exists from Exult.
bool fileExists(const char *fname)
{
	//string name = get_system_path(fname);
	std::string name = fname;

	bool    exists;
	struct stat sbuf;

	/*int uppercasecount = 0;
	do {
		exists = (stat(name, &sbuf) == 0);
		if (exists)
			return true; // found it!
	} while (base_to_uppercase(name, ++uppercasecount));*/
	exists = (stat(name.c_str(), &sbuf) == 0);
	if (exists)
		return true; // found it!

	// file not found
	return false;
}

Configuration *getConfig(char *p) {
	// Open config file, Create if it doesn't exist
	Configuration *config = new Configuration();
	//if (!config->readConfigFile(p, "config", false))
	//	config->readConfigFile(CONFIG_FILENAME, "config", false);
	bool gotConfig = false;
	if (get_system_path("<CONFIG>") == ".")
		gotConfig = config->readConfigFile(p, "config", false);
	else
		gotConfig = config->readConfigFile(CONFIG_FILENAME, "config", false);
	if(!gotConfig)
		return NULL;
	return config;
}

//
// Get the Game paths from the config file
//
	__declspec(dllexport) int __stdcall GetNuvieGamePaths(char *NuvieDir, char *U6Path, char *MDPath, char *SEPath, char *configPath, int MaxPath) {
		MessageBoxDebug(NULL, NuvieDir, "NuvieDir", MB_OK);
		MessageBoxDebug(NULL, U6Path, "U6Path", MB_OK);
		MessageBoxDebug(NULL, MDPath, "MDPath", MB_OK);
		MessageBoxDebug(NULL, SEPath, "SEPath", MB_OK);

		int p_size = strlen(NuvieDir) + strlen("/")+strlen(CONFIG_FILENAME) + MAX_STRLEN;
		char *p = new char[p_size];
		bool foundConfig = false;

		// Get the complete path for the config file
		Path config_path(NuvieDir);
		config_path.AddString(CONFIG_FILENAME);
		config_path.GetString(p, p_size);
		setup_program_paths();

		// Set defaults to blank, so the Installer can detect and handle
		const static char *u6_pathdef = "";
		const static char *md_pathdef = "";
		const static char *se_pathdef = "";
		const static char *config_pathdef = "";

		MessageBoxDebug(NULL, NuvieDir, p, MB_OK);

		try {
			// Open config file
			Configuration *config = getConfig(p);
			if(config == NULL)
			{
				std::strncpy(U6Path, u6_pathdef, MaxPath);
				std::strncpy(MDPath, md_pathdef, MaxPath);
				std::strncpy(SEPath, se_pathdef, MaxPath);
				std::strncpy(configPath, config_pathdef, MaxPath); // config is NOT valid
				delete [] p;
				return 0;
			}
			foundConfig = true;

			std::string dir;
			config->value("config/ultima6/gamedir", dir, u6_pathdef);
			std::strncpy(configPath, p, MaxPath); // config is valid
			if (dir != u6_pathdef) {
				std::strncpy(U6Path, dir.c_str(), MaxPath);
			} else {
				std::strncpy(U6Path, u6_pathdef, MaxPath);
			}

			config->value("config/martian/gamedir", dir, md_pathdef);
			if (dir != md_pathdef) {
				std::strncpy(MDPath, dir.c_str(), MaxPath);
			} else {
				std::strncpy(MDPath, md_pathdef, MaxPath);
			}

			config->value("config/savage/gamedir", dir, se_pathdef);
			if (dir != se_pathdef) {
				std::strncpy(SEPath, dir.c_str(), MaxPath);
			} else {
				std::strncpy(SEPath, se_pathdef, MaxPath);
			}
		} catch (...) {
			std::strncpy(U6Path, u6_pathdef, MaxPath);
			std::strncpy(MDPath, md_pathdef, MaxPath);
			std::strncpy(SEPath, se_pathdef, MaxPath);
			std::strncpy(configPath, config_pathdef, MaxPath); // config is NOT valid
		}

		delete [] p;
		return foundConfig?1:0;
	}

//
// Set Game paths in the config file
//
	__declspec(dllexport) void __stdcall SetNuvieGamePaths(char *NuvieDir, char *U6Path, char *MDPath, char *SEPath) {
		MessageBoxDebug(NULL, NuvieDir, "NuvieDir", MB_OK);
		MessageBoxDebug(NULL, U6Path, "U6Path", MB_OK);
		MessageBoxDebug(NULL, MDPath, "MDPath", MB_OK);
		MessageBoxDebug(NULL, SEPath, "SEPath", MB_OK);

		// TODO: Let Nuvie decide where to place the config.
		//const char *configFilePath = getConfigPathWin32();
		//MessageBoxDebug(NULL, configFilePath, "NuvieDir", MB_OK);
		
		int i;
		int p_size = strlen(NuvieDir) + strlen("/")+strlen(CONFIG_FILENAME) + MAX_STRLEN;
		char *p = new char[p_size];

		Path config_path(NuvieDir);
		config_path.AddString(CONFIG_FILENAME);
		config_path.GetString(p, p_size);
		setup_program_paths();

		MessageBoxDebug(NULL, p, "WriteConfig: p", MB_OK);

		try {
			Configuration *config = getConfig(p);

			// Set U6 Path
			MessageBoxDebug(NULL, p, "WriteConfig: Ultima6", MB_OK);
			if(!config->set("config/ultima6/gamedir", U6Path))
				MessageBoxDebug(NULL, p, "ERROR writing config/ultima6/gamedir", MB_OK);
			// Set MD Path
			MessageBoxDebug(NULL, p, "WriteConfig: Martian", MB_OK);
			if(!config->set("config/martian/gamedir", MDPath))
				MessageBoxDebug(NULL, p, "ERROR writing config/martian/gamedir", MB_OK);
			// Set SE Path
			MessageBoxDebug(NULL, p, "WriteConfig: Savage", MB_OK);
			if(!config->set("config/savage/gamedir", SEPath))
				MessageBoxDebug(NULL, p, "ERROR writing config/savage/gamedir", MB_OK);
			
			// Set Defaults
			//std::string s;
			//for (i = 0; config_defaults[i]; i += 2) {
			//	config.value(config_defaults[i], s, "");
			//	if (s.empty()) config.set(config_defaults[i], config_defaults[i + 1]);
			//}

			config->write();
			delete config;
		} catch (...) {
			printf("Exception occurred when while reading or writing config.");
		} 

		delete [] p;
	}

// set loadgame property
	__declspec(dllexport) void __stdcall SetNuvieLoadGame(char *NuvieDir, char *sLoadGame) {
		int i;

		int p_size = strlen(NuvieDir) + strlen("/")+strlen(CONFIG_FILENAME) + MAX_STRLEN;
		char *p = new char[p_size];

		Path config_path(NuvieDir);
		config_path.AddString(CONFIG_FILENAME);
		config_path.GetString(p, p_size);
		setup_program_paths();

		try {
			// Open config file
			Configuration *config = getConfig(p);

			// set
			std::string gameName("ultima6");
			if(strcmp(sLoadGame, "Martian Dreams") == 0)
				gameName = "martian";
			if(strcmp(sLoadGame, "The Savage Empire") == 0)
				gameName = "savage";
			config->set("config/loadgame", gameName);
			config->write();
			delete config;
		} catch (...) {
			printf("Exception occurred when while reading or writing config.");
		} 

		delete [] p;
	}
	
//
// Verify the Game Directory (game specified by 'gameName') contains the right stuff
//
	__declspec(dllexport) int __stdcall VerifyGameDirectory(char *path, char *gameName) {
		int i;
		std::string s(path);
		std::string p = "";
		s += "\\";
		//add_system_path("<STATIC>", s + "/static");

		// Check all the BASE files
		for (i = 0; BASE_Files[i]; i++) {
			p = s + BASE_Files[i];
			if (!fileExists(p.c_str()))
				return false;
		}

		// Check all the Game files
		if(strcmp(gameName,"ultima6")==0) {
			for (i = 0; U6_Files[i]; i++) {
				p = s + U6_Files[i];
				if (!fileExists(p.c_str()))
					return false;
			}
		}
		else if(strcmp(gameName,"martian")==0) {
			for (i = 0; MD_Files[i]; i++) {
				p = s + MD_Files[i];
				if (!fileExists(p.c_str()))
					return false;
			}
		}
		else if(strcmp(gameName,"savage")==0) {
			for (i = 0; SE_Files[i]; i++) {
				p = s + SE_Files[i];
				if (!fileExists(p.c_str()))
					return false;
			}
		}
		else 
			return false;
		return true;
	}
	
	
#ifdef __cplusplus
}
#endif

BOOL APIENTRY DllMain(HINSTANCE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     ) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

