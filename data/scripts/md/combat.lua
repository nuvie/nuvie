-- [objectnum] = range
local range_weapon_tbl = {
   [41]  = 4,
   [42]  = 5,
   [43]  = 4,
   [44]  = 9,
   [45]  = 9,
   [46]  = 9,
   [47]  = 4,
   [48]  = 5,
   [240] = 6,
   [241] = 6,
   [129] = 2,
   [261] = 2,
   [313] = 9,
   [40]  = 6,
   [366] = 3,
   [386] = 4,
   [364] = 3,
   [384] = 2,
}

function get_weapon_range(weapon)
   local range = range_weapon_tbl[weapon.obj_n]
   if range == nil then
      range = 1
   end
   return range
end

local weapon_dmg_tbl = {
   [16] = 30, --bloody saber
   [40] = 1, --Cupid's bow and arrows (charms)
   [41] = 15, --derringer
   [42] = 18, --revolver
   [43] = 20, --shotgun
   [44] = 30, --rifle
   [45] = 30, --Belgian combine
   [46] = 45, --elephant gun
   [47] = 8, --sling
   [48] = 12, --bow
   [49] = 15, --hatchet
   [50] = 20, --axe
   [51] = 10, --ball-peen hammer
   [52] = 25, --sledge hammer
   [54] = 10, --knife
   [55] = 20, --machete
   [56] = 25, --saber
   [65] = 15, --pick
   [66] = 8, --shovel
   [67] = 10, --hoe
   [68] = 10, --rake
   [69] = 15, --pitchfork
   [70] = 12, --cultivator
   [71] = 20, --scythe
   [72] = 10, --saw
   [102] = 12, --pry bar
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
   [129] = 60, --weed sprayer -- FIXME: no damage normally. Only effects plants?
   --[136] = 1, --tongs
   [241] = 20, --heat ray gun
   [242] = 10, --freeze ray gun
   --[243] = 1, --martian ritual pod knife
   [261] = 60, --spray gun -- FIXME: no damage normally. Only effects plants?
   [263] = 10, --martian hoe (couldn't be equipped in original)
   [264] = 20, --martian scythe (couldn't be equipped in original)
   [265] = 15, --martian pitchfork (couldn't be equipped in original)
   [266] = 10, --martian rake (couldn't be equipped in original)
   [267] = 8, --martian shovel (couldn't be equipped in original)
   [313] = 254, --M60 machine gun (scripted to only attack and kill the big bad)
   [327] = 15, --martian pick (couldn't be equipped in original)
   [401] = 12, --pool cue
}

function get_weapon_damage(weapon)
   local dmg
   if weapon.luatype == "actor" then
      dmg = actor_get_damage(weapon)
   else
      dmg = weapon_dmg_tbl[weapon.obj_n]
   end

   if dmg == nil then
      dmg = -1
   end

   return dmg
end

function attack_dex_saving_throw(attacker, defender, weapon)
   if defender == nil or defender.luatype == "obj" then
      return true
   end

   local attacker_value
   if weapon.luatype == "actor" and is_actor_stat_bit_set(weapon.obj_n, 5) then
      attacker_value = actor_str_adj(attacker)
   else
      attacker_value = actor_dex_adj(attacker)
   end

   local defender_value = actor_dex_adj(defender)

   if math.random(1, 30) >= math.floor((defender_value + 30 - attacker_value) / 2) then
      return true
   end

   return false
end

function out_of_ammo(attacker, weapon, print_message) -- untested function

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
         printl("OUT_OF_AMMUNITION")
         play_md_sfx(5)
      end
      return true
   end

   if weapon_obj_n == 48 and Actor.inv_has_obj_n(attacker, 64) == false then --bow, arrows
      if(print_message) then
         printl("OUT_OF_ARROWS")
         play_md_sfx(5)
      end
      return true
   end

   return false
end

function attack_target_with_weapon(actor, target_x, target_y, weapon)
   local target_range = actor_get_combat_range(actor, target_x, target_y)
   local weapon_range = get_weapon_range(weapon)

   if target_range > weapon_range then
      return 2 --out of range
   end

   local obj_n = weapon.obj_n
   if obj_n == 47 and not Actor.inv_has_obj_n(actor, 63) then --OBJ_SLING, OBJ_SLING_STONE
      return 1
   end

   if (obj_n == 41 or obj_n == 42) and not Actor.inv_has_obj_n(actor, 57) then --OBJ_DERRINGER, OBJ_REVOLVER, OBJ_PISTOL_ROUND
      return 1
   end

   if obj_n == 46 and not Actor.inv_has_obj_n(actor, 60) then --OBJ_ELEPHANT_GUN, OBJ_ELEPHANT_GUN_ROUND
      return 1
   end

   if obj_n == 44 and not Actor.inv_has_obj_n(actor, 59) then --OBJ_RIFLE, OBJ_RIFLE_ROUND
      return 1
   end

   if obj_n == 43 and not Actor.inv_has_obj_n(actor, 58) then --OBJ_SHOTGUN, OBJ_SHOTGUN_SHELL
      return 1
   end

   if (obj_n == 129 or obj_n == 261) and weapon.qty ~= 0 then --OBJ_WEED_SPRAYER, OBJ_SPRAY_GUN
      return 1
   end

   local var_10 = 0
   local var_12 = 0
   --OBJ_HEAT_RAY_GUN
   --OBJ_FREEZE_RAY_GUN
   --OBJ_BELGIAN_COMBINE
   if obj_n == 240 or obj_n == 241 or obj_n == 45 then
      if weapon.quality < 2 then
         var_10= 1
      end

      if weapon.quality ~= 1 then
         var_12 = 1
      end

      if obj_n == 45 then --OBJ_BELGIAN_COMBINE
         if var_10 ~= 0 then
            if not Actor.inv_has_obj_n(actor, 59) then --OBJ_RIFLE_ROUND
               var_10 = 0
            end
         end
         if var_12 ~= 0 then
            if not Actor.inv_has_obj_n(actor, 58) then --OBJ_SHOTGUN_SHELL
               var_12 = 0
            end
         end
      else
         if weapon.qty == 0 then
            return 1
         end
         if weapon.qty < (var_12 * 4) + var_10 then
            var_12 = 0
         end
      end

      if var_10 == 0 and var_12 == 0 then
         return 1
      end
   else
      if obj_n == 43 then --OBJ_SHOTGUN
         var_10 = 0
         var_12 = 1
      else
         var_10 = 1
         var_12 = 0
      end
   end

   local damage_mode = 0
   if obj_n == 240 then --OBJ_HEAT_RAY_GUN
      damage_mode = 3
   end

   if obj_n == 241 then --OBJ_FREEZE_RAY_GUN
      damage_mode = 1
   end

   g_attack_target = g_selected_obj

   if var_10 == 0 then
      --FIXME do end logic here.
      return 0
   end

   local is_ranged_attack = false
   if target_range > 1 then
      is_ranged_attack = true
   end

   if (obj_n >= 40 and obj_n <= 48) --OBJ_CUPIDS_BOW_AND_ARROWS, OBJ_BOW
      or obj_n == 129 --OBJ_WEED_SPRAYER
         or obj_n == 261 --OBJ_SPRAY_GUN
         or obj_n == 240 --OBJ_HEAT_RAY_GUN
         or obj_n == 241 --OBJ_FREEZE_RAY_GUN
   then
      is_ranged_attack = true
   end

   local damage
   if weapon.luatype == "actor" then
      damage = actor_get_damage(actor)
      if damage == nil then
         damage = 1
      end
   else
      damage = get_weapon_damage(weapon)
      if damage < 0 then
         damage = 1
      end
   end

   local does_damage

   local rockworm_actor =  find_rockworm_actor(g_attack_target)
   if rockworm_actor ~= nil
           and rockworm_actor.luatype == "actor"
           and obj_n ~= 129 --OBJ_WEED_SPRAYER
           and obj_n ~= 261 then --OBJ_SPRAY_GUN
      does_damage = attack_dex_saving_throw(actor, rockworm_actor, weapon)
   end

   return 0
end