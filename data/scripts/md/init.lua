local lua_file = nil

--load common functions
lua_file = nuvie_load("common/common.lua"); lua_file();

OBJLIST_OFFSET_HOURS_TILL_NEXT_HEALING  = 0x1cf2
OBJLIST_OFFSET_DREAM_MODE_FLAG          = 0x1d29

OBJLIST_OFFSET_BERRY_COUNTERS           = 0x1d2f

function dbg(msg_string)
	io.stderr:write(msg_string)
end

g_hours_till_next_healing = 0
g_in_dream_mode = false

function update_watch_tile()
   local anim_index = get_anim_index_for_tile(616) --616 = watch tile
   if anim_index ~= nil then
      local new_watch_tile = 416 + clock_get_hour() % 12
      anim_set_first_frame(anim_index, new_watch_tile)
   end
end

function load_game()
   objlist_seek(OBJLIST_OFFSET_HOURS_TILL_NEXT_HEALING)
   g_hours_till_next_healing = objlist_read1()
   
   objlist_seek(OBJLIST_OFFSET_DREAM_MODE_FLAG)
   g_in_dream_mode = bit32.btest(objlist_read2(), 0x10)
   
   update_watch_tile()
end

function save_game()
   objlist_seek(OBJLIST_OFFSET_HOURS_TILL_NEXT_HEALING)
   objlist_write1(g_hours_till_next_healing)
   
   objlist_seek(OBJLIST_OFFSET_DREAM_MODE_FLAG)
   local bytes = objlist_read2()
   if g_in_dream_mode then
      bytes = bit32.bor(bytes, 0x10)
   else
      bytes = bit32.band(bytes, 0xFFEF)
   end
   objlist_seek(OBJLIST_OFFSET_DREAM_MODE_FLAG)
   objlist_write2(bytes)

end



local g_container_obj_tbl = {
[80]=1, [81]=1, [82]=1,
[83]=1, [85]=1, [86]=1,
[87]=1, [89]=1,[304]=1,
[139]=1,[341]=1,[257]=1,
[104]=1,[284]=1,[285]=1
}

function is_container_obj(obj_num)
   if g_container_obj_tbl[obj_num] ~= nil then
      return true
   end
   return false
end

function search(obj)
   if obj.on_map == false then
      return
   end
   
   local found_obj = false
   local child
   local first_loop = true
   local prev_obj = nil
   for child in container_objs(obj) do
      if prev_obj ~= nil then
         printfl("SEARCH_NEXT_OBJ", prev_obj.look_string)
         Obj.moveToMap(prev_obj, obj.x, obj.y, obj.z)
      end

      if first_loop == true then
         found_obj = true
         printfl("SEARCHING_HERE_YOU_FIND", child.look_string)
         Obj.moveToMap(child, obj.x, obj.y, obj.z)
      else
         prev_obj = child
      end

      script_wait(50)
      first_loop = false
   end
   
   if prev_obj ~= nil then
      printfl("SEARCH_LAST_OBJ", prev_obj.look_string)
      Obj.moveToMap(prev_obj, obj.x, obj.y, obj.z)
   end
   
   if found_obj == false then
      printl("SEARCHING_HERE_YOU_FIND_NOTHING")
   else
      print(".\n")
   end
   
end

function look_obj(obj)
	printfl("YOU_SEE", obj.look_string);

	--FIXME usecode look description should be lua code.
	if usecode_look(obj) then
		print("\n")
		return false
	end

	print(".\n");
	
	if is_container_obj(obj.obj_n) then
	  search(obj)
	end

	return false
end

--tile_num, readied location
local g_readiable_objs_tbl = {
-- 0 = head
[512] = 0, --cap
[513] = 0, --cowboy hat
[514] = 0, --pith hat
[515] = 0, --military hat
[516] = 0, --derby
-- 1 = neck
[517] = 1, --kerchief
[518] = 1, --silk scarf
[519] = 1, --muffler
[1048] = 1, --martian jewelry
[1049] = 1, --martian jewelry
[1050] = 1, --martian jewelry
[1051] = 1, --martian jewelry
[1052] = 1, --martian jewelry
[1053] = 1, --martian jewelry
[1054] = 1, --martian jewelry
[1055] = 1, --martian jewelry
-- 3 = 1 handed
[527] = 3, --bloody saber
[552] = 3, --derringer
[553] = 3, --revolver
[560] = 3, --hatchet
[561] = 3, --axe
[562] = 3, --ball-peen hammer
[565] = 3, --knife
[566] = 3, --machete
[567] = 3, --saber
[622] = 3, --pry bar
[650] = 3, --weed sprayer
[667] = 3, --tongs
[1068] = 3, --martian ritual pod knife
-- 2 = torso
[528] = 2, --cloth jacket
[529] = 2, --wool sweater
[530] = 2, --cape
[531] = 2, --duster
[532] = 2, --wool overcoat
[533] = 2, --sheepskin jacket
[534] = 2, --artic parka
[608] = 2, --electric belt
-- 3 = shield hand
[630] = 3, --torch
[631] = 3, --lit torch
[632] = 3, --candlestick
[633] = 3, --lit candlestick
[634] = 3, --candelabra
[635] = 3, --lit candelabra
[636] = 3, --oil lamp
[637] = 3, --lit oil lamp
[638] = 3, --lantern
[639] = 3, --lit lantern
-- 6 = UNK pants or dress FIXME: putting this here for now
[536] = 6, --cotton pants
[537] = 6, --cotton dress
[538] = 6, --denim jeans
[539] = 6, --wool pants
[540] = 6, --wool dress
[541] = 6, --chaps and jeans
-- 7 = feet
[520] = 7, --man's shoes
[521] = 7, --woman's shoes
[522] = 7, --riding boots
[523] = 7, --ruby slippers
[524] = 7, --thigh boots
[525] = 7, --hip boots
[526] = 7, --winged shoes
[542] = 7, --man's shoes
-- 4 = two handed
[551] = 4, --Cupid's bow and arrows
[554] = 4, --shotgun
[555] = 4, --rifle
[556] = 4, --Belgian combine
[557] = 4, --elephant gun
[558] = 4, --sling
[559] = 4, --bow
[563] = 4, --sledge hammer
[576] = 4, --pick
[577] = 4, --shovel
[578] = 4, --hoe
[579] = 4, --rake
[580] = 4, --pitchfork
[581] = 4, --cultivator
[582] = 4, --scythe
[583] = 4, --saw
[717] = 4, --throw rug
[718] = 4, --red cape
[1066] = 4, --heat ray gun
[1067] = 4, --freeze ray gun
[1093] = 4, --spray gun
[1095] = 4, --martian hoe (couldn't be equipped in original)
[1096] = 4, --martian scythe (couldn't be equipped in original)
[1097] = 4, --martian pitchfork (couldn't be equipped in original)
[1098] = 4, --martian rake (couldn't be equipped in original)
[1099] = 4, --martian shovel (couldn't be equipped in original)
[1188] = 4, --M60 machine gun
[1206] = 4, --martian pick (couldn't be equipped in original)
[1897] = 4, --pool cue
-- 5 = finger gloves and bracelets FIXME: putting this here for now
[544] = 5, --lady's silk gloves
[545] = 5, --driving gloves
[546] = 5, --cotton work gloves
[547] = 5, --work gloves
[548] = 5, --wool mittens
[549] = 5, --rubber gloves
[550] = 5 --welding gloves
}

function wrap_coord(coord, level)
   if level > 0 and level < 6 then
      return coord
   end

   if coord < 0 then
      return 1024 + coord
   end
   
   return coord % 1024   
end

function obj_get_readiable_location(obj)
	if g_readiable_objs_tbl[obj.tile_num] ~= nil then
		return g_readiable_objs_tbl[obj.tile_num]
	end

	return -1	
end

function update_lamp_posts()
   --Turn lamps on/off if we have power and it is dark.
   local frame_n = 3
   if Actor.get_talk_flag(0x73, 4) and (hour < 6 or hour > 17) then
      frame_n = 7
   end
   
   local loc = player_get_location()
   for obj in find_obj(loc.z, 228) do --OBJ_LAMP_POST
      if obj ~= nil then
         obj.frame_n = frame_n
      end
   end
end

function play_md_sfx(sfx_id)
   --FIXME
end

function altcode_242_set_actor_talk_flag()
   print("NPC: ")
   local input = input_select(nil, true)
   local actor_num = tonumber(input, 16)
   local actor = Actor.get(actor_num)
   print("\n"..actor.name.."\n")
   print("flags: \n") --FIXME print talk flags
   print("\nBit: ")
   local bit = input_select_integer(nil, true)
   local value = Actor.get_talk_flag(actor, bit)
   local value_str = "off"
   if value == true then
      value_str = "on"
   end
   print(" is "..value_str..".\n")
   print("New value? ")
   value = input_select_integer(nil, true)
   
   value_str = "off"
   
   if value == 1 or value == "o" then
      value_str = "on"
      Actor.set_talk_flag(actor, bit)
   else
      Actor.clear_talk_flag(actor, bit)
   end
   print("\n"..value_str.."\n")
end

function altcode_250_create_object()
   print("Create Item:\nType:0x")
   local input = input_select(nil, true)
   local obj_n = tonumber(input, 16)
   local obj = Obj.new(obj_n)
   local tmp_obj = Obj.new(obj_n+1)
   if tmp_obj ~= nil and tmp_obj.tile_num - obj.tile_num > 1 then
      print("\nFrame:0x")
      input = input_select(nil, true)
      obj.frame_n = tonumber(input, 16) 
   end
   
   print("\nQual:0x")
   input = input_select(nil, true)
   obj.quality = tonumber(input, 16)
   
   if obj.stackable or obj_n == 196 or obj_n == 311 or obj_n == 312 then --OBJ_LEVER, OBJ_SWITCH, OBJ_SWITCH1
      print("\nQuan:0x")
      input = input_select(nil, true)
      obj.qty = tonumber(input, 16)
   end
   Obj.moveToInv(obj, Actor.get(1).actor_num)
   print("\n")
end

function handle_alt_code(altcode)
   if altcode == 242 then
      altcode_242_set_actor_talk_flag()
   elseif altcode == 250 then
      altcode_250_create_object()
   end
end

--load actor functions
local actor_load = nuvie_load("md/actor.lua");
if type(actor_load) == "function" then
	actor_load()
else
	if type(actor_load) == "string" then
		io.stderr:write(actor_load);
	end
end

-- init usecode
usecode_init = nuvie_load("md/usecode.lua"); usecode_init();
