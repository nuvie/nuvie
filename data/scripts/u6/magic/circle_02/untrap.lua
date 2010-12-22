local obj = select_obj();

local found = false

if obj ~= nil then
  local child
  for child in container_objs(obj) do  -- look through container for effect object. 
    if child.obj_n == 337 then --effect
      found = true
      Obj.removeFromCont(child)
      break
    end
  end
end

if found == true then
  print("\nSuccess.\n");
else
  print("\nNo effect\n");
end

child = nil
obj = nil
