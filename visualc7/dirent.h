#ifndef __dirent_h__
#define __dirent_h__
/*
 *  dirent.h
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

#define	MAXNAMELEN	260

typedef	enum	EOpenDirFlags
{
	__OPENDIR_PRESERVE_CASE	= (1<<0),
	__OPENDIR_FIND_HIDDEN	= (1<<1),
	__OPENDIR_FIND_LABEL	= (1<<2),
	__OPENDIR_NO_HIDDEN		= (1<<3)
};

typedef	enum	EDirAttributes
{
	A_RONLY		= (1<<0),
	A_HIDDEN	= (1<<1),
	A_SYSTEM	= (1<<2),
	A_LABEL		= (1<<3),
	A_DIR		= (1<<4),
	A_ARCHIVE	= (1<<5)
};

//	direct struct
typedef	struct 
{
	int				d_ino;
	int				d_reclen;
	int				d_namlen;
	char			d_name[MAXNAMELEN + 1];
	long			d_size;
	unsigned short	d_attr;
	unsigned short	d_time;
	unsigned short	d_date;
} direct;

//	_directontents struct
typedef	struct _dircontents_s
{
	char*					_d_entry;
	long					_d_size;
	unsigned short			_d_attr;
	unsigned short			_d_time;
	unsigned short			_d_date;
	_dircontents_s*			_d_next;
} _dircontents;

//	_dirdesc struct
typedef	struct 
{
	int				dd_id;
	long			dd_loc;
	_dircontents*	dd_contents;
	_dircontents*	dd_cp;
} _dirdesc;

struct dirent
{
	char d_namlen;
	char d_name [256];
};

typedef _dirdesc DIR;


int	closedir (DIR* pDir);
DIR* opendir (const char* pDirName);
dirent* readdir (DIR* pDir);
/*
void rewinddir (DIR* pDir);
void seekdir (DIR* pDir, long iLoc);
void telldir (DIR* pDir);
*/
#endif	__dirent_h__