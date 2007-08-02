obj = select_obj()

if obj ~= nil then
  if obj.obj_n == 122 or --candle
    obj.obj_n == 145 or --candelabra
    obj.obj_n == 164 or --fireplace
    obj.obj_n == 90 or --torch
    (obj.obj_n == 206 and obj.frame_n == 0) then --brazier
    if obj.frame_n == 0 or obj.frame_n == 2 then
      if obj.obj_n == 90 then
        Obj.use(obj) -- we want to use usecode to handle multi-torch stacks.
      else
        obj.frame_n = obj.frame_n + 1;
      end
      return print("\nSuccess\n");
    end
  end
end
return print("\nNo effect\n");
