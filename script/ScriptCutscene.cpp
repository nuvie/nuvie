/*
 *  ScriptCutscene.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Sep 20 2011.
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
#include <math.h>
#include "nuvieDefs.h"
#include "U6misc.h"
#include "U6Lib_n.h"
#include "NuvieIO.h"
#include "NuvieIOFile.h"
#include "U6Lzw.h"

#include "SoundManager.h"
#include "Font.h"

#include "ScriptCutscene.h"

static ScriptCutscene *cutScene = NULL;

static int nscript_image_set(lua_State *L);
static int nscript_image_get(lua_State *L);

bool nscript_new_image_var(lua_State *L, CSImage *image);
static int nscript_image_gc(lua_State *L);

static sint32 nscript_dec_image_ref_count(CSImage *image);

static int nscript_image_new(lua_State *L);
static int nscript_image_load(lua_State *L);
static int nscript_image_load_all(lua_State *L);
static int nscript_image_print(lua_State *L);
static int nscript_image_draw_line(lua_State *L);
static int nscript_image_static(lua_State *L);
static int nscript_image_bubble_effect_add_color(lua_State *L);
static int nscript_image_bubble_effect(lua_State *L);

static const struct luaL_Reg nscript_imagelib_m[] =
{
   { "__index", nscript_image_get },
   { "__newindex", nscript_image_set },
   { "__gc", nscript_image_gc },
   { NULL, NULL }
};

static int nscript_sprite_set(lua_State *L);
static int nscript_sprite_get(lua_State *L);

bool nscript_new_sprite_var(lua_State *L, CSSprite *image);
static int nscript_sprite_gc(lua_State *L);

static const struct luaL_Reg nscript_spritelib_m[] =
{
   { "__index", nscript_sprite_get },
   { "__newindex", nscript_sprite_set },
   { "__gc", nscript_sprite_gc },
   { NULL, NULL }
};

static int nscript_sprite_new(lua_State *L);

static int nscript_canvas_set_bg_color(lua_State *L);
static int nscript_canvas_set_palette(lua_State *L);
static int nscript_canvas_set_palette_entry(lua_State *L);
static int nscript_canvas_rotate_palette(lua_State *L);
static int nscript_canvas_set_update_interval(lua_State *L);
static int nscript_canvas_set_opacity(lua_State *L);
static int nscript_canvas_update(lua_State *L);
static int nscript_canvas_hide(lua_State *L);
static int nscript_canvas_hide_all_sprites(lua_State *L);
static int nscript_canvas_string_length(lua_State *L);

static int nscript_music_play(lua_State *L);
static int nscript_get_mouse_x(lua_State *L);
static int nscript_get_mouse_y(lua_State *L);
static int nscript_input_poll(lua_State *L);

static int nscript_config_set(lua_State *L);

void nscript_init_cutscene(lua_State *L, Configuration *cfg, GUI *gui, SoundManager *sm)
{
   cutScene = new ScriptCutscene(gui, cfg, sm);

   luaL_newmetatable(L, "nuvie.Image");
   luaL_register(L, NULL, nscript_imagelib_m);

   luaL_newmetatable(L, "nuvie.Sprite");
   luaL_register(L, NULL, nscript_spritelib_m);

   lua_pushcfunction(L, nscript_image_new);
   lua_setglobal(L, "image_new");

   lua_pushcfunction(L, nscript_image_load);
   lua_setglobal(L, "image_load");

   lua_pushcfunction(L, nscript_image_load_all);
   lua_setglobal(L, "image_load_all");

   lua_pushcfunction(L, nscript_image_print);
   lua_setglobal(L, "image_print");

   lua_pushcfunction(L, nscript_image_static);
   lua_setglobal(L, "image_static");

   lua_pushcfunction(L, nscript_sprite_new);
   lua_setglobal(L, "sprite_new");

   lua_pushcfunction(L, nscript_image_bubble_effect_add_color);
   lua_setglobal(L, "image_bubble_effect_add_color");

   lua_pushcfunction(L, nscript_image_bubble_effect);
   lua_setglobal(L, "image_bubble_effect");

   lua_pushcfunction(L, nscript_image_draw_line);
   lua_setglobal(L, "image_draw_line");

   lua_pushcfunction(L, nscript_canvas_set_bg_color);
   lua_setglobal(L, "canvas_set_bg_color");

   lua_pushcfunction(L, nscript_canvas_set_palette);
   lua_setglobal(L, "canvas_set_palette");

   lua_pushcfunction(L, nscript_canvas_set_palette_entry);
   lua_setglobal(L, "canvas_set_palette_entry");

   lua_pushcfunction(L, nscript_canvas_rotate_palette);
   lua_setglobal(L, "canvas_rotate_palette");

   lua_pushcfunction(L, nscript_canvas_set_update_interval);
   lua_setglobal(L, "canvas_set_update_interval");

   lua_pushcfunction(L, nscript_canvas_set_opacity);
   lua_setglobal(L, "canvas_set_opacity");


   lua_pushcfunction(L, nscript_canvas_update);
   lua_setglobal(L, "canvas_update");

   lua_pushcfunction(L, nscript_canvas_hide);
   lua_setglobal(L, "canvas_hide");

   lua_pushcfunction(L, nscript_canvas_hide_all_sprites);
   lua_setglobal(L, "canvas_hide_all_sprites");

   lua_pushcfunction(L, nscript_canvas_string_length);
   lua_setglobal(L, "canvas_string_length");

   lua_pushcfunction(L, nscript_music_play);
   lua_setglobal(L, "music_play");

   lua_pushcfunction(L, nscript_get_mouse_x);
   lua_setglobal(L, "get_mouse_x");

   lua_pushcfunction(L, nscript_get_mouse_y);
   lua_setglobal(L, "get_mouse_y");

   lua_pushcfunction(L, nscript_input_poll);
   lua_setglobal(L, "input_poll");

   lua_pushcfunction(L, nscript_config_set);
   lua_setglobal(L, "config_set");
}

bool nscript_new_image_var(lua_State *L, CSImage *image)
{
   CSImage **userdata;

   userdata = (CSImage **)lua_newuserdata(L, sizeof(CSImage *));

   luaL_getmetatable(L, "nuvie.Image");
   lua_setmetatable(L, -2);

   *userdata = image;

   if(image)
	   image->refcount++;

   return true;
}

CSImage *nscript_get_image_from_args(lua_State *L, int lua_stack_offset)
{
	   CSImage **s_image = (CSImage **)luaL_checkudata(L, lua_stack_offset, "nuvie.Image");
	   if(s_image == NULL)
		   return NULL;

	   return *s_image;
}

static int nscript_image_set(lua_State *L)
{
	CSImage **s_image;
	CSImage *image;
	const char *key;

	s_image = (CSImage **)lua_touserdata(L, 1);
	if(s_image == NULL)
		return 0;

	image = *s_image;
	if(image == NULL)
		return 0;

	key = lua_tostring(L, 2);

	if(!strcmp(key, "w"))
	{
		//obj->x = (uint16)lua_tointeger(L, 3);
		return 0;
	}
   return 0;
}


static int nscript_image_get(lua_State *L)
{
	CSImage **s_image;
	CSImage *image;
	const char *key;

	s_image = (CSImage **)lua_touserdata(L, 1);
	if(s_image == NULL)
		return 0;

	image = *s_image;
	if(image == NULL)
		return 0;

	key = lua_tostring(L, 2);

	if(!strcmp(key, "w"))
	{
		uint16 w, h;
		image->shp->get_size(&w, &h);
		lua_pushinteger(L, w);
		return 1;
	}

	if(!strcmp(key, "h"))
	{
		uint16 w, h;
		image->shp->get_size(&w, &h);
		lua_pushinteger(L, h);
		return 1;
	}

return 0;
}

static sint32 nscript_dec_image_ref_count(CSImage *image)
{
	if(image == NULL)
		return -1;

	image->refcount--;

	return image->refcount;
}

static int nscript_image_gc(lua_State *L)
{
   //DEBUG(0, LEVEL_INFORMATIONAL, "\nImage garbage Collection!\n");

   CSImage **p_image = (CSImage **)lua_touserdata(L, 1);
   CSImage *image;

   if(p_image == NULL)
      return false;

   image = *p_image;

   if(nscript_dec_image_ref_count(image) == 0)
   {
	   if(image->shp)
		   delete image->shp;
	   delete image;
   }

   return 0;
}

static int nscript_image_new(lua_State *L)
{
	uint16 width = lua_tointeger(L, 1);
	uint16 height = lua_tointeger(L, 2);

	U6Shape *shp = new U6Shape();
	if(shp->init(width, height) == false)
		return 0;

	CSImage *image = new CSImage(shp);

	nscript_new_image_var(L, image);
	return 1;
}

static int nscript_image_load(lua_State *L)
{
	const char *filename = lua_tostring(L, 1);
	int idx = -1;

	if(lua_gettop(L) >= 2)
		idx = lua_tointeger(L, 2);

	CSImage *image = cutScene->load_image(filename, idx);

	if(image)
	{
		nscript_new_image_var(L, image);
		return 1;
	}

	return 0;
}

static int nscript_image_load_all(lua_State *L)
{
	const char *filename = lua_tostring(L, 1);
	std::vector<CSImage *> images = cutScene->load_all_images(filename);

	if(images.empty())
	{
		return 0;
	}

	lua_newtable(L);

	for(uint16 i=0;i<images.size();i++)
	{
		lua_pushinteger(L, i);

		nscript_new_image_var(L, images[i]);
		lua_settable(L, -3);
	}

	return 1;
}

static int nscript_image_print(lua_State *L)
{
	CSImage *img = nscript_get_image_from_args(L, 1);
	const char *text = lua_tostring(L, 2);
	uint16 startx = lua_tointeger(L, 3);
	uint16 width = lua_tointeger(L, 4);
	uint16 x = lua_tointeger(L, 5);
	uint16 y = lua_tointeger(L, 6);
	uint8 color = lua_tointeger(L, 7);

	cutScene->print_text(img, text, &x, &y, startx, width, color);

	lua_pushinteger(L, x);
	lua_pushinteger(L, y);

	return 2;
}

static int nscript_image_draw_line(lua_State *L)
{
	CSImage *img = nscript_get_image_from_args(L, 1);
	uint16 sx = lua_tointeger(L, 2);
	uint16 sy = lua_tointeger(L, 3);
	uint16 ex = lua_tointeger(L, 4);
	uint16 ey = lua_tointeger(L, 5);
	uint8 color = lua_tointeger(L, 6);

	if(img)
	{
		img->shp->draw_line(sx, sy, ex, ey, color);
	}

	return 0;
}

static uint8 u6_flask_num_colors = 0;
static uint8 u6_flask_liquid_colors[8];

static int nscript_image_bubble_effect_add_color(lua_State *L)
{
	if(u6_flask_num_colors != 8)
	{
		u6_flask_liquid_colors[u6_flask_num_colors++] = lua_tointeger(L, 1);
	}

	return 0;
}

static int nscript_image_bubble_effect(lua_State *L)
{
	CSImage *img = nscript_get_image_from_args(L, 1);

	if(img && u6_flask_num_colors > 0)
	{
		unsigned char *data = img->shp->get_data();
		uint16 w, h;
		img->shp->get_size(&w, &h);

		for(int i=0;i<w*h;i++)
		{
			if(*data != 0xff)
			{
				*data = u6_flask_liquid_colors[NUVIE_RAND()%u6_flask_num_colors];
			}

			data++;
		}

	}
	return 0;
}

static int nscript_image_static(lua_State *L)
{
	CSImage *img = nscript_get_image_from_args(L, 1);

	if(img)
	{
		unsigned char *data = img->shp->get_data();
		uint16 w, h;
		img->shp->get_size(&w, &h);
		memset(data, 16, w * h);
		for(int i=0;i<1000;i++)
		{
			data[NUVIE_RAND()%(w*h)] = 0;
		}
	}
	return 0;
}

bool nscript_new_sprite_var(lua_State *L, CSSprite *sprite)
{
   CSSprite **userdata;

   userdata = (CSSprite **)lua_newuserdata(L, sizeof(CSSprite *));

   luaL_getmetatable(L, "nuvie.Sprite");
   lua_setmetatable(L, -2);

   *userdata = sprite;

   return true;
}

static int nscript_sprite_set(lua_State *L)
{
	CSSprite **s_sprite;
	CSSprite *sprite;
	const char *key;

	s_sprite = (CSSprite **)lua_touserdata(L, 1);
	if(s_sprite == NULL)
		return 0;

	sprite = *s_sprite;
	if(sprite == NULL)
		return 0;

	key = lua_tostring(L, 2);

	if(!strcmp(key, "x"))
	{
		sprite->x = lua_tointeger(L, 3);
		return 0;
	}

	if(!strcmp(key, "y"))
	{
		sprite->y = lua_tointeger(L, 3);
		return 0;
	}

	if(!strcmp(key, "opacity"))
	{
		int opacity = lua_tointeger(L, 3);
		sprite->opacity = (uint8)clamp(opacity, 0, 255);
		return 0;
	}

	if(!strcmp(key, "visible"))
	{
		sprite->visible = lua_toboolean(L, 3);
		return 0;
	}

	if(!strcmp(key, "image"))
	{
		if(sprite->image)
		{
			sprite->image->refcount--;
			if(sprite->image->refcount == 0)
				delete sprite->image;
		}

		sprite->image = nscript_get_image_from_args(L, 3);
		if(sprite->image)
			sprite->image->refcount++;

		return 0;
	}

	if(!strcmp(key, "clip_x"))
	{
		sprite->clip_rect.x = lua_tointeger(L, 3) + cutScene->get_x_off();
		return 0;
	}

	if(!strcmp(key, "clip_y"))
	{
		sprite->clip_rect.y = lua_tointeger(L, 3) + cutScene->get_y_off();
		return 0;
	}

	if(!strcmp(key, "clip_w"))
	{
		sprite->clip_rect.w = lua_tointeger(L, 3);
		return 0;
	}

	if(!strcmp(key, "clip_h"))
	{
		sprite->clip_rect.h = lua_tointeger(L, 3);
		return 0;
	}
	if(!strcmp(key, "text"))
	{
		const char *text = lua_tostring(L, 3);
		sprite->text = std::string(text);
	}
   return 0;
}


static int nscript_sprite_get(lua_State *L)
{
	CSSprite **s_sprite;
	CSSprite *sprite;
	const char *key;

	s_sprite = (CSSprite **)lua_touserdata(L, 1);
	if(s_sprite == NULL)
		return 0;

	sprite = *s_sprite;
	if(sprite == NULL)
		return 0;

	key = lua_tostring(L, 2);

	if(!strcmp(key, "x"))
	{
		lua_pushinteger(L, sprite->x);
		return 1;
	}

	if(!strcmp(key, "y"))
	{
		lua_pushinteger(L, sprite->y);
		return 1;
	}

	if(!strcmp(key, "opacity"))
	{
		lua_pushinteger(L, sprite->opacity);
		return 1;
	}

	if(!strcmp(key, "visible"))
	{
		lua_pushboolean(L, sprite->visible);
		return 1;
	}

	if(!strcmp(key, "image"))
	{
		if(sprite->image)
		{
			nscript_new_image_var(L, sprite->image);
			return 1;
		}
	}

	if(!strcmp(key, "text"))
	{
		lua_pushstring(L, sprite->text.c_str());
		return 1;
	}

return 0;
}

static int nscript_sprite_gc(lua_State *L)
{
   //DEBUG(0, LEVEL_INFORMATIONAL, "\nSprite garbage Collection!\n");

   CSSprite **p_sprite = (CSSprite **)lua_touserdata(L, 1);
   CSSprite *sprite;

   if(p_sprite == NULL)
      return false;

   sprite = *p_sprite;

   if(sprite)
   {
	   if(sprite->image)
	   {
		   if(nscript_dec_image_ref_count(sprite->image) == 0)
			   delete sprite->image;
	   }
	   cutScene->remove_sprite(sprite);
	   delete sprite;
   }

   return 0;
}

static int nscript_sprite_new(lua_State *L)
{
	CSSprite *sprite = new CSSprite();

	if(lua_gettop(L) >= 1 && !lua_isnil(L, 1))
	{
		sprite->image = nscript_get_image_from_args(L, 1);
		if(sprite->image)
			sprite->image->refcount++;
	}

	if(lua_gettop(L) >= 2 && !lua_isnil(L, 2))
	{
		sprite->x = lua_tointeger(L, 2);
	}

	if(lua_gettop(L) >= 3 && !lua_isnil(L, 3))
	{
		sprite->y = lua_tointeger(L, 3);
	}

	if(lua_gettop(L) >= 4 && !lua_isnil(L, 4))
	{
		sprite->visible = lua_toboolean(L, 4);
	}

	cutScene->add_sprite(sprite);

	nscript_new_sprite_var(L, sprite);
	return 1;
}

static int nscript_canvas_set_bg_color(lua_State *L)
{
	uint8 color = lua_tointeger(L, 1);
	cutScene->set_bg_color(color);

	return 0;
}

static int nscript_canvas_set_palette(lua_State *L)
{
	const char *filename = lua_tostring(L, 1);
	uint8 idx = lua_tointeger(L, 2);

	cutScene->load_palette(filename, idx);

	return 0;
}

static int nscript_canvas_set_palette_entry(lua_State *L)
{
	uint8 idx = lua_tointeger(L, 1);
	uint8 r = lua_tointeger(L, 2);
	uint8 g = lua_tointeger(L, 3);
	uint8 b = lua_tointeger(L, 4);

	cutScene->set_palette_entry(idx, r, g, b);

	return 0;
}

static int nscript_canvas_rotate_palette(lua_State *L)
{
	uint8 idx = lua_tointeger(L, 1);
	uint8 len = lua_tointeger(L, 2);

	cutScene->rotate_palette(idx, len);

	return 0;
}

static int nscript_canvas_set_update_interval(lua_State *L)
{
	uint16 loop_interval = lua_tointeger(L, 1);

	cutScene->set_update_interval(loop_interval);
	return 0;
}

static int nscript_canvas_set_opacity(lua_State *L)
{
	uint8 opacity = lua_tointeger(L, 1);

	cutScene->set_screen_opacity(opacity);
	return 0;
}

static int nscript_canvas_update(lua_State *L)
{
	cutScene->update();
	return 0;
}

static int nscript_canvas_hide(lua_State *L)
{
	cutScene->Hide();
	cutScene->get_sound_manager()->musicStop();
	return 0;
}

static int nscript_canvas_hide_all_sprites(lua_State *L)
{
	cutScene->hide_sprites();
	return 0;
}

static int nscript_canvas_string_length(lua_State *L)
{
	Font *font = cutScene->get_font();
	const char *str = lua_tostring(L, 1);

	lua_pushinteger(L, font->getStringWidth(str));
	return 1;
}

static int nscript_music_play(lua_State *L)
{
	const char *filename = lua_tostring(L, 1);

	cutScene->get_sound_manager()->musicPlay(filename);

	return 0;
}

static int nscript_get_mouse_x(lua_State *L)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	int scale_factor = cutScene->get_screen()->get_scale_factor();
	if(scale_factor > 1)
		x /= scale_factor;
	x -= cutScene->get_x_off();
	lua_pushinteger(L, x);
	return 1;
}

static int nscript_get_mouse_y(lua_State *L)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	int scale_factor = cutScene->get_screen()->get_scale_factor();
	if(scale_factor > 1)
		y /= scale_factor;
	y -= cutScene->get_y_off();
	lua_pushinteger(L, y);
	return 1;
}

static int nscript_input_poll(lua_State *L)
{
	SDL_Event event;
	bool poll_mouse_motion;
	if(lua_isnil(L, 1))
		poll_mouse_motion = false;
	else
		poll_mouse_motion = lua_toboolean(L, 1);
	while(SDL_PollEvent(&event))
	{
		//FIXME do something here.
		if(event.type == SDL_KEYDOWN)
		{
			if((((event.key.keysym.mod & KMOD_CAPS) == KMOD_CAPS && (event.key.keysym.mod & KMOD_SHIFT) == 0) || ((event.key.keysym.mod & KMOD_CAPS) == 0 && event.key.keysym.mod & KMOD_SHIFT)) && event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
				lua_pushinteger(L, event.key.keysym.sym-32);
			else
				lua_pushinteger(L, event.key.keysym.sym);
			return 1;
		}
		if(event.type == SDL_MOUSEBUTTONDOWN)
		{
			lua_pushinteger(L, 0);
			return 1;
		}
		if(poll_mouse_motion && event.type == SDL_MOUSEMOTION)
		{
			lua_pushinteger(L, 1);
			return 1;
		}
	}

	return 0;
}

static int nscript_config_set(lua_State *L)
{
	const char *config_key = lua_tostring(L, 1);

	if(lua_isstring(L, 2))
	{
		cutScene->get_config()->set(config_key, lua_tostring(L, 2));
	}
	else if(lua_isnumber(L, 2))
	{
		cutScene->get_config()->set(config_key, (int)lua_tointeger(L, 2));
	}
	else if(lua_isboolean(L, 2))
	{
		cutScene->get_config()->set(config_key, (bool)lua_toboolean(L, 2));
	}

	return 0;
}

ScriptCutscene::ScriptCutscene(GUI *g, Configuration *cfg, SoundManager *sm) : GUI_Widget(NULL)
{
	config = cfg;
	gui = g;

	int off;
	config->value("config/video/x_offset", off, 0);
	x_off = off;
	config->value("config/video/y_offset", off, 0);
	y_off = off;

	nuvie_game_t game_type = get_game_type(config);

	GUI_Widget::Init(NULL, 0, 0, g->get_width(), g->get_height());

	clip_rect.x = x_off;
	clip_rect.y = y_off;
	clip_rect.w = 320;
	clip_rect.h = 200;

	screen = g->get_screen();
	gui->AddWidget(this);
	Hide();
	sound_manager = sm;

	//FIXME this should be loaded by script.
	std::string path;


	font = new Font();

	if(game_type == NUVIE_GAME_U6)
	{
		config_get_path(config, "u6.set", path);
		font->init(path.c_str());
	}
	//FIXME load other fonts for MD / SE if needed here.

	next_time = 0;
	loop_interval = 40;

	screen_opacity = 255;
	bg_color = 0;
}

ScriptCutscene::~ScriptCutscene()
{
	delete font;
}

CSImage *ScriptCutscene::load_image(const char *filename, int idx)
{
	U6Lib_n lib_n;
	std::string path;
	CSImage *image = NULL;

	config_get_path(config, filename, path);

	U6Shape *shp = new U6Shape();

	if(idx >= 0)
	{
		U6Lzw lzw;

		U6Lib_n lib_n;
		uint32 decomp_size;
		unsigned char *buf = lzw.decompress_file(path.c_str(), decomp_size);
		NuvieIOBuffer io;
		io.open(buf, decomp_size, false);
		if(lib_n.open(&io, 4, NUVIE_GAME_MD))
		{
			if(shp->load(&lib_n, (uint32)idx))
			{
				image = new CSImage(shp);
			}
		}
		free(buf);
	}
	else
	{
		if(shp->load(path))
		{
			image = new CSImage(shp);
		}
	}

	if(image == NULL)
		delete shp;

	return image;
}

std::vector<CSImage *> ScriptCutscene::load_all_images(const char *filename)
{
	std::string path;
	CSImage *image = NULL;

	config_get_path(config, filename, path);


	std::vector<CSImage *> v;
	U6Lzw lzw;

	U6Lib_n lib_n;
	uint32 decomp_size;
	unsigned char *buf = lzw.decompress_file(path.c_str(), decomp_size);
	NuvieIOBuffer io;
	io.open(buf, decomp_size, false);
	if(!lib_n.open(&io, 4, NUVIE_GAME_MD))
	{
		free(buf);
		return v;
	}

	for(uint32 idx=0;idx<lib_n.get_num_items();idx++)
	{
		U6Shape *shp = new U6Shape();
		if(shp->load(&lib_n, (uint32)idx))
		{
			image = new CSImage(shp);
			v.push_back(image);
		}
	}
	free(buf);

	return v;

}
void ScriptCutscene::print_text(CSImage *image, const char *s, uint16 *x, uint16 *y, uint16 startx, uint16 width, uint8 color)
{
	int len=*x-startx;
	size_t start=0;
	size_t found;
	std::string str = s;
	std::list<std::string> tokens;
	int space_width = font->getStringWidth(" ");
	//uint16 x1 = startx;

	  found=str.find_first_of(" ", start);
	  while (found!=string::npos)
	  {
	    std::string token = str.substr(start,found-start);

	    int token_len = font->getStringWidth(token.c_str());

	    if(len + token_len + space_width > width)
	    {
	    	//FIXME render line here.
	    	list<std::string>::iterator it;
	    	int new_space = 0;
	    	if(tokens.size() > 1)
	    		new_space = floor((width - (len - space_width * (tokens.size() - 1))) / (tokens.size() - 1));

	    	  for(it=tokens.begin() ; it != tokens.end() ; it++ )
	    	  {
	    		  *x = font->drawStringToShape(image->shp, (*it).c_str(), *x, *y, color);
	    		  *x += new_space;
	    	  }
	    	*y += 8;
	    	*x = startx;
	    	len = token_len + space_width;
	    	tokens.clear();
	    	tokens.push_back(token);
	    }
	    else
	    {
	    	tokens.push_back(token);
	    	len += token_len + space_width;
	    }

	    start = found + 1;
	    found=str.find_first_of(" ", start);
	  }

	  list<std::string>::iterator it;

	  for(it=tokens.begin() ; it != tokens.end() ; it++ )
	  {
	  	 *x = font->drawStringToShape(image->shp, (*it).c_str(), *x, *y, color);
	  	 *x += space_width;
	  }

	  if(start < str.length())
	  {
		std::string token = str.substr(start, str.length() - start);
	    if(len + font->getStringWidth(token.c_str()) > width)
	    {
	    	*y += 8;
	    	*x = startx;
	    }
	  	*x = font->drawStringToShape(image->shp, token.c_str(), *x, *y, color);
	  }


	//font->drawStringToShape(image->shp, string, x, y, color);
}

void ScriptCutscene::load_palette(const char *filename, int idx)
{
	NuvieIOFileRead file;
	uint8 buf[0x240];
	uint8 unpacked_palette[0x300];
	std::string path;

	config_get_path(config, filename, path);


	if(file.open(path.c_str()) == false)
	{
		DEBUG(0,LEVEL_ERROR,"loading palette.\n");
		return;
	}

	if(strlen(filename) > 4 && strcasecmp((const char *)&filename[strlen(filename)-4], ".lbm") == 0)
	{
		//deluxe paint palette file.
		file.seek(0x30);
		file.readToBuf(unpacked_palette, 0x300);
	}
	else
	{
		file.seek(idx * 0x240);

		file.readToBuf(buf, 0x240);
	    //printf("pal%d\n",idx);

	    for (int i = 0; i < 0x100; i++)
	    {
	    	//printf("%d:",idx);
	        for (int j = 0; j < 3; j++)
	        {
	            int byte_pos = (i*3*6 + j*6) / 8;
	            int shift_val = (i*3*6 + j*6) % 8;
	            int color = ((buf[byte_pos] +
	                        (buf[byte_pos+1] << 8))
	                        >> shift_val) & 0x3F;
	            unpacked_palette[i*3+j] = (uint8) (color << 2);
	            //printf("%d ", unpacked_palette[i*3+j]);
	        }
	        //printf(" untitled\n");
	    }
	}
	screen->set_palette(unpacked_palette);
}

void ScriptCutscene::set_palette_entry(uint8 idx, uint8 r, uint8 g, uint8 b)
{
	screen->set_palette_entry(idx, r, g, b);
}

void ScriptCutscene::rotate_palette(uint8 idx, uint8 length)
{
	screen->rotate_palette(idx, length);
}

void ScriptCutscene::set_update_interval(uint16 interval)
{
	loop_interval = interval;
}

void ScriptCutscene::set_screen_opacity(uint8 new_opacity)
{
	screen_opacity = new_opacity;
}

void ScriptCutscene::hide_sprites()
{
	for(std::list<CSSprite *>::iterator it = sprite_list.begin(); it != sprite_list.end(); it++)
	{
		CSSprite *s = *it;
		if(s->visible)
			s->visible = false;
	}
}

void ScriptCutscene::update()
{
	if(cutScene->Status() == WIDGET_HIDDEN)
	{
		cutScene->Show();
		gui->force_full_redraw();
	}

	gui->Display();
	screen->preformUpdate();
	sound_manager->update();
	wait();
}

void ScriptCutscene::wait()
{
    uint32 now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+loop_interval;
        return;
    }

    uint32 delay = next_time-now;
    next_time += loop_interval;
    SDL_Delay(delay);
}

/* Show the widget  */
void ScriptCutscene::Display(bool full_redraw)
{
	if(full_redraw)
		screen->fill(bg_color,0,0,area.w, area.h);
	else
		screen->fill(bg_color,x_off,y_off,320, 200);

	if(screen_opacity > 0)
	{
		for(std::list<CSSprite *>::iterator it = sprite_list.begin(); it != sprite_list.end(); it++)
		{
			CSSprite *s = *it;
			if(s->visible)
			{
				if(s->image)
				{
					uint16 w, h;
					s->image->shp->get_size(&w, &h);
					uint16 x, y;
					s->image->shp->get_hot_point(&x, &y);
					screen->blit(x_off+s->x-x, y_off+s->y-y, s->image->shp->get_data(), 8, w, h, w, true, s->clip_rect.w != 0 ? &s->clip_rect : &clip_rect, s->opacity);
				}

				if(s->text.length() > 0)
				{
					font->drawString(screen, s->text.c_str(), s->x + x_off, s->y + y_off);
				}
			}
		}

		if(screen_opacity < 255)
		{
			screen->fade(x_off,y_off,320,200, screen_opacity, bg_color);
		}
	}

	if(full_redraw)
		screen->update(0,0,area.w,area.h);
	else
		screen->update(x_off,y_off,320, 200);
}


