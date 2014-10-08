local USE_EVENT_USE = 0x01
local USE_EVENT_READY = 0x0100

function search_container(obj)
	local child
	for child in container_objs(obj) do
		Obj.moveToMap(child, obj.x, obj.y, obj.z)
	end
end

function use_door(obj, actor)
	--FIXME check for blocked doorways.
	--FIXME check for locked doors.
	if obj.frame_n < 3 then
		obj.frame_n = 3
	else
		obj.frame_n = 1
	end
end

function use_crate(obj, target_obj, actor)
	if target_obj.frame_n == 2 then
		target_obj.frame_n = 1
		printl("YOU_PRIED_THE_NAILS_OUT_OF_THE_CRATE")
	else
		if target_obj.frame_n == 1 and obj.obj_n == 273 then
			target_obj.frame_n = 2
			printl("YOU_NAIL_THE_CRATE_SHUT")
		else
			printl("IT_HAS_NO_EFFECT")
		end
	end
	
end

function use_container(obj, actor)
	if obj.frame_n == 2 then
		if obj.obj_n == 86 or obj.obj_n == 284 then
			printl("IT_IS_NAILED_SHUT")
		else
			printl("IT_IS_LOCKED")
		end
	else
		if obj.frame_n == 1 then
			obj.frame_n = 0
			search_container(obj)
		else
			obj.frame_n = 1
		end
	end
end

function use_prybar_on_hatch(obj, target_obj, actor)

	if actor.actor_num ~= 1 then
		printfl("IS_NOT_STRONG_ENOUGH", actor.name)
		return
	end
	
	local tesla = Actor.get(16)
	if Actor.get_talk_flag(tesla, 4) == false then
		Actor.set_talk_flag(tesla, 2)
		Actor.talk(tesla)
	else
		play_midgame_sequence(1)
		
		Actor.set_talk_flag(tesla, 5)
		target_obj.obj_n = 428
		target_obj.frame_n = 0;
		target_obj.x = target_obj.x + 1
		target_obj.y = target_obj.y + 1
		
		local blood = Actor.get(18)
		Actor.set_talk_flag(blood, 3)
		Actor.set_talk_flag(blood, 6)
		Actor.talk(blood)
	end
end

function use_sextant(obj, actor)
	if actor.z ~= 0 then
		printl("NOT_WHILE_UNDERGROUND")
		return
	end

	local lat_str = "N"
	local long_str = "W"
	
	local lat = math.modf(((actor.y - 512) * 240) / 1024)
	local long = math.modf(((actor.x - 512) * 360) / 1024)
	
	if lat > 0 then
		lat_str = "S"
	else
		if lat == 0 then
			lat_str = " "
		end
	end
	
	if long == 180 or long == -180 or long == 0 then
		long_str = " "
	else
		if long < 0 then
			long_str = "E"
		end
	end
	
	lat = math.abs(lat)
	long = 180 - math.abs(long)
	printl("YOU_ARE_SOMEWHERE_NEAR")
	print(" \nLat:" ..lat.." "..lat_str.."\nLong:"..long.." "..long_str.."\n")
end

function use_berry(obj, actor)
   local actor_num = actor.actor_num
   if actor_num == 6 then
      printl("A_MECHANICAL_PERSON_CANT_EAT_BERRIES")
      return
   end
   
   --FIXME play_sfx 0x32
   local berry_type = obj.obj_n - 73 --OBJ_BERRY
   local first_berry = true
   
   if (berry_type == 0 and actor_is_affected_by_purple_berries(actor_num))
      or (berry_type == 1 and actor_is_affected_by_green_berries(actor_num))
      or (berry_type == 2 and actor_is_affected_by_brown_berries(actor_num)) then
      printl("YOU_EAT_A_MARTIAN_BERRY_YOU_FEEL_AN_INCREASE_IN_THE_STRANGE")
   else
      printl("YOU_EAT_A_MARTIAN_BERRY_YOU_FEEL_A_STRANGE")
   end

   if berry_type == 0 then
      printl("RELATIONSHIP_WITH_THINGS_AROUND_YOU")
      actor_increment_purple_berry_count(actor_num)
   elseif berry_type == 1 then
      printl("SENSITIVITY_TO_THE_FEELINGS_OF_OBJECTS_AROUND_YOU")
      actor_increment_green_berry_count(actor_num)
   elseif berry_type == 2 then
      printl("SENSE_OF_YOUR_SPATIAL_LOCATION")
      actor_increment_brown_berry_count(actor_num)
   elseif berry_type == 3 then
      printl("SUDDEN_FLUSH_DIZZINESS_AND_NAUSEA")
      actor.poisoned = false
      Actor.hit(actor, math.random(5, 0x14) + math.random(5, 0x14))
      local counter = actor_get_blue_berry_counter()
      actor_set_blue_berry_counter(counter + math.random(1, 2))
   end
   
   if obj.qty == 1 then
      Obj.removeFromEngine(obj)
   else
      obj.qty = obj.qty - 1
   end
end

function use_misc_text(obj)

	local obj_n = obj.obj_n
	if obj_n == 65 or obj_n == 66 or obj_n == 67 or obj_n == 263 or obj_n == 267 or obj_n == 327 then
		printl("YOU_CANT_DIG_ANY_DEEPER")
	elseif obj_n == 427 then
		printl("IT_IS_STUCK")
	elseif obj_n == 288 then
		printl("THE_CONTROL_PANEL_OPERATES_THE_DREAM_MACHINE")
	elseif obj_n == 199 then
		printl("YOU_NEED_TO_USE_PLIERS_TO_ATTACH_THE_CABLE")
	elseif obj_n == 131 then
		printl("YOU_WILL_CHEW_IT_INSTINCTIVELY")
	elseif obj_n == 184 then
		printl("THE_PLATE_IN_THE_CAMERA_HAS_ALREADY_BEEN_EXPOSED")
	elseif obj_n == 442 or obj_n == 443 or obj_n == 444 then
		printl("YOU_MUST_USE_A_SHOVEL_TO_MOVE_THAT")
	elseif obj_n == 293 then
		printl("YOU_CANT_DETERMINE_HOW_TO_READ_THE_TIME")
	elseif obj_n == 77 or obj_n == 78 then
		printl("THE_BERRIES_ARE_NOT_EDIBLE")
	elseif obj_n == 323 then
		printl("YOU_DONT_KNOW_HOW_IT_WORKS")
	else
		printl("IT_HAS_NO_EFFECT")
	end
end

function get_pile_obj_num(map_tile)
	if (map_tile >= 32 and map_tile <= 36) or map_tile == 12 or map_tile == 13 or (map_tile >= 40 and map_tile <= 63) then
		return 258 --OBJ_DIRT_PILE
	elseif map_tile == 124 or map_tile == 125 or map_tile == 127 then
		return 256 --OBJ_CHUNK_OF_ICE
	end

	return 0 --CANNOT DIG HERE 
end

function get_free_location_around_actor(actor)
	local x_tbl = {-1,0,1,1,1,0,-1,-1}
	local y_tbl = {-1,-1,-1,0,1,1,1,0}
	local pos = {}
	local i

	pos.z = actor.z
	
	for i=1,8 do
		pos.x = actor.x + x_tbl[i]
		pos.y = actor.y + y_tbl[i]
		if map_can_put(pos.x,pos.y,pos.z) == true and map_get_obj(pos.x,pos.y,pos.z) == nil then
			return pos
		end
	end
	
	return nil
end

function use_tool_on_ground(obj, target_obj, actor, target_x, target_y, target_z)
	if target_obj ~= nil then
		--check tile flag here.
		printl("THE_GROUND_IS_NOT_CLEAR_FOR_DIGGING")
		return
	end
		
	local map_tile = map_get_tile_num(target_x, target_y, target_z)
	
	local pile_obj_num = get_pile_obj_num(map_tile)
	
	if pile_obj_num == 0 then
		printl("IT_HAS_NO_EFFECT")
		return
	end

	local hole_obj_num = 257
	
	if pile_obj_num == 256 then
		hole_obj_num = 255
	end
	
	local hole = Obj.new(hole_obj_num)
	hole.temporary = true
	Obj.moveToMap(hole, target_x, target_y, target_z)

	local loc = get_free_location_around_actor(actor)
	
	if loc ~= nil then
		local pile = Obj.new(pile_obj_num)
		pile.temporary = true
		Obj.moveToMap(pile, loc.x, loc.y, loc.z)
	else
		Obj.removeFromEngine(hole)
		printl("YOU_CANT_DIG_HERE")
	end
end

function use_shovel_on_pile_to_hole(obj, target_obj, to_obj, actor)
	Obj.removeFromEngine(target_obj)
	Obj.removeFromEngine(to_obj)
	--FIXME play sfx here. 1B (twice)
	printl("YOU_FILLED_IN_THE_HOLE")
end

function use_ruby_slippers(obj, actor)
   if obj.readied == false then
      --FIXME check that we can ready this object.
      Obj.removeFromEngine(obj)
      Actor.inv_ready_obj(actor, obj)
      return
   end
   
   if obj.quality == 2 then
      printl("YOU_MAY_USE_THE_RUBY_SLIPPERS_TO_GO_HOME")
      local input = input_select("yn", false)
      if input == "Y" then
         play_end_sequence()
      else
         play_midgame_sequence(13)
      end
   else
      printl("CLICK")
      obj.quality = obj.quality + 1
   end
end

function foes_are_nearby()
   local loc = player_get_location()
   local i
   for i=1,0xff do
      local actor = Actor.get(i)
      if actor.alive and (actor.align == ALIGNMENT_EVIL or actor.align == ALIGNMENT_CHAOTIC) and actor.z == loc.z then
         if get_wrapped_dist(actor.x, loc.x) < 40 and get_wrapped_dist(actor.y, loc.y) < 40 then
            return true
         end
      end
   end
   
   return false
end

function is_target_visible_to_actor(actor, target)
   --FIXME
   return true
end

function npcs_are_nearby()
   local loc = player_get_location()
   local i
   local player = Actor.get_player_actor()
   for i=1,0xff do
      local actor = Actor.get(i)
      if actor.alive and actor.asleep == false and actor.in_party == false and actor.z == loc.z then
         if get_wrapped_dist(actor.x, loc.x) < 6 and get_wrapped_dist(actor.y, loc.y) < 6 and is_target_visible_to_actor(player, actor) then
            return actor
         end
      end
   end

   return nil
end

function rest_heal_party(hours_to_rest)
   local actor
   for actor in party_members() do
      local hp = math.random(3,8) + math.random(3,8) * math.floor(hours_to_rest / 2)
      local max_hp = actor_get_max_hp(actor)
      if actor.hp + hp > max_hp then
         actor.hp = max_hp
      else
         actor.hp = actor.hp + hp
      end
   end
end

function rest_level_up_actor(actor)
   if actor.actor_num > 15 then
      return
   end
   
   local exp_level_tbl = {
   [0] = 0,
   [1] = 100,
   [2] = 200,
   [3] = 400,
   [4] = 800,
   [5] = 1600,
   [6] = 3200,
   [7] = 6400,
   [8] = 32767,
}

   if actor.exp <= exp_level_tbl[actor.level] then
      return
   end
   
   actor.level = actor.level + 1
   
   local max_hp = actor_get_max_hp(actor)
   if actor.hp + 30 > max_hp then
      actor.hp = max_hp
   else
      actor.hp = actor.hp + 30
   end
   
   Actor.show_portrait(actor)
   
   local obj_n = actor.obj_n
   local gender = math.random(0,1)
   if obj_n == 342 or obj_n == 343 or obj_n == 345 or (obj_n >= 347 and obj_n <= 353) then
      gender = 0
   elseif obj_n == 344 or obj_n == 346 or (obj_n >= 354 and obj_n <= 357) then
      gender = 1
   end
   
   local gender_pronoun = "He"
   if gender == 1 then
      gender_pronoun = "She"
   end
   
   printfl("HAS_A_DREAM", actor.name)
   printfl("SEES_THREE_STONE_OBELISKS", gender_pronoun)
   printfl("FEELS_DRAWN_TO_ONE_OF_THE_OBELISKS", gender_pronoun)
   printfl("DOES_TOUCH_THE_OBELISK", actor.name)
   printl("WHICH_BHS")

   local answer = input_select("bhs", false)
   
   if answer == "B" then
      if actor.int < 30 then
         actor.int = actor.int + 1
      end
   elseif answer == "H" then
      if actor.dex < 30 then
         actor.dex = actor.dex + 1
      end   
   elseif answer == "S" then
      if actor.str < 30 then
         actor.str = actor.str + 1
      end  
   end
   
end

function use_tent(obj, actor)
   if player_is_in_solo_mode() then
      printl("NOT_WHILE_IN_SOLO_MODE")
      --FIXME play_sfx 5
      return
   end

   if g_in_dream_mode then
     printl("YOU_CANT_SLEEP_IN_A_DREAM")
     --FIXME play_sfx 5
     return
  end
  
   local tent_loc = {}
  
   if obj.on_map then
      tent_loc.x = obj.x
      tent_loc.y = obj.y
      tent_loc.z = obj.z
   else
      tent_loc = player_get_location()
   end
   
   local x, y
   
   for y = tent_loc.y - 2, tent_loc.y do
      for x = tent_loc.x - 1, tent_loc.x + 1 do
         local map_obj = map_get_obj(x,y,tent_loc.z)
         if map_obj ~= nil and map_obj.obj_n ~= 106 then
            if tile_get_flag(map_obj.tile_num, 3, 4) == false then
               printfl("TENT_OBJ_IN_THE_WAY", map_obj.name)
               --FIXME play_sfx 5
               return
            end
         end
      end
   end

   for y = tent_loc.y - 2, tent_loc.y do
      for x = tent_loc.x - 1, tent_loc.x + 1 do
         if tile_get_flag(map_get_tile_num(x,y,tent_loc.z), 1, 1) == true then --if map tile is impassible
            printl("THE_GROUND_IS_NOT_FLAT_ENOUGH")
            --FIXME play_sfx 5
            return
         end
      end
   end
   
   printl("REST")
   
   if party_is_in_combat_mode() then
      print(" - ")
      printl("NOT_WHILE_IN_COMBAT_MODE")
      --FIXME play_sfx 5
      return
   end
   
   if foes_are_nearby() then
      printl("NOT_WHILE_FOES_ARE_NEAR")
      --FIXME play_sfx 5
      return
   end

   local npc = npcs_are_nearby()
   if npc ~= nil then
      printfl("IS_TOO_NEAR_TO_SETUP_CAMP", npc.name)
      --FIXME play_sfx 5
      return
   end   
   
   --poison check

   local actor
   local poisoned = false
   for actor in party_members() do
      if actor.poisoned then
         poisoned = true
         printfl("IS_POISONED", actor.name)
      end
   end
   
   if poisoned then
      printl("DO_YOU_REALLY_WANT_TO_SLEEP")
      local answer = input_select("yn", false)
      if answer == "N" or answer == "n" then
         return
      end
   end

   local party_is_using_berries = false
   for actor in party_members() do
      local actor_num = actor.actor_num
      local green = actor_is_affected_by_green_berries(actor_num)
      local brown = actor_is_affected_by_brown_berries(actor_num)
      
      if brown or green then
         party_is_using_berries = true
         if brown and green then
            printfl("COMPLAINS_OF_TOO_MUCH_LIGHT_AND_INANIMATE", actor.name)
         elseif brown then
            printfl("COMPLAINS_OF_TOO_MUCH_LIGHT", actor.name)
         else --green
            printfl("COMPLAINS_OF_INANIMATE_THINGS_TALKING", actor.name)            
         end  
      end
      
   end
   
   if party_is_using_berries then
      if party_get_size() == 1 then
         printl("YOU_CANT_SLEEP")
      else
         printl("NOBODY_CAN_SLEEP")
      end
      
      return
   end
   
   local player = Actor.get_player_actor()
   player.x = tent_loc.x
   player.y = tent_loc.y
   
   local tent = Obj.new(134, 3)
   Obj.moveToMap(tent, player.x, player.y-1, player.z)
   
   tent = Obj.new(134, 5)
   Obj.moveToMap(tent, player.x+1, player.y-1, player.z)

   tent = Obj.new(134, 6)
   Obj.moveToMap(tent, player.x-1, player.y, player.z)

   tent = Obj.new(134, 9)
   Obj.moveToMap(tent, player.x+1, player.y, player.z)

   tent = Obj.new(134, 8)
   Obj.moveToMap(tent, player.x, player.y, player.z)
            
   party_move(player.x, player.y, player.z)
   
   script_wait(500)
   
   party_hide_all()
   
   tent.frame_n = 7
   
   local hour = clock_get_hour()
   local time
   if hour < 7 or hour > 16 then
      time = i18n("SUNRISE")
      if hour < 7 then
         hours_to_rest = 7 - hour
      else
         hours_to_rest = 24 - hour + 7
      end
   elseif hour <= 16 then
      time = i18n("SUNSET")
      hours_to_rest = 18 - hour
   end
   
   printfl("REST_UNTIL", time)
   local answer = input_select("yn", false)
   
   if answer == "N" or answer == "n" then
      printl("HOW_MANY_HOURS")
      hours_to_rest = input_select_integer("0123456789", true)
   end
   
   g_party_is_warm = true
   
   if g_hours_till_next_healing == 0 and hours_to_rest > 4 then
      rest_heal_party(hours_to_rest)
      g_hours_till_next_healing = 6
   end
   
   local can_level_up = false
   if hours_to_rest * 3 > party_get_size() then
      can_level_up = true
   end
   
   local i
   for i=0,hours_to_rest*3-1 do
      advance_time(20)
      script_wait(100)
      if i < party_get_size() then
         local actor = party_get_member(i)
         rest_level_up_actor(actor)
      end
   end

   local actor
   local poisoned = false
   for actor in party_members() do
      if actor.poisoned then
         if math.random(1, 8) + math.random(1, 8) >= 15 then
            actor.poisoned = false
            printfl("FEELS_BETTER", actor.name)
         else
            if actor.hp < hours_to_rest * 2 + 5 then
               actor.hp = 5
            else
               actor.hp = actor.hp - (hours_to_rest * 2 + 5)
            end
         end
      end
   end


   tent.frame_n = 8 --Open the tent flap
   party_show_all()
         
   party_move(player.x, player.y + 1, player.z)

   script_wait(500)
   
   --remove tent from map
   for tent in find_obj(player.z, 134) do
      if tent ~= nil and 
      ((tent.x == tent_loc.x and tent.y == tent_loc.y-1) or
       (tent.x == tent_loc.x+1 and tent.y == tent_loc.y-1) or
       (tent.x == tent_loc.x-1 and tent.y == tent_loc.y) or
       (tent.x == tent_loc.x+1 and tent.y == tent_loc.y) or
       (tent.x == tent_loc.x and tent.y == tent_loc.y))
      then
         Obj.removeFromEngine(tent)
      end
   end

   g_party_is_warm = false
end

function use_red_berry(obj, actor)
   if not party_is_in_combat_mode() then
      printl("THAT_WOULD_BE_A_WASTE_OUTSIDE_OF_COMBAT")
      return
   end
   
   if actor.frenzy == false then
      printfl("ENTERS_A_BATTLE_FRENZY", actor.name)
      --FIXME play sfx 0x32
   end
   
   actor.frenzy = true
   
   local qty = obj.qty
   
   if qty > 1 then
      obj.qty = qty - 1
   else
      Obj.removeFromEngine(obj)
   end
end

function use_spittoon(obj, actor)
   if not actor.hypoxia then
      printl("YOU_SPIT_INTO_THE_SPITTOON")
   else
      printl("YOUR_MOUTH_IS_TOO_DRY")
   end
end

local usecode_table = {
--OBJ_RUBY_SLIPPERS
[12]=use_ruby_slippers,
--OBJ_PICK
[65]={[255]=use_misc_text,[257]=use_misc_text}, --hole in ice, hole
--OBJ_SHOVEL
[66]={[255]=use_misc_text,[257]=use_misc_text,[258]={[257]=use_shovel_on_pile_to_hole},[0]=use_tool_on_ground}, --hole in ice, hole
--OBJ_HOE 
[67]={[255]=use_misc_text,[257]=use_misc_text}, --hole in ice, hole
--OBJ_BERRY
[73]=use_berry,
--OBJ_BERRY1
[74]=use_berry,
--OBJ_BERRY2
[75]=use_berry,
--OBJ_BERRY3
[76]=use_berry,
--OBJ_BERRY4
[77]=use_red_berry,
--OBJ_CLUMP_OF_ROUGE_BERRIES
[78]=use_misc_text, 
[86]=use_container,
[87]=use_container,
[96]=use_sextant,
[102]={[86]=use_crate,[427]=use_prybar_on_hatch},
[104]=use_container,
--OBJ_FOLDED_TENT
[106]=use_tent,
--OBJ_BLOB_OF_OXIUM
[131]=use_misc_text,
--OBJ_DOOR 
[152]=use_door,
--OBJ_CAMERA
[184]=use_misc_text,
--OBJ_CABLE_SPOOL
[199]=use_misc_text,
[222]=use_door,
--OBJ_MARTIAN_HOE
[263]={[255]=use_misc_text,[257]=use_misc_text}, --hole in ice, hole
--OBJ_MARTIAN_SHOVEL
[267]={
--on
   [255]=use_misc_text,
   [257]=use_misc_text,
   [258]={ --OBJ_DIRT_PILE
      --to
      [257]=use_shovel_on_pile_to_hole},
   [0]=use_tool_on_ground}, --hole in ice, hole
[273]={[86]=use_crate}, --Hammer needs more codes
[284]=use_container,
--OBJ_SPITTOON
[286]=use_spittoon,
--OBJ_DREAM_MACHINE1
[288]=use_misc_text,
--OBJ_MARTIAN_CLOCK
[293]=use_misc_text,
--OBJ_OXYGENATED_AIR_MACHINE
[323]=use_misc_text,
--OBJ_MARTIAN_PICK
[327]={[255]=use_misc_text,[257]=use_misc_text}, --hole in ice, hole
--OBJ_CLOSED_HATCH
[421]=use_door,
[427]=use_misc_text,
--OBJ_PILE_OF_ROCKS
[442]=use_misc_text,
--OBJ_PILE_OF_IRON_ORE
[443]=use_misc_text,
--OBJ_PILE_OF_COAL
[444]=use_misc_text,
}

function ready_winged_shoes(obj, actor)
   local player_loc = player_get_location()
   if player_loc.z == 2 then
      local bridge = Obj.new(146)
      bridge.temporary = false
      Obj.moveToMap(bridge, 0xc9, 0x9b, 2)
   end
   
   return true
end

function ready_tongs(obj, actor)
   printl("THE_TONGS_WILL_NOW_PROTECT_YOUR_HANDS")
   return true
end

function ready_throw_rug(obj, actor)
   obj.obj_n = 162 --Change to red cape
   return true
end

local usecode_ready_obj_table = {
[15]=ready_winged_shoes,
[136]=ready_tongs,
[161]=ready_throw_rug,
}

function unready_winged_shoes(obj, actor)
   printl("THEY_WONT_COME_OFF")
   return false
end

local usecode_unready_obj_table = {
[15]=unready_winged_shoes,
}

function has_usecode(obj, usecode_type)
   if usecode_type == USE_EVENT_USE and usecode_table[obj.obj_n] ~= nil then
      return true
   elseif usecode_type == USE_EVENT_READY and (usecode_ready_obj_table[obj.obj_n] ~= nil or usecode_unready_obj_table[obj.obj_n] ~= nil)then
      return true
   end
   
   return false
end

function use_obj_on_to(obj, target_obj, actor, use_to_tbl)
	local dir = get_direction(i18n("TO"))
	if dir == nil then
		printl("NOTHING")
		return
	end
	
	local to_x, to_y = direction_get_loc(dir, actor.x, actor.y)
	
	local to_obj = map_get_obj(to_x, to_y, actor.z)
	
	if to_obj ~= nil then
		print(to_obj.name.."\n")
		local func = use_to_tbl[to_obj.obj_n]
		if func ~= nil then
			func(obj, target_obj, to_obj, actor)
		end
	else
		printl("SOMETHING")
	end
end

function use_obj_on(obj, actor, use_on_tbl)
	local dir = get_direction(i18n("ON"))
	if dir == nil then
      printl("NOTHING")
		return
	end

	local target_x, target_y = direction_get_loc(dir, actor.x, actor.y)
	
	local target_obj = map_get_obj(target_x, target_y, actor.z)
	
	if target_obj ~= nil then
		print(target_obj.name.."\n")
		local on = use_on_tbl[target_obj.obj_n]
		if on ~= nil then
			if type(on) == "function" then
				local func = on
				func(obj, target_obj, actor)
			else
				use_obj_on_to(obj, target_obj, actor, on)
			end
		else
			local func = use_on_tbl[0]
			if func ~= nil then
				func(obj, target_obj, actor)
			else
				printl("NO_EFFECT")
			end
		end
	else
		local func = use_on_tbl[0]
		if func ~= nil then
			func(obj, nil, actor, target_x, target_y, actor.z)
		else
			printl("NOTHING")
		end
	end
end

function use_obj(obj, actor)
	if type(usecode_table[obj.obj_n]) == "function" then
		local func = usecode_table[obj.obj_n]
		if func ~= nil then
			print("\n")
			func(obj, actor)
		end
	else
		use_obj_on(obj, actor, usecode_table[obj.obj_n])
	end
end

function ready_obj(obj, actor)
   if type(usecode_ready_obj_table[obj.obj_n]) == "function" and obj.readied == false then
      local func = usecode_ready_obj_table[obj.obj_n]
      if func ~= nil then
         print("\n")
         return func(obj, actor)
      end
   end

   if type(usecode_unready_obj_table[obj.obj_n]) == "function" and obj.readied == true then
      local func = usecode_unready_obj_table[obj.obj_n]
      if func ~= nil then
         print("\n")
         return func(obj, actor)
      end
   end
   
   return true
end
