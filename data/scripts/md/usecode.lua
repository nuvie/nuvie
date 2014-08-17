local USE_EVENT_USE = 0x01

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


local usecode_table = {
--OBJ_RUBY_SLIPPERS
[12]=use_ruby_slippers,
--OBJ_PICK
[65]={[255]=use_misc_text,[257]=use_misc_text}, --hole in ice, hole
--OBJ_SHOVEL
[66]={[255]=use_misc_text,[257]=use_misc_text,[258]={[257]=use_shovel_on_pile_to_hole},[0]=use_tool_on_ground}, --hole in ice, hole
--OBJ_HOE 
[67]={[255]=use_misc_text,[257]=use_misc_text}, --hole in ice, hole
--OBJ_BERRY4
[77]=use_misc_text,
--OBJ_CLUMP_OF_ROUGE_BERRIES
[78]=use_misc_text, 
[86]=use_container,
[87]=use_container,
[96]=use_sextant,
[102]={[86]=use_crate,[427]=use_prybar_on_hatch},
[104]=use_container,
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


function has_usecode(obj, usecode_type)
	if usecode_type == USE_EVENT_USE and usecode_table[obj.obj_n] ~= nil then
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