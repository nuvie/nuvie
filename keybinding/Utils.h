/*
 *	Utils.h - Common utility routines.
 *
 *  Copyright (C) 1998-1999  Jeffrey S. Freedman
 *  Copyright (C) 2000-2011  The Exult Team
 *  Copyright (C) 2012  The Nuvie Team
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
#ifndef _UTILS_H_
#define _UTILS_H_

#include <fstream>
#include <string> // exception handling include

bool openFile
	(
	std::ifstream& in,			// Input stream to open.
	const char *fname			// May be converted to upper-case.
	);

bool fileExists(
	const char *fname
	);

/*
 *  File errors
 */

class file_exception : public std::exception {
	std::string  what_;
public:
	file_exception (const std::string& what_arg): what_ (what_arg) {  }
	const char *what() const throw () { return what_.c_str(); }
virtual ~file_exception () throw() {}
};

class	file_open_exception : public file_exception {
	static const std::string  prefix_;
public:
	file_open_exception (const std::string& file): file_exception("Error opening file "+file) {  }
};

#endif	/* _UTILS_H_ */
