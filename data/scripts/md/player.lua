local map_entrance_tbl = {
    {x=0x43,  y=0x51,  z=0x1},
    {x=0x80,  y=0x8D,  z=0x0},
    {x=0x76,  y=0x0F2, z=0x3},
    {x=0x7A,  y=0x0C,  z=0x3},
    {x=0x9A,  y=0x40,  z=0x4},
    {x=0x2B2, y=0x1CC, z=0x0},
    {x=0x73,  y=0x40,  z=0x4},
    {x=0x29D, y=0x1CE, z=0x0},
    {x=0x0B3, y=0x0E1, z=0x4},
    {x=0x27B, y=0x1F1, z=0x0},
    {x=0x0C3, y=0x70,  z=0x1},
    {x=0x0CB, y=0x1D1, z=0x0},
    {x=0x24,  y=0x0F1, z=0x1},
    {x=0x31A, y=0x232, z=0x0},
    {x=0x5C,  y=0x0F1, z=0x1},
    {x=0x34C, y=0x25A, z=0x0},
    {x=0x7C,  y=0x28,  z=0x4},
    {x=0x13E, y=0x118, z=0x0},
    {x=0x0A,  y=0x60,  z=0x5},
    {x=0x3B7, y=0x1C4, z=0x0},
    {x=0x5C,  y=0x0B1, z=0x5},
    {x=0x3DC, y=0x1DD, z=0x0},
    {x=0x5E,  y=0x29,  z=0x4},
    {x=0x227, y=0x1ab, z=0x0}
}

local pod_teleport_tbl = {
    {x=0xCA,  y=0x295},
    {x=0x34D, y=0x295},
    {x=0x13A, y=0x17A},
    {x=0x2CD, y=0x1FA}
}

function player_pass()
    printl("PASS")
    update_conveyor_belt(true)
end

--returns true if the player can move to rel_x, rel_y
function player_before_move_action(rel_x, rel_y)
    if rel_x ~= 0 and rel_y ~= 0 then
        return true
    end

    local player_loc = player_get_location()
    local z = player_loc.z
    local x = wrap_coord(player_loc.x+rel_x,z)
    local y = wrap_coord(player_loc.y+rel_y,z)

    for obj in objs_at_loc(x, y, z) do
        local obj_n = obj.obj_n
        if obj_n == 268 then --wheelbarrow
        move_wheelbarrow(obj, rel_x, rel_y)
        if can_move_obj(obj, rel_x, rel_y) then
            obj.x = obj.x + rel_x
            obj.y = obj.y + rel_y
        end
        elseif obj_n == 410 and can_move_obj(obj, rel_x, rel_y) then --railcar
        move_rail_cart(obj, rel_x, rel_y)
        elseif obj_n == 441 then --assembled drill
        move_drill(obj, rel_x, rel_y) --update drill direction
        if can_move_drill(obj, rel_x, rel_y) then
            obj.x = obj.x + rel_x
            obj.y = obj.y + rel_y
        end
        end
    end

    return true
end

function update_objects_around_party()
    local loc = player_get_location()
    for obj in find_obj_from_area(wrap_coord(loc.x - 5, loc.z), wrap_coord(loc.y - 5, loc.z), loc.z, 11, 11) do
        local obj_n = obj.obj_n
        if (obj_n == 227 and Actor.get_talk_flag(0x73, 2)) or --OBJ_DOOR3
                (obj_n == 179 and Actor.get_talk_flag(0x73, 4)) then --OBJ_CLOSED_DOOR
            if bit32.band(obj.quality, 0x80) == 0 then -- check if the door is stuck
                local base_frame = bit32.band(obj.frame_n, 2)
                local actor = map_get_actor(obj.xyz)
                if actor ~= nil then
                    if map_is_on_screen(obj.xyz) then
                        play_door_sfx()
                    end
                    obj.frame_n = base_frame + 9
                else
                    actor = map_get_actor(obj.x + movement_offset_x_tbl[base_frame + 1], obj.y + movement_offset_y_tbl[base_frame + 1], obj.z)
                    if actor == nil then
                        actor = map_get_actor(obj.x + movement_offset_x_tbl[base_frame + 4 + 1], obj.y + movement_offset_y_tbl[base_frame + 4 + 1], obj.z)
                    end

                    if actor ~= nil and map_is_on_screen(obj.xyz) then
                        play_door_sfx()
                        if obj.frame_n < 4 then
                            obj.frame_n = (base_frame) + 5
                        elseif obj.frame_n < 8 then
                            obj.frame_n = (base_frame) + 1
                        else
                            obj.frame_n = (base_frame) + 5
                        end
                    else
                        if obj.frame_n == 5 then
                            obj.frame_n = 1
                        elseif obj.frame_n == 7 then
                            obj.frame_n = 3
                        end

                    end
                end
            end
        elseif obj_n == 301 then --OBJ_REFLECTIVE_SURFACE
            if obj.frame_n < 3 then
                local actor = map_get_actor(obj.x, obj.y + 1, obj.z)
                if actor ~= nil then
                    local actor_num = actor.actor_num
                    if actor_num >= 0x70 or actor_num == 0x5d or (actor_num >= 0x6a and actor_num <= 0x6c) then
                        obj.frame_n = 0
                    elseif (actor_num < 0x20 and actor_num ~= 6)
                            or (actor_num >= 0x2a and actor_num <= 0x5c)
                            or actor_num == 0x60 or actor_num == 0x69 or actor_num == 0x6d then
                        obj.frame_n = 1
                    else
                        obj.frame_n = 2
                    end
                else
                    obj.frame_n = 0
                end
            end
        end

    end

end

function player_post_move_action(did_move)
    local player_loc = player_get_location()

    if did_move then
        update_conveyor_belt(true)
        update_objects_around_party()

        for obj in objs_at_loc(player_loc) do
            local obj_n = obj.obj_n
            if (obj_n == 175 or obj_n == 163 or obj_n == 180 or obj_n == 178 or (obj_n == 197 and obj.frame_n == 3) or obj_n == 210) then
                if  player_is_in_solo_mode() then
                    if obj_n == 163 or obj_n == 178 then
                        printl("YOU_MUST_BE_IN_PARTY_MODE_TO_LEAVE")
                    else
                        printl("YOU_MUST_BE_IN_PARTY_MODE_TO_ENTER")
                    end
                    return
                end
                if obj_n == 175 then --Mine entry
                    for transfer_obj in objs_at_loc(player_loc.x, player_loc.y-1, player_loc.z) do
                        transfer_obj.x = map_entrance_tbl[obj.quality].x
                        transfer_obj.y = map_entrance_tbl[obj.quality].y-1
                        transfer_obj.z = map_entrance_tbl[obj.quality].z
                    end
                elseif obj_n == 163 then --Mine exit
                    for transfer_obj in objs_at_loc(player_loc.x, player_loc.y+1, player_loc.z) do
                        transfer_obj.x = map_entrance_tbl[obj.quality].x
                        transfer_obj.y = map_entrance_tbl[obj.quality].y+2
                        transfer_obj.z = map_entrance_tbl[obj.quality].z
                    end
                elseif obj_n == 197 then
                    if Actor.get_talk_flag(0x73, 4) then
                        if obj.quality == 0 then
                            printl("NOTHING_APPEARS_TO_HAPPEN")
                        elseif obj.quality <= 4 then
                            local pod_exit = pod_teleport_tbl[obj.quality]
                            advance_time(0)
                            party_use_entrance(player_loc.x, player_loc.y, player_loc.z, pod_exit.x, pod_exit.y, 0)
                        end
                    else
                        printl("THE_ALIEN_DEVICE_DOESNT_SEEM_TO_BE_FUNCTIONING")
                    end
                    return
                end
                advance_time(0)
                party_use_entrance(player_loc.x, player_loc.y, player_loc.z, map_entrance_tbl[obj.quality])
            elseif obj_n == 461 then --OBJ_DREAM_TELEPORTER
                --FIXME add logic here.
            end
        end
    else
        actor_map_dmg(Actor.get_player_actor(), player_loc.x, player_loc.y, player_loc.z)
        play_md_sfx(0)
    end

end

function can_move_drill(drill, rel_x, rel_y)
    if can_move_obj(drill, rel_x, rel_y) then
        return true
    end

    local z = drill.z
    local x = wrap_coord(drill.x+rel_x,z)
    local y = wrap_coord(drill.y+rel_y,z)

    if map_get_obj(x, y, z, 175, true) == nil then --mine entrance
    return false
    end

    for obj in objs_at_loc(x, y, z) do
        if is_blood(obj.obj_n) == false then
            return false
        end
    end

    return true
end