/*
 *  TMXMap.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jan 18 2015.
 *  Copyright (c) 2015. All rights reserved.
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

#include "TileManager.h"
#include "Map.h"
#include "ObjManager.h"
#include "U6misc.h"
#include "TMXMap.h"

TMXMap::TMXMap(TileManager *tm, Map *m, ObjManager *om)
{
  tile_manager = tm;
  map = m;
  obj_manager = om;
  mapdata = NULL;
  game_type = NUVIE_GAME_NONE;
}

TMXMap::~TMXMap()
{

}

bool TMXMap::exportTmxMapFiles(std::string dir, nuvie_game_t type)
{
  savedir = dir;
  savename = get_game_tag(type);
  std::string filename;
  build_path(savedir, savename + "_tileset.bmp", filename);



  tile_manager->exportTilesetToBmpFile(filename);

  for(uint8 i=0;i<6;i++)
  {
    writeRoofTileset(i);
    exportMapLevel(i);
  }

  return true;
}

void TMXMap::writeRoofTileset(uint8 level)
{
  if(map->get_roof_data(level) == NULL)
  {
    return;
  }

  std::string filename = map->getRoofTilesetFilename();
  std::string destFilename;
  build_path(savedir, savename +"_roof_tileset.bmp", destFilename);
  NuvieIOFileRead read;
  NuvieIOFileWrite write;
  read.open(filename);
  write.open(destFilename);
  unsigned char *buf = read.readAll();
  write.writeBuf(buf, read.get_size());
  write.close();
  read.close();
  free(buf);
}

void TMXMap::writeLayer(NuvieIOFileWrite *tmx, uint16 sideLength, std::string layerName, uint16 gidOffset, uint16 bitsPerTile, unsigned char *data)
{
  std::string slen = sint32ToString((sint32)sideLength);
  std::string header = " <layer name=\"" + layerName + "\" width=\"" + slen + "\" height=\""
      + slen + "\">\n";
  header += "  <data encoding=\"csv\">\n";

  tmx->writeBuf((unsigned char *) header.c_str(), header.length());

  char buf[5]; // 'nnnn\0'

  uint16 mx,my;
  for(my=0;my<sideLength;my++)
  {
    for(mx=0;mx<sideLength;mx++)
    {
      uint16 gid = 0;
      if(bitsPerTile == 8) //base map is uint8
      {
        gid = (uint16)data[my * sideLength + mx]+1+gidOffset;
      }
      else //everything else is uint16
      {
        gid = ((uint16 *)data)[my * sideLength + mx]+1+gidOffset;
      }
      snprintf(buf, sizeof(buf), "%d", gid);
      tmx->writeBuf((unsigned char *)buf, strlen(buf));
      if(mx < sideLength-1 || my < sideLength-1) //don't write comma after last element in the array.
      {
        tmx->write1(',');
      }
    }
    tmx->write1('\n');
  }

  std::string footer = "  </data>\n";
  footer += " </layer>\n";

  tmx->writeBuf((unsigned char *)footer.c_str(), footer.length());
}

void TMXMap::writeObjectLayer(NuvieIOFileWrite *tmx, uint8 level)
{
  uint16 width = map->get_width(level);
  std::string xml = "<objectgroup name=\"Object Layer\">\n";
  tmx->writeBuf((unsigned char *)xml.c_str(), xml.length());

  for(uint16 y=0;y<width;y++)
  {
    for(uint16 x=0;x<width;x++)
    {
      U6LList *list = obj_manager->get_obj_list(x, y, level);
      if(list)
      {
        for(U6Link *link=list->start(); link != NULL; link=link->next)
        {
          Obj *obj = (Obj *)link->data;
          Tile *t = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
          std::string s = "  <object name=\""+encode_xml_entity(std::string(obj_manager->get_obj_name(obj)))+"\" gid=\""+sint32ToString(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n+1)+"\" x=\""+sint32ToString((x)*16)+"\" y=\""+sint32ToString((y+1)*16)+"\" width=\"16\" height=\"16\">\n";
          s += "    <properties>\n";
          s += "       <property name=\"obj_n\" value=\""+sint32ToString(obj->obj_n)+"\"/>\n";
          s += "       <property name=\"frame_n\" value=\""+sint32ToString(obj->frame_n)+"\"/>\n";
          s += "       <property name=\"qty\" value=\""+sint32ToString(obj->qty)+"\"/>\n";
          s += "       <property name=\"quality\" value=\""+sint32ToString(obj->quality)+"\"/>\n";
          s += "       <property name=\"status\" value=\""+sint32ToString(obj->status)+"\"/>\n";
          s += "    </properties>\n";
          s += "  </object>\n";
          if(t->dbl_width)
          {
            s += "  <object name=\""+std::string(obj_manager->get_obj_name(obj))+" -x\" gid=\""+sint32ToString(t->tile_num-1+1)+"\" x=\""+sint32ToString((x-1)*16)+"\" y=\""+sint32ToString((y+1)*16)+"\" width=\"16\" height=\"16\"/>\n";
          }
          if(t->dbl_height)
          {
            uint16 tile_num = t->tile_num - 1;
            if(t->dbl_width)
            {
              tile_num--;
            }
            s += "  <object name=\""+std::string(obj_manager->get_obj_name(obj))+" -y\" gid=\""+sint32ToString(tile_num+1)+"\" x=\""+sint32ToString((x)*16)+"\" y=\""+sint32ToString((y+1-1)*16)+"\" width=\"16\" height=\"16\"/>\n";
          }
          if(t->dbl_width && t->dbl_height)
          {
            uint16 tile_num = t->tile_num - 3;

            s += "  <object name=\""+std::string(obj_manager->get_obj_name(obj))+" -x,-y\" gid=\""+sint32ToString(tile_num+1)+"\" x=\""+sint32ToString((x-1)*16)+"\" y=\""+sint32ToString((y+1-1)*16)+"\" width=\"16\" height=\"16\"/>\n";
          }
          tmx->writeBuf((unsigned char *)s.c_str(), s.length());
        }
      }
    }
  }

  xml = "</objectgroup>\n";
  tmx->writeBuf((unsigned char *)xml.c_str(), xml.length());
}

bool TMXMap::exportMapLevel(uint8 level)
{
  NuvieIOFileWrite tmx;
  uint16 width = map->get_width(level);
  mapdata = map->get_map_data(level);
  char level_string[3]; // 'nn\0'
  std::string filename;
  snprintf(level_string, sizeof(level_string), "%d", level);
  build_path(savedir, savename + "_" + std::string(level_string) + ".tmx", filename);

  tmx.open(filename);
  std::string swidth = sint32ToString((sint32)width);
  std::string header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  header +=
      "<map version=\"1.0\" orientation=\"orthogonal\" renderorder=\"right-down\" width=\""
          + swidth + "\" height=\"" + swidth
          + "\" tilewidth=\"16\" tileheight=\"16\">\n";
  header +=
      " <tileset firstgid=\"1\" name=\"tileset\" tilewidth=\"16\" tileheight=\"16\">\n";
  header += "  <image source=\"" + savename
      + "_tileset.bmp\" trans=\"00dffc\" width=\"512\" height=\"1024\"/>\n";
  header += " </tileset>\n";

  if(map->get_roof_data(level) != NULL)
  {
    header +=
        " <tileset firstgid=\"2048\" name=\"roof_tileset\" tilewidth=\"16\" tileheight=\"16\">\n";
    header += "  <image source=\""+savename+"_roof_tileset.bmp\" trans=\"0070fc\" width=\"80\" height=\"3264\"/>\n";
    header += " </tileset>\n";
  }

  tmx.writeBuf((unsigned char *) header.c_str(), header.length());

  writeLayer(&tmx, width, "BaseLayer", 0, 8, mapdata);

  writeObjectLayer(&tmx, level);

  if(map->get_roof_data(level) != NULL)
  {
    writeLayer(&tmx, width, "RoofLayer", 2047, 16, (unsigned char *)map->get_roof_data(level));
  }

  std::string footer = "</map>\n";


  tmx.writeBuf((unsigned char *)footer.c_str(), footer.length());



  tmx.close();

  return true;
}

std::string TMXMap::sint32ToString(sint32 value)
{
  char buf[12];
  snprintf(buf, sizeof(buf), "%d", value);
  return std::string(buf);
}
