set_g_armageddon(true)
magic_casting_effect()
local i
for i=1,0xff do
   local actor = Actor.get(i)
   if i ~= 1 and i ~= 5 and actor ~= nil and actor.alive then --don't kill the Avatar or Lord British.
   	local x = actor.x
   	local y = actor.y
   	local z = actor.z
   	
   	for obj in actor_inventory(actor) do
   		Obj.moveToMap(obj, x, y, z)
   	end
   	
   	actor.alive = false
   	Actor.kill(actor, false) --false = don't create body
   end
end	