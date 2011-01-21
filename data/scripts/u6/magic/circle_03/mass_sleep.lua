  local loc = select_location()
  if loc ~= nil then
     local caster = magic_get_caster()
  
     projectile_anim(0x18b, caster.x, caster.y, loc.x, loc.y, 2, 0, 0)
     local hit_items = explosion_start(0x17d,loc.x,loc.y)
     
     local k,v
     
     for k,v in pairs(hit_items) do
        if v.luatype == "actor" then
           spell_put_actor_to_sleep(caster, v)
        end
     end

     --FIXME sound effect
     print("\nSuccess\n")
  end
