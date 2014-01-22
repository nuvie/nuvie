ALIGNMENT_DEFAULT = 0
ALIGNMENT_NEUTRAL = 1
ALIGNMENT_EVIL    = 2
ALIGNMENT_GOOD    = 3 
ALIGNMENT_CHAOTIC = 4

g_combat_range_tbl = {
0, 1, 2, 3, 4, 5, 6, 7,
1, 1, 2, 3, 4, 5, 6, 7,
2, 2, 2, 3, 4, 5, 6, 7,
3, 3, 3, 4, 5, 6, 7, 7,
4, 4, 4, 5, 6, 7, 7, 8,
5, 5, 5, 6, 7, 7, 8, 8,
6, 6, 6, 7, 7, 8, 8, 8,
7, 7, 7, 7, 8, 8, 8, 8}

function get_combat_range(absx, absy)
   if absx < 8 and absy < 8 then
      return g_combat_range_tbl[absx * 8 + absy + 1]
   end

   return 9
end

function get_weapon_range(obj_n)

   local range = g_range_weapon_tbl[obj_n]
   
   if range == nil then
      return 1
   end

   return range
end

function actor_randomise_stat(base_stat)
   local tmp = math.floor(base_stat/2)
   if tmp == 0 then
      return base_stat
   end
   
   return math.random(0, tmp) + math.random(0, tmp)  + base_stat - tmp
end

