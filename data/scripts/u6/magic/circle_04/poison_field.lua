loc = select_location_with_projectile(0x17c)

if loc == nil then return end
    
obj = Obj.new(318); --poison field
obj.x,obj.y,obj.z = loc.x,loc.y,loc.z
  
if Obj.moveToMap(obj) == nil then
	fade_obj_in(obj)
	magic_success()
else
	magic_not_possible()
end