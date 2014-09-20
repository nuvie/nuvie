io.stderr:write("actor.lua get here\n")

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

function actor_update_all()
	--FIXME
	advance_time(1)
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

function advance_time(num_turns)
   --FIXME
   local minute = clock_get_minute()
   
   clock_inc(num_turns)
   	
   if minute + num_turns >= 60 then
      update_actor_schedules()
      if g_hours_till_next_healing > 0 then
         g_hours_till_next_healing = g_hours_till_next_healing - 1
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
   
   actor.mtps = actor.mpts - points
end

function actor_radiation_check(actor, obj)
   if obj.obj_n == 448 or obj.obj_n == 449 then --OBJ_BLOCK_OF_RADIUM, OBJ_CHIP_OF_RADIUM
      --FIXME berries might protect against radiation.
      --mov     al, byte_41160[bx]
      --and     ax, 1111b
      if actor.actor_num == 6
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