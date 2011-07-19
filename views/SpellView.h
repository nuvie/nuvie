#ifndef __SpellView_h__
#define __SpellView_h__

/*
 *  SpellView.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jun 19 2011.
 *  Copyright (c) 2011. All rights reserved.
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
class TileManager;
class ObjManager;
class Screen;
class Actor;
class Text;
class U6Bmp;
class Spell;

class SpellView : public View {

 Obj *spell_container;
 Actor *caster;
 uint16 caster_reagents[8];

 uint8 level;
 uint8 spell_num;

 U6Bmp *background;
 sint16 cur_spells[16];
 bool all_spells_mode;
 bool event_mode; //this means we are reporting the spell_num back to the event class. Used by the enchant spell.
 public:
 SpellView(Configuration *cfg);
 ~SpellView();

 bool init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om);

 void set_spell_caster(Actor *actor, Obj *s_container, bool eventMode);
 sint16 get_selected_spell() { if(spell_container) { return spell_container->quality; } else return -1; }
 void Display(bool full_redraw);
 void PlaceOnScreen(Screen *s, GUI_DragManager *dm, int x, int y);
 GUI_status KeyDown(SDL_keysym key);
 GUI_status MouseDown(int x, int y, int button);

 protected:

 void add_command_icons(Screen *tmp_screen, void *view_manager);
 void hide_buttons();
 void show_buttons();
 void update_buttons();

 void move_left();
 void move_right();
 void move_up();
 void move_down();

 void set_prev_level();
 void set_next_level();

 uint8 fill_cur_spell_list();
 sint8 get_selected_index();

 void display_level_text();
 void display_spell_list_text();
 void display_spell_text(Spell *spell, uint16 line_num, uint8 selected_spell);

 uint16 get_available_spell_count(Spell *s);

 GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
};

#endif /* __SpellView_h__ */

