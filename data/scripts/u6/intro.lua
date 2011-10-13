g_img_tbl = {}

function poll_for_esc()
	input = input_poll()
	if input ~= nil and input == 27 then
		canvas_hide()
		return true
	end
	
	return false
end

function load_images(filename, num_images)
	--FIXME create a C function to do this.
	local i
	g_img_tbl = {}
	for i=0,num_images do
	g_img_tbl[i] = image_load(filename, i)
	end
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

function load_lounge()

	g_lounge_tbl["bg0"] = sprite_new(g_img_tbl[15], 210, 0, true)
	g_lounge_tbl["bg"] = sprite_new(g_img_tbl[0], 0, 0, true)
	g_lounge_tbl["bg1"] = sprite_new(g_img_tbl[1], 320, 0, true)
	g_lounge_tbl["avatar"] = sprite_new(g_img_tbl[0xd], 0, 63, true)

	load_clock()
end

function display_lounge()
	display_clock()
end

function scroll_lounge()
g_lounge_tbl.bg.x = g_lounge_tbl.bg.x - 1
g_lounge_tbl.bg1.x = g_lounge_tbl.bg1.x - 1
g_lounge_tbl.avatar.x = g_lounge_tbl.avatar.x - 2

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

	scroll_img = image_load("blocks.shp", 0)
	image_print(scroll_img, "Outside, a chill wind rises...", 39, 200, 39, 8, 0x3e)

	scroll.image = scroll_img
	scroll.x = 0x21
	scroll.y = 0x9d

	for i=0,319 do
		scroll_lounge()
		display_lounge()
		canvas_update()
	end

	input = input_poll()
	while input == nil do
		canvas_update()
		input = input_poll()
	end

	hide_lounge()
	scroll.visible = false
end

g_window_tbl = {}
function load_window()
	g_window_tbl["sky"] = sprite_new(g_img_tbl[1], 0, 0, true)
	g_window_tbl["cloud1"] = sprite_new(g_img_tbl[0], 0, 0, false)
	g_window_tbl["cloud2"] = sprite_new(g_img_tbl[2], 0, 0, false)
	g_window_tbl["cloud3"] = sprite_new(g_img_tbl[3], 0, 0, false)
	g_window_tbl["lightning"] = sprite_new(nil, 0, 0, false)
	g_window_tbl["ground"] = sprite_new(g_img_tbl[10], 0, 0x4c, true)
	g_window_tbl["trees"] = sprite_new(g_img_tbl[8], 0, 0, true)

	g_window_tbl["strike"] = sprite_new(nil, 0, 0, false)
	
	--FIXME rain here.
	local rain = {}
	local i
	for i = 0,100 do
		rain[i] = sprite_new(g_img_tbl[math.random(4,7)], math.random(0,320), math.random(0,200), false)
	end
	g_window_tbl["rain"] = rain
	
	g_window_tbl["frame"] = sprite_new(g_img_tbl[28], 0, 0, true)	
	g_window_tbl["window"] = sprite_new(g_img_tbl[26], 0x39, 0, true)

	g_window_tbl["flash"] = 0
	g_window_tbl["drops"] = 0
	g_window_tbl["rain_delay"] = 20
end

function display_window()

	local i
	local rain = g_window_tbl["rain"]
	local rand = math.random
	local c_num
	if g_window_tbl["cloud1"].visible == false or g_window_tbl["cloud1"].x > 320 then
		c_num = rand(1,3)
		if c_num == 1 then c_num = 0 end
		
		g_window_tbl["cloud1"].image = g_img_tbl[c_num]
		
		g_window_tbl["cloud1"].x = -310
		g_window_tbl["cloud1"].visible = true
	else
		g_window_tbl["cloud1"].x = g_window_tbl["cloud1"].x + 1
	end

	if g_window_tbl["cloud2"].visible == false or g_window_tbl["cloud2"].x > 320 then
		g_window_tbl["cloud2"].x = -150
		g_window_tbl["cloud2"].visible = true
	else
		g_window_tbl["cloud2"].x = g_window_tbl["cloud2"].x + 2
	end

	if g_window_tbl["cloud3"].visible == false or g_window_tbl["cloud3"].x > 320 then
		g_window_tbl["cloud3"].x = -140
		g_window_tbl["cloud3"].visible = true
	else
		g_window_tbl["cloud3"].x = g_window_tbl["cloud3"].x + 3
	end

	if g_window_tbl["flash"] > 0 then
		g_window_tbl["flash"] = g_window_tbl["flash"] - 1
	else
		if rand(0,5) == 0 then
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
end

function window_sequence()
	load_images("intro_2.shp", 0x1c)
	
	load_window()
	
	canvas_set_palette("palettes.int", 2)
		
	local input = input_poll()
	while input == nil do
		display_window()
		canvas_update()
		input = input_poll()
		if input ~= nil then
			break
		end
		canvas_update()
		input = input_poll()
	end
end

load_images("intro_1.shp", 0x46)
music_play("bootup.m")
--[ [
canvas_set_palette("palettes.int", 0)

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

--] ]

lounge_sequence()

window_sequence()

canvas_hide()