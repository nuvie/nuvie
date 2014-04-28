local lua_file = nil

--load common functions
lua_file = nuvie_load("common/intro_common.lua"); lua_file();



local function origin_fx_sequence()
	local g_img_tbl = image_load_all("title.lzc")
	
	canvas_set_palette("strax.pal", 0)

	
	local stars = sprite_new(g_img_tbl[0][0], 0, 24, true)
	local logo_image = image_new(282,82)
	image_blit(logo_image, g_img_tbl[0][1],0,16)
	image_blit(logo_image, g_img_tbl[0][2],g_img_tbl[0][1].w,14)
	image_blit(logo_image, g_img_tbl[0][3],g_img_tbl[0][1].w+g_img_tbl[0][2].w,0)
	
	local  logo = sprite_new(logo_image, 20, 70, false)
	
	local planet = sprite_new(g_img_tbl[12], 160, 48, true)

	planet.clip_x = 0
	planet.clip_y = 0
	planet.clip_w = 320
	planet.clip_h = 152
	local players = {}
	players[1] = create_player_sprite(g_img_tbl[1][0], 58, 118)
	players[2] = create_player_sprite(g_img_tbl[2][0], 186, 118)
	players[3] = create_player_sprite(g_img_tbl[3][0], 278, 118)
	
	players[4] = create_player_sprite(g_img_tbl[4][0], 58, 126)
	players[5] = create_player_sprite(g_img_tbl[5][0], 186, 126)
	players[6] = create_player_sprite(g_img_tbl[6][0], 278, 126)
	
	players[7] = create_player_sprite(g_img_tbl[7][0], 58, 134)
	players[8] = create_player_sprite(g_img_tbl[8][0], 186, 134)
	players[9] = create_player_sprite(g_img_tbl[9][0], 278, 134)
	
	local conductor = sprite_new(g_img_tbl[10][0], 158, 98, true)
	conductor.clip_x = 0
	conductor.clip_y = 24
	conductor.clip_w = 320
	conductor.clip_h = 128
	
	fade_in()

	music_play("strx_mus.lzc", 0)
	
	local i = 0
	for i=0,6,1 do
		conductor.image = g_img_tbl[10][i]

		update_players(players, g_img_tbl)
		if poll_for_key_or_button(4) == true then return end
	end
	for i=7,13,1 do
		conductor.image = g_img_tbl[10][i]

		update_players(players, g_img_tbl)
		if poll_for_key_or_button(4) == true then return end
	end
	for i=7,12,1 do
		conductor.image = g_img_tbl[10][i]

		update_players(players, g_img_tbl)
		if poll_for_key_or_button(4) == true then return end
	end
	local j
	for i=1,4,1 do
		for j=13,15,1 do
			conductor.image = g_img_tbl[10][j]
			if poll_for_key_or_button(1) == true then return end
		end
	
		conductor.image = g_img_tbl[10][14]
		if poll_for_key_or_button(2) == true then return end
		conductor.image = g_img_tbl[10][13]
		if poll_for_key_or_button(2) == true then return end
		conductor.image = g_img_tbl[10][16]

		if poll_for_key_or_button(1) == true then return end
			play_sfx(38, false)
	end
	
	for i=16,20,1 do
		conductor.image = g_img_tbl[10][i]
		if poll_for_key_or_button(4) == true then return end
	end
	if poll_for_key_or_button(200) == true then return end
	
	play_sfx(12, false)
	
	conductor.image = g_img_tbl[10][6]
	
	for i=1,21,1 do
		conductor.y = 98 + i * 12
		conductor.image.scale = 100 + i * 15
		
		for j=1,9,1 do
			players[j].y = players[j].y + 5
			players[j].image.scale = 100 + i * 5
			if j == 1 or j == 4 or j == 7 then
				players[j].x = players[j].x - 2
			end
			if j == 3 or j == 6 or j == 9 then
				players[j].x = players[j].x + 2
			end
		end
	
		if poll_for_esc(4) == true then return end
	end
	
	
	logo.visible = true
	logo.image.scale = 10	
	
	
	for i=1,18,1 do
		planet.y = planet.y + 6

		logo.image.scale = logo.image.scale + 5
		logo.x = math.floor((320 - logo.image.w) / 2)
		if i < 10 then
			logo.y = logo.y - 4
		else
			logo.y = logo.y + 1
		end
	
		if poll_for_key_or_button(4) == true then return end
	end
	
	fireworks(g_img_tbl, logo)
   fade_out()
end

function show_logos()
   local g_img_tbl = image_load_all("logos.lzc")
   local sprite = sprite_new(g_img_tbl[0], 0, 0, true)
   fade_in()
   if poll_for_key_or_button(175) == true then return end
   fade_out()
   sprite.image = g_img_tbl[1]
   fade_in()
   if poll_for_key_or_button(175) == true then return end
   fade_out()
   sprite.image = g_img_tbl[2]
   sprite.x = 15
   fade_in()
   if poll_for_key_or_button(175) == true then return end
   fade_out()
end

function play_intro()
   canvas_set_palette("md_title.pal", 0)
   music_play("mdd_mus.lzc", 0)
   show_logos()
end


mouse_cursor_visible(false)
canvas_set_update_interval(25)
canvas_set_bg_color(0)
canvas_set_opacity(0)

origin_fx_sequence()
canvas_hide_all_sprites()
play_intro()

canvas_hide()
