  local actor = select_actor()
  local caster = caster_get_location()
  
  if actor ~= nil then
     projectile_anim(0x17f, caster.x, caster.y, actor.x, actor.y, 1, 0, 0)

     hit_anim(actor.x, actor.y)
     actor.paralyzed = true
     print(actor.name.." is paralyzed.\n")
  end
