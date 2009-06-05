io.stderr:write("actor.lua get here\n");
actor_wt_tbl = {}

--Actor stats table
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

monster_inv_food = { 129, 133, 128 }

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
        obj = obj_new(336); --charge
        obj.quality = v
        obj.qty = 1
        obj.status = 56
        Actor.inv_add_obj(actor, obj);
        --add object into actor inventory
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
         obj.qty = 0
         Actor.inv_add_obj(actor, obj);
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
        --add object into actor inventory
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

io.stderr:write("actor.lua loaded\n");