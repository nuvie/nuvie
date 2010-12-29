  local loc = select_location()
  local caster = caster_get_location()
  
  projectile_anim(0x188, caster.x, caster.y, loc.x, loc.y, 1, 0, 0)
  --hit_anim(loc.x, loc.y)
  Actor.hit(map_get_actor(loc), math.random(1, 30))