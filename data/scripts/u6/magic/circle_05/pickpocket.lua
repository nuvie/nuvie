local caster = magic_get_caster()
local actor = select_actor_with_projectile(0x18b, caster)

if actor == nil then return end

print("Which object:")

local obj = get_obj_from_inventory(actor)

if obj ~= nil then
	print(obj.name.."\n")
	projectile(obj.tile_num, actor.x, actor.y, caster.x, caster.y, 2)
	Actor.inv_add_obj(caster, obj)
	magic_success()
end
