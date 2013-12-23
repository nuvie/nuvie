local USE_EVENT_USE = 0x01


function get_target_obj(prompt)

if prompt ~= nil then
print(prompt)
end

local obj = coroutine.yield("obj")

return obj
end

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

function use_hatch(obj, actor)
	print("\nIt is stuck.\n")
end

function use_crate(obj, target_obj, actor)
	if target_obj.frame_n == 2 then
		target_obj.frame_n = 1
		print("You pried the nails out of the crate.\n")
	else
		if target_obj.frame_n == 1 and obj.obj_n == 273 then
			target_obj.frame_n = 2
			print("You nail the crate shut.\n")
		else
			print("It has no effect.\n")
		end
	end
	
end

function use_container(obj, actor)
	if obj.frame_n == 2 then
		if obj.obj_n == 86 or obj.obj_n == 284 then
			print("It is nailed shut.\n")
		else
			print("It is locked.\n")
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
		print(actor.name.." is not strong enough.\n")
		return
	end
	
	local tesla = Actor.get(16)
	if Actor.get_talk_flag(tesla, 4) == false then
		Actor.set_talk_flag(tesla, 2)
		Actor.talk(tesla)
	else
		--FIXME play cutscene here
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


local usecode_table = {
[86]={["func"]=use_container},
[87]={["func"]=use_container},
[102]={["on"]={[86]=use_crate,[427]=use_prybar_on_hatch}},
[104]={["func"]=use_container},
[152]={["func"]=use_door},
[222]={["func"]=use_door},
[273]={["on"]={[86]=use_crate}}, --Hammer needs more codes
[284]={["func"]=use_container},
[421]={["func"]=use_door},
[427]={["func"]=use_hatch},
}


function has_usecode(obj, usecode_type)
	if usecode_type == USE_EVENT_USE and usecode_table[obj.obj_n] ~= nil then
		return true
	end
	
	return false
end

function use_obj(obj, actor)
	if usecode_table[obj.obj_n].on ~= nil then

		local target_obj = get_target_obj("On - ")
		if target_obj ~= nil then
			print(target_obj.name.."\n")
			local func = usecode_table[obj.obj_n].on[target_obj.obj_n]
			if func ~= nil then
				func(obj, target_obj, actor)
			else
				print("\nNo effect\n")
			end
		else
			print("nothing!\n")
		end
	else
		local func = usecode_table[obj.obj_n].func
		if func ~= nil then
			func(obj, actor)
		end
	end
end
