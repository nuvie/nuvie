function worktype_99_coker_move_to_coal_vein(actor)
   if actor_move(actor, DIR_NORTH) == false then
      local vein = map_get_obj(actor.x, actor.y-1, actor.z, 446) --OBJ_VEIN_OF_COAL FIXME should be -2 not -1 need to fix actor_move for coker.
      if vein ~= nil then
         if map_is_on_screen(actor.x, actor.y, actor.z) then
            play_md_sfx(0x10)
         end
         actor.wt = 0x9A
      end
   end
end

function worktype_9A_coker_drop_coal(actor)
   local obj = map_get_obj(actor.x, actor.y+1, actor.z, 188) --OBJ_CONVEYOR_BELT
   if obj ~= nil then
      local coal = Obj.new(447) --OBJ_HUGE_LUMP_OF_COAL
      Obj.moveToMap(coal, actor.x, actor.y+1, actor.z)
   else
      obj = map_get_obj(actor.x, actor.y+1, actor.z, 192) --OBJ_BARE_ROLLERS
      if obj == nil then
         actor_move(actor, DIR_SOUTH)
         return
      end
   end
   actor.wt = 0x9B
end

function worktype_9B_coker_wait_for_coal_to_move_away(actor)
   local obj = map_get_obj(actor.x, actor.y+1, actor.z, 447) --OBJ_HUGE_LUMP_OF_COAL
   if obj == nil then
      obj = map_get_obj(actor.x, actor.y+1, actor.z, 192) --OBJ_BARE_ROLLERS
      if obj == nil then
         actor.wt = 0x99
      end
   end
end

function worktype_9D_stoker_wait_for_coal(actor)
   local coal
   coal = map_get_obj(actor.x, actor.y+1, actor.z, 447) --OBJ_HUGE_LUMP_OF_COAL
   
   if coal ~= nil then
      while coal ~= nil do
         Obj.removeFromEngine(coal)
         coal = map_get_obj(actor.x, actor.y+1, actor.z, 447) --OBJ_HUGE_LUMP_OF_COAL
      end
      actor.wt = 0x9E
   end
   
end

function worktype_9E_stoker_walk_to_furnace(actor)
   if actor_move(actor, DIR_NORTH) == false then
      local furnace = map_get_obj(actor.x, actor.y-1, actor.z, 233)
      if furnace == nil then
         furnace = map_get_obj(actor.x+1, actor.y-1, actor.z, 233)
      end
      
      if furnace ~= nil then
         if Actor.get_talk_flag(0x72, 2) == false then
            activate_power_system()
         else
            if Actor.get_talk_flag(0x73, 2) == false or Actor.get_talk_flag(0x71, 3) == true then
               if Actor.get_talk_flag(0x71, 3) == true then
                  Actor.clear_talk_flag(0x73, 2)
                  Actor.clear_talk_flag(0x71, 3)
                  --FIXME sub_3F740
               end
               Actor.set_talk_flag(0x73, 2)
               activate_power_update_tiles()
               activate_tower_electricity()
               midgame_cutscene_2()
            end
         end
         actor.wt = 0x9C
      else
         stoker_blocked(actor)
      end
   end
end

function activate_tower_electricity()
   for obj in find_obj(0, 201) do --OBJ_TOWER_TOP
      if obj ~= nil then
         if obj.x >= 0x3d0 and obj.x <= 0x3f0 and obj.y >= 0x1d0 and obj.y <= 0x1e7 then
            local frame_n = obj.frame_n
            obj.frame_n = bit32.bor(frame_n, 4)
         end
      end
   end

   for obj in find_obj(0, 214) do --OBJ_POWER_CABLE
      if obj ~= nil then
         if obj.x >= 0x3d0 and obj.x <= 0x3f0 and obj.y >= 0x1d0 and obj.y <= 0x1e7 then
            obj.obj_n = 215 --OBJ_POWER_CABLE1
         end
      end
   end

end

function stoker_blocked(stoker)
   if map_is_on_screen(stoker.x, stoker.y, stoker.z) then
      printl("STOKERS_PATH_IS_BLOCKED")
      play_md_sfx(0)
   end  
end

function worktype_9C_stoker_return_to_conveyor_belt(actor)
   if map_get_obj(actor.x, actor.y+2, actor.z, 191) == nil then --OBJ_CONVEYOR_BELT2
      if actor_move(actor, DIR_SOUTH) == false then
         stoker_blocked(actor)
      end
   else
      actor.wt = 0x9D
   end
end

local worktype_tbl = {
   [0x99]=worktype_99_coker_move_to_coal_vein,
   [0x9a]=worktype_9A_coker_drop_coal,
   [0x9b]=worktype_9B_coker_wait_for_coal_to_move_away,
   [0x9c]=worktype_9C_stoker_return_to_conveyor_belt,
   [0x9d]=worktype_9D_stoker_wait_for_coal,
   [0x9e]=worktype_9E_stoker_walk_to_furnace,
}

function perform_worktype(actor)
   --print("wt="..actor.wt.."\n")
   local mpts = actor.mpts
   if worktype_tbl[actor.wt] ~= nil then
      local func = worktype_tbl[actor.wt]
      func(actor)
   end
   
   if mpts == actor.mpts then
      subtract_movement_pts(actor, 10)
   end
end

