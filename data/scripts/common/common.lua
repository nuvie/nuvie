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
DIR_NONE      = 8

function get_direction(prompt)

	if prompt ~= nil then
		print(prompt)
	end
	
	local dir = coroutine.yield("dir")

	return dir
end

function direction_string(dir)
	if dir ~= nil then
		if dir == DIR_NORTH then return "north" end
		if dir == DIR_NORTHEAST then return "northeast" end
		if dir == DIR_EAST then return "east" end
		if dir == DIR_SOUTHEAST then return "southeast" end
		if dir == DIR_SOUTH then return "south" end
		if dir == DIR_SOUTHWEST then return "southwest" end
		if dir == DIR_WEST then return "west" end
		if dir == DIR_NORTHWEST then return "northwest" end
	end
	
	return "unknown"
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

local g_dir_offset_tbl =
{
   [DIR_NORTH] = {["x"]=0, ["y"]=-1},
   [DIR_NORTHEAST] = {["x"]=1, ["y"]=-1},
   [DIR_EAST] = {["x"]=1, ["y"]=0},
   [DIR_SOUTHEAST] = {["x"]=1, ["y"]=1},
   [DIR_SOUTH] = {["x"]=0, ["y"]=1},
   [DIR_SOUTHWEST] = {["x"]=-1, ["y"]=1},
   [DIR_WEST] = {["x"]=-1, ["y"]=0},
   [DIR_NORTHWEST] = {["x"]=-1, ["y"]=-1},
   [DIR_NONE] = {["x"]=0, ["y"]=0}
}

function direction_get_loc(dir, from_x, from_y)
	return g_dir_offset_tbl[dir].x + from_x, g_dir_offset_tbl[dir].y + from_y
end

function abs(val)
   if val < 0 then
      return -val
   end
   
   return val
end


--load other common functions

local lua_file = nil

lua_file = nuvie_load("common/actor.lua"); lua_file();
