obj = select_obj();

if obj ~= nil then
  container = obj.container
  found = false
  for i=1,#container do  -- look through container for effect object. 
    child = container[i]
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
