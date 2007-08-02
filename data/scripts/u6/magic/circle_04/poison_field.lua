  loc = get_target()

  obj = Obj.new(318); --poison field
  obj.x,obj.y,obj.z = loc.x,loc.y,loc.z

  Obj.moveToMap(obj)
  obj = nil