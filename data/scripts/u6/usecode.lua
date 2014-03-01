local USE_EVENT_USE = 0x01

function use_telescope(obj, actor)
	mapwindow_center_at_location(obj.x, obj.y, obj.z)
	local dir = get_direction("Direction-")
	if dir == nil or dir == DIR_NONE then
		mapwindow_center_at_location(actor.x, actor.y, actor.z)
		print("nowhere.\n")
		return
	end
	local dir_string = direction_string(dir)
	dir_string = dir_string:gsub("^%l", string.upper)
	print(dir_string..".\n")
	
	local loc = mapwindow_get_location()
	
	--FIXME need to fade out blacking.
	
	mapwindow_set_enable_blacking(false)
		
	for i=0,40 do
		loc.x,loc.y = direction_get_loc(dir,loc.x, loc.y)
		mapwindow_set_location(loc.x, loc.y, loc.z)
		script_wait(50)
	end
	
	script_wait(600)
	
	mapwindow_set_enable_blacking(true)
	
	mapwindow_center_at_location(actor.x, actor.y, actor.z)
	print("\nDone\n")
	
end


local usecode_table = {
[154]=use_telescope
}

function has_usecode(obj, usecode_type)
	if usecode_type == USE_EVENT_USE and usecode_table[obj.obj_n] ~= nil then
		return true
	end

	return false
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
