magic_load({name="Explosion", invocation="VPF", reagents=0x0, circle=5, number=2,
  script= function ()
  size = math.random(2,7)
  dmg = math.random(10,40)
  loc = get_target()
  explosion_start(loc.x,loc.y,size,dmg)
  end
})
