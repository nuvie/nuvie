local caster = magic_get_caster()
local loc = select_location()

if loc == nil then return end

local hit_items = magic_wind(0x17c, caster, loc.x, loc.y)

local k, v
for k,v in pairs(hit_items) do
	if v.luatype == "actor" then
		spell_poison_actor(caster, v)
	end
end
