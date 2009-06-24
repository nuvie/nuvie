io.stderr:write("actor.lua get here\n");

--Actor stats table
--str,dex,int,hp,dmg,alignment,?,?,?,?,?,?,?,?,?,strength_based,?,?,?,?,?,spell table,weapon table,armor table,treasure table
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

actor_randomise_stat = function(base_stat)
   tmp = math.floor(base_stat/2);
   if tmp == 0 then
      return 0
   end
   
   return math.random(0, tmp) + math.random(0, tmp)  + base_stat - tmp;
end

actor_get_max_magic_points = function(actor)
   obj_n = actor.obj_n
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

actor_str_adj = function(actor)
   --FIXME
   --if actor flag bit 6 not set then
   -- return strength unadjusted
   
   str = actor.str;
   if str <= 3 then
      return 1
   end
   
   return str - 3
end

actor_dex_adj = function(actor)
   --FIXME
   --if actor flag bit 6 not set then
   -- don't subtract 3 from dex
   
   dex = actor.dex
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

--
-- actor_combat_hit_check(attacker, foe, weapon_obj)
--

actor_combat_hit_check = function(attacker, foe, weapon_obj)

	if foe.luatype ~= "actor" or weapon_obj.obj_n == 48 then --48 glass sword
      io.stderr:write(foe.luatype)
		return true
	end

	use_str = false
	obj_n = weapon_obj.obj_n

	if weapon_obj == attacker then
		if actor_tbl[obj_n][16] == 1 then --actor uses strength
			use_str = true
		end
	else
		if obj_n == 4 or obj_n == 34 or obj_n == 39 or obj_n == 44 then --spiked helm, club, mace, two-handed hammer
			use_str = true
		end
	end
   
   if use_str then
      attack_role = actor_str_adj(attacker)
   else
      attack_role = actor_dex_adj(attacker)
   end
   
   foe_dex = actor_dex_adj(foe)
   roll = math.random(1,30)
   
   io.stderr:write("foe_dex = "..foe_dex.." attack_role = "..attack_role.." random(1,30) = "..roll.."\n\n");
   
   if math.floor((foe_dex + 30 - attack_role) / 2) >= roll then
      return false
   end
   
   return true
end

--
-- actor_init(actor)
--

actor_init = function(actor)

 actor_base = actor_tbl[actor.obj_n];
 if actor_base ~= nil then
 
   actor.str = actor_randomise_stat(actor_base[1]);
   actor.dex = actor_randomise_stat(actor_base[2]);
   actor.int = actor_randomise_stat(actor_base[3]);
   hp = actor_randomise_stat(actor_base[4]);
   actor.hp = hp;
   actor.level = math.floor((hp + 29) / 30);
   actor.align = actor_base[6]
 else
   actor.str = 10
   actor.dex = 10
   actor.int = 10
   actor.hp = 30
   actor.level = 1
   --actor.status = 32
 end
 
   actor.wt = 8;
   actor.combat_mode = 8;
   actor.magic = actor_get_max_magic_points(actor)
   actor.mpts = actor.dex
   actor.exp = 100
   
   --add spells
   for i,v in ipairs(actor_base[22]) do
        obj = Obj.new(336); --charge
        obj.quality = v
        obj.qty = 1
        obj.status = 56
        Actor.inv_add_obj(actor, obj);
   end

   --actor weapon
   chance = 2;
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
         Actor.inv_add_obj(actor, obj);
         Actor.inv_ready_obj(actor, obj);
         if v == 41 or v == 54 then --bow, magic bow
            obj = Obj.new(55) --arrow
            obj.status = 49
            obj.qty = math.random(12, 24)
            Actor.inv_add_obj(actor, obj);
         elseif v == 42 or v == 50 then --crossbow, triple crossbow
            obj = Obj.new(56) --bolt
            obj.status = 49
            obj.qty = math.random(12, 24)
            Actor.inv_add_obj(actor, obj);           
         end
      end

      chance = chance * 2
   end
   
   -- actor armor
   chance = 2;
   for i,v in ipairs(actor_base[24]) do
      
      if math.random(1,chance) == 1 then
      
        obj = Obj.new(v);
        obj.status = 57;
        obj.qty = 1
        obj.status = 56
        Actor.inv_add_obj(actor, obj);
        Actor.inv_ready_obj(actor, obj);
      end
      
      chance = chance * 2
   end
   
   --actor treasure
   chance = 2;
   for i,v in ipairs(actor_base[25]) do
      
      if math.random(1,chance) == 1 then    
         if v == 98 then --chest
            chest = Obj.new(v)
            chest.frame_n = 1
            chest.status = 41
            chest.qty = math.random(1, 100);
            Actor.inv_add_obj(actor, chest);
            if math.random(0, 1) == 1 then
               obj = Obj.new(337) --effect
               obj.status = 41
               obj.qty = 1
               obj.quality = 22;
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
            Actor.inv_add_obj(actor, obj);
         else
            obj = Obj.new(v)
            obj.quality = 49
            obj.qty = 1
            Actor.inv_add_obj(actor, obj);
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
   absy = abs(target_y - y)
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
   
   range = range_weapon_tbl[obj_n]
   
   if range == nil then
      return 1
   end
   
   return range

end

local projectile_weapon_tbl = 
{
[33] = true, -- sling
[37] = true, -- throwing axe
[41] = true, -- bow
[42] = true, -- crossbow
[49] = true, -- boomerang
[83] = true, -- flask of oil
[50] = true, -- triple crossbow
[91] = true, -- Zu Ylem
[79] = true, -- lightning wand
[80] = true, -- fire wand
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

function get_weapon_dmg(weapon_obj_n)
   dmg = weapon_dmg_tbl[weapon_obj_n]
   
   if dmg == nil then
      dmg = actor_tbl[weapon_obj_n][5]
   end
   
   if dmg == nil then
      dmg = 1
   end
   
   return dmg
end

function actor_get_ac(actor)

   
   --FIXME if cursed -3
   --FIXME if protected + 3
   local ac = 0;
     
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
      exp_gained = Actor.hit(defender, max_dmg)
      if exp_gained == nil then
         exp_gained = 0
      end
      
      attacker.exp = attacker.exp + exp_gained
   else
      print(defender.name.." grazed.\n")
   end
end

--
-- actor_attack(attacker, foe, weapon)
--

actor_attack = function(attacker, foe, weapon)

   weapon_obj_n = weapon.obj_n

   io.stderr:write("actor_attack()\nrange = " .. get_attack_range(attacker.x,attacker.y, foe.x,foe.y).." weapon range="..get_weapon_range(weapon_obj_n));
   
   if get_attack_range(attacker.x,attacker.y, foe.x,foe.y) > get_weapon_range(weapon_obj_n) then
      return
   end
   
   --check for arrows or bolts is attacking with a bow or crossbow

   if (weapon_obj_n == 41 or weapon_obj_n == 54) and Actor.inv_has_obj_n(attacker, 55) == false then --bow, magic bow, arrows
      return --no arrows, bail out
   end
   if (weapon_obj_n == 42 or weapon_obj_n == 50) and Actor.inv_has_obj_n(attacker, 56) == false then --crossbow, triple crossbow, bolts
      return --no bolts, bail out
   end
   

   
   --FIXME face foe here
   --x = foe.x - attacker.x
   --c = foe.y - attacker.y
   is_range_weapon = false

   if weapon_obj_n ~= 40 and weapon_obj_n ~= 47 then -- morning star, halberd
      if abs(foe.x - attacker.x) > 1 or abs(foe.y - attacker.y) > 1 or projectile_weapon_tbl[weapon_obj_n] ~= nil then
         is_range_weapon = true
         --FIXME call sub_1D4AE
      end
   end
   
   --FIXME get triple crossbow bolts here.
   
   dmg = get_weapon_dmg(weapon_obj_n)
   
   --FIXME run unknown func sub_1EABC here.
   
   if weapon_obj_n == 336 or weapon_obj_n == 57 then
      --FIXME cast spell here.
   else
      --weapon here.
      if actor_combat_hit_check(attacker, foe, weapon) then
         actor_take_hit(attacker, foe, dmg)
      end
   end
end

io.stderr:write("actor.lua loaded\n");
