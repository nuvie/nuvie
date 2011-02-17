  local loc = select_location()
  local caster = magic_get_caster()
  
  projectile(0x189, caster.x, caster.y, loc.x, loc.y, 2)

  local hit_items = explosion(0x17e, loc.x,loc.y)
  
  for k,v in pairs(hit_items) do
     if v.luatype == "actor" then
        spell_take_fire_dmg(caster, v)
     end
  end

  