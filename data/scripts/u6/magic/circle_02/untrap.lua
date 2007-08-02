obj = select_obj();

found = false

if obj ~= nil then
  container = obj.container
  for i=1,#container do  -- look through container for effect object. 
    child = container[i]
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
