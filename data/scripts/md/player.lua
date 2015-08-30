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
    {x=0x5C,  y=0x0B0, z=0x5},
    {x=0x3DC, y=0x1DD, z=0x0},
    {x=0x5E,  y=0x29,  z=0x4}
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

function player_post_move_action(did_move)
    local player_loc = player_get_location()

    if did_move then
        update_conveyor_belt(true)

        for obj in objs_at_loc(player_loc) do
            if (obj.obj_n == 175 or obj.obj_n == 163) then
                if obj.obj_n == 175 then --Mine entry
                for transfer_obj in objs_at_loc(player_loc.x, player_loc.y-1, player_loc.z) do
                    transfer_obj.x = map_entrance_tbl[obj.quality].x
                    transfer_obj.y = map_entrance_tbl[obj.quality].y-1
                    transfer_obj.z = map_entrance_tbl[obj.quality].z
                end
                else --Mine exit
                for transfer_obj in objs_at_loc(player_loc.x, player_loc.y+1, player_loc.z) do
                    transfer_obj.x = map_entrance_tbl[obj.quality].x
                    transfer_obj.y = map_entrance_tbl[obj.quality].y+2
                    transfer_obj.z = map_entrance_tbl[obj.quality].z
                end
                end
                party_use_entrance(player_loc.x, player_loc.y, player_loc.z, map_entrance_tbl[obj.quality])
            end
        end
    else
        --FIXME do map damage here.
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