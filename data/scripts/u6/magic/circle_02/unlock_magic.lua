obj = select_obj()
if obj ~= nil then
  if obj.obj_n >= 297 and obj.obj_n <= 300 then --door
    if obj.frame_n >= 13 then
      obj.frame_n = obj.frame_n - 8
      obj = nil
      return print("\nunlocked!\n");
    end
  end
end

obj = nil
print("\nNo effect\n")
