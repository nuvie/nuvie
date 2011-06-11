print("To phase ")
local phase = input_select_integer(nil,true)


print("\n")

magic_casting_fade_effect()

if phase > 0 and phase < 9 then
	--FIXME the original walks the party into an invisible gate to teleport.
	party_move(g_moonstone_loc_tbl[phase].x, g_moonstone_loc_tbl[phase].y, g_moonstone_loc_tbl[phase].z)
	fade_in()
else
	play_sfx(SFX_FAILURE)
end