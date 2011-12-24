g_img_tbl = {}

function poll_for_esc()
	input = input_poll()
	if input ~= nil and input == 27 then
		return true
	end
	
	return false
end

function should_exit(input)
	if input ~=nil and input == 27 then
		return true
	end
	
	return false
end

function fade_out()
	local i
	for i=0xff,0,-3 do
		canvas_set_opacity(i)
		canvas_update()
	end
	
	return false
end

function fade_out_sprite(sprite, speed)
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

function fade_in()
	local i
	for i=0x0,0xff,3 do
		canvas_set_opacity(i)
		canvas_update()
	end

	return false
end

function fade_in_sprite(sprite, speed)
	local i
	if speed ~= nil then
		speed = speed
	else
		speed = 3
	end

	for i=0,0xff,speed do
		sprite.opacity = i
		canvas_update()
	end

	return false
end

function load_images(filename)
	g_img_tbl = image_load_all(filename)
end

g_clock_tbl = {}

function load_clock()
	g_clock_tbl["h1"] = sprite_new(nil, 0xdd, 0x14, true)
	g_clock_tbl["h2"] = sprite_new(nil, 0xdd+4, 0x14, true)
	g_clock_tbl["m1"] = sprite_new(nil, 0xdd+0xa, 0x14, true)
	g_clock_tbl["m2"] = sprite_new(nil, 0xdd+0xe, 0x14, true)
end

function display_clock()
	local t = os.date("*t")
	local hour = t.hour
	local minute = t.min
	
	if hour > 12 then
		hour = hour - 12
	end
	
	if hour < 10 then
		g_clock_tbl["h1"].image = g_img_tbl[12]
	else
		g_clock_tbl["h1"].image = g_img_tbl[3]
	end
	
	if hour >= 10 then
		hour = hour - 10
	end

	g_clock_tbl["h2"].image = g_img_tbl[hour+2]

	g_clock_tbl["m1"].image = g_img_tbl[math.floor(minute / 10) + 2]
	g_clock_tbl["m2"].image = g_img_tbl[(minute % 10) + 2]
		
end

g_lounge_tbl = {}

g_tv_base_x = 0xe5
g_tv_base_y = 0x32

function load_lounge()

	g_lounge_tbl["bg0"] = sprite_new(g_img_tbl[15], 210, 0, true)
	g_lounge_tbl["bg"] = sprite_new(g_img_tbl[0], 0, 0, true)
	g_lounge_tbl["bg1"] = sprite_new(g_img_tbl[1], 320, 0, true)
	g_lounge_tbl["avatar"] = sprite_new(g_img_tbl[0xd], 0, 63, true)

	g_lounge_tbl["tv"] = {sprite_new(nil, g_tv_base_x ,g_tv_base_y, true), sprite_new(nil, g_tv_base_x ,g_tv_base_y, true), sprite_new(nil, g_tv_base_x ,g_tv_base_y, true), sprite_new(nil, g_tv_base_x ,g_tv_base_y, true), sprite_new(nil, g_tv_base_x ,g_tv_base_y, true)}

	for i=1,5 do
		local sprite = g_lounge_tbl["tv"][i]
		sprite.clip_x = g_tv_base_x
		sprite.clip_y = g_tv_base_y
		sprite.clip_w = 57
		sprite.clip_h = 37
	end

	g_lounge_tbl["finger"] = sprite_new(g_img_tbl[0xe], 143, 91, true)
	
	g_lounge_tbl["tv_static"] = image_new(57,37)
	
	load_clock()
end

g_tv_loop_pos = 0
g_tv_loop_cnt = 0
g_tv_cur_pos = 1
g_tv_cur_program = 3

g_tv_news_image = 0
g_tv_news_image_tbl = {0x5,0x6,0x7,0x0D,0x0E,0x18,0x19,0x1F,0x20}

g_tv_programs = {
{0x82,0x82,0x80,0x3,0x2,0x8A,0x2,0x8A,0x1,0x8A,0x1,0x8A,0x0,0x8A,0x0,0x8A,0x1,0x8A,0x1,0x81}, --breathing
{0x82,0x82,0x80,0x28,0x3,0x8B,0x81}, --intestines
{0x82,0x82,0x80,0x4,0x4,0x81,0x80,0x4,0x8,0x81,0x80,0x4,0x9,0x81,0x80,0x4,0x0A,0x81,0x80,0x4,0x0B,0x81,0x80,0x4,0x0C,0x81}, --vacuum
{0x82,0x82,0x87,0x80,0x46,0x0F,0x86,0x84,0x9,0x10,0x10,0x10,0x11,0x11,0x11,0x12,0x12,0x12,0x81}, --tv anchor man
{0x82,0x82,0x80,0x32,0x83,0x81}, --endless road
{0x82,0x82,0x80,0x5,0x27,0x8A,0x28,0x8A,0x29,0x81,0x80,0x6,0x2A,0x8A,0x2A,0x8A,0x2B,0x8A,0x2B,0x8A,0x2C,0x8A,0x2C,0x8A,0x2D,0x8A,0x2D,0x81,0x80,0x0A,0x2E,0x8A,0x2F,0x8A,0x30,0x8A,0x84,0x9,0x2E,0x2E,0x2E,0x2F,0x2F,0x2F,0x30,0x30,0x30,0x8A,0x2E,0x8A,0x2F,0x8A,0x30,0x81}, --rock band
{0x82,0x82,0x80,0x55,0x16,0x17,0x84,0x0C,0x13,0x13,0x13,0x13,0x14,0x14,0x14,0x14,0x15,0x15,0x15,0x15,0x88,0x81,0x80,0x0F,0x16,0x84,0x2,0x1A,0x1B,0x89,0x88,0x81,0x80,0x3,0x16,0x1A,0x89,0x88,0x81,0x80,0x3,0x16,0x1C,0x88,0x81,0x80,0x3,0x16,0x1D,0x88,0x81,0x80,0x3,0x16,0x1E,0x88,0x81,0x80,0x3,0x16,0x23,0x88,0x81,0x80,0x3,0x16,0x24,0x88,0x81,0x80,0x32,0x16,0x88,0x81} --pledge now!
}


g_tv_x_off = {
0x0,0x0,0x0,0x0,0x0,0x1F,0x1F,0x1F,0x0,0x0,0x0,0x0,0x0,0x1F,0x1F,
0x0,0x9,0x9,0x9,0x0C,0x0C,0x0C,0x0,0x4,0x1F,0x1F,0x4,0x0,0x4,
0x4,0x4,0x1F,0x1F,0x0,0x0,0x6,0x6,0x8,0x4,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
}

g_tv_y_off = {
0x0,0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x0,0x0,0x0,0x0,0x0,0x2,0x2,0x0,0x7,0x7,
0x7,0x3,0x3,0x3,0x0,0x2,0x2,0x2,0x0,0x0,0x0,0x0,0x1,0x2,0x2,0x0,0x0,0x3,
0x8,0x1D,0x1C,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0
}

g_tv_pledge_counter = 0
g_tv_pledge_image = 37

g_tv_road_offset = 0xe

function display_tv_sprite(s_idx, image_num)
	local sprite = g_lounge_tbl.tv[s_idx]
	if sprite ~= nil then
		sprite.image = g_img_tbl[0x10+image_num]
		sprite.x = g_tv_base_x + g_tv_x_off[image_num+1]
		sprite.y = g_tv_base_y + g_tv_y_off[image_num+1]
		sprite.clip_x = g_tv_base_x
		sprite.clip_y = g_tv_base_y
		sprite.visible = true
	end
end

function display_tv()
	local should_exit = false
	local s_idx = 1
	for i=1,5 do
		g_lounge_tbl["tv"][i].visible = false
	end
	
	g_lounge_tbl.finger.visible = false
	
	while should_exit == false do
		local item = g_tv_programs[g_tv_cur_program][g_tv_cur_pos]
		if item < 0x80 then
			display_tv_sprite(s_idx, item)
			s_idx = s_idx + 1
		elseif item == 0x82 then
			--static
			local sprite = g_lounge_tbl.tv[s_idx]
			if sprite ~= nil then
				sprite.image = g_lounge_tbl["tv_static"]
				image_static(sprite.image)
				sprite.x = g_tv_base_x
				sprite.y = g_tv_base_y
				sprite.clip_x = g_tv_base_x
				sprite.clip_y = g_tv_base_y
				sprite.visible = true
			end
			g_lounge_tbl.finger.visible = true
			should_exit = true
		elseif item == 0x80 then --loop start
			g_tv_cur_pos = g_tv_cur_pos + 1
			g_tv_loop_cnt = g_tv_programs[g_tv_cur_program][g_tv_cur_pos]
			g_tv_loop_pos = g_tv_cur_pos
		elseif item == 0x81 then --loop end
			if g_tv_loop_cnt > 0 then
				g_tv_cur_pos = g_tv_loop_pos
				g_tv_loop_cnt = g_tv_loop_cnt - 1
			end
			should_exit = true
		elseif item == 0x83 then --endless road
			g_tv_road_offset = g_tv_road_offset - 1
			if g_tv_road_offset == 0 then
				g_tv_road_offset = 0xe
			end
			g_tv_y_off[0x23] = 0x15 - g_tv_road_offset
			display_tv_sprite(s_idx, 0x22)
			s_idx = s_idx + 1
			g_tv_y_off[0x23] = 0x24 - g_tv_road_offset
			display_tv_sprite(s_idx, 0x22)
			s_idx = s_idx + 1
			display_tv_sprite(s_idx, 0x21)
			s_idx = s_idx + 1
		elseif item == 0x84 then --select random image from list.
			local rand_len = g_tv_programs[g_tv_cur_program][g_tv_cur_pos+1]
			item = g_tv_programs[g_tv_cur_program][g_tv_cur_pos+math.random(1,rand_len)+1]
			display_tv_sprite(s_idx, item)
			s_idx = s_idx + 1
			g_tv_cur_pos = g_tv_cur_pos + 1 + rand_len
		elseif item == 0x86 then --display news image
			display_tv_sprite(s_idx, g_tv_news_image)
			s_idx = s_idx + 1
		elseif item == 0x87 then --select news image
			g_tv_news_image = g_tv_news_image_tbl[math.random(1, 9)]
		elseif item == 0x88 then --pledge now!
			g_tv_pledge_counter = g_tv_pledge_counter + 1
			if g_tv_pledge_counter % 4 == 0 then
				display_tv_sprite(s_idx, g_tv_pledge_image)
				s_idx = s_idx + 1
			end
			
			if g_tv_pledge_counter == 16 then
				if g_tv_pledge_image == 37 then
					g_tv_pledge_image = 38
				else
					g_tv_pledge_image = 37
				end
				g_tv_pledge_counter = 0
			end
		elseif item == 0x89 then
			display_tv_sprite(s_idx, math.random(50, 52))
			s_idx = s_idx + 1
		elseif item == 0x8a then
			should_exit = true
		elseif item == 0x8b then
			canvas_rotate_palette(0xe0, 5)
		end
		
		g_tv_cur_pos = g_tv_cur_pos + 1
		if g_tv_programs[g_tv_cur_program][g_tv_cur_pos] == nil then
			g_tv_cur_program = g_tv_cur_program + 1
			g_tv_cur_pos = 1
			
			if g_tv_programs[g_tv_cur_program] == nil then
				g_tv_cur_program = 1
			end
		end
	end
end

g_tv_update = true

function display_lounge()
	display_clock()
	if g_tv_update == true then
		display_tv()
		g_tv_update = false
	else
		g_tv_update = true
	end
end

function scroll_lounge()
g_lounge_tbl.bg.x = g_lounge_tbl.bg.x - 1
g_lounge_tbl.bg1.x = g_lounge_tbl.bg1.x - 1
g_tv_base_x = g_tv_base_x - 1
g_lounge_tbl.avatar.x = g_lounge_tbl.avatar.x - 2
g_lounge_tbl.finger.x = g_lounge_tbl.finger.x - 2

if g_lounge_tbl.bg1.x < 166 and g_lounge_tbl.bg1.x % 2 == 0 then
	g_lounge_tbl.bg0.x = g_lounge_tbl.bg0.x - 1
end

scroll_clock()
end

function scroll_clock()
g_clock_tbl["h1"].x = g_clock_tbl["h1"].x - 1
g_clock_tbl["h2"].x = g_clock_tbl["h2"].x - 1
g_clock_tbl["m1"].x = g_clock_tbl["m1"].x - 1
g_clock_tbl["m2"].x = g_clock_tbl["m2"].x - 1
end

function hide_lounge()

	g_lounge_tbl["bg0"].visible = false
	g_lounge_tbl["bg"].visible = false
	g_lounge_tbl["bg1"].visible = false
	g_lounge_tbl["avatar"].visible = false
	g_lounge_tbl["tv"].visible = false
end

function lounge_sequence()

	load_lounge()

	canvas_set_palette("palettes.int", 1)

	--for i=0,0xff do
		--display_lounge()
		--canvas_update()
	--end
	local scroll_img = image_load("blocks.shp", 1)
	local scroll = sprite_new(scroll_img, 1, 0x9f, true)

	local x, y = image_print(scroll_img, "Upon your world, five seasons have passed since your ", 8, 312, 8, 10, 0x3e)
	image_print(scroll_img, "triumphant homecoming from Britannia.", 8, 312, x, y, 0x3e)

	local input = input_poll()

	while input == nil do
		display_lounge()
		canvas_update()
		input = input_poll()
	end
	
	if should_exit(input) then
		return false
	end
	
	scroll_img = image_load("blocks.shp", 2)
	x, y = image_print(scroll_img, "You have traded the Avatar's life of peril and adventure ", 7, 310, 7, 8, 0x3e)
	x, y = image_print(scroll_img, "for the lonely serenity of a world at peace. But ", 7, 310, x, y, 0x3e)
	x, y = image_print(scroll_img, "television supermen cannot take the place of friends ", 7, 310, x, y, 0x3e)
	image_print(scroll_img, "who died at your side!", 7, 310, x, y, 0x3e)

	scroll.image = scroll_img
	scroll.x = 1
	scroll.y = 0x98

	input = input_poll()

	while input == nil do
		display_lounge()
		canvas_update()
		input = input_poll()
	end
	
	if should_exit(input) then
		return false
	end

	scroll_img = image_load("blocks.shp", 0)
	image_print(scroll_img, "Outside, a chill wind rises...", 39, 200, 39, 8, 0x3e)

	scroll.image = scroll_img
	scroll.x = 0x21
	scroll.y = 0x9d

	for i=0,319 do
		scroll_lounge()
		display_lounge()
		canvas_update()
		input = input_poll()
		if should_exit(input) then
			return false
		end
	end

	input = input_poll()
	while input == nil do
		canvas_update()
		input = input_poll()
	end

	if should_exit(input) then
		return false
	end
	
	hide_lounge()
	scroll.visible = false
	
	return true
end

g_window_tbl = {}
function load_window()
	local rand = math.random
	g_window_tbl["cloud_x"] = -400
	
	g_window_tbl["sky"] = sprite_new(g_img_tbl[1], 0, 0, true)
	g_window_tbl["cloud1"] = sprite_new(g_img_tbl[0], g_window_tbl["cloud_x"], -5, true)
	g_window_tbl["cloud2"] = sprite_new(g_img_tbl[0], g_window_tbl["cloud_x"], -5, true)
	g_window_tbl["clouds"] = {}
	
	local i
	for i=1,5 do
		table.insert(g_window_tbl["clouds"], sprite_new(g_img_tbl[rand(2,3)], rand(0,320) - 260, rand(0, 30), true))
	end
	
	g_window_tbl["lightning"] = sprite_new(nil, 0, 0, false)
	g_window_tbl["ground"] = sprite_new(g_img_tbl[10], 0, 0x4c, true)
	g_window_tbl["trees"] = sprite_new(g_img_tbl[8], 0, 0, true)

	g_window_tbl["strike"] = sprite_new(g_img_tbl[rand(19,23)], 158, 114, false)
	

	
	--FIXME rain here.
	local rain = {}
	local i
	for i = 0,100 do
		rain[i] = sprite_new(g_img_tbl[math.random(4,7)], math.random(0,320), math.random(0,200), false)
	end
	g_window_tbl["rain"] = rain
	
	g_window_tbl["frame"] = sprite_new(g_img_tbl[28], 0, 0, true)	
	g_window_tbl["window"] = sprite_new(g_img_tbl[26], 0x39, 0, true)
	g_window_tbl["door_left"] = sprite_new(g_img_tbl[24], 320, 0, true)
	g_window_tbl["door_right"] = sprite_new(g_img_tbl[25], 573, 0, true)
		
	g_window_tbl["flash"] = 0
	g_window_tbl["drops"] = 0
	g_window_tbl["rain_delay"] = 20
	g_window_tbl["lightning_counter"] = 0
	
end

function hide_window()
	g_window_tbl["sky"].visible = false
	g_window_tbl["cloud1"].visible = false
	g_window_tbl["cloud2"].visible = false

	local i
	for i=1,5 do
		g_window_tbl["clouds"][i].visible = false
	end

	g_window_tbl["lightning"].visible = false
	g_window_tbl["ground"].visible = false
	g_window_tbl["trees"].visible = false

	g_window_tbl["strike"].visible = false

	local i
	for i = 0,100 do
		g_window_tbl["rain"][i].visible = false
	end

	g_window_tbl["frame"].visible = false
	g_window_tbl["window"].visible = false
	g_window_tbl["door_left"].visible = false
	g_window_tbl["door_right"].visible = false

end

function display_window()

	local i
	local rain = g_window_tbl["rain"]
	local rand = math.random
	local c_num

	--FIXME display clouds here.
	local cloud
	for i,cloud in ipairs(g_window_tbl["clouds"]) do
		if cloud.x > 320 then
			cloud.x = rand(0, 320) - 320
			cloud.y = rand(0, 30)
		end
		
		cloud.x = cloud.x + 2
	end

	g_window_tbl["cloud_x"] = g_window_tbl["cloud_x"] + 1
	if g_window_tbl["cloud_x"] == 320 then
		g_window_tbl["cloud_x"] = 0
	end
	
	g_window_tbl["cloud1"].x = g_window_tbl["cloud_x"]
	g_window_tbl["cloud2"].x = g_window_tbl["cloud_x"] - 320
	
	if rand(0, 6) == 0 and g_window_tbl["lightning_counter"] == 0 then --fixme var_1a, var_14
		g_window_tbl["lightning_counter"] = rand(1, 4)
		g_window_tbl["lightning"].image = g_img_tbl[rand(11,18)]
		g_window_tbl["lightning"].visible = true
		g_window_tbl["lightning_x"] = rand(-5,320)
		g_window_tbl["lightning_y"] = rand(-5,200)
	end

	if g_window_tbl["lightning_counter"] > 0 then
		g_window_tbl["lightning"].x = g_window_tbl["lightning_x"] + rand(0, 3)
		g_window_tbl["lightning"].y = g_window_tbl["lightning_y"] + rand(0, 3)
	end

	if (g_window_tbl["lightning_counter"] > 0 and rand(0,3) == 0) or g_window_tbl["strike"].visible == true then
		canvas_set_palette_entry(0x58, 0x40,0x94,0xfc)
		canvas_set_palette_entry(0x5a, 0x40,0x94,0xfc)
		canvas_set_palette_entry(0x5c, 0x40,0x94,0xfc)
	else
		canvas_set_palette_entry(0x58, 0x20,0xa4,0x80)
		canvas_set_palette_entry(0x5a, 0x14,0x8c,0x74)
		canvas_set_palette_entry(0x5c, 0x0c,0x74,0x68)
	end
	
	if rand(0,1) == 0 then
		g_window_tbl["strike"].image = g_img_tbl[rand(19,23)]
	end
	
	if g_window_tbl["flash"] > 0 then
		g_window_tbl["flash"] = g_window_tbl["flash"] - 1
	else
		if rand(0,5) == 0 or g_window_tbl["strike"].visible == true then
			g_window_tbl["window"].image = g_img_tbl[27]
			g_window_tbl["flash"] = rand(1, 5)
		else
			g_window_tbl["window"].image = g_img_tbl[26]
		end
	end

	if g_window_tbl["drops"] < 100 then
		if rand(0,g_window_tbl["rain_delay"]) == 0 then
			g_window_tbl["rain_delay"] = g_window_tbl["rain_delay"] - 2
			if g_window_tbl["rain_delay"] < 1 then
				g_window_tbl["rain_delay"] = 1
			end
			i = g_window_tbl["drops"]
			rain[i].visible = true
			rain[i].image = g_img_tbl[rand(4,7)]
			rain[i].y = -4
			rain[i].x = rand(0,320)
			g_window_tbl["drops"] = i + 1
		end
	end
	
	for i = 0,g_window_tbl["drops"] do

		if rain[i].visible == true then
			rain[i].x = rain[i].x + 2
			rain[i].y = rain[i].y + 8
			if rain[i].x > 320 or rain[i].y > 200 then
				rain[i].visible = false
			end
		else
			rain[i].visible = true
			rain[i].image = g_img_tbl[rand(4,7)]
			rain[i].y = -4
			rain[i].x = rand(0,320)
		end
	end
	
	if g_window_tbl["lightning_counter"] > 0 then
		g_window_tbl["lightning_counter"] = g_window_tbl["lightning_counter"] - 1
	end
end

function window_update()
	local input = input_poll()
	
	while input == nil do
		display_window()
		canvas_update()
		input = input_poll()
		if input ~= nil then
			break
		end
		canvas_update()
	end

	return should_exit(input)
end

function window_sequence()
	load_images("intro_2.shp")
	
	load_window()
	
	canvas_set_palette("palettes.int", 2)
	
	local i = 0
	local input
	while i < 20 do
		display_window()
		canvas_update()
		input = input_poll()
		if should_exit(input) then
			return false
		end
		
		i = i + 1
		canvas_update()
	end
	
	local scroll_img = image_load("blocks.shp", 1)
	local scroll = sprite_new(scroll_img, 1, 0x98, true)
	
	local x, y = image_print(scroll_img, "...and in moments, the storm is upon you.", 8, 312, 36, 14, 0x3e)

	if window_update() == true then
		return false
	end
	
	scroll_img = image_load("blocks.shp", 1)
	x, y = image_print(scroll_img, "Tongues of lightning lash the sky, conducting an unceasing ", 8, 310, 8, 10, 0x3e)
	image_print(scroll_img, "crescendo of thunder....", 8, 310, x, y, 0x3e)
	scroll.image = scroll_img
	
	if window_update() == true then
		return false
	end

	scroll_img = image_load("blocks.shp", 1)
	x, y = image_print(scroll_img, "In a cataclysm of sound and light, a bolt of searing ", 8, 310, 8, 10, 0x3e)
	image_print(scroll_img, "blue fire strikes the earth!", 8, 310, x, y, 0x3e)
	scroll.image = scroll_img
	
	g_window_tbl["strike"].visible = true
		
	if window_update() == true then
		return false
	end
	
	scroll_img = image_load("blocks.shp", 1)
	x, y = image_print(scroll_img, "Lightning among the stones!", 8, 310, 73, 10, 0x3e)
	image_print(scroll_img, "Is this a sign from distant Britannia?", 8, 310, 41, 18, 0x3e)
	scroll.image = scroll_img
		
	--scroll window.
	i = 0
	while i < 320 do

		display_window()
		canvas_update()
		input = input_poll()
		if should_exit(input) then
			return false
		end
	
		canvas_update()
	
		g_window_tbl["window"].x = g_window_tbl["window"].x - 2
		g_window_tbl["frame"].x = g_window_tbl["frame"].x - 2
		g_window_tbl["door_left"].x = g_window_tbl["door_left"].x - 2
		g_window_tbl["door_right"].x = g_window_tbl["door_right"].x - 2
		i = i + 2
		if i > 160 and g_window_tbl["strike"].visible == true then
			g_window_tbl["strike"].visible = false
		end
	end
	
	if window_update() == true then
		return false
	end
	
	scroll.visible = false
	i = 0
	while i < 68 do
	
		display_window()
		canvas_update()
		input = input_poll()
		if should_exit(input) then
			return false
		end
	
		canvas_update()
		input = input_poll()
	
		g_window_tbl["door_left"].x = g_window_tbl["door_left"].x - 1
		g_window_tbl["door_right"].x = g_window_tbl["door_right"].x + 1
		i = i + 1
	end

	scroll_img = image_load("blocks.shp", 2)
	x, y = image_print(scroll_img, "You bolt from your house, stumbling, running blind in the", 7, 310, 8, 12, 0x3e)
	x, y = image_print(scroll_img, " storm. Into the forest, down the path, through the ", 7, 310, x, y, 0x3e)
	image_print(scroll_img, "rain... to the stones.", 7, 310, x, y, 0x3e)
	scroll.image = scroll_img
	scroll.visible = true

	if window_update() == true then
		return false
	end

	scroll.visible = false
	
end

function stones_update()
	local input = input_poll()

	while input == nil do
		stones_rotate_palette()
		canvas_update()
		input = input_poll()
		if input ~= nil then
			break
		end
	end

	return should_exit(input)
end

function stones_shake_moongate()
	local input = input_poll()

	while input == nil do
		stones_rotate_palette()
		g_stones_tbl["moon_gate"].x = 0x7c + math.random(0, 1)
		g_stones_tbl["moon_gate"].y = 5 + math.random(0, 3)
		canvas_update()
		stones_rotate_palette()
		g_stones_tbl["moon_gate"].x = 0x7c + math.random(0, 1)
		g_stones_tbl["moon_gate"].y = 5 + math.random(0, 3)
		canvas_update()
		input = input_poll()
		if input ~= nil then
			break
		end
	end
	
	return should_exit(input)
end

g_stones_pal_counter = 0

function stones_rotate_palette()
	if g_stones_pal_counter == 4 then
		canvas_rotate_palette(144, 16)
		g_stones_pal_counter = 0
	else
		g_stones_pal_counter = g_stones_pal_counter + 1
	end
end

g_stones_tbl = {}

function stones_sequence()

	load_images("intro_3.shp")

	canvas_set_palette("palettes.int", 3)

	g_stones_tbl["bg"] = sprite_new(g_img_tbl[0], 0, 0, true)
	g_stones_tbl["stone_cover"] = sprite_new(g_img_tbl[3], 0x96, 0x64, false)
	g_stones_tbl["gate_cover"] = sprite_new(g_img_tbl[4], 0x5e, 0x66, false)
	g_stones_tbl["hand"] = sprite_new(g_img_tbl[1], 0xbd, 0xc7, false)
	g_stones_tbl["moon_gate"] = sprite_new(g_img_tbl[2], 0x7c, 0x64, false)
	g_stones_tbl["moon_gate"].clip_x = 0
	g_stones_tbl["moon_gate"].clip_y = 0
	g_stones_tbl["moon_gate"].clip_w = 320
	g_stones_tbl["moon_gate"].clip_h = 0x66
	g_stones_tbl["avatar"] = sprite_new(g_img_tbl[7], -2, 0x12, false)

	local scroll_img = image_load("blocks.shp", 2)
	local scroll = sprite_new(scroll_img, 1, 0xc, true)

	local x, y = image_print(scroll_img, "Near the stones, the smell of damp, blasted earth hangs ", 7, 303, 7, 8, 0x3e)
	x, y = image_print(scroll_img, "in the air. In a frozen moment of lightning-struck ", 7, 303, x, y, 0x3e)
	x, y = image_print(scroll_img, "daylight, you glimpse a tiny obsidian stone in the ", 7, 303, x, y, 0x3e)
	x, y = image_print(scroll_img, "midst of the circle!", 7, 303, x, y, 0x3e)
	
	fade_in()

	if stones_update() == true then
		return false
	end
	
	g_stones_tbl["stone_cover"].visible = true
	g_stones_tbl["hand"].visible = true
	
	scroll_img = image_load("blocks.shp", 0)
	image_print(scroll_img, "Wondering, you pick it up....", 8, 234, 0x2a, 8, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x21
	scroll.y = 0x1e
	
	local i
	for i=0xc7,0x54,-2  do
		--		display_stones()
		g_stones_tbl["hand"].y = i
		canvas_update()
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end

	if stones_update() == true then
		return false
	end
	
	g_stones_tbl["bg"].image = g_img_tbl[5]
	g_stones_tbl["stone_cover"].visible = false
	g_stones_tbl["gate_cover"].visible = true
	
	scroll_img = image_load("blocks.shp", 1)
	x, y = image_print(scroll_img, "...and from the heart of the stones, a softly glowing door ", 7, 303, 7, 10, 0x3e)
	image_print(scroll_img, "ascends in silence!", 7, 303, x, y, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0xa0
	
	g_stones_tbl["moon_gate"].visible = true

	for i=0x64,0x5,-1  do
		--		display_stones()
		stones_rotate_palette()
		g_stones_tbl["moon_gate"].y = i
		canvas_update()
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end

	for i=0x54,0xc7,2  do
		--		display_stones()
		stones_rotate_palette()
		g_stones_tbl["hand"].y = i
		canvas_update()
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end
	
	g_stones_tbl["hand"].visible = false
	
	if stones_update() == true then
		return false
	end
	
	g_stones_tbl["hand"].image = g_img_tbl[6]
	g_stones_tbl["hand"].x = 0x9b
	g_stones_tbl["hand"].visible = true
	scroll.visible = false
	
	for i=0xc7,0x44,-2  do
		stones_rotate_palette()
		g_stones_tbl["hand"].y = i
		g_stones_tbl["bg"].y = g_stones_tbl["bg"].y - 1
		g_stones_tbl["gate_cover"].y = g_stones_tbl["gate_cover"].y - 1
		g_stones_tbl["moon_gate"].y = g_stones_tbl["moon_gate"].y - 1
		canvas_update()
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end
	
	scroll_img = image_load("blocks.shp", 2)
	x, y = image_print(scroll_img, "Exultant memories wash over you as you clutch the stone. ", 7, 303, 7, 8, 0x3e)
	x, y = image_print(scroll_img, "When last you saw an orb such as this, it was cast down ", 7, 303, x, y, 0x3e)
	image_print(scroll_img, "by Lord British to banish the tyrant Blackthorn!", 7, 303, x, y, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x98
	scroll.visible = true
	
	if stones_update() == true then
		return false
	end
	
	scroll.visible = false
	
	for i=0x44,0xc7,2  do
		stones_rotate_palette()
		g_stones_tbl["hand"].y = i
		g_stones_tbl["bg"].y = g_stones_tbl["bg"].y + 1
		g_stones_tbl["gate_cover"].y = g_stones_tbl["gate_cover"].y + 1
		g_stones_tbl["moon_gate"].y = g_stones_tbl["moon_gate"].y + 1
		canvas_update()
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end
	
	g_stones_tbl["hand"].visible = false
	
	scroll_img = image_load("blocks.shp", 2)
	image_print(scroll_img, "But your joy soon gives way to apprehension.", 7, 303, 16, 8, 0x3e)
	image_print(scroll_img, "The gate to Britannia has always been blue...", 7, 303, 18, 24, 0x3e)
	image_print(scroll_img, "as blue as the morning sky.", 7, 303, 76, 32, 0x3e)
	scroll.image = scroll_img
	scroll.visible = true
		
	if stones_update() == true then
		return false
	end
	
	scroll_img = image_load("blocks.shp", 1)
	x,y = image_print(scroll_img, "Abruptly, the portal quivers and begins to sink ", 7, 303, 7, 10, 0x3e)
	image_print(scroll_img, "into the ground.  Its crimson light wanes!", 7, 303, x, y, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0xa0
	
	if stones_shake_moongate() == true then
		return false
	end
	
	scroll_img = image_load("blocks.shp", 1)
	x,y = image_print(scroll_img, "Desperation makes the decision an easy one.", 7, 303, 22, 14, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0xa0
		
	if stones_shake_moongate() == true then
		return false
	end
	
	scroll.visible = false
	
	g_stones_tbl["avatar"].visible = true
	
	canvas_set_palette_entry(0x19, 0, 0, 0)
		
	for i=0,19,1 do
		g_stones_tbl["avatar"].image = g_img_tbl[7+i]
		
		local j
		for j=0,4 do
			canvas_update()
			stones_rotate_palette()
			g_stones_tbl["moon_gate"].x = 0x7c + math.random(0, 1)
			g_stones_tbl["moon_gate"].y = 5 + math.random(0, 3)
		end
						
		g_stones_tbl["avatar"].y = g_stones_tbl["avatar"].y - 3

		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end
	
	
	for i=0xff,0,-3 do
		canvas_update()
		stones_rotate_palette()
		g_stones_tbl["moon_gate"].x = 0x7c + math.random(0, 1)
		g_stones_tbl["moon_gate"].y = 5 + math.random(0, 3)
		
		g_stones_tbl["avatar"].opacity = i
	
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end

	canvas_set_palette_entry(0x19, 0x74, 0x74, 0x74)
	
	g_stones_tbl["moon_gate"].x = 0x7c
	
	for i=0x5,0x64,1  do
		stones_rotate_palette()
		g_stones_tbl["moon_gate"].y = i
		canvas_update()
		local input = input_poll()
		if input ~= nil and should_exit(input) then
			return false
		end
	end

	g_stones_tbl["bg"].image = g_img_tbl[0]
	g_stones_tbl["moon_gate"].visible = false
	g_stones_tbl["gate_cover"].visible = false
	g_stones_tbl["stone_cover"].visible = true
	
	if stones_update() == true then
		return false
	end

	return true
end

function play()

load_images("intro_1.shp")
music_play("bootup.m")
--[ [
canvas_set_palette("palettes.int", 0)
canvas_set_update_interval(40)

local img = g_img_tbl[0x45]

local background = sprite_new(img)

background.x = 0
background.y = 0
background.opacity = 0
background.visible = true

music_play("bootup.m")
for i=0,0xff,3 do
	background.opacity = i
	if poll_for_esc() == true then return end
	canvas_update()
end

for i=0xff,0,-3 do
background.opacity = i
	if poll_for_esc() == true then return end

--if input ~= nil and input == ESCAPE_KEY then
--	return
--end
canvas_update()
end

img = g_img_tbl[0x46]

background.image = img

for i=0,0xff,3 do
	background.opacity = i
		if poll_for_esc() == true then return end

	canvas_update()
end

for i=0xff,0,-3 do
background.opacity = i
	if poll_for_esc() == true then return end

canvas_update()
end

background.visible = false



if lounge_sequence() == false then
	return 
end

if window_sequence() == false then
	return
end

fade_out()
hide_window()
--] ]
stones_sequence()
end

function main_menu_load()
	g_menu = {}
	
	canvas_set_palette("palettes.int", 0)
		
	local title_img_tbl = image_load_all("titles.shp")
	g_menu["title"] = sprite_new(title_img_tbl[0], 0x13, 0, true)
	g_menu["subtitle"] = sprite_new(title_img_tbl[1], 0x3b, 0x2f, false)

	g_menu["menu"] = sprite_new(image_load("mainmenu.shp", 0), 0x31, 0x53, false)
	
	fade_in()
	
	g_menu["subtitle"].visible = true
	g_menu["menu"].visible = true
	
	fade_in_sprite(g_menu["menu"])
end

function main_menu()
	g_menu["title"].visible = true
	g_menu["subtitle"].visible = true
	g_menu["menu"].visible = true
	
	local input = input_poll()

	while input == nil do
		canvas_update()
		input = input_poll()
		if input ~= nil then
			if input == 113 then     --q quit
				break
			elseif input == 105 then --i
				--run intro here
			elseif input == 99 then  --c
				fade_out_sprite(g_menu["menu"],6)
				if create_character() == true then
					return "J"
				end
				fade_in_sprite(g_menu["menu"])
				--create character
			elseif input == 116 then --t
				--transfer a character
			elseif input == 97 then  --a
				--acknowledgments
			elseif input == 106 then --j
				return "J"
			end
			input = nil
		end
	end
	
	return "Q"
end

g_keycode_tbl =
{
[32]=" ",

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
function create_character()
	music_play("create.m")

	local bg = sprite_new(image_load("vellum1.shp",0), 0x10, 0x50, true)
	image_print(bg.image, "By what name shalt thou be called?", 7, 303, 36, 24, 0x48)
	
	local name = sprite_new(nil, 0x34, 0x78, true)
	name.text = ""
	local input = nil
	while input == nil do
		canvas_update()
		input = input_poll()
		if input ~= nil then
			if should_exit(input) == true then
				bg.visible = false
				name.visible = false
				return false
			end
			local name_text = name.text
			local len = string.len(name_text)
			if input == 8 and len > 0 then
				name.text = string.sub(name_text, 1, len - 1)
			elseif input == 13 and len > 0 then --return
				break;
			elseif g_keycode_tbl[input] ~= nil and len < 13 then
				name.text = name_text..g_keycode_tbl[input]
			end
			input = nil
		end
	end
	
	image_print(bg.image, "And art thou Male, or Female?", 7, 303, 52, 56, 0x48)
	input = nil
	while input == nil do
		canvas_update()
		input = input_poll()
		if input ~= nil then
			if should_exit(input) == true then
				bg.visible = false
				name.visible = false
				return false
			end
			if input == 77 or input == 109 then
				--male
				break
			elseif input == 70 or input == 102 then
				--female
				break
			end
			
			input = nil
		end
	end
	
	return true
end

play()
fade_out()

g_img_tbl = nil
g_clock_tbl = nil
g_lounge_tbl = nil
g_window_tbl = nil
g_stones_tbl = nil

canvas_hide_all_sprites()

main_menu_load()
local cmd = ""

while cmd ~= "Q" and cmd ~= "J" do
	cmd = main_menu()
end

canvas_hide()
