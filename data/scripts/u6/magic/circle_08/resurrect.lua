obj = select_obj()

if obj ~= nil and obj.obj_n == 339 then --dead body
  actor = Actor.get(obj.quality)
  Actor.resurrect(actor, obj.x, obj.y, obj.z)
  print("\nSuccess\n");
else
  print("\nNo effect\n")
end

obj = nil
actor = nil
