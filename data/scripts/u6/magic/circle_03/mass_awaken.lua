  local loc = select_location()
  if loc ~= nil then
     local caster = caster_get_location()
  
     projectile_anim(0x18b, caster.x, caster.y, loc.x, loc.y, 2, 0, 0)
     local hit_items = explosion_start(0x17f,loc.x,loc.y)
  
     local k,v
  
     for k,v in pairs(hit_items) do
        if v.luatype == "actor" then
           v.asleep = false
        end
     end

     --FIXME sound effect
     print("\nSuccess\n")
  end

