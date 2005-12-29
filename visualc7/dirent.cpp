/*
 *  dirent.cpp
 *  Nuvie
 *
 *	Win32 wrappers for POSIX file functions.
 *
 *  Created by Jonathan E. Wright on Sun May 30 2004.
 *  Copyright (c) 2004 The Nuvie Team. All rights reserved.
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

#include "NuvieDefs.h"
#include "dirent.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <io.h>


//	NOTE:	don't add any virtual methods to this so that it remains 
//			structurally compatible with the _dircontents struct.
class	CDIREntry	:	public	_dircontents
{
	public:
		CDIREntry (const char* pName, long iSize, unsigned short sAttr,
				   unsigned short sTime, unsigned short sDate)
		{
			_d_entry = NULL;

			Name (pName);
			
			_d_size = iSize;
			_d_attr = sAttr;
			_d_time = sTime;
			_d_date = sDate;

			_d_next = NULL;
		}

		~CDIREntry ()
		{
			if (_d_entry != NULL)
				delete _d_entry;
		}

		CDIREntry*	Next ()
		{
			return	(CDIREntry*)_d_next;
		}

		void	Next (CDIREntry* pNext)
		{
			_d_next = pNext;
		}

		const char* Name ()
		{
			return	_d_entry;
		}

		void Name (const char* pName)
		{
			assert (pName != NULL);
			if (_d_entry != NULL)
				delete _d_entry;

			unsigned long dwLen = strlen (pName);

			_d_entry = new char [dwLen + 1];
			strcpy (_d_entry, pName);
		}

		size_t NameLen ()
		{
			if (_d_entry == NULL)
				return	0;

			return	strlen (_d_entry);
		}
};

//	class extending DIR struct for constructor / destructor functionality
//	NOTE:	don't add any virtual methods to this so that it remains 
//			structurally compatible with the DIR struct.
class	CDIR	:	public	DIR
{
	public:
		CDIR ()
		{
			dd_id		= -1;
			dd_loc		= 0;
			dd_contents	= NULL;
			dd_cp		= NULL;
		}

		~CDIR ()
		{
			Close ();
		}

		void	Close ()
		{
			if (dd_contents == NULL)
				return;

			CDIREntry*	pCur = (CDIREntry*)dd_contents;
			while (pCur != NULL)
			{
				CDIREntry* pNext = pCur->Next ();
				delete pCur;
				pCur = pNext;
			}
		}

		bool	Open (const char* pDirName)
		{
			_finddata_t	FindData;

			this->dd_id = -1;
			this->dd_loc = 0;
			this->dd_contents = NULL;
			this->dd_cp = NULL;

			//	add wildcards to end of dir path
			char strTemp [MAXNAMELEN];
			strcpy (strTemp, pDirName);
			if (strTemp [strlen (strTemp) - 1] != '//')
				strcat (strTemp, "/");
			strcat (strTemp, "*.*");

			int	id = _findfirst (strTemp, &FindData);
			if (id < 0)
				return	false;

			this->dd_id = id;

			do 
			{
				//	TODO:	construct correct file attributes from Win32
				unsigned short	sAttr = 0;

				//	TODO:	construct correct time and date from Win32 time
				unsigned short	sTime = 0;
				unsigned short	sDate = 0;

				CDIREntry* pEntry = new CDIREntry (FindData.name,
												   FindData.size,
												   sAttr,
												   sTime,
												   sDate);
				if (this->dd_contents == NULL)
					this->dd_contents = pEntry;
				else
					this->dd_cp->_d_next = pEntry;

				//	use dd_cp as a pointer to the last entry
				this->dd_cp = pEntry;

			} while (_findnext (this->dd_id, &FindData) == 0);

			//	reset the current pointer to the first directory entry
			this->dd_cp = this->dd_contents;

			return	true;
		}

	public:
		//	m_DirData is filled in by readdir and then a a pointer to m_DirData 
		//	is returned.  This matches the spec I found somewhere of how readdir
		//	is supposed to work (one dirent per directory stream)
		dirent	m_DirData;
};


// a vector with CDIR pointer; it additionally has a destructor that
// does auto deletion of pointers still in the list
class CDIRList: public std::vector<CDIR*>
{
public:
   ~CDIRList()
   {
      std::for_each(begin(), end(), item_deleter);
   }

   static void item_deleter(CDIR* pDir){ delete pDir; }
};

//	just a list of DIRs that are open.  This may not suitable for LOTS
//	of dirs being opened at once, but should suffice for Nuvie.
static CDIRList g_OpenDirs;




//=============================================================================
// Interface
//=============================================================================

DIR* opendir (const char* pDirName)
{
   CDIR* pDir = new CDIR;
   if (pDir == NULL)
      return NULL;

   if (pDir->Open (pDirName) == false)
      return NULL;

   g_OpenDirs.push_back(pDir);

   return (DIR*)pDir;
}

int closedir (DIR* pDir)
{
   if (pDir == NULL)
      return -1;

   _findclose (pDir->dd_id);

   CDIR* pRealDir = (CDIR*)(pDir);

   std::vector<CDIR*>::iterator iter =
      std::find(g_OpenDirs.begin(), g_OpenDirs.end(), pRealDir);

   if (iter == g_OpenDirs.end())
      return -1;

   g_OpenDirs.erase(iter);

   delete pRealDir;

   return 0;
}

dirent* readdir (DIR* pDir)
{
	CDIR* pRealDir = (CDIR*)(pDir);
	if (pRealDir == NULL)
		return	NULL;

	//	store a pointer to the current dir entry
	CDIREntry*	pCur = (CDIREntry*)(pRealDir->dd_cp);
	if (pCur == NULL)
		return	NULL;

	//	move to the next dir entry
	pRealDir->dd_cp = pCur->Next ();

	//	fill in the return struct
	pRealDir->m_DirData.d_namlen = pCur->NameLen ();
	strcpy (pRealDir->m_DirData.d_name, pCur->Name ());

	return	&pRealDir->m_DirData;
}
