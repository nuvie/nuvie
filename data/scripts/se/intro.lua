-- TODO - ALL TRANSITIONS - FADE IN/OUT, STAR FADE
-- TODO - CURSORS
-- TODO - CONFIRM/CONFIGURE KEYS TO ADVANCE/BYPASS
-- TODO - MEMORY MANAGEMENT (prevent leaks)
-- TODO - CREATE NEW CHARACTER OVER EXISTING?
-- TODO - CHANGE MENU FOR NO CHARACTER YET CREATED? (new install)

local function poll_for_key_or_button(cycles)
	local input
	input = input_poll()
	-- TODO Detect Key or Button Press (Not mouse movement)
	if input ~= nil and input == 27 then
		return true
	end
	return false
end

local function poll_for_esc(cycles)
	local input
	if cycles == nil then
		input = input_poll()
		if input ~= nil and input == 27 then
			return true
		end
	else
		local i
		for i=0,cycles,1 do
			local input = input_poll()
			if input ~= nil and input == 27 then
				return true
			end
			canvas_update()
		end
	end
	return false
end

local function wait_for_input()
	local input = nil
	while input == nil do
		canvas_update()
		input = input_poll()
		if input ~= nil then
			break
		end
	end
	
	return input
end

local function should_exit(input)
	if input ~=nil and input == 27 then
		return true
	end
	
	return false
end

local function fade_in()
	local i
	for i=0x0,0xff,3 do
		canvas_set_opacity(i)
		canvas_update()
	end
	
	return false
end

local function fade_out()
	local i
	for i=0xff,0,-3 do
		canvas_set_opacity(i)
		canvas_update()
	end
	
	return false
end

local function fade_out_sprite(sprite, speed)
	local i
	if speed ~= nil then
		speed = -speed
	else
		speed = -3
	end
	
	for i=0xff,0,speed do
		sprite.opacity = i
		canvas_update()
	end

	return false
end

local function destroy_sprite(sprite)
-- TODO Correct Memory Management
	sprite.visible = false
--	sprite_gc(sprite)
end

local function destroy_sprites(sprites, number)
	for j=0,number-1,1 do
		destroy_sprite(sprites[j+1])
	end
	return false
end

local function opaque_sprites(sprites, number)
	for j=0,number-1,1 do
		-- TODO Opaque = 0x100 or 0xff???
		sprites[j+1].opacity = 0x100
		canvas_update()
	end
	return false
end

local function fade_out_sprites(sprites, speed, number)
	local i
	if speed ~= nil then
		speed = -speed
	else
		speed = -3
	end

	for i=0xff,0,speed do
		for j=0,number-1,1 do
			sprites[j+1].opacity = i
			canvas_update()
		end
	end

	-- Fully Transparent
	for j=0,number-1,1 do
		sprites[j+1].opacity = 0
		canvas_update()
	end

	return false
end

local function fade_in_sprites(sprites, speed, number)
	local i
	if speed ~= nil then
		speed = speed
	else
		speed = 3
	end

	for i=0,0xff,speed do
		for j=0,number-1,1 do
			sprites[j+1].opacity = i
			canvas_update()
		end
	end

	-- Fully Opaque
	for j=0,number-1,1 do
		-- TODO Opaque = 0x100 or 0xff???
		sprites[j+1].opacity = 0x100
		canvas_update()
	end

	return false
end

local function update_players(players, g_img_tbl)
local rand = math.random

	players[1].image = g_img_tbl[1][rand(0,12)]
	players[2].image = g_img_tbl[2][rand(0,8)]
	players[3].image = g_img_tbl[3][rand(0,2)]
	players[4].image = g_img_tbl[4][rand(0,6)]
	players[5].image = g_img_tbl[5][rand(0,4)]
	players[6].image = g_img_tbl[6][rand(0,2)]
	players[7].image = g_img_tbl[7][rand(0,4)]
	players[8].image = g_img_tbl[8][rand(0,4)]
	players[9].image = g_img_tbl[9][rand(0,3)]
		
end

local function create_player_sprite(image, x, y)
	local sprite = sprite_new(image, x, y, true)
	sprite.clip_x = 0
	sprite.clip_y = 0
	sprite.clip_w = 320
	sprite.clip_h = 152

	return sprite
end

local function origin_fx_sequence()
	local g_img_tbl = image_load_all("title.lzc")
	
	canvas_set_palette("savage.pal", 2)

	
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

	music_play("music.lzc", 19)
	
	local i = 0
	for i=0,6,1 do
		conductor.image = g_img_tbl[10][i]

		update_players(players, g_img_tbl)
		if poll_for_esc(4) == true then return end
	end
	for i=7,13,1 do
		conductor.image = g_img_tbl[10][i]

		update_players(players, g_img_tbl)
		if poll_for_esc(4) == true then return end
	end
	for i=7,12,1 do
		conductor.image = g_img_tbl[10][i]

		update_players(players, g_img_tbl)
		if poll_for_esc(4) == true then return end
	end
	local j
	for i=1,4,1 do
		for j=13,15,1 do
			conductor.image = g_img_tbl[10][j]
			if poll_for_esc(1) == true then return end
		end
	
		conductor.image = g_img_tbl[10][14]
		if poll_for_esc(2) == true then return end
		conductor.image = g_img_tbl[10][13]
		if poll_for_esc(2) == true then return end
		conductor.image = g_img_tbl[10][16]

		if poll_for_esc(1) == true then return end
			play_sfx(38, false)
	end
	
	for i=16,20,1 do
		conductor.image = g_img_tbl[10][i]
		if poll_for_esc(4) == true then return end
	end
	if poll_for_esc(200) == true then return end
	
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
	
		if poll_for_esc(4) == true then return end
	end
	
	if poll_for_esc(200) == true then return end
end

local function intro_sequence(g_img_tbl)
	
	canvas_set_palette("savage.pal", 0)
	music_play("music.lzc", 18)
		
	local  logo = sprite_new(g_img_tbl[0][0], 0, 0, true)
	fade_in()
	if poll_for_esc(175) == true then return end
	fade_out()
	logo.image = g_img_tbl[0][1]
	fade_in()
	if poll_for_esc(175) == true then return end
	fade_out()

	canvas_set_palette("savage.pal", 0)
	local  bg = sprite_new(g_img_tbl[1][0], 0, 0, true) -- Background 0
	local  bg1 = sprite_new(g_img_tbl[1][1], 320, 0, true) -- Final Section
	local  bg2 = sprite_new(g_img_tbl[1][2], 0, 0, true) -- Background 1
	local  bg3 = sprite_new(g_img_tbl[1][3], 320, 0, true) -- Bushes w/ Triceratops
	local  bg4 = sprite_new(g_img_tbl[1][4], 640, 0, true) -- Left section of Tower
	local  t1 = sprite_new(g_img_tbl[2][0], 0, 240, true)
	local  bg5 = sprite_new(g_img_tbl[1][5], 0, 0, true) -- Dinosaurs & Bushes
	local  bg6 = sprite_new(g_img_tbl[1][6], 320, 0, true) -- Tree w/ Sleeping Dinosaur
	local  bg7 = sprite_new(g_img_tbl[1][7], 640, 0, true) -- Bushes on sides w/ flower on left
	canvas_set_opacity(0xff)
	i = 0
	while i < 240 do
	      canvas_update()
	      t1.y = t1.y - 3
	      if (t1.y < 0) then t1.y = 0 end
	      i = i + 3
	      if poll_for_esc(1) == true then return end
	end
	if poll_for_esc(30) == true then return end
	fade_out_sprite(t1, 4)
	i = 0
	current_credit = 1
	local  credit_sprite = sprite_new(g_img_tbl[2][current_credit], 0, 0, true)
	credit_time = 15
	credit_show = 1
	while i < 636 do
	      canvas_update()
	      bg.x = bg.x - 1
	      bg1.x = bg1.x - 1
	      bg2.x = bg2.x - 2
	      bg3.x = bg3.x - 2
	      bg4.x = bg4.x - 2
	      bg5.x = bg5.x - 3
	      bg6.x = bg6.x - 3
	      bg7.x = bg7.x - 3
	      i = i + 3
	      credit_time = credit_time - 1
	      if (credit_time == 0) then
	      	 if (credit_show == 1) then
		    credit_show = 0
		    credit_sprite.visible = false
		    credit_time = 20
		 else
		    credit_show = 1
		    current_credit = current_credit + 1
		    if (current_credit < 7) then
		       credit_sprite = sprite_new(g_img_tbl[2][current_credit], 0, 0, true)
		       credit_time = 15
		    end
		 end
	      end
	      if poll_for_esc(8) == true then return end
	end
end

local story_so_far_text = {
"You had disturbing dreams of a faraway jungle,",
"an engangered princess, and the black",
"moonstone you carried away from Britannia in",
"your last adventure there.",
"The spirit of Lord British commanded you to",
"find out all you could about the stone.",
"",
"",
"So you took it to your old friend Dr. Rafkin,",
"curator of the local museum of natural",
"history, hoping that he could unravel the",
"mystery.",
"At Dr. Rafkin's, you met ace reporter and",
"\"Ultimate Adventures\" correspondent Jimmy",
"Malone.  Malone's a little too nosy to suit you...",
"",
"...but he kept his wits when everything went",
"haywire, when Dr. Rafkin's experiments on your",
"moonstone sent the whole museum lab through",
"a bizarre black moongate!",
"You were shocked to lift your head and see",
"jungle around you: Jungle, and this enormous",
"creature descending... descending upon--",
"",
"Her! The woman you dreamt of! A fiery tribal",
"she-warrior of some time-lost land... You and",
"your friends rescued her from the giant",
"pteranodon and befriended her.",
"She said her name was Aiela, princess of the",
"Kurak tribe, and seemed to be quite taken with",
"you, her most unusual rescuer.",
"",
"But Aiela's hated suitor, Darden the Huge,",
"prince of the Urali tribe, arrived with his men,",
"scattering your friends and smashing you into",
"the earth.",
"Now you lie broken and defeated, struggling to",
"regain consciousness...",
"",
""
}

local function story_so_far(img_tbl2)
	local prev_f1 = nil
	local prev_f2 = nil
	canvas_set_palette("savage.pal", 1)
	for i = 0, 9, 1 do
		-- Need to figure out if I need to get a new version
		-- of the Top level sprite or not
		local new_img = image_copy(img_tbl2[5][0])
		local  f1 = sprite_new(new_img, 0, 0, true)
		local  f2 = sprite_new(img_tbl2[5][i+1], 120, 45, true)
		if prev_f1 ~= nil then
			destroy_sprite(prev_f1)
			prev_f1 = nil
		end
		if prev_f2 ~= nil then
			destroy_sprite(prev_f2)
			prev_f2 = nil
		end
		prev_f1 = f1;
		prev_f2 = f2;
		image_print(new_img, story_so_far_text[i*4+1], 0, 300, 25, 136, 0)
		image_print(new_img, story_so_far_text[i*4+2], 0, 300, 25, 144, 0)
		image_print(new_img, story_so_far_text[i*4+3], 0, 300, 25, 152, 0)
		image_print(new_img, story_so_far_text[i*4+4], 0, 300, 25, 160, 0)
		wait_for_input()
	end
	if prev_f1 ~= nil then
		destroy_sprite(prev_f1)
	end
	if prev_f2 ~= nil then
		destroy_sprite(prev_f2)
	end
	canvas_set_palette("savage.pal", 0)
end

local function shaman_drop_anim(sprites, color)
	-- TODO - TIMING
	-- Arm Anim 4-9
	-- Drop Anim 10-18
	local prev_shaman = nil
	for i = 4,9,1 do
		sprites[i].visible = true
		if prev_shaman ~= nil then
			prev_shaman.visible = false
		end
		prev_shaman = sprites[i]
		canvas_update()
	end
	local prev_drop = nil
	for i = 10,26,1 do
		sprites[27+color].visible = true
		sprites[i].visible = true
		if prev_drop ~= nil then
			prev_drop.visible = false
		end
		prev_drop = sprites[i]
		canvas_update()
	end
	sprites[27+color].visible = false
	prev_drop.visible = false
	for i = 8,4,-1 do
		sprites[i].visible = true
		if prev_shaman ~= nil then
			prev_shaman.visible = false
		end
		prev_shaman = sprites[i]
		canvas_update()
	end
	prev_shaman.visible = false
	canvas_update()
end

-- Organization of questions (Red, Yellow, Blue)
-- Int (Red) vs Str (Yellow), Int (Red) vs Dex (Blue), Str (Yellow) vs. Dex (Blue)
-- 4 Lines for Each Question (nil on final question means no first screen)
-- Two sets of questions for each type, reversing answer order (Red vs. Yellow, then Yellow vs. Red)
local create_questions_text = {
"Your chief has told you to guard the village while a battle rages nearby. Your chief fights in the battle",
"Will you (a) obey the word of your chief, or (b) join your fellow warriors in the fight?",
nil,
nil,
"You have sworn to protect your chief at any cost. Yet you see him foully murder a warrior of his own tribe.",
"The murdered man's brother asks you what you saw.",
"Will you (a) keep your oath and protect your king, or (b) tell the truth of the matter?",
nil,
"Your chief gives you a pouch of gems to take to another; he has not counted them.",
"On the path to the other village, you find a lamed warrior who cannot hunt.",
"He could trade you a few of your gems for food, and they will not be missed.",
"Will you (a) deliver the gems as you were charged, or (b) give the warrior a gem?",
"One warrior borrows another's spear and fails to return it. Days later, he mislays his own spear and you find it.",
"Do you (a) return it to him, or (b) give it to the warrior who is owed the spear?",
nil,
nil,
"A huge, powerful warrior stands against you and demands you give him your food.",
"Will you (a) throw his demand in his teeth and attack him, or (b) give him your food, since it is clear he is hungry?",
nil,
nil,
"You fight a warrior you hate, and knock his spear from his hands.",
"Another blow and he will be dead.",
"Will you (a) let him surrender, and spare him if he does, or (b) slay him where he stands?",
nil,
}

local function ask_question(q_num, q_ord, sprites, text_image, images, text_width)
	-- Do I need to display two pages?
	local q_index = q_num * 2 + q_ord
	-- Do I need to display two pages?
	local two_questions = false
	if create_questions_text[q_index*4+3+1] ~= nil then two_questions = true end
	local txt_ind = 1
	local x,y
	if (two_questions) then
		image_blit(text_image, images[4][2], 0, 0)
		x, y = image_print(text_image, create_questions_text[q_index*4+txt_ind], 8, text_width, 8, 10, 0x00)
		x, y = image_print(text_image, create_questions_text[q_index*4+txt_ind+1], 8, text_width, 8, y+20, 0x00)
		canvas_update()
		wait_for_input()
		txt_ind = 3
	end
	image_blit(text_image, images[4][2], 0, 0)
	x, y = image_print(text_image, create_questions_text[q_index*4+txt_ind], 8, text_width, 8, 10, 0x00)
	x, y = image_print(text_image, create_questions_text[q_index*4+txt_ind+1], 8, text_width, 8, y+20, 0x00)
	if txt_ind ~= 3 then
		if create_questions_text[q_index*4+txt_ind+2] ~= nil then
			x, y = image_print(text_image, create_questions_text[q_index*4+txt_ind+2], 8, text_width, 8, y+20, 0x00)
		end
	end
	local answer = -1
	while answer == -1 do
		canvas_update()
		input = input_poll(true)
		if input ~= nil then
			if input == 97 or input == 98 then
				-- Some Math to get Answer from Question & Input
				local q_off = q_ord
				if (input == 98) then q_off = 1-q_off end
				if q_num == 2 then
					answer = q_off + 1
				else
					answer = q_num * q_off + q_off
				end
			end
		end
	end
	shaman_drop_anim(sprites, answer)
	return answer
end

g_keycode_tbl =
{
[32]=" ",
[39]="'",
[44]=",",
[45]="-",
[46]=".",
[48]="0",
[49]="1",
[50]="2",
[51]="3",
[52]="4",
[53]="5",
[54]="6",
[55]="7",
[56]="8",
[57]="9",
[65]="A",
[66]="B",
[67]="C",
[68]="D",
[69]="E",
[70]="F",
[71]="G",
[72]="H",
[73]="I",
[74]="J",
[75]="K",
[76]="L",
[77]="M",
[78]="N",
[79]="O",
[80]="P",
[81]="Q",
[82]="R",
[83]="S",
[84]="T",
[85]="U",
[86]="V",
[87]="W",
[88]="X",
[89]="Y",
[90]="Z",

[97]="a",
[98]="b",
[99]="c",
[100]="d",
[101]="e",
[102]="f",
[103]="g",
[104]="h",
[105]="i",
[106]="j",
[107]="k",
[108]="l",
[109]="m",
[110]="n",
[111]="o",
[112]="p",
[113]="q",
[114]="r",
[115]="s",
[116]="t",
[117]="u",
[118]="v",
[119]="w",
[120]="x",
[121]="y",
[122]="z",
}

	g_stats = {}	-- Int/Str/Dex

local function create_new_character(img_tbl2)
	local input
	canvas_set_palette("savage.pal", 1)
	local create_char_sprites = {}
	for i=0,2,1 do
		g_stats[i] = 16 + math.random(0,2)
	end
	create_char_sprites[1] = sprite_new(img_tbl2[1][2], 0, 0, true) -- Full Screen Border Around Shaman
	create_char_sprites[2] = sprite_new(img_tbl2[1][0], 8, 28, true) -- Create Character Shaman
	create_char_sprites[4] = sprite_new(img_tbl2[2][0], 8, 28, false) -- Shaman Arm Anim1
	create_char_sprites[5] = sprite_new(img_tbl2[2][1], 8, 28, false) -- Shaman Arm Anim2
	create_char_sprites[6] = sprite_new(img_tbl2[2][2], 8, 28, false) -- Shaman Arm Anim3
	create_char_sprites[7] = sprite_new(img_tbl2[2][3], 8, 28, false) -- Shaman Arm Anim4
	create_char_sprites[8] = sprite_new(img_tbl2[2][4], 8, 28, false) -- Shaman Arm Anim5
	create_char_sprites[9] = sprite_new(img_tbl2[2][5], 8, 28, false) -- Shaman Arm Anim6
	create_char_sprites[27] = sprite_new(img_tbl2[3][17], 8, 28, false) -- Red
	create_char_sprites[28] = sprite_new(img_tbl2[3][18], 8, 28, false) -- Yellow
	create_char_sprites[29] = sprite_new(img_tbl2[3][19], 8, 28, false) -- Blue
	create_char_sprites[10] = sprite_new(img_tbl2[3][0], 8, 28, false) -- Drop Anim
	create_char_sprites[11] = sprite_new(img_tbl2[3][1], 8, 28, false) -- 
	create_char_sprites[12] = sprite_new(img_tbl2[3][2], 8, 28, false) -- 
	create_char_sprites[13] = sprite_new(img_tbl2[3][3], 8, 28, false) -- 
	create_char_sprites[14] = sprite_new(img_tbl2[3][4], 8, 28, false) -- 
	create_char_sprites[15] = sprite_new(img_tbl2[3][5], 8, 28, false) -- 
	create_char_sprites[16] = sprite_new(img_tbl2[3][6], 8, 28, false) -- 
	create_char_sprites[17] = sprite_new(img_tbl2[3][7], 8, 28, false) -- 
	create_char_sprites[18] = sprite_new(img_tbl2[3][8], 8, 28, false) -- 
	create_char_sprites[19] = sprite_new(img_tbl2[3][9], 8, 28, false) -- 
	create_char_sprites[20] = sprite_new(img_tbl2[3][10], 8, 28, false) -- 
	create_char_sprites[21] = sprite_new(img_tbl2[3][11], 8, 28, false) -- 
	create_char_sprites[22] = sprite_new(img_tbl2[3][12], 8, 28, false) -- 
	create_char_sprites[23] = sprite_new(img_tbl2[3][13], 8, 28, false) -- 
	create_char_sprites[24] = sprite_new(img_tbl2[3][14], 8, 28, false) -- 
	create_char_sprites[25] = sprite_new(img_tbl2[3][15], 8, 28, false) -- 
	create_char_sprites[26] = sprite_new(img_tbl2[3][16], 8, 28, false) -- 
	create_char_sprites[3] = sprite_new(img_tbl2[1][1], 8, 28, true) -- Weird Border Around Shaman
	-- TODO BORDER SHADOW (Solid Black)
	-- TODO Memory Management on Image?
	-- Intro Screen1
	local scroll_img = image_copy(img_tbl2[4][2])
	local text_width = 130
	create_char_sprites[30] = sprite_new(scroll_img, 170, 25, true) -- Border
	local x, y = image_print(scroll_img, "You are in a dirt-floored hut... and a tribesman with wise eyes stares down at you.", 8, text_width, 8, 10, 0x00)
	x, y = image_print(scroll_img, "You feel as though you are floating. You cannot move your limbs. It is like a dream... yet you sense it is not a dream.", 8, text_width, 8, y+20, 0x00)
	canvas_update()
	wait_for_input()
	-- Intro Screen2
	image_blit(scroll_img, img_tbl2[4][2], 0, 0)
	local x, y = image_print(scroll_img, "The man speaks: \"Yes, warrior, you can hear. You see Intanya, shaman and healer. Intanya will heal you... but to concoct his healing potion, he must ask you questions.\"", 8, text_width, 8, 10, 0x00)
	canvas_update()
	wait_for_input()
	-- Intro Screen3
	image_blit(scroll_img, img_tbl2[4][2], 0, 0)
	x, y = image_print(scroll_img, "\"In order to heal your spirit, Intanya must know your spirit, so you must answer with truthful words.\"", 8, text_width, 8, 10, 0x00)
	x, y = image_print(scroll_img, "\"Intanya will speak of deeds and choices; you must answer with the choices you would make.\"", 8, text_width, 8, y+20, 0x00)
	canvas_update()
	wait_for_input()
	-- Name Input
	image_blit(scroll_img, img_tbl2[4][2], 0, 0)
	x, y = image_print(scroll_img, "\"First, though, Intanya must know the warrior's name, for there is much power in names.", 8, text_width, 8, 10, 0x00)
	x, y = image_print(scroll_img, "\"What does the warrior call himself?\"", 8, text_width, 8, y+20, 0x00)
	canvas_update()
	local name = sprite_new(nil, 178, 25+y+10, true)
	create_char_sprites[31] = name
	local num_sprites = 31
	name.text = ""
	name.text_color = 0x00
	while input == nil do
		canvas_update()
		input = input_poll()
		if input ~= nil then
			if should_exit(input) == true then
				destroy_sprites(create_char_sprites, num_sprites)
				canvas_set_palette("savage.pal", 0)
				return false
			end
			local name_text = name.text
			local len = string.len(name_text)
			if (input == 8 or input == 276) and len > 0 then
				name.text = string.sub(name_text, 1, len - 1)
				if len == 1 then -- old len
					char_index = 0
				else
					char_index = string.byte(name_text, len -1)
				end
			elseif input == 13 and len > 0 then --return
				break;
			elseif g_keycode_tbl[input] ~= nil and len < 13 then
				char_index = input
				name.text = name_text..g_keycode_tbl[input]
			elseif input == 273 then --up
				if char_index == 0 then
					if len > 0 then
						char_index = 97 --a
					else
						char_index = 65 --A
					end
				elseif char_index == 32 then --gap in characters
					char_index = 39
				elseif char_index == 39 then --gap in characters
					char_index = 44
				elseif char_index == 46 then --gap in characters
					char_index = 48
				elseif char_index == 57 then --gap in characters
					char_index = 65
				elseif char_index == 90 then --gap in characters
					char_index = 97
				elseif char_index == 122 then --last char
					char_index = 32
				else
					char_index = char_index + 1
				end

				if len > 0 then -- erase char
					name_text = string.sub(name_text, 1, len - 1)
				end
				name.text = name_text..g_keycode_tbl[char_index]
			elseif input == 274 then --down
				if char_index == 0 then
					if len > 0 then
						char_index = 122 --z
					else
						char_index = 90 --Z
					end
				elseif char_index == 39 then --gap in characters
					char_index = 32
				elseif char_index == 44 then --gap in characters
					char_index = 39
				elseif char_index == 48 then --gap in characters
					char_index = 46
				elseif char_index == 65 then --gap in characters
					char_index = 57
				elseif char_index == 97 then --gap in characters
					char_index = 90
				elseif char_index == 32 then --first char
					char_index = 122
				else
					char_index = char_index - 1
				end

				if len > 0 then -- erase char
					name_text = string.sub(name_text, 1, len - 1)
				end
				name.text = name_text..g_keycode_tbl[char_index]
			elseif input == 275 and len < 13 then --right
				char_index = 97 --a
				name.text = name_text.."a"
			end
			input = nil
		end
	end

--	name.x = 0x10 + (284 - canvas_string_length(name.text)) / 2
	name.visible = false

	-- Questions
	local q1,q2,q3,q1d,q2d,q3d
	q1 = math.random(0,2)
	q1d = math.random(0,1)
	q2d = math.random(0,1)
	q3d = math.random(0,1)
	-- Answers are 0-Red, 1-Yellow, 2-Blue (order of sprites in file)
	local a1 = ask_question(q1, q1d, create_char_sprites, scroll_img, img_tbl2, text_width)
	-- TODO REVERSE ENGINEER STATS MECHANISM
	-- Currently randomly starts 16-18, then adds 3+(0-2) for each question
	-- When using SE, values started 16-18, 19-23, and 22-28
	-- The algorithm I'm using changes the last value to 22-29
	-- This is close enough for right now
	g_stats[a1] = g_stats[a1] + 3 + math.random(0,2)
	if a1 == 0 then
		q2 = 2
	elseif a1 == 1 then
		q2 = 1
	else
		q2 = 0
	end
	local a2 = ask_question(q2, q2d, create_char_sprites, scroll_img, img_tbl2, text_width)
	g_stats[a2] = g_stats[a2] + 3 + math.random(0,2)
	-- This formula points to the right question
	q3 = a1 + a2 - 1
	-- If same question as Q1, ask the opposite question
	if q1 == q3 then q3d = 1-q3d end
	local a3 = ask_question(q3, q3d, create_char_sprites, scroll_img, img_tbl2, text_width)
	g_stats[a3] = g_stats[a3] + 3 + math.random(0,2)

	config_set("config/newgame", true)
	config_set("config/newgamedata/name", name.text)
	config_set("config/newgamedata/str", g_stats[1])
	config_set("config/newgamedata/dex", g_stats[2])
	config_set("config/newgamedata/int", g_stats[0])
	config_set("config/newgamedata/hitpoints", 56+(g_stats[1]-16)*2)

	destroy_sprites(create_char_sprites, num_sprites)
	canvas_set_palette("savage.pal", 0)
end

-- Could/should get heights from sprite, but was getting error when trying
local credit_heights = {
      182, 157, 100, 188, 136, 193, 183, 176, 97, 190, 190, 50
}

local function about_the_savage_empire(img_tbl2)
	local next_sprite = 3
	local ypos = 200
	local about_sprites = {}
	-- TODO Set Speed to appropriate scroll speed
	local speed = 2
	local s1 = sprite_new(img_tbl2[6][0],   100, ypos, true)
	about_sprites[1] = s1
	ypos = ypos + credit_heights[1]
	local s2 = sprite_new(img_tbl2[6][1],   100, ypos, true)
	about_sprites[2] = s1
	ypos = ypos + credit_heights[2]
	local s3 = sprite_new(img_tbl2[6][2],   100, ypos, true)
	about_sprites[3] = s1
	ypos = ypos + credit_heights[3]
	local s4 = sprite_new(img_tbl2[6][3],   100, ypos, true)
	about_sprites[4] = s1
	ypos = ypos + credit_heights[4]
	local s5 = sprite_new(img_tbl2[6][4],   100, ypos, true)
	about_sprites[5] = s1
	ypos = ypos + credit_heights[5]
	local s6 = sprite_new(img_tbl2[6][5],   100, ypos, true)
	about_sprites[6] = s1
	ypos = ypos + credit_heights[6]
	local s7 = sprite_new(img_tbl2[6][6],   100, ypos, true)
	about_sprites[7] = s1
	ypos = ypos + credit_heights[7]
	local s8 = sprite_new(img_tbl2[6][7],   100, ypos, true)
	about_sprites[8] = s1
	ypos = ypos + credit_heights[8]
	local s9 = sprite_new(img_tbl2[6][8],   100, ypos, true)
	about_sprites[9] = s1
	ypos = ypos + credit_heights[9]
	local s10 = sprite_new(img_tbl2[6][9],  100, ypos, true)
	about_sprites[10] = s1
	ypos = ypos + credit_heights[10]
	local s11 = sprite_new(img_tbl2[6][10], 100, ypos, true)
	about_sprites[11] = s1
	ypos = ypos + credit_heights[11]
	local s12 = sprite_new(img_tbl2[6][11], 100, ypos, true)
	about_sprites[12] = s1
	ypos = ypos + credit_heights[12]
	local done = 0
	-- TODO SET CLIP RECT, Approximately line 75
	while done < ypos do
		canvas_update()
		s1.y = s1.y - speed
		s2.y = s2.y - speed
		s3.y = s3.y - speed
		s4.y = s4.y - speed
		s5.y = s5.y - speed
		s6.y = s6.y - speed
		s7.y = s7.y - speed
		s8.y = s8.y - speed
		s9.y = s9.y - speed
		s10.y = s10.y - speed
		s11.y = s11.y - speed
		s12.y = s12.y - speed
		done = done + speed
		if poll_for_key_or_button() == true then
			break
		end
	end
	destroy_sprites(about_sprites, 12)
end

local function main_menu(img_tbl2)
	local input
	local g_menu_idx = 0
	local seTitle = sprite_new(img_tbl2[0][0], 0, 0, true)
	local menu_sprites = {}
	local menu1 = sprite_new(img_tbl2[0][5], 85, 117, true) -- The Story So far...
	local menu2 = sprite_new(img_tbl2[0][1], 85, 135, true) -- Create New Character
	local menu3 = sprite_new(img_tbl2[0][9], 85, 153, true) -- About the Savage Empire
	local menu4 = sprite_new(img_tbl2[0][3], 85, 171, true) -- Journey Onward
	local m1_focus = sprite_new(img_tbl2[0][6], 85, 117, true) -- The Story So far...
	local m2_focus = sprite_new(img_tbl2[0][2], 85, 135, false) -- Create New Character
	local m3_focus = sprite_new(img_tbl2[0][10], 85, 153, false) -- About the Savage Empire
	local m4_focus = sprite_new(img_tbl2[0][4], 85, 171, false) -- Journey Onward
	local m1_highlight = sprite_new(img_tbl2[0][13], 85, 117, false) -- The Story So far...
	local m2_highlight = sprite_new(img_tbl2[0][11], 85, 135, false) -- Create New Character
	local m3_highlight = sprite_new(img_tbl2[0][15], 85, 153, false) -- About the Savage Empire
	local m4_highlight = sprite_new(img_tbl2[0][12], 85, 171, false) -- Journey Onward
	menu_sprites[1] = menu1
	menu_sprites[2] = menu2
	menu_sprites[3] = menu3
	menu_sprites[4] = menu4
	menu_sprites[5] = m1_highlight
	menu_sprites[6] = m2_highlight
	menu_sprites[7] = m3_highlight
	menu_sprites[8] = m4_highlight
	menu_sprites[9] = m1_focus
	menu_sprites[10] = m2_focus
	menu_sprites[11] = m3_focus
	menu_sprites[12] = m4_focus
	while true do
		if g_menu_idx == 0 then
			m1_focus.visible = true
			m2_focus.visible = false
			m3_focus.visible = false
			m4_focus.visible = false
		elseif g_menu_idx == 1 then
			m1_focus.visible = false
			m2_focus.visible = true
			m3_focus.visible = false
			m4_focus.visible = false
		elseif g_menu_idx == 2 then
			m1_focus.visible = false
			m2_focus.visible = false
			m3_focus.visible = true
			m4_focus.visible = false
		elseif g_menu_idx == 3 then
			m1_focus.visible = false
			m2_focus.visible = false
			m3_focus.visible = false
			m4_focus.visible = true
		end
		canvas_update()
		input = input_poll(true)
		if input ~= nil then
			-- TODO ADD MOUSE BUTTONS
			if input == 113 then
				return "Q"
				-- TODO Add Space & Return
			elseif input == 13 then --or input == 21 then -- space or return
				if g_menu_idx == 0 then
					menu1.visible = false
					m1_focus.visible = false
					m1_highlight.visible = true
					canvas_update()
					story_so_far(img_tbl2)
					menu1.visible = true
					m1_focus.visible = true
					m1_highlight.visible = false
				elseif g_menu_idx == 1 then
					create_new_character(img_tbl2)
				elseif g_menu_idx == 2 then
					menu3.visible = false
					m3_focus.visible = false
					m3_highlight.visible = true
					canvas_update()
					fade_out_sprites(menu_sprites, 50, 12)
					about_the_savage_empire(img_tbl2)
					menu3.visible = true
					m3_focus.visible = true
					m3_highlight.visible = false
					fade_in_sprites(menu_sprites, 50, 12)
				elseif g_menu_idx == 3 then
					menu4.visible = false
					m4_focus.visible = false
					m4_highlight.visible = true
					canvas_update()
					return "J"
				end
			elseif input == 274 then -- down key
				g_menu_idx = g_menu_idx + 1
				if (g_menu_idx == 4) then g_menu_idx = 0 end
			elseif input == 273 then -- up key
				g_menu_idx = g_menu_idx - 1
				if (g_menu_idx == -1) then g_menu_idx = 3 end
			end
		end
	end
	wait_for_input()
	menu1.visible = false
	menu2.visible = false
	menu3.visible = false
	menu4.visible = false
--	canvas_set_palette("savage.pal", 1)
--	local  e4 = sprite_new(img_tbl2[4][3], 0, 0, true) -- Small Head/Eyes
--	local  e5 = sprite_new(img_tbl2[4][4], 50, 0, true) -- Truth
--	local  e6 = sprite_new(img_tbl2[4][5], 100, 0, true) -- Small Head/Eyes
--	local  e7 = sprite_new(img_tbl2[4][6], 150, 0, true) -- Love
--	local  e8 = sprite_new(img_tbl2[4][7], 100, 0, true) -- Small Head/Eyes
--	local  e9 = sprite_new(img_tbl2[4][8], 100, 0, true) -- Full Top
--	local  e10 = sprite_new(img_tbl2[4][9], 100, 0, true) -- Full Top2
--	local  h1 = sprite_new(img_tbl2[7][0], 0, 0, true) -- Small Top1 (Correct Palette?)
--	local  h2 = sprite_new(img_tbl2[7][1], 50, 50, true) -- Small Top2
end

canvas_set_update_interval(25)
canvas_set_bg_color(0)
canvas_set_opacity(0)

--TODO Fireworks
origin_fx_sequence()
	
fade_out()

-- Load Graphics for Intro & Main Menu
local g_img_tbl = image_load_all("intro.lzc")
local img_tbl2 = image_load_all("create.lzc")
intro_sequence(g_img_tbl)

main_menu(img_tbl2)

canvas_hide_all_sprites()
canvas_hide()