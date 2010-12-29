  loc = select_location()
  caster = caster_get_location()
  
  projectile_anim(0x189, caster.x, caster.y, loc.x, loc.y, 2, 0, 0)
  explosion_start(loc.x,loc.y,0,0)
  --FIXME need to deal out damage here!
  