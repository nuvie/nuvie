local caster = magic_get_caster()
local actor = select_actor_with_projectile(0x236, caster)

if actor == nil then return end

--FIXME add hit logic here.