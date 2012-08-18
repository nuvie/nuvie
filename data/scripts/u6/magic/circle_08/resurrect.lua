local obj = select_obj()

magic_casting_fade_effect()

if obj ~= nil then --FIXME armageddon flag.
  fade_obj_blue(obj)
  --dead body, mouse, dead gargoyle, dead cyclops;
  if (obj.obj_n == 339 or obj.obj_n == 0x162 or obj.obj_n == 0x155 or obj.obj_n == 0x154) then
    if obj.quality < 0xe0 then --greater than 0xe0 temp actors
      if obj.quality ~= 0 then
        local actor = Actor.get(obj.quality)
        Actor.resurrect(actor, obj.x, obj.y, obj.z, obj)
        magic_success()
      else
        print("\nThe corpse is too old!\n")
      end
    end
  else
    magic_no_effect()
  end
else
  magic_no_effect()
end


