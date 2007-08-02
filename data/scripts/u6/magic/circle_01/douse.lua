obj = select_obj()

if obj ~= nil then
  if obj.obj_n == 122 or --candle
    obj.obj_n == 145 or --candelabra
    obj.obj_n == 164 or --fireplace
    obj.obj_n == 90 or --torch
    (obj.obj_n == 206 and obj.frame_n == 1) then --flaming brazier
    if obj.frame_n == 1 or obj.frame_n == 3 then
      if obj.obj_n == 90 then
        Obj.use(obj) -- we want to use usecode to remove the torch
      else
        obj.frame_n = obj.frame_n - 1;
      end
      return print("\nSuccess\n");
    end
  end

return print("\nNo effect\n");
end
