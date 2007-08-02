-- Dispel Field,  An Grav

-- 317 Fire Field
-- 318 Poison Field
-- 319 Protection Field
-- 320 Sleep Field

obj = select_obj()

if obj ~= nil then
  if obj.obj_n >= 317 and obj.obj_n <= 320 then
    map_remove_obj(obj)
    obj = nil
    return print("\nSuccess\n")
  end
end

obj = nil
print("\nNo effect\n")
 
