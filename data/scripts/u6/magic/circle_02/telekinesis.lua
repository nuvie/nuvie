item = select_actor_or_obj_with_projectile(0x17f)

if item == nil then return end

if item.luatype == "obj" then
	if item.obj_n == 0x120 or item.obj_n == 0x10c then --crank, lever
		fade_obj_blue(item)
		Obj.use(item)
	elseif item.weight >= 0.01 then
		print("\nFIXME move object here.\n")
	else
		print("\nNot Possible\n")
	end
else
	magic_no_effect() --do nothing with actors.
end
