local obj = select_obj();

if obj ~= nil then
  local found = false
  local child
  for child in container_objs(obj) do  -- look through container for effect object. 
    if child.obj_n == 337 then --effect
      found = true
      break
    end
  end
  if found == true then
    print("\nIt's trapped.\n");
  else
    print("\nNo trap.\n");
  end
else
print("\nNo effect\n");
end

child = nil
obj = nil
