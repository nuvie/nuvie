canvas_set_palette("palettes.int", 0)

local img = image_load("intro_1.shp", 0x45)

local background = sprite_new(img)

background.x = 0
background.y = 0
background.opacity = 0
background.visible = true

for i=0,0xff,3 do
	background.opacity = i
--input = input_poll()
--if input ~= nil and input == ESCAPE_KEY then
--	return
--end
	canvas_update()
end

for i=0xff,0,-3 do
background.opacity = i
--input = input_poll()
--if input ~= nil and input == ESCAPE_KEY then
--	return
--end
canvas_update()
end

img = image_load("intro_1.shp", 0x46)

background.image = img

for i=0,0xff,3 do
	background.opacity = i
--input = input_poll()
--if input ~= nil and input == ESCAPE_KEY then
--	return
--end
	canvas_update()
end

for i=0xff,0,-3 do
background.opacity = i
--input = input_poll()
--if input ~= nil and input == ESCAPE_KEY then
--	return
--end
canvas_update()
end

img = image_load("intro_1.shp", 0x0)

background.image = img
background.opacity = 0xff

img = image_load("intro_1.shp", 0xd)
local avatar = sprite_new(img)

avatar.y = 63
canvas_set_palette("palettes.int", 1)
for i=0,0xff do
canvas_update()
end
canvas_hide()