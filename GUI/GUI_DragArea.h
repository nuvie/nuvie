#ifndef __GUI_DragArea_h__
#define __GUI_DragArea_h__
/*
 *  GUI_DragArea.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Sep 02 2003.
 *  Copyright (c) 2003 The Nuvie Team. All rights reserved.
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

class GUI_DragArea {

 protected:
 
 bool dragging;
 
 public:
 
  GUI_DragArea() { dragging = false; }
  
  virtual bool drag_accept_drop(int x, int y, int message, void *data) { return false; }
  virtual void drag_perform_drop(int x, int y, int message, void *data) { return; }
  virtual void drag_drop_failed(int x, int y, int message, void *data) { return; }
  virtual void drag_drop_success(int x, int y, int message, void *data) { return; }

};

#endif /* __GUI_DragArea_h__ */
