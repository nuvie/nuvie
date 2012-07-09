function dbg(msg_string)
	io.stderr:write(msg_string)
end

function load_game()
end

function save_game()
end

function look_obj(obj)
	print("you see " .. obj.look_string);

	--FIXME usecode look description should be lua code.
	if usecode_look(obj) then
		print("\n")
		return false
	end

	print(".\n");
	return true
end

--tile_num, readied location
local g_readiable_objs_tbl = {
-- 0 = head
-- 1 = neck
[630] = 1, --tooth necklace
[631] = 1, --jade necklace
[696] = 1, --lei
-- 3 = 1 handed
[512] = 3, --spear of shamap
[541] = 3, --black staff
[545] = 3, --club
[546] = 3, --obsidian knife
[547] = 3, --spear
[548] = 3, --throwing axe
[549] = 3, --axe
[550] = 3, --rock hammer
[554] = 3, --obsidian sword
[558] = 3, --atl atl
[560] = 3, --boomerang
[565] = 3, --knife
[574] = 3, --scissors
[677] = 3, --metal hammer
[700] = 3, --grenade
[701] = 3, --lit grenade
[900] = 3, --torch
[901] = 3, --lit torch
[1028] = 3, --device
[1029] = 3, --activated device
-- 2 = torso
[518] = 2, --cloth armor
[519] = 2, --leather armour
[520] = 2, --bark armor
-- 3 = shield hand
[513] = 3, --shield of krukk
[524] = 3, --bark shield
[525] = 3, --leather shield
[526] = 3, --stegosaurus shield
[606] = 3, --kotl shield
-- 7 = feet
-- 4 = two handed - FIXME: guns (561, 571, 572) can't be equiped by natives
[552] = 4, --bow
[553] = 4, --blowgun
[553] = 4, --two handed hammer
[561] = 4, --modern rifle
[571] = 4, --bamboo flintlock
[572] = 4, --loaded bamboo flintlock
[640] = 4, --fire extinguisher
[676] = 4, --fire axe
[689] = 4, --bamboo pole
[692] = 4, --digging stick
[904] = 4, --fishing pole
-- 9 = finger
[600] = 9, --ring (not equipable in original)
}

function obj_is_readiable(obj)
	if g_readiable_objs_tbl[obj.tile_num] ~= nil then
		return true
	end

	return false
end

function obj_get_readiable_location(obj)
	if g_readiable_objs_tbl[obj.tile_num] ~= nil then
		return g_readiable_objs_tbl[obj.tile_num]
	end
	
	return -1	
end

--load actor functions
local actor_load = nuvie_load("se/actor.lua");
if type(actor_load) == "function" then
	actor_load()
else
	if type(actor_load) == "string" then
		io.stderr:write(actor_load);
	end
end
