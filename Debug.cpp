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
#include <time.h>
#include <nuvieDefs.h>

void debug(DebugLevelType level, const char *format, ...)
{
  static const char* DebugLevelNames[]= { "EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTIFICATION", "INFORMATIONAL", "DEBUGGING" };
  static DebugLevelType CurrentDebugLevel=LEVEL_DEBUGGING;

  time_t now;
  va_list ap;
  va_start(ap,format);
  
  if (format==NULL) { 
    CurrentDebugLevel=level;
    return;
  }
  if (level>CurrentDebugLevel) { return; } // Don't call ourselves here to log something like 'message suppressed'
  now=time(NULL);
  fprintf(stderr,"%zd [%s]>",(size_t)now,DebugLevelNames[(unsigned char)level]);
  vfprintf(stderr,format,ap);
  va_end(ap);
}

/* test code / documentation.
int main(char ** argv,int argc)
{
  DEBUG(LEVEL_EMERGENCY,NULL); // to set the debug cut-off rather high
  DEBUG(LEVEL_EMERGENCY,"%d %c %s\n",1,'a',"aarrgghh..");
  DEBUG(LEVEL_ALERT,"%d %c %s\n",1,'a',"RED"); // should be suppressed
  DEBUG(LEVEL_DEBUGGING,NULL); // to allow all messages through.
  DEBUG(LEVEL_DEBUGGING,"%d %c %s\n",1,'a',"debugging");
  return 1;
}
*/
