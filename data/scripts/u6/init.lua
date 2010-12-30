--note nuvie direction values aren't the same as the original it uses the following scheme
--701
--6 2
--543

DIR_NORTH     = 0
DIR_EAST      = 1
DIR_SOUTH     = 2
DIR_WEST      = 3
DIR_NORTHEAST = 4
DIR_SOUTHEAST = 5 
DIR_SOUTHWEST = 6
DIR_NORTHWEST = 7

ALIGNMENT_DEFAULT = 0
ALIGNMENT_NEUTRAL = 1
ALIGNMENT_EVIL    = 2
ALIGNMENT_GOOD    = 3 
ALIGNMENT_CHAOTIC = 4

-- some common functions

function alignment_is_evil(align)
   if align == ALIGNMENT_EVIL or align == ALIGNMENT_CHAOTIC then return true end
   
   return false
end

function direction_string(dir)
   if dir == DIR_NORTH then return "NORTH" end
   if dir == DIR_NORTHEAST then return "NORTHEAST" end
   if dir == DIR_EAST then return "EAST" end
   if dir == DIR_SOUTHEAST then return "SOUTHEAST" end
   if dir == DIR_SOUTH then return "SOUTH" end
   if dir == DIR_SOUTHWEST then return "SOUTHWEST" end
   if dir == DIR_WEST then return "WEST" end
   if dir == DIR_NORTHWEST then return "NORTHWEST" end
   
   return "UNKNOWN"
end

local dir_rev_tbl =
{
   [DIR_NORTH] = DIR_SOUTH,
   [DIR_NORTHEAST] = DIR_SOUTHWEST,
   [DIR_EAST] = DIR_WEST,
   [DIR_SOUTHEAST] = DIR_NORTHWEST,
   [DIR_SOUTH] = DIR_NORTH,
   [DIR_SOUTHWEST] = DIR_NORTHEAST,
   [DIR_WEST] = DIR_EAST,
   [DIR_NORTHWEST] = DIR_SOUTHEAST
}
   
function direction_reverse(dir) return dir_rev_tbl[dir] end
   
function abs(val)
   if val < 0 then
      return -val
   end
   
   return val
end

function advance_game_time(nturns)
 if nturns == 0 then return end

 coroutine.yield("adv_game_time", nturns);
end

function get_target()
 local loc = {}
 loc = coroutine.yield("target")

 return loc
end

function obj_new(obj_n, frame_n, status, qty, quality, x, y, z)
  local obj = {}
  
  obj["obj_n"] = obj_n or 0
  obj["frame_n"] = frame_n or 0
  obj["status"] = status or 0
  obj["qty"] = qty or 0
  obj["quality"] = quality or 0

  obj["x"] = x or 0
  obj["y"] = y or 0
  obj["z"] = z or 0

  return obj
end

function run_script(script)
  local t = {};
  setmetatable(t, {__index = _G});
  setfenv(0, t);
  t.body = nuvie_load(script);
  t.body();
end

function look_obj(obj)
   print("Thou dost see " .. obj.look_string);
   local weight = obj.weight; --FIXME this could be a problem if we want to change Lua_number type to int. 
   if weight ~= 0 then
      if obj.qty > 1 and obj.stackable then
         print(". They weigh");
      else
         print(". It weighs");
      end
      
      print(string.format(" %.1f", weight).." stones");
   end

   --FIXME usecode look description should be lua code.
   if usecode_look(obj) then
      print("\n")
      return false
   end
   
   local dmg = weapon_dmg_tbl[obj.obj_n];
   if dmg ~= nil then
      if weight ~= 0 then
         print(" and")
      else
         print(". It")
      end
      
      print(" can do "..dmg.." point")
      if dmg > 1 then print("s") end
      print(" of damage")

   end
   
   local ac = armour_tbl[obj.obj_n]
   if ac ~= nil then
      if weight ~= 0 or dmg ~= 0 then
         print(" and")
      else
         print(". It")
      end
      
      print(" can absorb "..ac.." point")
      if ac > 1 then print("s") end
      print(" of damage")
   end
   
   print(".\n");
   return true
end

function player_subtract_karma(k)
	local karma = player_get_karma() - k
	if karma < 0 then karma = 0 end
	
	player_set_karma(karma)
end

function player_add_karma(k)
local karma = player_get_karma() + k
if karma >= 100 then karma = 99 end

player_set_karma(karma)
end

--load actor functions
actor_load = nuvie_load("u6/actor.lua");
if type(actor_load) == "function" then
   actor_load()
else
   if type(actor_load) == "string" then
      io.stderr:write(actor_load);
   end
end

-- init magic
magic_init = nuvie_load("u6/magic.lua"); magic_init();
