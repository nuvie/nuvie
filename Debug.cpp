/*
 *  Debug.cpp
 *  Nuvie
 *
 *  Created by Pieter Luteijn on Wed Aug 08 2007.
 *  Copyright (c) 2007. All rights reserved.
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <nuvieDefs.h>
#ifndef WITHOUT_DEBUG

void debug(bool no_header,DebugLevelType level, const char *format, ...)
{
// original
//  static const char* DebugLevelNames[]= { "EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTIFICATION", "INFORMATIONAL", "DEBUGGING" };
//  shorter, because spammy enough as is.
//  static const char* DebugLevelNames[]= { "EMERG", "ALERT", "CRIT.", "ERROR", "WARN.", "NOTE.", "INFO.", "DEBUG" };
//  shorter, because spammy enough as is.
  static const char* DebugLevelNames[]= { "!", "A", "C", "E", "W", "N", "I", "D" };
  static DebugLevelType CurrentDebugLevel=LEVEL_DEBUGGING;

  time_t now;
  va_list ap;
  va_start(ap,format);
  
  if (format==NULL) { 
    CurrentDebugLevel=level;
    return;
  }
  if (!strcmp(format,"!!increase!!\n")) { 
    unsigned char c=(unsigned char) CurrentDebugLevel;
    if (c<7) {c++;}
    level=CurrentDebugLevel=(DebugLevelType) c;
  }
  if (!strcmp(format,"!!decrease!!\n")) { 
    unsigned char c=(unsigned char) CurrentDebugLevel;
    if (c>0) {c--;}
    level=CurrentDebugLevel=(DebugLevelType) c;
  }
  if (level>CurrentDebugLevel) { return; } // Don't call ourselves here to log something like 'message suppressed'
  now=time(NULL);
  if (!no_header) {fprintf(stderr,"%zd [%s]> ",(size_t)now,DebugLevelNames[(unsigned char)level]);}
  vfprintf(stderr,format,ap);
  va_end(ap);
}
#endif /* WITHOUT_DEBUG */

/* test code / documentation.
int main(char ** argv,int argc)
{
  DEBUG(0,LEVEL_EMERGENCY,NULL); // to set the debug cut-off rather high
  DEBUG(0,LEVEL_EMERGENCY,"%d %c %s\n",1,'a',"aarrgghh..");
  DEBUG(1,LEVEL_EMERGENCY,"continuation of aarrgghh..");
  DEBUG(0,LEVEL_ALERT,"%d %c %s\n",1,'a',"RED"); // should be suppressed
  DEBUG(0,LEVEL_DEBUGGING,NULL); // to allow all messages through.
  DEBUG(0,LEVEL_DEBUGGING,"%d %c %s\n",1,'a',"debugging");
  return 1;
}
*/
