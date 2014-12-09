function look_pocketwatch(obj)
   printl("THE_TIME_IS")
   local am = true
   local hour = clock_get_hour()
   local minute = clock_get_minute()
   if hour >= 12 then
   am = false
   end
   if hour > 12 then
   hour = hour - 12
   end
   local time = "TIME_AM"
   if am == false then
   time = "TIME_PM"
   end
   
   printfl(time, hour, minute)
end

function look_cannon(obj)
   if obj.quality ~= 0 then
      printl("THE_CANNON_WILL_FIRE_STEEL_CANNON_BALLS")
   end
end

function look_barrow(obj)
   if obj.qty == 0 then
      printl("IT_IS_EMPTY")
      return
   end
   
   local quality = obj.quality
   local material
   if quality == 1 then
      material = i18n("DIRT")
   elseif quality == 2 then
      material = i18n("ROCK")
   elseif quality == 2 then
      material = i18n("IRON_ORE")
   elseif quality == 2 then
      material = i18n("COAL")
   end
   
   if obj.qty == 1 then
      printfl("IT_HAS_1_LOAD_OF", material)
   else
      printfl("IT_HAS_N_LOADS_OF", obj.qty, material)
   end
end

function get_lat_long_string(x, y)
   local lat_str = "N"
   local long_str = "W"
   
   local lat = math.modf(((y - 512) * 240) / 1024)
   local long = math.modf(((x - 512) * 360) / 1024)
   
   if lat > 0 then
      lat_str = "S"
   else
      if lat == 0 then
         lat_str = " "
      end
   end
   
   if long == 180 or long == -180 or long == 0 then
      long_str = " "
   else
      if long < 0 then
         long_str = "E"
      end
   end
   
   lat = math.abs(lat)
   long = 180 - math.abs(long)
   
   return lat..lat_str.." "..long..long_str
end

function look_marker_flag(obj)
   local names = {
   [1]="Coprates Chasma",
   [2]="Arsia Mons",
   [3]="Pavonis Mons",
   [4]="Ascraeus Mons",
   [5]="Alber Tholus",
   [6]="Elysium Mons",
   [7]="Hecates Tholus",
   [8]="Terra Sirenum",
   [9]="Noctis Labyrinthus",
   [10]="Deuteronicus Mensae",
   [11]="Syrtis Major Planum",
   [12]="Olympus Mons",
   }

   if obj.z == 0 then
      if obj.quality <= 12 then
         if obj.quality ~= 0 then
            print(names[obj.quality])
            print(" "..get_lat_long_string(obj.x,obj.y).."\n")
         end
      else
         printl("AARGH")
      end
   end
end

function look_broken_strap(obj)
   local spector = Actor.get(2)
   Actor.set_talk_flag(spector, 6)
   Actor.talk(spector)
end

function look_metal_woman(obj)
   if obj.quality ~= 0 then
      printl("IT_HAS_A_HEARTSTONE")
   end
end

function look_covered_martian_seed(obj)
   if obj.frame_n < 4 then
      local quality = obj.quality
      if quality == 15 then
         printl("IT_IS_GROWING")
      elseif quality == 16 then
         printl("IT_IS_RIPE")
      else
         printl("IT_IS_NOT_GROWING")
      end
   end
end

local look_usecode = {
[91]=look_pocketwatch,
[98]=look_pocketwatch,
[172]=look_marker_flag,
[251]=look_covered_martian_seed,
[268]=look_barrow, --OBJ_MARTIAN_WHEEL_BARROW
[287]=look_metal_woman,
[333]=look_cannon,
[410]=look_barrow, --OBJ_RAIL_CAR
[460]=look_broken_strap,
}

function look_obj(obj)
   printfl("YOU_SEE", obj.look_string);
   
   --FIXME usecode look description should be lua code.
   if usecode_look(obj) then
      print("\n")
      return false
   end
   
   print(".\n");
   
   if look_usecode[obj.obj_n] ~= nil then
      look_usecode[obj.obj_n](obj)
      print("\n")
   end
   
   if is_container_obj(obj.obj_n) then
      search(obj)
   end
   
   return false
end