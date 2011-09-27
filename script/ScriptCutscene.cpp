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
#include "nuvieDefs.h"
#include "U6misc.h"
#include "U6Lib_n.h"
#include "NuvieIO.h"
#include "NuvieIOFile.h"
#include "U6Lzw.h"

#include "ScriptCutscene.h"

static ScriptCutscene *cutScene = NULL;

static int nscript_image_set(lua_State *L);
static int nscript_image_get(lua_State *L);

bool nscript_new_image_var(lua_State *L, CSImage *image);
static int nscript_image_gc(lua_State *L);

static sint32 nscript_dec_image_ref_count(CSImage *image);

static int nscript_image_load(lua_State *L);

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

static int nscript_canvas_set_palette(lua_State *L);
static int nscript_canvas_update(lua_State *L);
static int nscript_canvas_hide(lua_State *L);

void nscript_init_cutscene(lua_State *L, Configuration *cfg, GUI *gui)
{
   cutScene = new ScriptCutscene(gui, cfg);

   luaL_newmetatable(L, "nuvie.Image");
   luaL_register(L, NULL, nscript_imagelib_m);

   luaL_newmetatable(L, "nuvie.Sprite");
   luaL_register(L, NULL, nscript_spritelib_m);

   lua_pushcfunction(L, nscript_image_load);
   lua_setglobal(L, "image_load");

   lua_pushcfunction(L, nscript_sprite_new);
   lua_setglobal(L, "sprite_new");

   lua_pushcfunction(L, nscript_canvas_set_palette);
   lua_setglobal(L, "canvas_set_palette");

   lua_pushcfunction(L, nscript_canvas_update);
   lua_setglobal(L, "canvas_update");

   lua_pushcfunction(L, nscript_canvas_hide);
   lua_setglobal(L, "canvas_hide");
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
	   delete image;

   return 0;
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

	if(lua_gettop(L) >= 1)
	{
		sprite->image = nscript_get_image_from_args(L, 1);
		if(sprite->image)
			sprite->image->refcount++;
	}

	cutScene->add_sprite(sprite);

	nscript_new_sprite_var(L, sprite);
	return 1;
}

static int nscript_canvas_set_palette(lua_State *L)
{
	const char *filename = lua_tostring(L, 1);
	uint8 idx = lua_tointeger(L, 2);

	cutScene->load_palette(filename, idx);

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
	return 0;
}

ScriptCutscene::ScriptCutscene(GUI *g, Configuration *cfg) : GUI_Widget(NULL, 0, 0, g->get_width(), g->get_height())
{
	config = cfg;
	gui = g;
	screen = g->get_screen();
	gui->AddWidget(this);
	Hide();
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

void ScriptCutscene::load_palette(const char *filename, int idx)
{
	NuvieIOFileRead file;
	uint8 buf[0x240];
	std::string path;

	config_get_path(config, filename, path);

	if(file.open(path.c_str()) == false)
	{
		DEBUG(0,LEVEL_ERROR,"loading palette.\n");
		return;
	}

	file.seek(idx * 0x240);

	file.readToBuf(buf, 0x240);

	    uint8 unpacked_palette[0x300];
	    for (int i = 0; i < 0x100; i++)
	    {
	        for (int j = 0; j < 3; j++)
	        {
	            int byte_pos = (i*3*6 + j*6) / 8;
	            int shift_val = (i*3*6 + j*6) % 8;
	            int color = ((buf[byte_pos] +
	                        (buf[byte_pos+1] << 8))
	                        >> shift_val) & 0x3F;
	            unpacked_palette[i*3+j] = (uint8) (color << 2);
	        }
	    }

	screen->set_palette(unpacked_palette);
}

void ScriptCutscene::update()
{
	if(cutScene->Status() == WIDGET_HIDDEN)
		cutScene->Show();

	gui->Display();
	screen->preformUpdate();
	SDL_Delay(20); //FIXME implement scriptable wait period here.
}

/* Show the widget  */
void ScriptCutscene::Display(bool full_redraw)
{
	screen->fill(0,0,0,area.w, area.h);
	for(std::list<CSSprite *>::iterator it = sprite_list.begin(); it != sprite_list.end(); it++)
	 {
		CSSprite *s = *it;
		uint16 w, h;
		s->image->shp->get_size(&w, &h);
		screen->blit(s->x, s->y, s->image->shp->get_data(), 8, w, h, w, true, NULL, s->opacity);
	 }

	 screen->update(area.x,area.y,area.w,area.h);
}
