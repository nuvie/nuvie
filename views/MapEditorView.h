#ifndef __MapEditorView_h__
#define __MapEditorView_h__

/*
 *  MapEditorView.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Mar 28 2012.
 *  Copyright (c) 2012. All rights reserved.
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

#include "View.h"

class Configuration;
class GUI_Button;
class MapWindow;

class MapEditorView: public View 
{
private:
	SDL_Surface *roof_tiles;
	MapWindow *map_window;
	uint16 selectedTile;
	uint16 tile_offset;
	GUI_Button *up_button;
	GUI_Button *down_button;
public:
	MapEditorView(Configuration *config);
	virtual ~MapEditorView();

	bool init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Font *f, Party *p, TileManager *tm, ObjManager *om);
	void Display(bool full_redraw);
	GUI_status KeyDown(SDL_Keysym key);
    GUI_status MouseDown(int x, int y, int button);
    GUI_status MouseUp(int x, int y, int button);
    GUI_status MouseMotion(int x,int y,Uint8 state);

    virtual void close_view();
    GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
protected:
    void setTile(uint16 x, uint16 y, uint8 level);
    void toggleGrid();
private:
    void update_selected_tile_relative(sint32 rel_value);
};

#endif /* __MapEditorView_h__ */
