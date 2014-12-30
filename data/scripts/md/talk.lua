function open_gates_at_olympus_mons()
   local gate = map_get_obj(0x2c3, 0x1f3, 0, 181) -- OBJ_GATE
   if gate ~= nil then
      gate.x = 0x2c2
      gate.frame_n = 3
   else
      printl("AARGH")
   end
   
   gate = map_get_obj(0x2c4, 0x1f3, 0, 181) -- OBJ_GATE
   if gate ~= nil then
      gate.frame_n = 7
   else
      printl("AARGH")
   end
   
end


local talk_script_tbl = {
   [6]=open_gates_at_olympus_mons,
}

function talk_script(script_number)
   if talk_script_tbl[script_number] ~= nil then
      talk_script_tbl[script_number]()
   end
end
