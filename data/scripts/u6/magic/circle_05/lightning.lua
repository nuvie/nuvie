  local loc = select_location()
  local caster = caster_get_location()
  
  projectile(0x188, caster.x, caster.y, loc.x, loc.y, 1)
--FIXME actor_hit
  Actor.hit(map_get_actor(loc), math.random(1, 30))