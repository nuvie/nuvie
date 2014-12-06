io.stderr:write("actor.lua get here\n")

--Worktypes      
WT_NOTHING                = 0x0  --do nothing

WT_FOLLOW                 = 0x1  --follow avatar (in party)

WT_PLAYER                 = 0x2  --player mode

WT_SLEEP                  = 0x91

--Actor stats table
--[obj_num] = {str,dex,int,hp,alignment,,damage??,,,,,,,,,,,,,,,,}
actor_tbl = {
--OBJ_YOURSELF5
[343] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_YOURSELF6
[344] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_POOR_MONK
[342] = {22,22,22,60,ALIGNMENT_EVIL,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_SCIENTIST
[345] = {15,15,25,30,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_YOUNG_WOMAN
[346] = {18,18,22,60,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_COWBOY
[347] = {20,25,18,60,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_GENTLEMAN
[348] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_UNIFORMED_MAN
[349] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_ADVENTURER
[350] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_COMMON_FELLOW
[351] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_MAN
[352] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_MAN_IN_WHITE
[353] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_LADY
[354] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_WOMAN
[355] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_ADVENTURESS
[356] = {25,25,25,90,ALIGNMENT_GOOD,0,6,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_METAL_WOMAN1
[357] = {25,25,25,90,ALIGNMENT_GOOD,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_MECHANICAL_MAN
[358] = {25,15,10,40,ALIGNMENT_GOOD,4,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_MARTIAN
[369] = {15,15,20,30,ALIGNMENT_NEUTRAL,0,6,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_SEXTELLEGER
[359] = {20,20,10,45,ALIGNMENT_CHAOTIC,4,35,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_BUSHALO
[360] = {25,12,11,35,ALIGNMENT_NEUTRAL,8,20,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
--OBJ_PLANTELLOPE
[361] = {15,22,9,20,ALIGNMENT_NEUTRAL,1,12,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_ROCKWORM2
[372] = {20,15,10,40,ALIGNMENT_CHAOTIC,8,25,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_WISP
[377] = {20,20,20,80,ALIGNMENT_NEUTRAL,3,30,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
--OBJ_AIRSQUID
[378] = {10,20,15,10,ALIGNMENT_EVIL,0,6,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0},
--OBJ_AIRSQUID1
[380] = {15,15,8,30,ALIGNMENT_EVIL,4,20,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_GIANT_MAW
[373] = {30,10,9,100,ALIGNMENT_CHAOTIC,3,30,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_TENTACLE1
[362] = {20,20,9,10,ALIGNMENT_CHAOTIC,2,20,1,1,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
--OBJ_CREEPING_CACTUS
[383] = {20,12,3,30,ALIGNMENT_CHAOTIC,4,25,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_AMMONOID
[363] = {15,15,3,20,ALIGNMENT_CHAOTIC,10,12,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_PROTO_MARTIAN
[364] = {20,19,10,20,ALIGNMENT_EVIL,2,10,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
--OBJ_MONSTER_FOOTPRINTS
[145] = {20,19,10,20,ALIGNMENT_EVIL,2,12,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0},
--OBJ_POD_DEVIL
[384] = {15,18,3,25,ALIGNMENT_CHAOTIC,4,12,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_JUMPING_BEAN
[385] = {10,22,10,10,ALIGNMENT_CHAOTIC,1,4,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_HEDGEHOG
[365] = {17,12,8,20,ALIGNMENT_CHAOTIC,3,20,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_CREEPER
[374] = {15,15,3,20,ALIGNMENT_CHAOTIC,1,16,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_AGROBOT
[376] = {29,18,10,30,ALIGNMENT_CHAOTIC,10,20,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
--OBJ_CANAL_WORM
[379] = {20,20,15,30,ALIGNMENT_CHAOTIC,5,25,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0,0},
--OBJ_DUST_DEVIL
[381] = {30,30,30,255,ALIGNMENT_NEUTRAL,99,60,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
--OBJ_SAND_TRAPPER
[386] = {25,25,10,40,ALIGNMENT_CHAOTIC,4,18,1,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0},
--OBJ_OXY_LEECH
[375] = {10,25,12,20,ALIGNMENT_CHAOTIC,1,10,0,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_CAVE_WORM
[366] = {15,15,15,30,ALIGNMENT_EVIL,2,15,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_JANITOR
[367] = {20,15,10,30,ALIGNMENT_CHAOTIC,10,20,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
--OBJ_BUSHRAT
[387] = {10,25,10,20,ALIGNMENT_EVIL,2,12,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_PLANTHER
[368] = {15,22,12,20,ALIGNMENT_CHAOTIC,4,20,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_MINOTAUR
[398] = {20,18,8,90,ALIGNMENT_NEUTRAL,2,15,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0},
--OBJ_YOUR_MOTHER
[391] = {12,20,15,255,ALIGNMENT_CHAOTIC,9,1,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0},
--OBJ_TREE
[408] = {25,18,5,90,ALIGNMENT_NEUTRAL,3,12,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_SOMETHING_YOU_SHOULDNT2
[424] = {25,18,5,90,ALIGNMENT_NEUTRAL,3,12,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
--OBJ_HUGE_RED_SPOT
[403] = {20,20,10,255,ALIGNMENT_EVIL,99,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}

g_party_is_warm = false

-- Berry logic
function actor_is_affected_by_purple_berries(actor_num)

   if actor_num < 16 and timer_get(actor_num*3) > 0 then
      return true
   end
   
   return false
end

function actor_is_affected_by_green_berries(actor_num)
 
   if actor_num < 16 and timer_get(actor_num*3+1) > 0 then
      return true
   end
   
   return false
end

function actor_is_affected_by_brown_berries(actor_num)
 
   if actor_num < 16 and timer_get(actor_num*3+2) > 0 then
      return true
   end
   
   return false
end

function actor_get_purple_berry_count(actor_num)
   if actor_num < 16 then
      return timer_get(actor_num*3)
   end
   
   return 0
end

function actor_get_green_berry_count(actor_num)
   if actor_num < 16 then
      return timer_get(actor_num*3+1)
   end
   
   return 0
end

function actor_get_brown_berry_count(actor_num)
   if actor_num < 16 then
      return timer_get(actor_num*3+2)
   end
   
   return 0
end

function actor_decrement_berry_counter(actor, berry_type)
   local actor_num = actor.actor_num
   local count = timer_get(actor_num*3+berry_type)
   if count > 0 and math.random(1, actor_int_adj(actor) * 4) == 1 then
      timer_set(actor_num*3+berry_type, count - 1)
      if (actor_num == 0 and g_in_dream_mode) or 
         (actor_num ~= 0 and g_in_dream_mode == false) then
         play_md_sfx(0x32)
         printl("A_PSYCHIC_POWER_FADES")
      end
   end   
end

function actor_decrement_berry_counters(actor)
   actor_decrement_berry_counter(actor, 0) --purple
   actor_decrement_berry_counter(actor, 1) --green
   actor_decrement_berry_counter(actor, 2) --brown
end

function actor_increment_berry_counter(actor_num, berry_type)
   local count = timer_get(actor_num*3+berry_type)
   if actor_num < 16 and count < 10 then
      timer_set(actor_num*3+berry_type, count + 1)
   end
end

function actor_increment_purple_berry_count(actor_num)
   actor_increment_berry_counter(actor_num, 0)
end

function actor_increment_green_berry_count(actor_num)
   actor_increment_berry_counter(actor_num, 1)
end

function actor_increment_brown_berry_count(actor_num)
   actor_increment_berry_counter(actor_num, 2)
end

function actor_get_blue_berry_counter()
   return timer_get(16*3)
end

function actor_set_blue_berry_counter(new_value)
   timer_set(16*3, new_value)
end

--
-- actor_init(actor)
--

function actor_init(actor, alignment)

 local actor_base = actor_tbl[actor.obj_n]
 if actor_base ~= nil then
   actor.str = actor_randomise_stat(actor_base[1])
   actor.dex = actor_randomise_stat(actor_base[2])
   actor.int = actor_randomise_stat(actor_base[3])
   actor.hp = actor_randomise_stat(actor_base[4])

   actor.level = 0
   actor.align = actor_base[5]
 else
   actor.str = 15
   actor.dex = 15
   actor.int = 15
   actor.hp = 30
   actor.level = 1
   actor.align = ALIGNMENT_CHAOTIC
 end
 
 if alignment ~= nil and alignment ~= ALIGNMENT_DEFAULT then
   actor.align = alignment
 end
   
 actor.wt = 8
 actor.combat_mode = 8
 actor.mpts = actor.dex
 actor.exp = 0
   
end

function actor_map_dmg(actor, map_x, map_y, map_z)
	--FIXME
end

function perform_worktype(actor)
   print("wt="..actor.wt.."\n")
   subtract_movement_pts(actor, 10)
end

function actor_update_all()
   --update leader here.
   --pathfinding here.
   
   local actor
   local selected_actor
   repeat
      selected_actor = nil
      local di = 0
      local dex_6 = 1
      repeat
         local player_loc = player_get_location()
         local var_C = (player_loc.x - 16) - (player_loc.x - 16) % 8
         local var_A = (player_loc.y - 16) - (player_loc.y - 16) % 8
         
         for actor in party_members() do
            if actor.wt == WT_FOLLOW and actor.mpts < 0 then
               actor.mpts = 0
            end
         end
         
         local player_z = player_loc.z
         for i=0,0xff do
            local actor = Actor.get(i)
            --FIXME need to check 11000b not zero on obj_flags
            if actor.obj_n ~= 0 and actor.z == player_z and actor.mpts > 0 and actor.paralyzed == false and (actor.asleep == false or actor.wt == 0x80) and actor.wt ~= WT_NOTHING and actor.alive == true then
                  --FIXME need to check map wrapping here.
                  if abs(actor.x - var_C) > 0x27 or abs(actor.y - var_A) > 0x27 then
                     if actor.wt >= 0x83 and actor.wt <= 0x86 then
                        --move actor to schedule location if it isn't on screen
                        local sched_loc = actor.sched_loc
                        if map_is_on_screen(sched_loc.x, sched_loc.y, sched_loc.z) == false then
                           Actor.move(actor, sched_loc.x, sched_loc.y, sched_loc.z)
                           actor_wt_walk_to_location(actor) --this will cancel the pathfinder and set the new worktype
                           subtract_movement_pts(actor, 10)
                           ----dgb("\nActor SCHEDULE TELEPORT "..actor.actor_num.." to ("..sched_loc.x..","..sched_loc.y..","..sched_loc.z..")\n")
                        end
                     end
                  else
                     if actor.wt ~= WT_FOLLOW then
                        if actor.wt == 0x80 then
                           -- actor_set_worktype_from_schedule(actor)
                           actor.wt = actor.sched_wt
                        end

                        local dex_adjusted = actor_dex_adj(actor) 
                        local dx = (actor.mpts * dex_6) - dex_adjusted * di
                        if actor.mpts >= dex_adjusted or dx > 0 or dx == 0 and dex_adjusted > dex_6 then
                           selected_actor = actor
                           di = actor.mpts
                           dex_6 = dex_adjusted
                        end
                        
                        if dex_adjusted <= actor.mpts then
                           break
                        end
                     end
                  end
            end
         end
         
         if di <= 0 then
            for i=0,0xff do
               local actor = Actor.get(i)
               local dex_adjusted = actor_dex_adj(actor)
               if actor.mpts >= 0 then
                  actor.mpts = dex_adjusted
               else
                  actor.mpts = actor.mpts + dex_adjusted
               end
            end
            advance_time(1)
         end
         
      until di > 0

      if selected_actor.wt ~= WT_PLAYER and selected_actor.wt ~= WT_FOLLOW then
         --print("perform_worktype("..selected_actor.name.."("..selected_actor.actor_num..") dex = "..selected_actor.dex.." mpts = "..selected_actor.mpts..").\n")
         perform_worktype(selected_actor)
         --FIXME update party leader
         if selected_actor.wt > 1 and selected_actor.wt < 0x1b then
            --FIXME targetting?? do *(&bjlist_unk_19f1_ptr + actor_num) = actor_num
         end
      end

   until selected_actor.obj_n ~= 0 and selected_actor.wt == WT_PLAYER    

   if selected_actor ~= nil then --swap player to next party member with 'command' combat worktype.
    local old_player = Actor.get_player_actor()
    player_set_actor(selected_actor)
    old_player.wt = WT_PLAYER --reset worktype to player as it gets changed to follow in Player::set_actor() :-(
   end
 
   
   display_prompt(true)
end

-- [objectnum] = range
   g_range_weapon_tbl = {
      [40] = 6,  --Cupid's bow and arrows
      [41] = 4, --derringer
      [42] = 5, --revolver
      [43] = 4, --shotgun
      [44] = 7, --rifle (whole screen)
      [45] = 7, --combine (whole screen)
      [46] = 7, --elephant gun (whole screen)
      [47] = 4, -- sling
      [48] = 5, -- bow
      [129] = 2, --weed sprayer
      [240] = 6, --heat ray gun
      [241] = 6, --freeze ray gun
      [261] = 2, --spray gun
      [313] = 7, --M60 machine gun (not sure, I think this is a scripted event?)
   }

local projectile_weapon_tbl = --FIXME weed sprayer and spray gun
{
--obj_n = {tile_num, initial_tile_rotation, speed, rotation_amount} --FIXME: all untested
[40] = {575, 90,4, 0}, -- Cupid's bow and arrows
[41] = {259, 90,4, 0}, -- derringer --FIXME: tile_num, rot, speed, amount
[42] = {259, 90,4, 0}, -- revolver --FIXME: rot, speed, amount
[43] = {262, 90,4, 0}, -- shotgun --FIXME: rot, speed, amount
[44] = {259, 90,4, 0}, -- rifle --FIXME: rot, speed, amount
[45] = {259, 90,4, 0}, -- Belgian combine rifle --FIXME: rot, speed, amount (has 3 modes)
--[45] = {262, 90,4, 0}, -- Belgian combine spread(has two) --FIXME: rot, speed, amount
[46] = {259, 90,4, 0}, -- elephant gun --FIXME: tile_num, rot, speed, amount
[47] = {398, 0, 2, 0}, -- sling --FIXME: tile_num, rot, speed, amount
[48] = {575, 90,4, 0}, -- bow
[241] = {334, 90,4, 0}, -- heat ray gun --FIXME: rot, speed, amount (has 3 modes)
[242] = {335, 90,4, 0}, -- freeze ray gun --FIXME: rot, speed, amount (has 3 modes)
[313] = {267, 90,4, 0}, -- M60 machine gun --FIXME: rot, speed, amount (if even used)

}

weapon_dmg_tbl = { --FIXME: all damage is made up
[16] = 2, --bloody saber
[40] = 2, --Cupid's bow and arrows (charms)
[41] = 2, --derringer
[42] = 2, --revolver
[43] = 2, --shotgun
[44] = 2, --rifle
[45] = 2, --Belgian combine
[46] = 2, --elephant gun
[47] = 2, --sling
[48] = 2, --bow
[49] = 2, --hatchet
[50] = 2, --axe
[51] = 2, --ball-peen hammer
[52] = 2, --sledge hammer
[54] = 2, --knife
[55] = 2, --machete
[56] = 2, --saber
[65] = 2, --pick
[66] = 2, --shovel
[67] = 2, --hoe
[68] = 2, --rake
[69] = 2, --pitchfork
[70] = 2, --cultivator
[71] = 2, --scythe
[72] = 2, --saw
[102] = 2, --pry bar
--[109] = 1, --torch
--[110] = 1, --lit torch
--[111] = 1, --candlestick
--[112] = 1, --lit candle
--[113] = 1, --candelabra
--[114] = 1, --lit andelabra
--[115] = 1, --oil lamp
--[116] = 1, --lit oil lamp
--[117] = 1, --lantern
--[118] = 1, --lit lantern
[129] = 1, --weed sprayer -- FIXME no damage normally not sure what it does
--[136] = 1, --tongs
[241] = 2, --heat ray gun
[242] = 2, --freeze ray gun
--[243] = 1, --martian ritual pod knife
[261] = 1, --spray gun -- FIXME no damage normally not sure what it does
[263] = 2, --martian hoe (couldn't be equipped in original)
[264] = 2, --martian scythe (couldn't be equipped in original)
[265] = 2, --martian pitchfork (couldn't be equipped in original)
[266] = 2, --martian rake (couldn't be equipped in original)
[267] = 2, --martian shovel (couldn't be equipped in original)
[313] = 2, --M60 machine gun (scripted to only attack and kill the big bad)
[327] = 8, --martian pick (couldn't be equipped in original)
[333] = 2, --cannon
[401] = 2, --pool cue
}

armour_tbl = --FIXME: all armor value is made up
{
[1] = 1, --cap
[2] = 1, --cowboy hat
[3] = 1, --pith helmet
[4] = 1, --military helmet
[5] = 1, --derby
[6] = 1, --kerchief
[7] = 1, --silk scarf
[8] = 1, --muffler
[9] = 1, --man's shoes
[10] = 1, --woman's shoes
[11] = 1, --riding boots
[12] = 1, --ruby slippers
[13] = 1, --thigh boots
[14] = 1, --hip boots
[15] = 1, --winged shoes
[17] = 1, --cloth jacket
[18] = 1, --wool sweater
[19] = 1, --cape
[20] = 1, --duster
[21] = 1, --wool overcoat
[22] = 1, --sheepskin jacket
[23] = 1, --arctic parka
[25] = 1, --cotton pants
[26] = 1, --cotton dress
[27] = 1, --denim jeans
[28] = 1, --wool pants
[29] = 1, --wool dress
[30] = 1, --chaps and jeans
[31] = 1, --man's shoes
[33] = 1, --lady's silk gloves
[34] = 1, --driving gloves
[35] = 1, --cotton work gloves
[36] = 1, --work gloves
[37] = 1, --wool mittens
[38] = 1, --rubber gloves
[39] = 1, --welding gloves
[90] = 1, --electric belt?
[234] = 1, --martian jewelry
}

function out_of_ammo(attacker, weapon, print_message) -- untest function

	local weapon_obj_n = weapon.obj_n

	if ((weapon_obj_n == 41 or weapon_obj_n == 42) and Actor.inv_has_obj_n(attacker, 57) == false) --derringer, revolver, pistol rounds
	    or (weapon_obj_n == 43 and Actor.inv_has_obj_n(attacker, 58) == false) --shotgun, shotgun shell
	    or (weapon_obj_n == 44 and Actor.inv_has_obj_n(attacker, 59) == false) --rifle, rifle round
	    or (weapon_obj_n == 45 and weapon.quality == 0 and (Actor.inv_has_obj_n(attacker, 58) == false or Actor.inv_has_obj_n(attacker, 59) == false)) --belgian combine (combine), shotgun shell, rifle round
	    or (weapon_obj_n == 45 and weapon.quality == 1 and Actor.inv_has_obj_n(attacker, 59) == false) --belgian combine (rifle), rifle round
	    or (weapon_obj_n == 45 and weapon.quality == 2 and Actor.inv_has_obj_n(attacker, 58) == false) --belgian combine (shotgun), shotgun shell
	    or (weapon_obj_n == 46 and Actor.inv_has_obj_n(attacker, 60) == false) --elephant gun, elephant gun round
	    or (weapon_obj_n == 47 and Actor.inv_has_obj_n(attacker, 63) == false) --sling, sling stone
	    or ((weapon_obj_n == 240 or weapon_obj_n == 241 or weapon_obj_n == 129 or weapon_obj_n == 261) and obj.qty == 0) then --heat ray gun, freeze ray gun, weed sprayer, spray gun
		if(print_message) then
			print("Out of ammunition!\n")
			--FIXME add sfx here
		end
		return true
	end

	if weapon_obj_n == 48 and Actor.inv_has_obj_n(attacker, 64) == false then --bow, arrows
		if(print_message) then
			print("Out of arrows!\n")
			--FIXME add sfx here
		end
		return true
	end

	return false
end

local clothing_warmth_tbl = {
[1] =    1, -- OBJ_CAP
[2] =    2, -- OBJ_COWBOY_HAT
[3] =    3, -- OBJ_PITH_HELMET
[4] =    3, -- OBJ_MILITARY_HELMET
[5] =    2, -- OBJ_DERBY
[6] =    1, -- OBJ_KERCHIEF
[7] =    1, -- OBJ_SILK_SCARF
[8] =    2, -- OBJ_MUFFLER
[9] =    1, -- OBJ_MANS_SHOES
[10] =   1, -- OBJ_WOMANS_SHOES
[11] =   2, -- OBJ_RIDING_BOOTS
[14] =   4, -- OBJ_HIP_BOOTS
[13] =   3, -- OBJ_THIGH_BOOTS
[17] =   2, -- OBJ_CLOTH_JACKET
[18] =   3, -- OBJ_WOOL_SWEATER
[19] =   3, -- OBJ_CAPE
[20] =   4, -- OBJ_DUSTER
[21] =   4, -- OBJ_WOOL_OVERCOAT
[22] =   3, -- OBJ_SHEEPSKIN_JACKET
[23] =   5, -- OBJ_ARCTIC_PARKA
[90] =  12, -- OBJ_ELECTRIC_BELT
[25] =   2, -- OBJ_COTTON_PANTS
[26] =   2, -- OBJ_COTTON_DRESS
[27] =   2, -- OBJ_DENIM_JEANS
[28] =   3, -- OBJ_WOOL_PANTS
[29] =   3, -- OBJ_WOOL_DRESS
[30] =   3, -- OBJ_CHAPS_AND_JEANS
[33] =   1, -- OBJ_LADYS_SILK_GLOVES
[34] =   1, -- OBJ_DRIVING_GLOVES
[35] =   1, -- OBJ_COTTON_WORK_GLOVES
[36] =   2, -- OBJ_WORK_GLOVES
[37] =   3, -- OBJ_WOOL_MITTENS
[38] =   1, -- OBJ_RUBBER_GLOVES
[39] =   3, -- OBJ_WELDING_GLOVES
}

function clothing_get_warmth_rating(obj)
   local rating = clothing_warmth_tbl[obj.obj_n]
   if rating == nil then
      rating = 0
   end
   
   return rating
end

local lit_lightsource_tbl = {
[110] =  1, -- OBJ_LIT_TORCH
[112] =  1, -- OBJ_LIT_CANDLE
[114] =  1, -- OBJ_LIT_CANDELABRA
[116] =  1, -- OBJ_LIT_OIL_LAMP
[118] =  1, -- OBJ_LIT_LANTERN
}

function is_lit_lightsource(obj)
   if lit_lightsource_tbl[obj.obj_n] ~= nil then
      return true
   end
   
   return false
end

function actor_str_adj(actor)
   local actor_num = actor.actor_num
   local str = actor.str
   
   if actor.hypoxia then
      str = str - 3
   end
   
   if actor_is_affected_by_purple_berries(actor_num) then
      str = str - 3
   end
   
   if actor.frenzy then
      str = str + 3
      if str > 30 then
         str = 30
      end
   end
   
   if str <= 3 then
      return 1
   end

   return str
end

function actor_dex_adj(actor)

   local dex = actor.dex
   if actor.hypoxia then
      if dex <= 3 then
         dex = 1
      else
         dex = dex - 3
      end
   end

   if actor.frenzy then
      dex = dex + 3
      if dex >= 30 then
         dex = 30
      end
   end
   
   if actor.asleep then
      dex = 1
   end
   
   return dex
end

function actor_int_adj(actor)
   local int = actor.int
   
   if actor.hypoxia == true or (actor.frenzy and actor.actor_num ~= 1) then
      int = int - 3
   end
   
   if int < 1 then int = 1 end
   
   return int
end


function actor_map_dmg(actor, map_x, map_y, map_z) --FIXME
   local obj_n = actor.obj_n
   local actor_type = actor_tbl[obj_n]
   
   if actor.alive == false or actor.hit_flag == true then
      return
   end
end

function actor_remove_charm(actor)

   actor.charmed = false;
   actor.align = actor.old_align
   
   if actor.in_party then
      actor.align = ALIGNMENT_GOOD
   end
   
   if party_is_in_combat_mode() then
      actor.wt = actor.combat_mode
   else
      if player_is_in_solo_mode() then
         actor.wt = WT_NOTHING
      else
         actor.wt = WT_FOLLOW
      end
   end

   party_update_leader()

end

function advance_time(num_turns)
   --FIXME
   local rand = math.random
   local hour = clock_get_hour()
   
   local quake = Actor.get_talk_flag(0x46, 3) --rasputin

   if quake then
      if rand(0, 4) == 0 then
         quake_start(1, 200)
      end
   end
   
   local max_light = 0
   
   local actor_num
   for actor_num=0,0xff do
      local actor = Actor.get(actor_num)
      if actor.alive then
         if g_in_dream_mode == false and actor.in_party and actor_num ~= 0 then
            if num_turns > 0 and actor.asleep == false then --FIXME I think we also need to check distance < 0x27 from either mapwindow or player.
               if actor_num == 6 or Actor.get_talk_flag(0x10, 5) == false or Actor.inv_has_obj_n(actor, 131) then --OBJ_BLOB_OF_OXIUM
                  if actor.hypoxia then
                     actor.hypoxia = false
                     printfl("BREATHES_EASIER", actor.name)
                  end
               else
                  if actor.hypoxia == false then
                     actor.hypoxia = true
                     printfl("GASPS_FOR_AIR", actor.name)
                  end
               end
  
               local warmth_rating = 0
               local obj
               for obj in actor_inventory(actor) do
                  if obj.readied then
                     warmth_rating = warmth_rating + clothing_get_warmth_rating(obj)
                     
                     if is_lit_lightsource(obj) then
                        if rand(0, 1) == 1 then
                           if obj.quality <= num_turns then
                              if obj.obj_num == 116 --OBJ_LIT_OIL_LAMP
                                 or obj.obj_num == 118 then --OBJ_LIT_LANTERN
                                 if obj.obj_num == 116 then --OBJ_LIT_OIL_LAMP
                                    obj.obj_n = 115 --OBJ_OIL_LAMP
                                 else --OBJ_LIT_LANTERN
                                    obj.obj_n = 117--OBJ_LANTERN
                                 end
                                 obj.quality = 0
                                 printfl("THE_IS_OUT_OF_FUEL", obj.name)
                              else
                                 printfl("WENT_OUT", obj.look_string)
                                 Obj.removeFromEngine(obj)
                              end
                           else
                              obj.quality = obj.quality - num_turns
                           end
                        end
                     --FIXME update max_light here. probably not needed as light is updated elsewhere in nuvie.
                     --if max_light < obj.light then
                     --max_light = obj.light
                     --end
                     end
                  end
               end
               
               if g_party_is_warm or actor.z ~= 0 then
                  if actor.cold then
                     actor.cold = false
                     printfl("FEELS_WARMER", actor.name)
                  end
               else
                  local cold_status = 0
                  
                  if hour <= 3 or hour >= 22 then
                     if warmth_rating >= 10 then
                        cold_status = 1
                     else
                        cold_status = 2
                     end
                  elseif hour <= 6 or hour >= 18 then
                     if warmth_rating < 10 then
                        cold_status = 1
                     end
                  end
                  
                  if actor_num == 6 then
                     cold_status = 0
                  end
                  
                  if cold_status == 0 then
                     if actor.cold then
                        actor.cold = false
                        printfl("FEELS_WARMER", actor.name)
                     end
                  else
                     if actor.cold == false then
                        actor.cold = true
                        printfl("IS_FREEZING", actor.name)
                     end
                     if num_turns ~= 0 then
                        for i=1,num_turns do
                           if rand(0, 1) == 0 then
                              if cold_status == 2 then
                                 printfl("IS_FREEZING", actor.name)
                                 Actor.hit(actor, rand(1, 2))                              
                              end
                           else
                              printfl("IS_FREEZING", actor.name)
                              Actor.hit(actor, rand(1, 2))
                              if cold_status == 2 then
                                 Actor.hit(actor, rand(1, 2))
                              end
                           end
                        end
                     end
                  end
               end

            end               
         end
                  
         if num_turns ~= 0 then
            for i=1,num_turns do
               --FIXME what does  word_4E6FA do?

               --remove battle frenzy from actor when the party exits combat mode
               if actor.frenzy and not party_is_in_combat_mode() then
                  actor.frenzy = false
               end
               
               if actor.poisoned then
                  if rand(0, 25) == 0 then
                     actor.poisoned = false
                  end
               end

               if actor.charmed and rand(0, 0x19) == 0 then
                  actor_remove_charm(actor)
               end
               
               if actor.paralyzed then
                  if actor_num == 6 or (rand(0, 3) == 0 and actor.str >= rand(1, 0x1e)) then --FIXME used adjusted str
                     actor.paralyzed = false
                  end
               end
 
               if actor.asleep and actor.wt ~= WT_SLEEP and (not g_party_is_warm or not actor.in_party) then
                  --FIXME check sub_2B0EC(actor.x,actor.y,actor.z)
                  if rand(0,0x14) == 0 then
                     actor.asleep = false
                     --FIXME bit 3 set on 1af1 flags
                  end
               end
               
               if actor.poisoned and actor_num ~= 6 and rand(0, 7) == 0 then
                  Actor.hit(actor, 1)
               end
               
               if actor_num < 8 then
                  actor_decrement_berry_counters(actor)
                  
                  for obj in actor_inventory(actor, true) do
                     local obj_n = obj.obj_n
                     if obj_n == 160 and obj.frame_n > 1 then --OBJ_EMPTY_BUCKET with ice
                        if rand(0, 100) < 10 then
                           printl("SOME_ICE_HAS_MELTED")
                           obj.frame_n = 1
                        end
                     elseif obj_n == 256 then --OBJ_CHUNK_OF_ICE
                        if rand(0, 100) < 10 then
                           printl("SOME_ICE_HAS_MELTED")
                           Obj.removeFromEngine(obj)
                        end
                     elseif obj_n == 448 or (obj_n == 449 and actor_num ~= 6 and actor.poisoned == false) then --OBJ_BLOCK_OF_RADIUM, OBJ_CHIP_OF_RADIUM
                        if obj.in_container == false or obj.parent.obj_n ~= 139 then --OBJ_LEAD_BOX
                           printfl("IS_POISONED", actor.name)
                           actor.poisoned = true
                        end
                     end
                  end                       
               end
               
            end
         end

         actor_map_dmg(actor, actor.x, actor.y, actor.z)
         actor.hit_flag = false
      end
   end
   
   local minute = clock_get_minute()
   
   clock_inc(num_turns)
   	
   if minute + num_turns >= 60 then
      
      update_watch_tile()
      
      update_actor_schedules()
      if g_hours_till_next_healing > 0 then
         g_hours_till_next_healing = g_hours_till_next_healing - 1
      end
      
      update_lamp_posts()
      
      local blue_berry_counter = actor_get_blue_berry_counter()
      if blue_berry_counter > 0 then
         actor_get_blue_berry_counter(blue_berry_counter - 1)
      end
      
      if not g_party_is_warm and not g_in_dream_mode and Actor.get_talk_flag(0x10, 5) then
         for actor in party_members() do
            if actor.actor_num ~= 6 and not actor.asleep then
               local oxium = Actor.inv_get_obj_n(actor, 131) --OBJ_BLOB_OF_OXIUM
               if oxium ~= nil then
                  if oxium.qty > 1 then
                     oxium.qty = oxium.qty - 1
                     if actor.hypoxia then
                        actor.hypoxia = false
                        printfl("BREATHES_EASIER", actor.name)
                     end
                  else
                     Obj.removeFromEngine(oxium)
                     if Actor.inv_get_obj_n(131) ~= nil then
                        if actor.hypoxia then
                           actor.hypoxia = false
                           printfl("BREATHES_EASIER", actor.name)
                        end
                     else
                        if actor.hypoxia == false then
                           actor.hypoxia = true
                           printfl("GASPS_FOR_AIR", actor.name)
                        end
                     end
                  end
               else
                  if actor.hypoxia == false then
                     actor.hypoxia = true
                     printfl("GASPS_FOR_AIR", actor.name)                     
                  end
               end
            end
         end
      end
      
   end
end

function can_get_obj_override(obj)
   return false
end

function subtract_movement_pts(actor, points)
   if actor.actor_num < 16 then
      if party_is_in_combat_mode() == false then
         points = points - 2
      end
   end

   if points < 1 then
      points = 1
   end
   
   actor.mpts = actor.mpts - points
end

function actor_radiation_check(actor, obj)
   if obj.obj_n == 448 or obj.obj_n == 449 then --OBJ_BLOCK_OF_RADIUM, OBJ_CHIP_OF_RADIUM

      local actor_num = actor.actor_num
      if actor_num == 6
         or actor_is_affected_by_purple_berries(actor_num) -- purple berries protect against radiation
         or Actor.inv_get_readied_obj_n(actor, ARM) == 136 --OBJ_TONGS
         or Actor.inv_get_readied_obj_n(actor, ARM_2) == 136 then
         return
      end
      
      actor.poisoned = true
      printl("OUCH_IT_IS_VERY_HOT")
   end
end

function actor_get_obj(actor, obj) -- FIXME need to limit inventory slots

	if obj.getable == false then
		print("\nThat is not possible.")
		return false
	end
	
	if Actor.can_carry_obj_weight(actor, obj) == false then
		print("\nThe total is too heavy.")
		return false
	end

--		print("\nYou are carrying too much already.");

   subtract_movement_pts(actor, 3)

   actor_radiation_check(actor, obj)

   --FIXME more logic here.
	return true
end

function actor_get_max_hp(actor)
   if actor.actor_num == 6 then
      return 0xf0
   end
   
   if actor.in_party then
      return actor.str * 2 + actor.level * 24
   end
   
   --FIXME return actor max hp from stat table.
   return 1;
end
