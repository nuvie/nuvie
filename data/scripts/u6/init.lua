ALIGNMENT_DEFAULT = 0
ALIGNMENT_NEUTRAL = 1
ALIGNMENT_EVIL    = 2
ALIGNMENT_GOOD    = 3 
ALIGNMENT_CHAOTIC = 4

-- some common functions

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
  obj = {}
  
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
