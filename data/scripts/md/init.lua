
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
[512] = 0, --cap
[513] = 0, --cowboy hat
[514] = 0, --pith hat
[515] = 0, --military hat
[516] = 0, --derby
[1151] = 0, --headgear
-- 1 = neck
[517] = 1, --kerchief
[608] = 1, --electric belt CHECKME
[518] = 1, --silk scarf
[519] = 1, --muffler
[1048] = 1, --martian jewelry
[1049] = 1, --martian jewelry
[1050] = 1, --martian jewelry
[1051] = 1, --martian jewelry
[1052] = 1, --martian jewelry
[1053] = 1, --martian jewelry
[1054] = 1, --martian jewelry
[1055] = 1, --martian jewelry
-- 3 = 1 handed
[527] = 3, --bloody saber CHECKME
[552] = 3, --derringer
[553] = 3, --revolver
[560] = 3, --hatchet
[561] = 3, --axe
[562] = 3, --ball-peen hammer CHECKME
[565] = 3, --knife
[566] = 3, --machete
[567] = 3, --saber
[622] = 3, --pry bar
[650] = 3, --weed sprayer
[667] = 3, --tongs
[1068] = 3, --martian ritual pod knife
-- 2 = torso
[528] = 2, --cloth jacket
[529] = 2, --wool sweater
[530] = 2, --cape
[531] = 2, --duster
[532] = 2, --wool overcoat
[533] = 2, --sheepskin jacket
[534] = 2, --artic parka
-- 3 = shield hand
[630] = 3, --torch
[631] = 3, --lit torch
[632] = 3, --candlestick
[633] = 3, --lit candlestick
[634] = 3, --candelabra
[635] = 3, --lit candelabra
[636] = 3, --oil lamp
[637] = 3, --lit oil lamp
[638] = 3, --lantern
[639] = 3, --lit lantern
-- 6 = UNK pants or dress FIXME: putting this here for now
[536] = 6, --cotton pants
[537] = 6, --cotton dress
[538] = 6, --denim jeans
[539] = 6, --wool pants
[540] = 6, --wool dress
[541] = 6, --chaps and jeans
-- 7 = feet
[520] = 7, --man's shoes
[521] = 7, --woman's shoes
[522] = 7, --riding boots
[523] = 7, --ruby slippers
[524] = 7, --thigh boots
[525] = 7, --hip boots
[526] = 7, --winged shoes
[542] = 7, --man's shoes
-- 4 = two handed
[551] = 4, --Cupid's bow and arrows
[554] = 4, --shotgun
[555] = 4, --rifle
[556] = 4, --Belgian combine
[557] = 4, --elephant gun
[558] = 4, --sling
[559] = 4, --bow
[563] = 4, --sledge hammer
[576] = 4, --pick
[577] = 4, --shovel
[578] = 4, --hoe
[579] = 4, --rake
[580] = 4, --pitchfork
[581] = 4, --cultivator
[582] = 4, --scythe
[583] = 4, --saw
[1066] = 4, --heat ray gun
[1067] = 4, --freeze ray gun
[1093] = 4, --spray gun
[1095] = 4, --martian hoe (couldn't be equipped in original)
[1096] = 4, --martian scythe (couldn't be equipped in original)
[1097] = 4, --martian pitchfork (couldn't be equipped in original)
[1098] = 4, --martian rake (couldn't be equipped in original)
[1099] = 4, --martian shovel (couldn't be equipped in original)
[1188] = 4, --M60 machine gun
[1206] = 4, --martian pick (couldn't be equipped in original)
[1897] = 4, --pool cue
-- 5 = finger gloves and bracelets FIXME: putting this here for now
[544] = 5, --lady's silk gloves
[545] = 5, --driving gloves
[546] = 5, --cotton work gloves
[547] = 5, --work gloves
[548] = 5, --wool mittens
[549] = 5, --rubber gloves
[550] = 5 --welding gloves
}


function obj_get_readiable_location(obj)
	if g_readiable_objs_tbl[obj.tile_num] ~= nil then
		return g_readiable_objs_tbl[obj.tile_num]
	end

	return -1	
end

--load actor functions
local actor_load = nuvie_load("md/actor.lua");
if type(actor_load) == "function" then
	actor_load()
else
	if type(actor_load) == "string" then
		io.stderr:write(actor_load);
	end
end

