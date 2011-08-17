function actor_map_dmg(actor, map_x, map_y, map_z)
	--FIXME
end

function actor_update_all()
	--FIXME
end

function advance_time(num_turns)
	--FIXME
	local minute = clock_get_minute()
	
	clock_inc(num_turns)
		
	if minute + num_turns >= 60 then
		update_actor_schedules()
	end
end
