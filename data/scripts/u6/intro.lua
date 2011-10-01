g_img_tbl = {}

function poll_for_esc()
	input = input_poll()
	if input ~= nil then
		canvas_hide()
		return true
	end
	
	return false
end

function load_images()
	local i
	for i=0,0x46 do
	g_img_tbl[i] = image_load("intro_1.shp", i)
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

if g_lounge_tbl.bg1.x < 165 and g_lounge_tbl.bg1.x % 2 == 0 then
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

load_images()
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

load_lounge()

canvas_set_palette("palettes.int", 1)

--for i=0,0xff do
--display_lounge()
--canvas_update()
--end

for i=0,319 do
scroll_lounge()
display_lounge()
canvas_update()
end

for i=0,200 do
canvas_update()
end

canvas_hide()