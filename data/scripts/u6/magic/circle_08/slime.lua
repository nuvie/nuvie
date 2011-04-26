local loc = player_get_location()
local caster = magic_get_caster()
local x = loc.x - 5
local y = loc.y - 5
local z = loc.z

for x = loc.x - 5,loc.x + 5 do
	for y = loc.y - 5,loc.y + 5 do
		local actor = map_get_actor(x, y, z)

		if actor ~= nil then
			local actor_type = actor_tbl[actor.obj_n]
			
			if actor.align == ALIGNMENT_EVIL and (actor_type == nil or actor_type[12] == 0) then
				if actor_int_check(caster, actor) == false then
					--FIXME need to fade to new slime frame here.
					actor.base_obj_n = 0x177 --convert actor into slime
					if actor.temp == true then
						--remove inventory
						local obj
						for obj in actor_inventory(actor) do
							Actor.inv_remove_obj(actor, obj)
						end  
					end
				end
			end
		end
	end
end
slime_update_frames()
