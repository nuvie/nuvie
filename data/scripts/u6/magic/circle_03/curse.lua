  actor = select_actor()
  if actor ~= nil then
     local caster = caster_get_location()
  
     projectile_anim(0x17f, caster.x, caster.y, actor.x, actor.y, 2, 0, 0)
     actor.cursed = true
     --FIXME sound effect
     print("\nSuccess\n")
  end
