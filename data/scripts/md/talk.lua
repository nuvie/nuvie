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

function talk_to_actor(actor_num)
   local actor = Actor.get(actor_num)
   print(actor.name.."\n")
   Actor.talk(actor_num)
   print("\n")
end

function talk_conveyor()
   talk_to_actor(0x72)
end

function talk_tower()
   talk_to_actor(0x73)
end

function talk_dream_machine()
   talk_to_actor(0x74)
end

local talk_obj_tbl = {
   [0xC] = function() printl("YOU_RECEIVE_A_MOMENTARY_IMPRESSION") end,
   [0x5C] = function() printl("THIS_WAS_WORN_LAST_BY_A_DYING_MAN") end,
   [0x64] = talk_dream_machine,
   [0xBC] = talk_conveyor,
   [0xBF] = talk_conveyor,
   [0xC0] = talk_conveyor,
   [0xC8] = talk_tower,
   [0xC9] = talk_tower,
   [0xD6] = talk_tower,
   [0xD7] = talk_tower,
   [0xE7] = function() printl("THIS_WAS_USED_BY_A_MARTIAN_MEASURING_TIME") end,
   [0xEA] = function() printl("THIS_JEWELRY_WAS_WORN_BY") end,
   [0xF6] = function() printl("THESE_FOOTBAGS_WERE_WORN_BY") end,
   [0xF7] = function() printl("THIS_SAW_WAS_USED_BY") end,
   [0xF9] = function() printl("THIS_DEVICE_WAS_USED_BY") end,
   [0x113] = function() printl("THIS_SCULPTURE_HAD_GREAT_RITUAL_OR_RELIGIOUS_SIGNIFICANCE") end,
   [0x120] = talk_dream_machine,
   [0x121] = talk_dream_machine,
   [0x122] = talk_dream_machine,

}

function talk_to_obj(obj)

   local player = Actor.get_player_actor()
   if actor_is_affected_by_green_berries(player.actor_num) then
      if bit32.band(obj.status, 1) == 1 then
         local talk_function = talk_obj_tbl[obj.obj_n]
         if talk_function ~= nil then
            talk_function()
            return true
         end
      else
         printl("YOU_RECIEVE_NO_PSYCHIC_IMPRESSIONS")
      end
   else
      printl("NOTHING")
   end

   return false
end
