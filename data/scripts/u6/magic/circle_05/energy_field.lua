  loc = get_target()

  obj = obj_new(319); --protection field
  obj.x,obj.y,obj.z = loc.x,loc.y,loc.z

  obj_manager_add(obj)
  obj = nil

