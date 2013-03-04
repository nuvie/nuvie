local g_img_tbl = {}

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

local g_pal_counter = 0

local function stones_rotate_palette()
	if g_pal_counter == 4 then
		canvas_rotate_palette(144, 16)
		g_pal_counter = 0
	else
		g_pal_counter = g_pal_counter + 1
	end
end
local function play()

	g_img_tbl = image_load_all("end.shp")
	music_play("end.m")
--[ [
	canvas_set_palette("endpal.lbm", 0)
	canvas_set_update_interval(25)
	sprite_new(g_img_tbl[7], 0, 0, true)--bg
	sprite_new(g_img_tbl[4], 106, 0, true)--wall
	moongate = sprite_new(g_img_tbl[3], 9, 0x7d, true)--moongate
	moongate.clip_x = 0
	moongate.clip_y = 0
	moongate.clip_w = 320
	moongate.clip_h = 0x7e
	sprite_new(g_img_tbl[5], 0x49, 0x57, true)--lens
	sprite_new(g_img_tbl[6], 0xda, 0x57, true)--lens
	
	local scroll = sprite_new(nil, 1, 0xc, true)
	local x, y
	local scroll_img = image_load("blocks.shp", 1)
	x, y = image_print(scroll_img, "A glowing portal springs from the floor!", 7, 303, 7, 10, 0x3e)
	--image_print(scroll_img, "ascends in silence!", 7, 303, x, y, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0xa0
	
	for i=0x7d,-0xc,-1  do
		stones_rotate_palette()
		moongate.y = i
		canvas_update()
		--local input = input_poll()
		--if input ~= nil and should_exit(input) then
		--return false
		--end
	end
	wait_for_input()
end

play()
