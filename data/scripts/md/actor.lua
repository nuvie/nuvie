io.stderr:write("actor.lua get here\n")

--Actor stats table
--[objnum] = {str,dex,int,hp,dmg,alignment,can talk,drops blood,?,?,?,lives in water,flies-immune to tremor,repel undead (not used anymore),poisonous,strength_based,double dmg from fire,immune to magic (fire only),immune to poison,undead and immune to death spells,immune to sleep spell,{spell table},{weapon table},{armor table},{treasure table},exp_related see actor_hit()}
actor_tbl = {
--unconscious Martian
[291] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--student
[309] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--student
[310] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[318] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[319] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[342] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[343] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[344] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--scientist
[345] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--young woman
[346] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--cowboy
[347] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--gentleman
[348] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--uniformed man
[349] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--adventurer
[350] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--common gellow
[351] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--man
[352] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--man in white
[353] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--lady
[354] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--woman
[355] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--adventuress
[356] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--metal woman
[357] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--mechanical man
[358] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--sextelleger
[359] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--bushalo
[360] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--plantellope
[361] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--tentacle
[362] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--ammonoid
[363] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--proto martian
[364] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--hedgehog
[365] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--cave worm
[366] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--janitor
[367] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--planther
[368] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--martian
[369] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--rockworm base
[370] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--rockworm
[371] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--rockworm
[372] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--giant maw
[373] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--creeper
[374] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--oxy leech
[375] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--agrobot
[376] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--wisp
[377] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--airsquid
[378] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--canal worm
[379] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--airsquid
[380] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--dust devil
[381] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--mechanical man
[382] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--creeping cactus
[383] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--pod devil
[384] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--jumping bean
[385] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--sand trapper
[386] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--bushrat
[387] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--glow worm
[388] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--king
[389] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--shadowlord
[390] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--your mother
[391] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--horse
[392] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--Dibbs
[393] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--Raxachk
[394] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[396] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--yourself
[397] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0},
--minotaur
[398] = {1, 28, 4, 3, 1, ALIGNMENT_NEUTRAL, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, 0}
}

function actor_map_dmg(actor, map_x, map_y, map_z)
	--FIXME
end

function actor_update_all()
	--FIXME
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
   dbg("target_x="..target_x.." target_y="..target_y.." x="..x.." y="..y.." absx="..absx.." absy="..absy)
   if absx < 8 and absy < 8 then
      return combat_range_tbl[absx * 8 + absy + 1]
   end
   
   return 9
end

local get_weapon_range = function(obj_n)
   local range_weapon_tbl = {
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
   
   local range = range_weapon_tbl[obj_n]
   
   if range == nil then
      return 1
   end
   
   return range

end

local projectile_weapon_tbl = 
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
--[129] = 2, --weed sprayer
--[136] = 1, --tongs
[241] = 2, --heat ray gun
[242] = 2, --freeze ray gun
--[243] = 1, --martian ritual pod knife
[261] = 2, --spray gun
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

function advance_time(num_turns)
	--FIXME
	local minute = clock_get_minute()
	
	clock_inc(num_turns)
		
	if minute + num_turns >= 60 then
		update_actor_schedules()
	end
end
