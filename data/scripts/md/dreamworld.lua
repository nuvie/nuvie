function finish_dream_quest(actor)
end

function wake_from_dream()
end


function actor_use_dream_machine(actor, dream_quality)
   if actor.actor_num ~= 1 then
      --FIXME advance time till dawn.
   else
      play_midgame_sequence(4)
      party_set_party_mode()
      local dream_actor = Actor.get(0)
--      party_hide_all()
      local dream_x, dream_y
      if dream_quality == 1 then
         dream_x, dream_y = 0x64, 0x3b
      else
         dream_x, dream_y = 0x93, 0x34
      end

      dream_actor.obj_n = actor.obj_n
      dream_actor.frame_n = actor.frame_n

      Actor.move(dream_actor, dream_x, dream_y, 2)
      player_set_actor(dream_actor)

   end

end