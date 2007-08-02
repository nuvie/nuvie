  loc = get_target()

  obj = Obj.new(320); --sleep field
  obj.x,obj.y,obj.z = loc.x,loc.y,loc.z

  Obj.moveToMap(obj)
  obj = nil

