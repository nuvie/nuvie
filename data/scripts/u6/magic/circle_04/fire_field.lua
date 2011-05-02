  loc = select_location()

  obj = Obj.new(317); --fire field
  obj.x,obj.y,obj.z = loc.x,loc.y,loc.z

  Obj.moveToMap(obj)
  obj = nil
