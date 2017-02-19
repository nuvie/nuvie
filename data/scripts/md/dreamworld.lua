function finish_dream_quest(actor)
end

function wake_from_dream()
   --FIXME dreamworld_cleanup_state() The original calls this with actor zero as an argument.
   g_objlist_1d22_unk = 0
   local minutes = 60 - clock_get_minute()
   local current_hour = clock_get_hour()
   current_hour = current_hour + 1
   if current_hour >= 8 then
      minutes = minutes + (24 - current_hour + 8) * 60
   else
      minutes = minutes + (8 - current_hour) * 60
   end
   clock_inc(minutes)
   --FIXME reset walk_direction_modifier

   for actor in party_members() do
      actor_clear_berry_counters(actor.actor_num)
      if actor.poisoned then
         if actor.hp <= 30 then
            if actor.hp <= 10 then
               actor.hp = 1
            else
               actor.hp = math.random(1, 10)
            end
         else
            actor.hp = math.random(24, 30)
         end
      end
   end
   local dream_actor = Actor.get(0)
   party_set_in_vehicle(false)
   dream_actor.visible = false
   party_show_all()
   party_update_leader()
   g_in_dream_mode = false
   printl("YOU_WAKE_UP")

end


function actor_use_dream_machine(actor, dream_quality)
   if actor.actor_num ~= 1 then
      --FIXME advance time till dawn.
      printfl("ACTOR_DREAMS_UNTIL_DAWN_WHILE_THE_PARTY_WAITS", actor.name)
      actor.asleep = true
      g_party_is_warm = true
      advance_time(60 - clock_get_minute())
      while clock_get_hour() ~= 8 do
         advance_time(20)
         script_wait(100)
      end
      g_party_is_warm = false
   else
      play_midgame_sequence(4)
      party_set_party_mode()
      local dream_actor = Actor.get(0)

      local dream_x, dream_y
      if dream_quality == 1 then
         dream_x, dream_y = 0x64, 0x3b
      else
         dream_x, dream_y = 0x93, 0x34
      end

      --FIXME need to copy over more data from avatar actor.
      dream_actor.obj_n = actor.obj_n
      dream_actor.frame_n = actor.frame_n
      dream_actor.base_obj_n = actor.base_obj_n
      dream_actor.wt = WT_PLAYER
      dream_actor.visible = true
      dream_actor.hp = actor.max_hp

      Actor.move(dream_actor, dream_x, dream_y, 2)
      player_set_actor(dream_actor)
      party_set_in_vehicle(true)
      party_hide_all()
      g_in_dream_mode = true


   end
end

local dreamworld_cleanup_tbl = {
   [5]=function() end,
   [0x20]=function() end,
   [0x40]=function() end,
   [0x44]=function() end,
   [0xa0]=function() end,
}

local dreamworld_unk_tbl = {
   [0x4]=function() end,
   [0x40]=function() end,
   [0x85]=function() end,
   [0xA1]=function() end,
   [0xA4]=function() end,
   [0xC0]=function() end,
   [0xC1]=function() end,
   [0xC4]=function() end,
   [0xC5]=function() end,
   [0xE0]=function() end,
}

local dreamworld_init_tbl = {
   [0x5]=function() end,
   [0x20]=function() end,
   [0x25]=function() end,
   [0x44]=function() end,
   [0x60]=function() end,
   [0xA5]=function() end,
   [0xC0]=function() end,
   [0xC4]=function() end,
   [0xC5]=function() end,
}

function dreamworld_cleanup_state(obj)
   local dream_actor = Actor.get(0)
   local new_stage =  bit32.band(obj.status, 0xe5)
   if g_current_dream_stage ~= 0 and new_stage ~= 0xa5 and new_stage ~= 0xe5 then
      if dreamworld_cleanup_tbl[g_current_dream_stage] ~= nil then
         dreamworld_cleanup_tbl[g_current_dream_stage]()
      end

   end

   if new_stage == 1 then
      g_current_dream_stage = 0
      return
   elseif new_stage == 0x24 then
      wake_from_dream()
      return
   elseif g_current_dream_stage ~= 0 and dreamworld_unk_tbl[new_stage] ~= nil then
      dreamworld_unk_tbl[new_stage]()
   end

   if new_stage ~= 0xa5 then
      --FIXME clean up temp objects
      g_objlist_1d22_unk = 0
   end

   actor_clear_berry_counters(dream_actor.actor_num)
   local player_loc = player_get_location()
   player_move(obj.quality, obj.qty, player_loc.z)

   if new_stage == 0xe5 then
      if not Actor.get_talk_flag(0x66, 3) then
         g_objlist_1d22_unk = 6
      end
   else
      if dreamworld_init_tbl[new_stage] ~= nil then
         dreamworld_init_tbl[new_stage]()
      end

      g_current_dream_stage = new_stage
   end

end