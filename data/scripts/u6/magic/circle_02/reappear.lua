local caster = magic_get_caster()

local loc = select_location_with_prompt("Location: ")

magic_casting_fade_effect(caster)

if loc == nil or g_vanish_obj.obj_n == 0 then magic_no_effect() return end

local hit_x, hit_y =  map_line_hit_check(caster.x, caster.y, loc.x, loc.y, loc.z)
projectile(0x17f, caster.x, caster.y, hit_x, hit_y, 2)

if hit_x ~= loc.x or hit_y ~= loc.y then magic_blocked() return end

local obj = Obj.new(g_vanish_obj.obj_n, g_vanish_obj.frame_n)

if Obj.moveToMap(obj, loc) ~= nil then
	magic_no_effect()
else
	g_vanish_obj.obj_n = 0
	g_vanish_obj.frame_n = 0
	magic_success()
end