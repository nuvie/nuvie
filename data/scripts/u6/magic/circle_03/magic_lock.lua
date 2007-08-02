obj = select_obj()

if obj ~= nil and obj.obj_n >= 297 and obj.obj_n <= 300 then
   if obj.frame_n <= 7 then
      if obj.frame_n <= 3 then
         print("\nCan't (Un)lock an opened door\n")
      else
          --FIXME need to handle a door which is already magically locked.
         if obj.frame_n >= 5 and obj.frame_n <= 7 then
            obj.frame_n = obj.frame_n + 8
            print("\nmagically locked!\n")
         end
      end
      print("\nSuccess\n")
   end
else
  print("\nNo effect\n")
end

