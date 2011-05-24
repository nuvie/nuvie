local obj = select_obj()

magic_casting_fade_effect()

if obj == nil then magic_no_effect() return end

local obj_n = obj.obj_n

if obj_n == 0x153 or obj_n == 0x162 or obj_n == 0x155 or obj_n == 0x155 or obj_n == 0x154 then
	actor_talk(Actor.get(obj.quality))
end
