print("To phase ")
local phase = input_select_integer(nil,true)


print("\n")

magic_casting_fade_effect()

if phase > 0 and phase < 9 then
	--FIXME the original walks the party into an invisible gate to teleport.
  -- 20110629 Pieter Luteijn: need to check what the original does for non
  -- buried moonstones. If cooordinates are set to 0,0,0; using them might be 
  -- painful. See also FIXME in init.lua where the moonstone location table is
  -- not yet filled with the actual table loaded from in Weather.cpp 
	party_move(g_moonstone_loc_tbl[phase].x, g_moonstone_loc_tbl[phase].y, g_moonstone_loc_tbl[phase].z)
	fade_in()
else
	play_sfx(SFX_FAILURE)
end
