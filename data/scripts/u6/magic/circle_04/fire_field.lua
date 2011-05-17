  loc = select_location_with_projectile(0x17e)

  if loc == nil then return end
  
  obj = Obj.new(317); --fire field
  obj.x,obj.y,obj.z = loc.x,loc.y,loc.z

  if map_can_put_obj(loc) then
     Obj.moveToMap(obj)
     fade_obj_in(obj)
     magic_success()
  else
     magic_not_possible()
  end