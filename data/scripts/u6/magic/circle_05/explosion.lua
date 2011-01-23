  local loc = select_location()
  local caster = magic_get_caster()
  
  projectile_anim(0x189, caster.x, caster.y, loc.x, loc.y, 2, 0, 0)
  local hit_items = explosion_start(0x17e, loc.x,loc.y)
  
  for k,v in pairs(hit_items) do
     if v.luatype == "actor" then
        spell_take_fire_dmg(caster, v)
     end
  end

  