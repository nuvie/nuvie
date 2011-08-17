function load_game()
end

function save_game()
end

function look_obj(obj)
	print("you see " .. obj.look_string);

	--FIXME usecode look description should be lua code.
	if usecode_look(obj) then
		print("\n")
		return false
	end

	print(".\n");
	return true
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
