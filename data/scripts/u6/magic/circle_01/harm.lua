actor = select_actor()

if actor ~= nil then
	damage = math.random(0,10)
	Actor.hit(actor, damage);
	if damage > 0 then
		return print("\nSuccess\n");
	end
end

return print("\nno effect\n");
