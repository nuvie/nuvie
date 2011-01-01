io.stderr:write("actor.lua get here\n")


--Worktypes      
WT_NOTHING                = 0x0  --do nothing

WT_FOLLOW                 = 0x1  --follow avatar (in party)

WT_PLAYER                 = 0x2  --player mode

WT_FRONT                  = 0x3  --combat front
WT_REAR                   = 0x4  --combat rear
WT_FLANK                  = 0x5  --combat flank
WT_BERSERK                = 0x6  --combat berserk
WT_RETREAT                = 0x7  --combat retreat
WT_ASSAULT                = 0x8  --combat assault
WT_FLEE                   = 0x9  --run away from party
WT_LIKE                   = 0xa  --like the party. eg dog
WT_UNFRIENDLY             = 0xb  --wander around but attack if any party member gets too close.
WT_WANDER_NEAR_PLAYER     = 0xc  --guards use this.
WT_TANGLE                 = 0xd  --tangle vine
WT_STATIONARY             = 0xe  --stationary attack
WT_GUARD_WALK_EAST_WEST   = 0xf
WT_GUARD_WALK_NORTH_SOUTH = 0x10
--11
--12
WT_UNK_13                 = 0x13

WT_FACE_NORTH             = 0x87
WT_FACE_EAST              = 0x88
WT_FACE_SOUTH             = 0x89
WT_FACE_WEST              = 0x8a
WT_WALK_NORTH_SOUTH       = 0x8b
WT_WALK_EAST_WEST         = 0x8c
WT_WALK_SOUTH_NORTH       = 0x8d
WT_WALK_WEST_EAST         = 0x8e
WT_WANDER_AROUND          = 0x8f
WT_WORK                   = 0x90
WT_SLEEP                  = 0x91



WT_PLAY_LUTE              = 0x95
WT_BEG                    = 0x96

WT_BELL_RINGER            = 0x98
WT_FIGHTING               = 0x99
WT_MOUSE                  = 0x9a
WT_ATTACK_PARTY           = 0x9b


wt_rear_max_monster = nil
wt_rear_min_monster = nil
wt_rear_max_party = nil
wt_rear_min_party = nil
      
combat_avg_x, combat_avg_y, party_avg_x, party_avg_y = -1, -1, -1, -1
      
wt_front_target_actor = nil

wt_num_monsters_near = 0

movement_offset_x_tbl  = {0, 1, 1, 1, 0, -1, -1, -1} 
movement_offset_y_tbl = {-1, -1, 0, 1, 1, 1, 0, -1}

--Actor stats table
--str,dex,int,hp,dmg,alignment,can talk,?,?,?,?,?,?,takes half dmg,?,strength_based,double dmg from fire,immune to magic,?,?,immune to sleep spell,spell table,weapon table,armor table,treasure table
actor_tbl = {
[364] = {5, 5, 2, 10, 1, ALIGNMENT_CHAOTIC, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, {}, {}, {}, {}},
[429] = {20, 10, 3, 30, 10, ALIGNMENT_CHAOTIC, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, {}, {}, {}, {}},
[427] = {16, 15, 2, 10, 8, ALIGNMENT_CHAOTIC, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, {}, {}, {}, {}},
[410] = {25, 25, 25, 30, 4, ALIGNMENT_GOOD, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[344] = {5, 25, 4, 5, 6, ALIGNMENT_CHAOTIC, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[384] = {8, 8, 9, 6, 3, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {38,34}, {185}, {}},
[356] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[381] = {8, 8, 9, 6, 2, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[357] = {17, 6, 3, 40, 15, ALIGNMENT_CHAOTIC, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, {}, {}, {}, {}},
[428] = {14, 7, 5, 10, 3, ALIGNMENT_NEUTRAL, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {}, {}, {129}},
[424] = {27, 8, 7, 40, 20, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {44,33,34}, {20,3,13}, {98}},
[367] = {35, 26, 31, 100, 20, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, {17,128,34,80}, {}, {}, {}},
[350] = {11, 18, 5, 8, 2, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {129}},
[349] = {5, 16, 8, 6, 3, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[411] = {40, 35, 27, 150, 30, ALIGNMENT_EVIL, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, {34,83,68,96,119}, {}, {}, {98,88,88}},
[369] = {22, 22, 13, 50, 10, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, {34,68}, {}, {}, {88,88}},
[387] = {10, 14, 15, 8, 4, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {38}, {185}, {88}},
[385] = {15, 15, 10, 8, 4, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {105,101}, {17}, {88}},
[376] = {20, 17, 11, 20, 6, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {43,42}, {21,10,3,22}, {88,88}},
[362] = {15, 32, 33, 50, 6, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, {68,65,69}, {46,41,47,49}, {18,1}, {58}},
[363] = {25, 19, 8, 30, 15, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {34,49,37}, {1,9,18,20}, {}},
[355] = {2, 10, 21, 20, 1, ALIGNMENT_EVIL, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, {20,68,80}, {}, {}, {}},
[352] = {1, 10, 9, 20, 12, ALIGNMENT_EVIL, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, {128,99}, {}, {}, {}},
[353] = {2, 26, 7, 5, 1, ALIGNMENT_CHAOTIC, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[382] = {9, 30, 9, 100, 8, ALIGNMENT_EVIL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {90,80}, {22,10,3}, {98}},
[370] = {19, 13, 8, 20, 12, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {34,36}, {17,9}, {88}},
[430] = {22, 27, 7, 15, 3, ALIGNMENT_NEUTRAL, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[431] = {22, 27, 7, 15, 3, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[374] = {26, 11, 2, 50, 15, ALIGNMENT_CHAOTIC, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, {69,96,98,68}, {}, {}, {}},
[343] = {1, 40, 1, 1, 1, ALIGNMENT_CHAOTIC, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, {}, {}, {}, {}},
[383] = {14, 16, 12, 8, 4, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {38,33}, {17}, {88}},
[409] = {30, 30, 30, 255, 255, ALIGNMENT_GOOD, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[386] = {12, 16, 14, 8, 4, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {41,33,43}, {19,1,9}, {88,158}},
[378] = {10, 14, 22, 30, 4, ALIGNMENT_EVIL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {17,34,20,49,68,80,69,83}, {38}, {17,1}, {58,98,88}},
[380] = {12, 12, 18, 8, 4, ALIGNMENT_EVIL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {38,43}, {17,1}, {88}},
[98] = {22, 9, 8, 30, 15, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, {18,80}, {}, {}, {98,98}},
[372] = {20, 27, 13, 30, 20, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, {}, {38,43,36,37}, {9,17,18}, {98,88}},
[354] = {2, 25, 3, 5, 2, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[359] = {3, 9, 3, 5, 2, ALIGNMENT_NEUTRAL, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[342] = {5, 13, 4, 8, 6, ALIGNMENT_CHAOTIC, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, {}, {}, {}, {}},
[347] = {21, 20, 16, 30, 20, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, {17,80,66,68}, {}, {}, {58,88}},
[360] = {3, 17, 2, 5, 2, ALIGNMENT_CHAOTIC, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, {}, {}, {}, {}},
[426] = {13, 13, 3, 20, 8, ALIGNMENT_CHAOTIC, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, {}, {}, {}, {}},
[346] = {17, 17, 9, 70, 30, ALIGNMENT_EVIL, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, {34,83,51}, {}, {}, {}},
[348] = {5, 10, 4, 6, 2, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {129}},
[412] = {30, 30, 30, 30, 30, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[413] = {60, 21, 8, 200, 60, ALIGNMENT_CHAOTIC, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[368] = {10, 14, 7, 20, 6, ALIGNMENT_EVIL, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, {}, {43,36,37,41,38}, {9,1}, {88}},
[375] = {6, 6, 2, 15, 4, ALIGNMENT_CHAOTIC, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, {}, {}, {}, {}},
[358] = {3, 7, 6, 5, 2, ALIGNMENT_CHAOTIC, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[361] = {9, 11, 4, 10, 8, ALIGNMENT_CHAOTIC, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, {89}, {}, {}, {129}},
[345] = {24, 20, 8, 50, 20, ALIGNMENT_EVIL, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, {17,68}, {}, {}, {}},
[377] = {15, 18, 12, 15, 4, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {43,41,33}, {19,1,9}, {88,88}},
[366] = {20, 25, 1, 30, 1, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, {}, {}, {}, {}},
[371] = {18, 14, 9, 15, 6, ALIGNMENT_EVIL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {37,36,43,41,34}, {9,18,1,19}, {98,88,88}},
[379] = {11, 12, 12, 8, 4, ALIGNMENT_CHAOTIC, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, {}, {43,33}, {17}, {88}},
[373] = {1, 40, 20, 40, 1, ALIGNMENT_CHAOTIC, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, {20,69,129}, {}, {}, {}},
[351] = {8, 16, 8, 8, 4, ALIGNMENT_CHAOTIC, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[365] = {20, 5, 1, 30, 1, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, {20}, {}, {}, {}},
[414] = {20, 20, 20, 20, 20, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[415] = {10, 10, 10, 10, 10, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[423] = {10, 10, 10, 10, 10, ALIGNMENT_CHAOTIC, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[388] = {4, 18, 8, 6, 2, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}},
[263] = {11, 12, 12, 8, 4, ALIGNMENT_CHAOTIC, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}}
}

monster_inv_food = { 129, 133, 128 } --portion of meat, ham, loaf of bread

function actor_int_check(attacker, defender)

   if (math.floor(actor_int_adj(attacker) / 2) + 15) - actor_int_adj(defender) > math.random(1, 30) then
      return true
   end
   
   return false
-- if((actor1_int_adj / 2 + 15) - actor2_int_adj > random(1,30))
--return 

end

function subtract_movement_pts(actor, pts)
   if actor.obj_n == 0x1af then pts = math.floor(pts / 2) end --if horse with rider
    
   --fixme check with the original there are two timers that are check but never seem to be used.
   
   if pts < 1 then pts = 1 end
   
   actor.mpts = actor.mpts - pts
end

function subtract_map_movement_pts(actor)
   subtract_movement_pts(actor, 5) --FIXME need to code this up.
end

function actor_find_max_xy_distance(actor, x, y)
   x, y = abs(actor.x - x), abs(actor.y - y)
   return (x > y) and x or y
end

function actor_move(actor, direction, flag)
   print("actor_move("..actor.name..", "..direction_string(direction)..", "..flag..") actor("..actor.x..","..actor.y..")\n");
   local x,y,z = actor.x, actor.y, actor.z
   if direction == DIR_NORTH then y = y - 1 end
   if direction == DIR_SOUTH then y = y + 1 end
   if direction == DIR_EAST then x = x + 1 end
   if direction == DIR_WEST then x = x - 1 end
   
   local did_move = Actor.move(actor, x, y, z)
   
   --actor.direction = direction
      
   if did_move then
      subtract_movement_pts(actor, 5)
      actor.direction = direction
      print("actor_move() did move actor("..actor.x..","..actor.y..")\n");
   end --FIXME need to handle this properly with map movement pts.
   
   return did_move and 1 or 0
end

function actor_move_diagonal(actor, x_direction, y_direction)
   local x,y,z = actor.x, actor.y, actor.z
   
   if y_direction == DIR_NORTH then
      y = y - 1
      direction = x_direction == DIR_EAST and DIR_NORTHEAST or DIR_NORTHWEST
   end
   if y_direction == DIR_SOUTH then
      y = y + 1
      direction = x_direction == DIR_EAST and DIR_SOUTHEAST or DIR_SOUTHWEST
   end
   if x_direction == DIR_EAST then
      x = x + 1
      direction = y_direction == DIR_NORTH and DIR_NORTHEAST or DIR_SOUTHEAST
   end
   if x_direction == DIR_WEST then
      x = x - 1
      direction = y_direction == DIR_NORTH and DIR_NORTHWEST or DIR_SOUTHWEST
   end

   print("actor_move_diagonal("..actor.name..", "..direction_string(direction)..")\n");

   local did_move = Actor.move(actor, x, y, z)
   
   if did_move then
      print("did move\n");
      subtract_movement_pts(actor, 5)
      actor.direction = y_direction
   end --FIXME need to handle this properly with map movement pts.
   
   return did_move and 1 or 0
end

function actor_move_towards_loc(actor, map_x, map_y)
   print("move actor "..actor.name.." from ("..actor.x..","..actor.y..") towards ("..map_x..","..map_y..")\n")
   local var_2 = (word_30A6B == 1) and 0 or 1
   local var_6 = 1
   local diff_x = map_x - actor.x
   local diff_y = map_y - actor.y 

   if (diff_x == 0 and diff_y == 0) or actor.wt == 0xE then subtract_movement_pts(actor, 5) return 0 end

   local x_direction, y_direction
   
   if diff_x ~= 0 then
      x_direction = (diff_x >= 0) and DIR_EAST or DIR_WEST
   else
      x_direction = (math.random(0, 1) == 0) and DIR_WEST or DIR_EAST
   end

   if diff_y ~= 0 then
      y_direction = (diff_y >= 0) and DIR_SOUTH or DIR_NORTH
   else
      y_direction = (math.random(0, 1) == 0) and DIR_SOUTH or DIR_NORTH
   end

   unk_30A72 = 0

   local var_4
   
   if abs(diff_x) >= 4 or abs(diff_y) >= 4 then
      var_4 = (math.random(1, abs(diff_x) + abs(diff_y)) <= abs(diff_x)) and 0 or 1
   else
      var_4 = (abs(diff_x) >= abs(diff_y) or abs(diff_x) ~= abs(diff_y) or math.random(0, 1) == 0) and 0 or 1
   end
print("var_4 = "..var_4.."\n")
   if var_4 == 0 then
      if actor_move(actor, x_direction, var_2) == 0 then
      	if actor_move_diagonal(actor, x_direction, y_direction) == 0 then
      		if actor_move(actor, y_direction, var_2) == 0 then
      			if math.random(0, 1) ~= 0 or actor_move(actor, (y_direction == DIR_NORTH) and DIR_SOUTH or DIR_NORTH, 1) == 0 then
      
         			subtract_map_movement_pts(actor)
         			var_6 = 0 --didn't move anywhere
         		end
         	end
         end
      end

   else

      if actor_move(actor, y_direction, var_2) == 0 then
      	if actor_move_diagonal(actor, x_direction, y_direction) == 0 then
      		if actor_move(actor, x_direction, var_2) == 0 then
      			if math.random(0, 1) ~= 0 or actor_move(actor, (x_direction == DIR_EAST) and DIR_WEST or DIR_EAST, 1) == 0 then
      
         			subtract_map_movement_pts(actor)
         			var_6 = 0 --didn't move anywhere
         		end
         	end
         end
      end

   end

   unk_30A72 = 1
print("var_6 = "..var_6)
   return var_6

end

function toss_actor(actor, from_x, from_y, from_z, arg_0)

   local random = math.random
   local player_loc = player_get_location()

   local i
   for i=1,8 do

      local new_x = random(1, 4) + random(1, 4) + from_x - 5
      local new_y = random(1, 4) + random(1, 4) + from_y - 5
      
      if arg_0 == 0 
         or player_loc.x - 5 > new_x or player_loc.x + 5 < new_x or player_loc.y - 5 > new_y or player_loc.y + 5 < new_y then
         
         if Actor.move(actor, new_x, new_y, from_z) == true then return true end
         
      end
   end
   
   return false
end

function actor_randomise_stat(base_stat)
   local tmp = math.floor(base_stat/2)
   if tmp == 0 then
      return 0
   end
   
   return math.random(0, tmp) + math.random(0, tmp)  + base_stat - tmp
end

function actor_get_max_magic_points(actor)
   local obj_n = actor.obj_n
   if obj_n == 410 then --avatar
      return actor.int * 2
   end
   
   if obj_n == 378 then --mage
      return actor.int
   end
   
   if obj_n == 377 or obj_n == 386 then --swashbuckler, musician
      return floor(actor.int/2)
   end
   
   return 0
end

function actor_str_adj(actor)
   --FIXME
   --if actor flag bit 6 not set then
   -- return strength unadjusted
   
   local str = actor.str
   if str <= 3 then
      return 1
   end
   
   return str - 3
end

function actor_dex_adj(actor)
   --FIXME
   --if actor flag bit 6 not set then
   -- don't subtract 3 from dex
   
   local dex = actor.dex
   if dex <= 3 then
      dex = 1
   else
      dex = dex - 3
   end
   
   --FIXME
   --if timestop and actor not in party then
   -- dex = 1
   
   --FIXME
   --if actor asleep then
   -- dex = 1
   
   return dex
end

function actor_int_adj(actor)
   local int = actor.int
   
   --FIXME if mutant/cursed actor flag 0x40 then subtract 3 points
   
   if int < 1 then int = 1 end
   
   return int
end

--
-- actor_combat_hit_check(attacker, foe, weapon_obj)
--

function actor_combat_hit_check(attacker, foe, weapon_obj)

   if foe == nil then return false end
      
	if foe.luatype ~= "actor" or weapon_obj.obj_n == 48 then --48 glass sword
      io.stderr:write(foe.luatype)
		return true
	end

	local use_str = false
	local obj_n = weapon_obj.obj_n

	if weapon_obj == attacker then
		if actor_tbl[obj_n][16] == 1 then --actor uses strength
			use_str = true
		end
	else
		if obj_n == 4 or obj_n == 34 or obj_n == 39 or obj_n == 44 then --spiked helm, club, mace, two-handed hammer
			use_str = true
		end
	end
   
   local attack_role
   
   if use_str then
      attack_role = actor_str_adj(attacker)
   else
      attack_role = actor_dex_adj(attacker)
   end
   
   local foe_dex = actor_dex_adj(foe)
   local roll = math.random(1,30)
   
   io.stderr:write("foe_dex = "..foe_dex.." attack_role = "..attack_role.." random(1,30) = "..roll.."\n\n")
   
   if math.floor((foe_dex + 30 - attack_role) / 2) >= roll then
      return false
   end
   
   return true
end

--
-- actor_init(actor)
--

actor_init = function(actor)

 local actor_base = actor_tbl[actor.obj_n]
 if actor_base ~= nil then
 
   actor.str = actor_randomise_stat(actor_base[1])
   actor.dex = actor_randomise_stat(actor_base[2])
   actor.int = actor_randomise_stat(actor_base[3])
   local hp = actor_randomise_stat(actor_base[4])
   actor.hp = hp
   actor.level = math.floor((hp + 29) / 30)
   actor.align = actor_base[6]
 else
   actor.str = 10
   actor.dex = 10
   actor.int = 10
   actor.hp = 30
   actor.level = 1
   --actor.status = 32
 end
 
   actor.wt = 8
   actor.combat_mode = 8
   actor.magic = actor_get_max_magic_points(actor)
   actor.mpts = actor.dex
   actor.exp = 100
   
   local obj,i,v,j,qty,chance,chest
   
   --add spells
   for i,v in ipairs(actor_base[22]) do
        obj = Obj.new(336) --charge
        obj.quality = v
        obj.qty = 1
        obj.status = 56
        Actor.inv_add_obj(actor, obj)
   end

   --actor weapon
   chance = 2
   for i,v in ipairs(actor_base[23]) do
      if v >= 36 and v <= 38 then --spear, throwing axe, dagger
         qty = math.random(6, 12)
      else
         qty = 1
      end

      for j=1,qty do
         obj = Obj.new(v)
         obj.status = 57
         if v == 90 then
            obj.qty = 1
         else
            obj.qty = 0
         end
         Actor.inv_add_obj(actor, obj)
         Actor.inv_ready_obj(actor, obj)
         if v == 41 or v == 54 then --bow, magic bow
            obj = Obj.new(55) --arrow
            obj.status = 49
            obj.qty = math.random(12, 24)
            Actor.inv_add_obj(actor, obj)
         elseif v == 42 or v == 50 then --crossbow, triple crossbow
            obj = Obj.new(56) --bolt
            obj.status = 49
            obj.qty = math.random(12, 24)
            Actor.inv_add_obj(actor, obj)           
         end
      end

      chance = chance * 2
   end
   
   -- actor armor
   chance = 2
   for i,v in ipairs(actor_base[24]) do
      
      if math.random(1,chance) == 1 then
      
        obj = Obj.new(v)
        obj.status = 57
        obj.qty = 1
        obj.status = 56
        Actor.inv_add_obj(actor, obj)
        Actor.inv_ready_obj(actor, obj)
      end
      
      chance = chance * 2
   end
   
   --actor treasure
   chance = 2
   for i,v in ipairs(actor_base[25]) do
      
      if math.random(1,chance) == 1 then    
         if v == 98 then --chest
            chest = Obj.new(v)
            chest.frame_n = 1
            chest.status = 41
            chest.qty = math.random(1, 100)
            Actor.inv_add_obj(actor, chest)
            if math.random(0, 1) == 1 then
               obj = Obj.new(337) --effect
               obj.status = 41
               obj.qty = 1
               obj.quality = 22
               Obj.moveToCont(obj, chest)
            end
            if math.random(0, 3) ~= 0 then
               obj = Obj.new(88) --gold coin
               obj.status = 41
               obj.qty = math.random(1, 100)
               Obj.moveToCont(obj, chest)
            end
            if math.random(0, 3) ~= 0 then
               obj = Obj.new(monster_inv_food[math.random(0, 2)])
               obj.status = 41
               obj.qty = math.random(1, 10)
               Obj.moveToCont(obj, chest)
            end
            if math.random(0, 3) == 0 then
               obj = Obj.new(90) --torch
               obj.status = 41
               obj.qty = math.random(1, 6)
               Obj.moveToCont(obj, chest)
            end
            if math.random(0, 10) == 0 then
               obj = Obj.new(77) --gem
               obj.status = 41
               obj.qty = math.random(1, 4)
               Obj.moveToCont(obj, chest)
            end
         elseif v == 58 then --spell
            if math.random(0, 3) ~= 0 and actor.obj_n ~= 362 then --Gargoyle
               obj = Obj.new(88)
               obj.status = 49
               obj.qty = math.random(1, 100)
               Actor.inv_add_obj(actor, obj)
            end
            if math.random(0, 3) == 0 then
               obj = Obj.new(monster_inv_food[math.random(0, 2)])
               obj.status = 49
               obj.qty = math.random(1, 100)
               Actor.inv_add_obj(actor, obj)
            end
            if math.random(0, 3) == 0 then
               obj = Obj.new(275) --potion
               obj.frame_n = math.random(0, 7) --random potion type
               obj.status = 49
               obj.qty = 1
               Actor.inv_add_obj(actor, obj)
            end              
         elseif v == 88 then --gold coin
            obj = Obj.new(v)
            obj.status = 49
            obj.qty = math.random(1,30) + math.random(1, 30)
            Actor.inv_add_obj(actor, obj)
         else
            obj = Obj.new(v)
            obj.quality = 49
            obj.qty = 1
            Actor.inv_add_obj(actor, obj)
         end

      end
      
      chance = chance * 2
   end   
end


local get_attack_range = function(x,y,target_x,target_y)
   local combat_range_tbl = {
0, 1, 2, 3, 4, 5, 6, 7, 
1, 1, 2, 3, 4, 5, 6, 7, 
2, 2, 2, 3, 4, 5, 6, 7,
3, 3, 3, 4, 5, 6, 7, 7,
4, 4, 4, 5, 6, 7, 7, 8,
5, 5, 5, 6, 7, 7, 8, 8,
6, 6, 6, 7, 7, 8, 8, 8,
7, 7, 7, 7, 8, 8, 8, 8}

   local absx = abs(target_x - x)
   local absy = abs(target_y - y)
   io.stderr:write("target_x="..target_x.." target_y="..target_y.." x="..x.." y="..y.." absx="..absx.." absy="..absy)
   if absx < 8 and absy < 8 then
      return combat_range_tbl[absx * 8 + absy + 1]
   end
   
   return 9
end

local get_weapon_range = function(obj_n)
   local range_weapon_tbl = {
      [40] = 2, -- morning star
      [47] = 2, -- halberd
      [38] = 3, -- dagger
      [83] = 5, -- flask of oil
      [33] = 4, -- sling
      [37] = 3, -- throwing axe
      [36] = 4, -- spear
      [41] = 5, -- bow
      [42] = 7, -- crossbow
      [57] = 7, -- spellbook
      [336] = 5, -- charge
      [49] = 5, -- boomerang
      [50] = 7, -- triple crossbow
      [412] = 5, -- ship
      [91] = 4, -- Zu Ylem
      [54] = 7, -- magic bow
      [79] = 7, -- lightning wand
      [80] = 7, -- fire wand
   }
   
   local range = range_weapon_tbl[obj_n]
   
   if range == nil then
      return 1
   end
   
   return range

end

local projectile_weapon_tbl = 
{
--obj_n = {tile_num, initial_tile_rotation, speed, doesRotate}
[33] = {398, 0, 2, false}, -- sling
[36] = {547, 45,3, false}, -- spear
[37] = {548, 0, 2, true}, -- throwing axe
[38] = {549, 0, 2, true}, -- dagger
[41] = {566, 90,4, false}, -- bow
[42] = {567, 90,4, false}, -- crossbow
[49] = {560, 0, 4, true}, -- boomerang
[83] = {601, 0, 2, false}, -- flask of oil
[50] = {567, 90,4, false}, -- triple crossbow
[91] = {612, 0, 2, true}, -- Zu Ylem
[79] = {392, 0, 2, false}, -- lightning wand
[80] = {393, 0, 2, false}, -- fire wand
}

weapon_dmg_tbl = {
[4] = 4, --spiked helm
[33] = 6, --sling
[34] = 8, --club
[35] = 8, --main gauche
[36] = 10, --spear
[37] = 10, --throwing axe
[38] = 6, --dagger
[39] = 15, --mace
[40] = 15, --morning star
[41] = 10, --bow
[42] = 12, --crossbow
[43] = 15, --sword
[44] = 20, --two-handed hammer
[45] = 20, --two-handed axe
[46] = 20, --two-handed sword
[47] = 30, --halberd
[49] = 8, --boomerang
[50] = 12, --triple crossbow
[48] = 255, --glass sword
[13] = 4, --spiked shield
[221] = 90, --cannon
[83] = 4, --flask of oil
[54] = 20, --magic bow
[109] = 2, --rolling pin
[100] = 6, --scythe
[101] = 4, --pitchfork
[103] = 4, --pick
[104] = 4, --shovel
[105] = 4, --hoe
[113] = 4, --cleaver
[114] = 4, --knife
[141] = 4, --decorative sword
[412] = 30, --ship
[91] = 0, --Zu Ylem
[78] = 4, --staff
[79] = 30, --lightning wand
[80] = 20, --fire wand
}

armour_tbl =
{
[1] = 1, --leather helm
[2] = 2, --chain coif
[3] = 3, --iron helm
[4] = 3, --spiked helm
[5] = 2, --winged helm
[6] = 2, --brass helm
[7] = 3, --Gargoyle Helm
[8] = 5, --magic helm
[9] = 2, --wooden shield
[10] = 3, --curved heater
[11] = 3, --winged shield
[12] = 3, --kite shield
[13] = 2, --spiked shield
[14] = 2, --black shield
[15] = 4, --door shield
[16] = 5, --magic shield
[17] = 1, --cloth armour
[18] = 2, --leather armor
[19] = 3, --ring mail
[20] = 4, --scale mail
[21] = 5, --chain mail
[22] = 7, --plate mail
[23] = 10, --magic armour
[35] = 1, --main gauche
[24] = 2, --spiked collar
[256] = 5, --protection ring
}

triple_crossbow_offset_tbl = 
{
 {
  4,5,5,5,5,6,6,6,6,6,6,
  4,4,5,5,5,6,6,6,6,6,7,
  4,4,4,5,5,6,6,6,6,7,7,
  4,4,4,4,5,6,6,6,7,7,7,
  4,4,4,4,4,6,6,7,7,7,7,
  4,4,4,4,4,0,0,0,0,0,0,
  3,3,3,3,2,2,0,0,0,0,0,
  3,3,3,2,2,2,1,0,0,0,0,
  3,3,2,2,2,2,1,1,0,0,0,
  3,2,2,2,2,2,1,1,1,0,0,
  2,2,2,2,2,2,1,1,1,1,0
 },
 {
  2,2,2,2,2,2,3,3,3,3,4,
  1,2,2,2,2,2,3,3,3,4,4,
  1,1,2,2,2,2,3,3,4,4,4,
  1,1,1,2,2,2,3,4,4,4,4,
  1,1,1,1,2,2,4,4,4,4,4,
  0,0,0,0,0,0,4,4,4,4,4,
  0,0,0,0,0,6,6,5,5,5,5,
  0,0,0,0,7,6,6,6,5,5,5,
  0,0,0,7,7,6,6,6,6,5,5,
  0,0,7,7,7,6,6,6,6,6,5,
  0,7,7,7,7,6,6,6,6,6,6
 }
}

function get_weapon_dmg(weapon_obj_n)
   local dmg = weapon_dmg_tbl[weapon_obj_n]
   
   if dmg == nil and actor_tbl[weapon_obj_n] ~= nil then
      dmg = actor_tbl[weapon_obj_n][5]
   end
   
   return dmg
end

function actor_get_ac(actor)

   
   --FIXME if cursed -3
   --FIXME if protected + 3
   local ac = 0
   local obj
     
   for obj in actor_inventory(actor) do
      if obj.readied then
      
         local armour = armour_tbl[obj.obj_n]
         if armour ~= nil then
            ac = ac + armour
         end
      end
   end

   return ac
end

function actor_take_hit(attacker, defender, max_dmg)
   if attacker.obj_n == 357 then --corpser
      attacker.frame_n = 1 --reveal corpser.
   end
   if max_dmg == -1 then
      max_dmg = 1
   elseif max_dmg > 1 and max_dmg ~= 255 then
      max_dmg = math.random(1, max_dmg)
   end
   
   local ac
   if defender.luatype == "actor" then
      ac = actor_get_ac(defender)
      if max_dmg ~= 255 and ac > 0 then
         max_dmg = max_dmg - math.random(1, ac)
      end
      print("\nac = "..ac.."\n")
   else
      ac = 0 --object
   end
   
   if max_dmg > 0 then
      if defender.luatype == "actor" and defender.wt > 1 and defender.wt < 16 then
         --FIXME defender now targets attacker. I think.
      end
      --FIXME hit defending actor properly here.
      local exp_gained = actor_hit(defender, max_dmg)
      if exp_gained == nil then
         exp_gained = 0
      end
      
      attacker.exp = attacker.exp + exp_gained
   else
      print(defender.name.." grazed.\n")
   end
end

function actor_hit(defender, max_dmg)
	
	local defender_obj_n = defender.obj_n
	local exp_gained = 0
	
	if defender_obj_n == 0x1a7 then -- balloon.
		return 0
	end
	
	if defender.luatype == "actor" then
		--actor logic here
		if defender_obj_n == 409 then --lord british
			defender.hp = 0xff
		elseif defender_obj_n == 414 or defender_obj_n == 415 then --skiff, raft
			-- FIXME hit random party member here.
		else
			-- actor hit logic here
			exp_gained = Actor.hit(defender, max_dmg) --FIXME need to bring death function into script.
			if defender.alive == true then
				if defender.wt == 9 then --flee
					defender.wt = 8 --assault
				end
				if defender_obj_n == 375 then --slime
					slime_divide(defender)
				end
			end
		end
	else
		--object logic here
		if defender_obj_n == 0x16e then -- tangle vine
			--FIXME do something with tangle vine.
		elseif defender.stackable == false and defender.qty ~= 0 then
			hit_anim(defender.x, defender.y)
			if (defender_obj_n < 0x129 or defender_obj_n > 0x12c or defender.frame_n < 0xc) --check frame_n for door objects
				and (defender_obj_n ~= 0x62 or defender.frame_n ~= 3) then --don't attack open chests
				if defender.qty <= max_dmg then
					print(defender.name .. " broken!\n")
	
					local child
					for child in container_objs(defender) do  -- look through container for effect object. 
					  if child.obj_n == 337 then --effect
					  	--FIXME use effect here.
					  	break
					  else
					  	Obj.moveToMap(child, defender.x, defender.y, defender.z)
					  end
					end
					
					if defender_obj_n == 0x7b then --mirror
						--FIXME play mirror smashing sound
						defender.frame_n = 2
						player_subtract_karma(10)
					else
						map_remove_obj(defender)
					end
				else
					defender.qty = defender.qty - max_dmg
				end
			end
		end
	end
	
	return exp_gained
end

function slime_divide(slime)

	local random = math.random
	local from_x, from_y, from_z = slime.x, slime.y, slime.z
	local i
	for i=1,8 do

		local new_x = random(1, 2) + from_x - 1
		local new_y = random(1, 2) + from_y - 1

		if map_can_put(new_x, new_y, from_z) then
			Actor.clone(slime, new_x, new_y, from_z)
			slime_update_frames()
			print("Slime divides!\n")
			return
		end
	end
end

function bitor(x, y)
 local p = 1
  while p < x do p = p + p end
  while p < y do p = p + p end
  local z = 0 
  repeat
   if p <= x or p <= y then
     z = z + p 
     if p <= x then 
     x = x - p 
     end 
     if p <= y then y = y - p end
   
   end
  p = p * 0.5
  until p < 1
  return z
end 

function slime_update_frames()

	local i, j
	local pow = math.pow
	
	for i=1,0x100 do
		local actor = Actor.get(i)
		if actor.obj_n == 0x177 and actor.alive then --slime
			local new_frame_n = 0;
			local idx = 0
			for j = 1,8,2 do
				local tmp_actor = map_get_actor(actor.x + movement_offset_x_tbl[j], actor.y + movement_offset_y_tbl[j], actor.z)
				if tmp_actor ~= nil and tmp_actor.obj_n == 0x177 and tmp_actor.alive then
					new_frame_n = new_frame_n + pow(2,idx)	
				end
				idx = idx + 1
			end
			actor.frame_n = new_frame_n
		end
	end
end

function combat_range_check_target(actor_attacking)

   if actor_attacking.wt > 1 and actor_attacking.wt < 0x10 then
--[[
      target = actor_attacking.target_obj
      if target ~= actor_attacking then
      
         if target.obj_n ~= 0 and target.alive == true and target.asleep == false and target.paralysed == false and target.corpser_flag == false then
         
            if abs(actor_attacking.x - target.x) < 2 then
            
               if abs(actor_attacking.y - target.y) < 2 and (time_stop_spell_timer == 0 or target.in_party == true) then
               
                  return false
               end
            end
         end
      end
--]]
   end
   
return true
end

--
-- actor_attack(attacker, target, weapon)
--

function actor_attack(attacker, target_x, target_y, target_z, weapon)

   local random = math.random
   local weapon_obj_n = weapon.obj_n
   local weapon_quality = weapon.quality

   local foe = map_get_actor(target_x, target_y, target_z)
   
   if foe == nil then
      foe = map_get_obj(target_x, target_y, target_z);
   end
   
   io.stderr:write("\nactor_attack()\nrange = " .. get_attack_range(attacker.x,attacker.y, target_x, target_y).." weapon range="..get_weapon_range(weapon_obj_n))
   if weapon_obj_n ~= attacker.obj_n then
      io.stderr:write("\nweapon = "..weapon.name.."obj_n = "..weapon_obj_n.."\n")
   end
   
   if get_attack_range(attacker.x,attacker.y, target_x, target_y) > get_weapon_range(weapon_obj_n) then
      return
   end

   --check for arrows or bolts is attacking with a bow or crossbow

   if (weapon_obj_n == 41 or weapon_obj_n == 54) and Actor.inv_has_obj_n(attacker, 55) == false then --bow, magic bow, arrows
      return --no arrows, bail out
   end
   if (weapon_obj_n == 42 or weapon_obj_n == 50) and Actor.inv_has_obj_n(attacker, 56) == false then --crossbow, triple crossbow, bolts
      return --no bolts, bail out
   end


   local x_diff = target_x - attacker.x
   local y_diff = target_y - attacker.y
   local attacker_direction
   
   if abs(x_diff) <= abs(y_diff) then
     attacker_direction = (y_diff >= 0) and DIR_SOUTH or DIR_NORTH
   else
     attacker_direction = (x_diff >= 0) and DIR_EAST or DIR_WEST
   end

   local actor_obj_n = attacker.obj_n
   --[[
   if actor_obj_n == 0x19c then --ship

      if(((*(&objlist_npc_movement_flags + actor_attacking) & 6 xor attacker_direction) & 2) == 0)
      {
         attacker_direction = attacker_direction xor 2;
      }
      else
      {
         attacker_direction = *(objlist_npc_movement_flags + actor_attacking) & 7;
      }
   end
--]]

   attacker.direction = attacker_direction
   if actor_obj_n >= 0x170 and actor_obj_n <= 0x174 then -- skeleton, mongbat
      local frame_n = attacker.frame_n
      attacker.frame_n = (frame_n - (frame_n % 4)) + 2
   end

   
   local is_range_weapon = false

   if weapon_obj_n ~= 40 and weapon_obj_n ~= 47 then -- morning star, halberd
      if abs(x_diff) > 1 or abs(y_diff) > 1 or projectile_weapon_tbl[weapon_obj_n] ~= nil then
         is_range_weapon = true
         if combat_range_check_target(attacker) == false then return 1 end
      end
   end
     
   local dmg = get_weapon_dmg(weapon_obj_n)
   if dmg == nil then
      dmg = 1
   end
   
   --FIXME run unknown func sub_1EABC here.
   local player_loc = player_get_location()  

   if weapon_obj_n == 0x150 or weapon_obj_n == 0x39 then --charge, spellbook

      if weapon_quality == 0x81 and actor_obj_n == 0x175 then -- special wisp magic (wisp teleport), wisp
                  
         if toss_actor(attacker, player_loc.x, player_loc.y, player_loc.z) == true then
         
            print("Wisp teleports!\n")
         end

         return
      end
      
      local spell_retcode = 0
      --FIXME cast spell here.
      --spell_retcode = cast_magic_spell(weapon_quality, 1)

      magic_cast_spell(weapon_quality, attacker, {x = target_x, y = target_y, z = target_z})
      
      if weapon_quality == 0x50 and spell_retcode == 0xfe then
         print(foe.name .. " is charmed.\n")
      elseif weapon_quality == 0x45 and spell_retcode == 0 then
         print(foe.name .. " is paralyzed.\n")
      end
      
      return
   end
   
   
   --weapon here.
   
   local hit_actor = actor_combat_hit_check(attacker, foe, weapon)
   local num_bolts
   
   if is_range_weapon == true then
   
      if hit_actor == false then
         hit_actor = nil
         target_x = target_x + random(0, 2) - 1
         target_y = target_y + random(0, 2) - 1
      end
      
      if weapon_obj_n == 0x32 then --triple crossbow
         num_bolts = Actor.inv_get_obj_total_qty(attacker, 0x38)
         print("total num_bolts = "..num_bolts.."\n")
         if num_bolts > 3 then num_bolts = 3 end
      end
      
      --FIXME might need to get new foe here.
      combat_range_weapon_1D5F9(attacker, target_x, target_y, target_z, foe, weapon)
      
   else --standard weapon
      if actor_find_max_xy_distance(attacker, player_loc.x, player_loc.y) < 6 then
         --play_sound_effect(0x11, 0);
      end
   end
   
   if hit_actor == nil then
      hit_actor = actor_combat_hit_check(attacker, foe, weapon)
   end
   
   
   if weapon_obj_n == 0x32 then --triple crossbow

      local off = ((attacker.y - target_y + 5) * 11) + (attacker.x - target_x + 5)
      local i
      for i=1,num_bolts do
      
         if i > 1 then
            print("num_bolts = "..num_bolts.." off = "..off.." target_x = "..target_x.." target_y = "..target_y.."attacker.x = "..attacker.x.." attacker.y = "..attacker.y.."\n\n")
            local t = triple_crossbow_offset_tbl[i-1][off+1]
            
            foe = map_get_actor(target_x + movement_offset_x_tbl[t+1], target_y + movement_offset_y_tbl[t+1], player_loc.z)
            print("new_x = "..target_x + movement_offset_x_tbl[t+1].." new_y = "..target_y + movement_offset_y_tbl[t+1].."\n");
            hit_actor = actor_combat_hit_check(attacker, foe, weapon);
         end

         if hit_actor == true then
            print("triple xbow hit actor dmg = "..dmg.."\n");
            actor_take_hit(attacker, foe, dmg);
         end

      end
   else
   
      if hit_actor == true then
      
         local actor_base = actor_tbl[foe.obj_n]
         if actor_base ~= nil and actor_base[14] == 1 -- takes half dmg
            and weapon_obj_n ~= 0x30 and weapon_obj_n ~= 0x32 and weapon_obj_n ~= 0x36 then --glass sword, triple crossbow, magic bow
            dmg = math.floor((dmg + 1) / 2)
         end
         
         if weapon_obj_n ~= 0x5b then --Zu Ylem
            actor_take_hit(attacker, foe, dmg)
         end
      
         if weapon_obj_n == 0x30 then
            print("Thy sword hath shattered!\n")
            Actor.inv_remove_obj(attacker, weapon)
         end
      end
      
      if weapon_obj_n == 0x31 then --boomerang
         --projectile_anim(attacker.x, attacker.y, 1, target_x, target_y, 0x64, *(&bastile_ptr_data + 0x62), 1)
         projectile_anim(projectile_weapon_tbl[weapon_obj_n][1], target_x, target_y, attacker.x, attacker.y, projectile_weapon_tbl[weapon_obj_n][3], 0, projectile_weapon_tbl[weapon_obj_n][2])
      end
   end
end


function combat_range_weapon_1D5F9(attacker, target_x, target_y, target_z, foe, weapon)

   local weapon_obj_n = weapon.obj_n
   local random = math.random
   
   if weapon_obj_n == 0x32 then --triple cross bow
      local index = ((attacker.y - target_y + 5) * 11) + (attacker.x - target_x + 5) + 1
      local triple_crossbow_targets = {
                      {x=target_x,
                       y=target_y,
                       z=target_z}, 
                      {x=target_x + movement_offset_x_tbl[triple_crossbow_offset_tbl[1][index]+1],
                       y=target_y + movement_offset_y_tbl[triple_crossbow_offset_tbl[1][index]+1],
                       z=target_z},
                      {x=target_x + movement_offset_x_tbl[triple_crossbow_offset_tbl[2][index]+1],
                       y=target_y + movement_offset_y_tbl[triple_crossbow_offset_tbl[2][index]+1],
                       z=target_z}
                    }
                    
      projectile_anim_multi(projectile_weapon_tbl[weapon_obj_n][1], attacker.x, attacker.y, triple_crossbow_targets, projectile_weapon_tbl[weapon_obj_n][3], 0, projectile_weapon_tbl[weapon_obj_n][2])
   else     
      projectile_anim(projectile_weapon_tbl[weapon_obj_n][1], attacker.x, attacker.y, target_x, target_y, projectile_weapon_tbl[weapon_obj_n][3], 0, projectile_weapon_tbl[weapon_obj_n][2])
   end
    
   if weapon_obj_n == 0x5b then
      --Zu Ylem
      if foe ~= nil and foe.obj_n ~= 0 then
         Actor.inv_remove_obj_qty(attacker, 0x5b, 1)
         spell_put_actor_to_sleep(attacker, foe)
      end
   elseif weapon_obj_n == 0x4f or weapon_obj_n == 0x50 then
      --lightning wand, fire wand
      if random(1, 100) == 37 then
      	Actor.inv_remove_obj(attacker, weapon)
      	print("A wand has vanished!\n")
      end
   elseif weapon_obj_n == 0x53 then
      --flask of oil

      Actor.inv_remove_obj_qty(attacker, 0x53, 1)
                  
      if map_is_water(target_x,target_y,target_z) == false then
	      local obj = Obj.new(317); --fire field
	      Obj.moveToMap(obj, target_x, target_y, target_z)
      end
      
   elseif weapon_obj_n == 0x24 or weapon_obj_n == 0x25 or weapon_obj_n == 0x26 then
      --spear, throwing axe, dagger

      if Actor.inv_remove_obj_qty(attacker, weapon_obj_n, 1) == 1 and map_is_water(target_x,target_y,target_z) == false then
	      local obj = Obj.new(weapon_obj_n);
		  Obj.moveToMap(obj, target_x, target_y, target_z)
	  end
      	      
   elseif weapon_obj_n == 0x29 or weapon_obj_n == 0x2a or weapon_obj_n == 0x32 or weapon_obj_n == 0x36 then
      --bow, crossbow, triple crossbow, magic bow
      local projectile_obj = nil
      if weapon_obj_n == 0x29 or weapon_obj_n == 0x36 then --bow, magic bow
      	projectile_obj = 0x37 --arrow
      else
      	projectile_obj = 0x38 --bolt
      end
      
      local qty = 1
      if weapon_obj_n == 0x32 then -- triple crossbow
      	qty = 3
      end
      
      Actor.inv_remove_obj_qty(attacker, projectile_obj, qty)
   end
   
   return 1
end

--
-- actor_get_weapon()
--
function actor_get_weapon(attacker, foe)
   
   if foe == nil then return nil end
   
   local range = get_attack_range(attacker.x, attacker.y, foe.x, foe.y)
   print("range = "..range.."\n")
   local max_dmg = 0
   local obj, weapon
   
   for obj in actor_inventory(attacker) do
      if obj.readied and obj.obj_n == 57 or --spellbook
      obj.obj_n == 336 and math.random(0,3) == 0 and math.random(0, obj.quality) < 16 then --charge (spell)
         print("magic object quality = "..obj.quality.."\n");
         return obj
      else
         dmg = get_weapon_dmg(obj.obj_n)  
         if dmg ~= nil and dmg > max_dmg and get_weapon_range(obj.obj_n) >= range then
            max_dmg = dmg
            weapon = obj
         end
      end
   end
   
   if weapon == nil then --attack with bare hands.
   	weapon = attacker
   end
   
   print("weapon: "..weapon.name.." dmg="..get_weapon_dmg(weapon.obj_n).."\n")
      	
   return weapon
end

--
-- actor_calculate_avg_coords()
--
function actor_calculate_avg_coords()
   local n = 0
   local avg_x = 0
   local avg_y = 0
   
   local player_loc = player_get_location()
   
   local player_x = player_loc.x
   local player_y = player_loc.y
   
   local actor
   for actor in party_members() do
      if actor.wt ~= WT_FLANK and actor.wt ~= WT_BERSERK then
         n = n + 1
         avg_x = avg_x + actor.x
         avg_y = avg_y + actor.y
      end
   end
   
   if n > 0 then
      party_avg_x = math.floor(avg_x / n)
      party_avg_y = math.floor(avg_y / n)
   else
      party_avg_x = player_x
      party_avg_y = player_y
   end
  
   n = 0
   avg_x = 0
   avg_y = 0
   
   local i 
   for i=1,0x100 do
      actor = Actor.get(i)
      
      if actor.obj_n ~= 0 and actor.alive then
         
         if (actor.align == ALIGNMENT_EVIL or actor.align == ALIGNMENT_CHAOTIC) then
            if actor.wt ~= WT_RETREAT or (abs(actor.x -  player_x) <= 5 and abs(actor.y - player_y) <= 5) then
               if abs(actor.x - player_x) < 0x18 and abs(actor.y - player_y) < 0x18 then
                  n = n + 1
                  avg_x = avg_x + actor.x
                  avg_y = avg_y + actor.y
               end
            end
         end
      end
   end
   
   wt_num_monsters_near = n
   
   player_dir = 0 --FIXME load from party roster.
         
   if n > 0 then
      combat_avg_x = math.floor((avg_x + n / 2) / n)
      combat_avg_y = math.floor((avg_y + n / 2) / n)
      
      wt_rear_max_monster = nil
      wt_rear_min_monster = nil
      --wt_rear_max_party = nil
      --wt_rear_min_party = nil
      
      local tmp_x = combat_avg_x - party_avg_x
      local tmp_y = combat_avg_y - party_avg_y
      
      wt_front_target_actor = nil
      local min_m_pos = 0
      local max_m_pos = 0
      local min_p_pos = 0
      local max_p_pos = 0
      --var_16 = 0x8000
      
      local var_A
      for i=1,0x100 do
         actor = Actor.get(i)
         if actor.obj_n ~= 0 and actor.alive and (actor.wt == WT_FRONT or actor.wt == WT_PLAYER) then
            --var_C = ((actor.x - party_avg_x) * tmp_x) + ((actor.y - party_avg_y) * tmp_y)
            var_A = ((actor.x - party_avg_x) * tmp_y) - ((actor.y - party_avg_y) * tmp_x)
          --  print("ok "..actor.name.."\n")
            if actor.in_party == false then
               if wt_front_target_actor == nil then wt_front_target_actor = actor end
               if var_A > max_m_pos then max_m_pos, wt_rear_max_monster = var_A, actor end
               if var_A < min_m_pos then min_m_pos, wt_rear_min_monster = var_A, actor end
            else
           print("yup var_A["..i.."] = "..var_A.."\n") 
               if var_A > max_p_pos then
               	print("var_A > max_p_pos\n")
               	max_p_pos, wt_rear_max_party = var_A, actor
               end
               if var_A < min_p_pos then
                print("var_A < min_p_pos\n")
               	min_p_pos, wt_rear_min_party = var_A, actor
               end
               --if var_C > var_16 then var_16, unk_3DEAD = var_C, actor end           
            end
         end
      end
   else
      combat_avg_x = player_x + movement_offset_x_tbl[player_dir + 1]
      combat_avg_y = player_y + movement_offset_y_tbl[player_dir + 1]            
   end
   
   if combat_avg_x == party_avg_x and combat_avg_y == party_avg_y then
      combat_avg_x = combat_avg_x + movement_offset_x_tbl[player_dir + 1]
      combat_avg_y = combat_avg_y + movement_offset_y_tbl[player_dir + 1]
   end
   print("PartyAvg("..party_avg_x..","..party_avg_y..") CombatAvg("..combat_avg_x..","..combat_avg_y..") numMonsters = "..wt_num_monsters_near.."\n")
end

--
-- actor_update_all()
--
function actor_update_all()
print("actor_update_all()\n")
   local player_loc = player_get_location()
   actor_calculate_avg_coords()
   local actor
   repeat
      local selected_actor = nil
      local di = 0
      local dex_6 = 1
      repeat
         local var_C = (player_loc.x - 16) - (player_loc.x - 16) % 8
         local var_A = (player_loc.y - 16) - (player_loc.y - 16) % 8
         
         for actor in party_members() do
            if actor.wt == WT_FOLLOW and actor.mpts < 0 then
               actor.mpts = 0
            end
         end
         
         for i=1,0x100 do
            local actor = Actor.get(i)
            if actor.obj_n ~= 0 then --not paralysed not asleep
               if actor.wt ~= WT_NOTHING and actor.alive == true and actor.mpts > 0 and actor.z == player_loc.z then
                  if abs(actor.x - var_C) > 0x27 or abs(actor.y - var_A) > 0x27 then
                     if actor.wt >= 0x83 and actor.wt <= 0x86 then
                        --actor_move_to_sched_loc
                     end
                  else
                     if actor.wt ~= WT_FOLLOW then
                        if actor.wt == 0x80 then
                           -- actor_set_worktype_from_schedule(actor)
                        end
                        
                        local dx = (actor.mpts * dex_6) - actor.dex * di
                        if actor.mpts >= actor.dex or dx > 0 or dx == 0 and actor.dex > dex_6 then
                           selected_actor = actor
                           di = actor.mpts
                           dex_6 = actor.dex
                        end
                        
                        if actor.mpts >= actor.dex then
                           break
                        end
                     end
                  end
               end
            end
         end
         

         if di <= 0 then
            local i,m
            for i=1,0x100 do
               local actor = Actor.get(i)
               if actor.obj_n ~= 0 then
                  if actor.mpts >= 0 then
                     m = actor.dex
                  else
                     m = actor.mpts + actor.dex
                  end
                  
                  actor.mpts = m
               end
            end
            --advance_time(1)
         end
      until di > 0
      
      --if actor.corpser_bit == 0 then
      if selected_actor.wt ~= WT_PLAYER and selected_actor.wt ~= WT_FOLLOW then
         print("perform_worktype("..selected_actor.name.." dex = "..selected_actor.dex.." mpts = "..selected_actor.mpts..").\n")
         perform_worktype(selected_actor)
         --sub_1C34A() --update map window??
         if selected_actor.wt > 1 and selected_actor.wt < 0x10 then
            --do *(&objlist_ptr_unk_18f1 + actor_num) = actor_num
         end
      end
      --sub_4726()
      --end --corsper bit == 0

   until selected_actor.obj_n ~= 0 and selected_actor.wt == WT_PLAYER -- and actor_num.corpser_bit not set.
end

local tangle_vine_frame_n_tbl = {
1, 3, 1, 4,
5, 0, 4, 0,
1, 2, 1, 5,
2, 0, 3, 0
}

function move_tanglevine(actor, new_direction)
   local actor_x = actor.x
   local actor_y = actor.y
   local old_direction = actor.direction
   
   if new_direction ~= direction_reverse(old_direction) then

      if actor_move(actor, new_direction, 1) ~= 0 then
         local player_loc = player_get_location()

         local tangle_obj = Obj.new(0x16e) --tanglevine

         tangle_obj.frame_n = tangle_vine_frame_n_tbl[old_direction * 4 + new_direction + 1]
         tangle_obj.status = 0x20
         tangle_obj.quality = 0 --actor.id_n

         Obj.moveToMap(tangle_obj, actor_x, actor_y, player_loc.z)
         
         actor.frame_n = tangle_vine_frame_n_tbl[new_direction * 4 + math.random(0, 3) + 1]

         return true
      end
   end

   return false
end

function actor_wt_front(actor)
	print("actor_wt_front("..actor.name..")\n")
	if actor_wt_front_1FB6E(actor) ~= 0 then
		if actor.in_party == true then 
			actor_wt_attack(actor)
		else
			if actor_find_max_xy_distance(actor, party_avg_x, party_avg_y) >= 8 then
				subtract_movement_pts(actor, 5)
			else
				if actor ~= wt_front_target_actor then
					actor_wt_attack(actor)
				else
					if math.random(0, 7) ~= 0 then
						actor_wt_attack(actor)
					else
						actor_move_towards_loc(actor, party_avg_x, party_avg_y)
					end
				end
			end
		end
		
		return
	end
	
	return
end

function actor_wt_front_1FB6E(actor)
   print("actor_wt_front_1FB6E()\n")
   if (wt_num_monsters_near == 0 or wt_front_target_actor == nil) and actor.in_party == false then
      subtract_movement_pts(actor, 5)
      return 1
   end
   
   local player_loc = player_get_location()
   
   local centre_x,centre_y,diff_x,diff_y,var_1E,var_24,var_14,var_12,var_20,var_1C,var_10,target_x,target_y,var_E
   local actor_x = actor.x
   local actor_y = actor.y
   if actor.in_party == false then 
      centre_x = party_avg_x
      centre_y = party_avg_y
      diff_x = wt_front_target_actor.x - centre_x
      diff_y = wt_front_target_actor.y - centre_y
   else
      centre_x = player_loc.x
      centre_y = player_loc.y
      diff_x = combat_avg_x - centre_x
      diff_y = combat_avg_y - centre_y
   end
   
   print("actor_wt_front_1FB6E() actor = ("..actor_x..","..actor_y..") centre = ("..centre_x..","..centre_y..") player = ("..player_loc.x..","..player_loc.y..")\n")
   var_1E = (actor_x - centre_x) * diff_y - (actor_y - centre_y) * diff_x
   if var_1E <= 0 then
      var_24 = 0
   else
      var_24 = 1
   end
   
   if var_1E == 0 then
      var_24 = math.random(0, 1)
   end
   
   if diff_y <= 0 then
      var_14 = -1
   else
      var_14 = 1
   end
   
   if diff_x <= 0 then
      var_12 = 1
   else
      var_12 = -1
   end
   
   if var_24 == 0 then
      var_14 = -var_14
      var_12 = -var_12
   end
   
   local tmp_actor
   if actor.in_party == true then
      tmp_actor = Actor.get(1) --FIXME big hack here. --objlist_party_roster --FIXME get player actor here.
   else
      tmp_actor = wt_front_target_actor
   end
   
   print("tmp_actor = "..tmp_actor.name.." at ("..tmp_actor.x..","..tmp_actor.y..")\n")
   var_20 = (tmp_actor.x - centre_x) * diff_x + (tmp_actor.y - centre_y) * diff_y
   if actor.in_party == true then
      var_20 = var_20 + abs(diff_x) + abs(diff_y)
   end
   
   var_1C = diff_x * diff_x + diff_y * diff_y
   if var_1C == 0 then
      var_1C = 1
   end
   
   print("getting target var_20 = "..var_20.." diff_x = "..diff_x.." diff_y = "..diff_y.." var_1C = "..var_1C.."\n")
   target_x = math.floor((var_20 * diff_x) / var_1C) + centre_x
   target_y = math.floor((var_20 * diff_y) / var_1C) + centre_y
   
   unk_30A72 = 0
   
   local chunk_x = player_loc.x - 16
   chunk_x = chunk_x - (chunk_x % 8)
   
   local chunk_y = player_loc.y - 16
   chunk_y = chunk_y - (chunk_y % 8)
   
   local found_actor = false
   repeat
      if target_x < chunk_x or chunk_x + 0x27 < target_x or target_y < chunk_y or chunk_y + 0x27 < target_y or (actor_x == target_x and actor_y == target_y) then
         unk_30A72 = 1
         print("combat_front returned. too far away. actor=("..actor_x..","..actor_y..") target=("..target_x..","..target_y..") chunk=("..chunk_x..","..chunk_y..")\n")
         return 1
      end
   
      if map_get_actor(target_x, target_y, player_loc.z) ~= nil then
         found_actor = true
      else
         found_actor = false
      end
   
      if found_actor then
         var_10 = (target_x + var_14 - centre_x) * diff_x + (target_y - centre_y) * diff_y
         var_E = (target_x - centre_x) * diff_x + (target_y + var_12 - centre_y) * diff_y
   
         if abs(var_10 - var_20) >= abs(var_E - var_20) then
            target_y = target_y + var_12
         else
            target_x = target_x + var_14
         end
      end
   until found_actor == false
   
   
   mpts = actor.mpts
   if actor_move_towards_loc(actor, target_x, target_y) ~= 0 then
      if actor.in_party == false or actor.x == target_x and actor.y == target_y then
         return 0
      end
   
      if wt_num_monsters_near == 0 then
         actor.mpts = mpts
      end
   
      if actor_move_towards_loc(actor, target_x, target_y) ~= 0 then
         return 0
      end
   else
      actor.mpts = mpts
   end
   
   actor_wt_attack(actor)
   
   return 0
end

function actor_wt_rear(actor)
   print("actor_wt_rear()\n")
   local var_C = 0
   local player_loc = player_get_location()
   local var_2,var_4,avg_y,avg_x,di,dx,ax,avg_diff_y
   if actor.in_party == false then
      
      if wt_num_monsters_near == 0 then subtract_movement_pts(actor, 5) return end
      
      var_4 = wt_rear_min_monster
      var_2 = wt_rear_max_monster
      
      if var_4 == nil then var_4 = actor end
      if var_2 == nil then var_2 = actor end
      
      avg_x = combat_avg_x
      avg_y = combat_avg_y
      di = party_avg_x - combat_avg_x
      avg_y_diff = party_avg_y - combat_avg_y
      
   else
      
      if wt_num_monsters_near == 0 then actor_move_towards_loc(actor, player_loc.x, player_loc.y) return end
      
      var_4 = wt_rear_max_party
      var_2 = wt_rear_min_party
      avg_x = party_avg_x
      avg_y = party_avg_y
      di = combat_avg_x - party_avg_x
      avg_y_diff = combat_avg_y - party_avg_y
   end
   
   local var_10 = 0x7fff
   local align = actor.align
   local i, var_12
   for i=0,0x100 do
      local a = Actor.get(i)
      
      if a.alive and a.wt == WT_FRONT and a.align == align then
         
         var_12 = (a.x - avg_x) * di + (a.y - avg_y) * avg_y_diff

         if var_12 < var_10 then var_10 = var_12 end
         
      end
      
   end
   
   var_12 = (actor.x - avg_x) * di + (actor.y - avg_y) * avg_y_diff
   local mpts = actor.mpts
   local var_C 
   if actor.in_party == false or actor_find_max_xy_distance(actor, player_loc.x, player_loc.y) <= 3 then
      
      if var_12 < var_10 then
         
         var_12 = (actor.x - avg_x) * avg_y_diff - (actor.y - avg_y) * di
         
         dx = (var_4.x - avg_x) * avg_y_diff
         if dx - (var_4.y - avg_y) * di >= var_12 then
            
            dx = (var_2.x - avg_x) * avg_y_diff
            ax = (var_2.y - avg_y) * di
            if dx - ax <= var_12 then
               
               var_C = 1
               
            else
               
               ax = (actor_move_towards_loc(actor, actor.x + avg_y_diff, actor.y - di) and -1 or 0) + 1
               var_C = ax
            end
            
         else
            
            ax = (actor_move_towards_loc(actor, actor.x - avg_y_diff, actor.y + di) and -1 or 0) + 1
            var_C = ax
         end
         
      else
         
         ax = (actor_move_towards_loc(actor, actor.x - di, actor.y - avg_y_diff) and -1 or 0) + 1
         var_C = ax
      end
      
   else
      
      ax = (actor_move_towards_loc(actor, player_loc.x, player_loc.y) and -1 or 0) + 1
      var_C = ax
   end
   
   if var_C ~= 0 then
      
      actor.mpts = mpts
      actor_wt_attack(actor)
   end


end

function actor_wt_combat_tanglevine(actor)
   local random = math.random
   local di
   local abs = abs
   
   local target = actor_find_target(actor)
   if target ~= nil then

      local target_x = target.x
      local target_y = target.y
      local actor_x = actor.x
      local actor_y = actor.y
      if abs(target_x - actor_x) < 2 and abs(target_y - actor_y) < 2 and random(0, 1) ~= 0 then
      
         actor_attack(actor, target_x, target_y, target_z, actor)
         subtract_movement_pts(actor, 10)
         return
      end

      if abs(target_x - actor_x) < 5 and abs(target_y - actor_y) < 5 and random(0, 3) == 0 then
      
         target_x = target_x - actor_x
         target_y = target_y - actor_y

         if abs(target_x) <= abs(target_y) then
         
            di = (target_y <= 0) and DIR_NORTH or DIR_SOUTH

            if move_tanglevine(actor, di) == 0  then
            
               di = (target_x <= 0) and DIR_WEST or DIR_EAST
               move_tanglevine(actor, di)
               return
            end
         
         else
         
            di = (target_x <= 0) and DIR_WEST or DIR_EAST

            if move_tanglevine(actor, di) == 0 then
            
               di = (target_y <= 0) and DIR_NORTH or DIR_SOUTH
               move_tanglevine(actor, di)
               return
            end
         end
      end
   end

   if random(0, 3) == 0 then

      di = random(0, 3) --random direction north south east west
      if actor.direction == di then
      
         di = direction_reverse(di)
      end 

      move_tanglevine(actor, di)

   else
      actor.mpts = 0
   end

   return 
end


function actor_wt_combat_stationary(actor)
   local rand = math.random
   local align = actor.align
   if align == ALIGNMENT_NEUTRAL then

      subtract_movement_pts(actor, 5)
      return
   end

   local i
   for i=1,0x10 do

      local target_x = rand(0, 8) + actor.x - 4
      local target_y = rand(0, 8) + actor.y - 4
--      local g_obj = sub_1D351(actor, target_x, target_y)
      local target_actor = map_get_actor(target_x, target_y, actor.z)

      if target_actor ~= nil and actor_ok_to_attack(actor, target_actor) == true and target_actor.alive == true and target_actor.align ~= align and target_actor.align ~= ALIGNMENT_NEUTRAL then
      
         actor_attack(actor, target_x, target_y, actor.z, actor_get_weapon(actor, target_actor))
         subtract_movement_pts(actor, 10)
         return
      end

   end

   subtract_movement_pts(actor, 5)
   
   return
end

function actor_wt_walk_straight(actor)
   if math.random(0, 1) == 0 then subtract_movement_pts(actor, 5) return end
   
   local wt = actor.wt
   local sched = actor.sched_loc
   local dir
   
   if wt < WT_WALK_NORTH_SOUTH or actor.x ~= sched.x or actor.y ~= sched.y then
      dir = actor.direction
   else
      if wt == WT_WALK_NORTH_SOUTH then dir = DIR_NORTH end
      if wt == WT_WALK_EAST_WEST then dir = DIR_EAST end
      if wt == WT_WALK_SOUTH_NORTH then dir = DIR_SOUTH end
      if wt == WT_WALK_WEST_EAST then dir = DIR_WEST end
      actor.direction = dir
   end
   
   local mpts = actor.mpts

   if actor_move(actor, dir, 1) == 0 then

      dir = direction_reverse(dir)

      actor.mpts = mpts
      actor_move(actor, dir, 1)
      actor.direction = dir
   end
   
end

function actor_wt_wander_around(actor)
   local rand = math.random
   if rand(0, 7) ~= 0 then subtract_movement_pts(actor, 5) return end

   local random_wander_range = function ()
      local i = 0 
      while rand(0, 1) ~= 0 do i = i + 1 end
      if rand(0, 1) ~= 0 then i = -i end
      return i
   end
   
   local abs=abs
   local sched = actor.sched_loc
   local sched_x_offset = actor.x - sched.x;
   local sched_y_offset = actor.y - sched.y;
   local direction
   
   if abs(sched_y_offset) - abs(sched_x_offset) >= random_wander_range() then
      direction = (random_wander_range() <= sched_y_offset) and DIR_NORTH or DIR_SOUTH
   else
      direction = (random_wander_range() <= sched_x_offset) and DIR_WEST or DIR_EAST
   end

   actor.direction = direction
   actor_move(actor, direction, 1)

   return
end

function actor_move_towards_player(actor)
local rand = math.random
if actor.wt ~= WT_STATIONARY then
   if actor.in_party == true then
      local player_loc = player_get_location()
      local x,y = player_loc.x, player_loc.y
      if actor.x ~= x and actor.y ~= y and rand(0, 3) == 0 then
         actor_move_towards_loc(actor, x, y)
         return
      end
   end
      
   if rand(0, 7) == 0 then
      actor_move(actor, rand(0, 3), 1)
      return
   end
end
subtract_movement_pts(actor, 5)
end

function actor_wt_attack(actor)
print("actor_wt_attack()\n");

   g_obj = actor_find_target(actor)
   if g_obj ~= nil then print("target at ("..g_obj.x..","..g_obj.y..")\n") end
   
   local weapon_obj = actor_get_weapon(actor, g_obj)
   
   if g_obj ~= nil then

      local target_x = g_obj.x
      local target_y = g_obj.y
      local actor_x = actor.x
      local actor_y = actor.y
      local weapon_range = get_weapon_range(weapon_obj.obj_n)

      if abs(target_x - actor_x) < 8 and abs(target_y - actor_y) < 8 and 
       get_attack_range(actor_x, actor_y, target_x, target_y) <= weapon_range then

         if sub_1D59F(actor, target_x, target_y, weapon_range, 0) == true then
            actor_attack(actor, g_obj.x, g_obj.y, g_obj.z, weapon_obj)
            subtract_movement_pts(actor, 10)
            return
         end

         if random(0, 1) == 0 then
         
            target_x = g_obj.y - actor_y + actor_x
            target_y = actor_y - g_obj.x - actor_x
         
         else
         
            target_x = actor_x - g_obj.y - actor_y
            target_y = g_obj.x - actor_x + actor_y
         end

         actor_move_towards_loc(actor, target_x, target_y)
         return
      end

      if actor.wt ~= 3 then actor_move_towards_loc(actor, target_x, target_y) return end

      subtract_movement_pts(actor, 5)
      return
   end

   if actor.wt ~= 3 then actor_move_towards_player(actor) return end


   subtract_movement_pts(actor, 5)

end

function actor_wt_timid(actor)
	local player_loc = player_get_location()
	if actor_find_max_xy_distance(actor, player_loc.x, player_loc.y) > 7 then
	   actor.mpts = 0
	   return
	end
	local target = actor_find_target(actor)
	if target ~= nil then
	   actor_move_towards_player(actor)
	   return
	end
	local diff_x = target.x - actor.x
	local diff_y = target.y - actor.y
	local var_4
	if actor.wt ~= WT_RETREAT then
	   var_4 = 3
	else
	   var_4 = 8
	end
	
	if abs(diff_x) < var_4 and abs(diff_y) < var_4 then
	   local var_2 = actor.mpts
	   if actor_move_towards_loc(actor, actor.x - diff_x, actor.y - diff_y) == 0 then
	      actor.mpts = var_2
	      if actor.wt ~= WT_MOUSE and actor.wt ~= WT_FLEE then
	         actor_wt_attack(actor)
	      end
	   end
	else
	   if actor.align == ALIGNMENT_GOOD and actor.wt ~= WT_FLEE and actor.wt ~= WT_MOUSE then
	      move_actor_towards_loc(actor, party_avg_x, party_avg_y)
	   else
	      worktype_move_towards_player(actor)
	   end
	end
	
	if actor.wt == WT_RETREAT then
	   if math.random(0, 3) == 0 then
	      if actor.level > actor.hp then
	         actor.hp = actor.hp + 1
	      end
	
	      if math.floor((actor.hp * 4) / actor.level) > 0 then
	         actor.wt = actor.combat_mode
	      end
	   end
	end
end

function actor_wt_like(actor)
	local actor_x = actor.x
	local actor_y = actor.y
	local party_actor
	local random = math.random
	
	for party_actor in party_members() do

	   if abs(party_actor.x - actor_x) < 3 and abs(party_actor.y - actor_y) < 3 then
	      if random(0, 1) == 0 then
	         actor_move_towards_loc(actor, party_avg_x, party_avg_y)
	         return
	      end
	
	      break
	   end
	end
	
	if random(0, 1) == 0 then
	   subtract_movement_pts(actor, 5)
	else
	   if random(0, 1) == 0 then
	      if random(0, 1) == 0 then
	         actor_y = actor_y - 10
	      else
	         actor_y = actor_y + 10
	      end
	   else
	      if random(0, 1) == 0 then
	         actor_x = actor_x - 10
	      else
	         actor_x = actor_x + 10
	      end
	   end
	   actor_move_towards_loc(actor, actor_x, actor_y)
	end
	
	return
end

function actor_wt_unfriendly(actor)
	local actor_x = actor.x
	local actor_y = actor.y
	local party_actor
	local random = math.random
	
	for party_actor in party_members() do
	   if abs(party_actor.x - actor_x) < 3 and abs(party_actor.y - actor_y) < 3 then
	
	      if random(0, 7) == 0 then
	         actor.wt = WT_ASSAULT
	         actor_wt_attack(actor)
	         return 
	      end
	
	      break
	   end
	end
	
	if random(0, 1) == 0 then
	   subtract_movement_pts(actor, 5)
	else
	   if random(0, 1) == 0 then
	      if random(0, 1) == 0 then
	         actor_y = actor_y - 10
	      else
	         actor_y = actor_y + 10
	      end
	   else
	      if random(0, 1) == 0 then
	         actor_x = actor_x - 10
	      else
	         actor_x = actor_x + 10
	      end
	   end
	   actor_move_towards_loc(actor, actor_x, actor_y)
	end
	
	return
end

wt_tbl = {
[WT_NOTHING] = {"WT_NOTHING", perform_worktype},
[WT_FRONT] = {"WT_FRONT", actor_wt_front},
[WT_REAR] = {"WT_REAR", actor_wt_rear},
--WT_FLANK
--WT_BERSERK
[WT_RETREAT] = {"WT_RETREAT", actor_wt_timid},
[WT_ASSAULT] = {"WT_ASSAULT", actor_wt_attack},
[WT_FLEE] = {"WT_FLEE", actor_wt_timid},
[WT_LIKE] = {"WT_LIKE", actor_wt_like},
[WT_UNFRIENDLY] = {"WT_UNFRIENDLY", actor_wt_unfriendly},
[WT_WANDER_NEAR_PLAYER] = {"WT_WANDER_NEAR_PLAYER", actor_move_towards_player},
[WT_TANGLE] = {"WT_TANGLE", actor_wt_combat_tanglevine},
[WT_STATIONARY] = {"WT_STATIONARY", actor_wt_combat_stationary},
[WT_GUARD_WALK_EAST_WEST] = {"WT_GUARD_WALK_EAST_WEST", actor_wt_walk_straight},
[WT_GUARD_WALK_NORTH_SOUTH] = {"WT_GUARD_WALK_NORTH_SOUTH", actor_wt_walk_straight},
--11
--12
[WT_UNK_13] = {"WT_UNK_13", actor_wt_timid},
[WT_WALK_NORTH_SOUTH] = {"WT_WALK_NORTH_SOUTH", actor_wt_walk_straight},
[WT_WALK_EAST_WEST]   = {"WT_WALK_EAST_WEST", actor_wt_walk_straight},
[WT_WALK_SOUTH_NORTH] = {"WT_WALK_SOUTH_NORTH", actor_wt_walk_straight},
[WT_WALK_WEST_EAST]   = {"WT_WALK_WEST_EAST", actor_wt_walk_straight},
[WT_WANDER_AROUND] = {"WT_WANDER_AROUND", actor_move_towards_player},
[WT_MOUSE] = {"WT_MOUSE", actor_wt_timid}

--[WT_] = {"WT_", actor_wt_rear}
}

function actor_ok_to_attack(actor, target_actor)

   if target_actor.visible == false and (target_actor.in_party == false or actor.in_party == false) then return false end
   
   if target_actor.z ~= actor.z then return false end
   
   if target_actor.obj_n == 0x165 and target_actor.frame_n == 0 then return false end
   
   --need to check if target_actor's corpser bit is set eg dragged under
   --need to check tileflag3 bit 4 is not set. The Ignore flag.
   return true

end

function actor_find_target(actor)

   local target_actor = nil
   local align = actor.align
   local actor_x = actor.x
   local actor_y = actor.y
   local var_2 = 0x7fff
   local i
   
   for i=1,0x100 do

      local tmp_actor = Actor.get(i)

      if tmp_actor.obj_n ~= 0 and tmp_actor.alive == true and tmp_actor ~= actor and actor_ok_to_attack(actor, tmp_actor) == true then
      
         if actor.wt == WT_FLEE or
            actor.wt == WT_MOUSE or 
            actor.wt == WT_UNK_13 or
            actor.wt == WT_RETREAT or
            actor.wt == WT_FIGHTING or 
            (align ~= ALIGNMENT_NEUTRAL or actor.wt == WT_ATTACK_PARTY and tmp_actor.align == ALIGNMENT_GOOD) and
            (align ~= ALIGNMENT_CHAOTIC or tmp_actor.align ~= ALIGNMENT_CHAOTIC) and
            tmp_actor.align ~= ALIGNMENT_NEUTRAL and
            (align ~= ALIGNMENT_GOOD or alignment_is_evil(tmp_actor.align) == true) and
            (align ~= ALIGNMENT_EVIL or tmp_actor.align == ALIGNMENT_GOOD or tmp_actor.align == ALIGNMENT_CHAOTIC) then
         
            local target_x = tmp_actor.x
            local target_y = tmp_actor.y
            if actor_find_max_xy_distance(actor, target_x, target_y) <= 8 and (tmp_actor.wt ~= 7 or abs(target_x - g_player_x) <= 5 and abs(target_y - g_player_y) <= 5) then
            
               local var_6 = (target_x - actor_x)^2 + (target_y - actor_y)^2 
               if var_6 < var_2 or var_6 == var_2 and math.random(0, 1) ~= 0 then
               
                  var_2 = var_6
                  target_actor = tmp_actor
               end
            end
         end
      end

   end

   return target_actor
end

--FIXME this is a line of sight check for combat.
function sub_1D59F(actor, target_x, target_y, weapon_obj)
   return true
end



function perform_worktype(actor)
   
   if wt_tbl[actor.wt] == nil then
      subtract_movement_pts(actor, 5)
      return
   end
   
   print("wt = "..wt_tbl[actor.wt][1].."\n")
   
   local func = wt_tbl[actor.wt][2]
   func(actor)
   
   if actor.mpts == 0 then subtract_movement_pts(actor, 0xa) end
   
end

function spell_put_actor_to_sleep(attacker, foe)

   print("spell_put_actor_to_sleep("..attacker.name..",foe)\n")
   
   local actor_base = actor_tbl[foe.obj_n]
   if actor_base == nil or actor_base[21] == 0 then -- 21 is immune to sleep
      if actor_int_check(attacker, foe) == false then
         print("FIXME: put actor to sleep.\n")
         return 0xfe
      else
         return 1
      end
   end
   
   return 2
end

io.stderr:write("actor.lua loaded\n")
