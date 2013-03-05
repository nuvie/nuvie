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
		canvas_rotate_palette(16, 16)
		g_pal_counter = 0
	else
		g_pal_counter = g_pal_counter + 1
	end
end

local function rotate_and_wait()
	local input = nil
	while input == nil do
		stones_rotate_palette()
		canvas_update()
		input = input_poll()
		if input ~= nil then
			break
		end
	end
end

local function play()

	g_img_tbl = image_load_all("end.shp")
	music_play("end.m")
--[ [
	canvas_set_palette("endpal.lbm", 0)
	canvas_set_update_interval(25)
	sprite_new(g_img_tbl[7], 0, 0, true)--bg
	local wall_transparent = sprite_new(g_img_tbl[0], 106, 0, false)--wall
	local codex = sprite_new(g_img_tbl[1], 0x97, 0x1d, false)
	local codex_opened = sprite_new(g_img_tbl[2], 0x85, 0x17, false)
	local wall = sprite_new(g_img_tbl[4], 106, 0, true)--wall
	moongate = sprite_new(g_img_tbl[3], 9, 0x7d, true)--moongate
	moongate.clip_x = 0
	moongate.clip_y = 0
	moongate.clip_w = 320
	moongate.clip_h = 0x7e
	
	local characters = sprite_new(g_img_tbl[8], 0, 0, false)
	characters.clip_x = 0
	characters.clip_y = 0
	characters.clip_w = 160
	characters.clip_h = 200
		
	local characters1 = sprite_new(g_img_tbl[9], 0, 0, false)
	characters1.clip_x = 0
	characters1.clip_y = 0
	characters1.clip_w = 160
	characters1.clip_h = 200
		
	local blue_lens = sprite_new(g_img_tbl[5], 0x49, 0x57, true)--lens
	local red_lens = sprite_new(g_img_tbl[6], 0xda, 0x57, true)--lens
	
	local scroll_img = image_load("end.shp", 0xb)

	image_print(scroll_img, "A glowing portal springs from the floor!", 7, 303, 7, 10, 0x3e)
	local scroll = sprite_new(scroll_img, 1, 0xa0, true)
	
	local input
	local i
	
	for i=0x7d,-0xc,-1  do
		stones_rotate_palette()
		moongate.y = i
		canvas_update()
		input = input_poll()
		if input ~= nil then
			moongate.y = -0xc
			break
		end
	end
	
	rotate_and_wait()
	
	characters.visible = true
	
	scroll_img = image_load("end.shp", 0xa)
	image_print(scroll_img, "From its crimson depths Lord British emerges, trailed by the mage Nystul. Anguish and disbelief prevail on the royal seer's face, but Lord British directs his stony gaze at you and speaks as if to a wayward child.", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x85
	
	rotate_and_wait()
	
	scroll_img = image_load("end.shp", 0xc)
	image_print(scroll_img, "\"Thou didst have just cause to burgle our Codex, I trust\127 His Majesty says. \"But for Virtue's sake...", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x97
	
	for i=-0xc,0x7d,1  do
		stones_rotate_palette()
		moongate.y = i
		canvas_update()
		input = input_poll()
		if input ~= nil then
			moongate.y = 0x7d
			break
		end
	end
	
	moongate.visible = false
	
	scroll_img = image_load("end.shp", 0xb)
	image_print(scroll_img, "\"WHAT HAST THOU DONE WITH IT?\127", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x0
	scroll.y = 0xa0

	
	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xa)
	image_print(scroll_img, "You pick up the concave lens and pass it to the King. \"Was the book ever truly ours, Your Majesty? Was it written for Britannia alone? Thou dost no longer hold the Codex, but is its wisdom indeed lost? Look into the Vortex, and let the Codex answer for itself!\127", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x85
	
	wait_for_input()
	
	blue_lens.visible = false
	
	characters.clip_x = 160
	characters.clip_w = 160
	characters.visible = false
	
	characters1.visible = true
	 
	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xc)
	image_print(scroll_img, "As Lord British holds the glass before the wall, the Codex of Ultimate Wisdom wavers into view against a myriad of swimming stars!", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x97
	
	wall_transparent.visible = true
	
	for i=0xff,0,-3  do
		wall.opacity = i
		--FIXME do star field here.
		canvas_update()
		input = input_poll()
		if input ~= nil then
			wall.opacity = 0
			break
		end
	end

	codex.opacity = 0
	codex.visible = true

	for i=0,0xff,3  do
		codex.opacity = i
		--FIXME do star field here.
		canvas_update()
		input = input_poll()
		if input ~= nil then
			codex.opacity = 0xff
			break
		end
	end

	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xb)
	image_print(scroll_img, "Yet the book remains closed.", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x0
	scroll.y = 0xa0
	
	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xb)
	image_print(scroll_img, "And waves of heat shimmer in the air, heralding the birth of another red gate!", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x0
	scroll.y = 0x98
	
	moongate.x = 0xe6
	moongate.y = 0x7d
	moongate.visible = true
	
	for i=0x7d,-0xc,-1  do
		stones_rotate_palette()
		moongate.y = i
		canvas_update()
		input = input_poll()
		if input ~= nil then
			moongate.y = -0xc
			break
		end
	end
	
	rotate_and_wait()
	
	characters.visible = true
	
	scroll_img = image_load("end.shp", 0xa)
	image_print(scroll_img, "King Draxinusom of the Gargoyles strides forward, flanked by a small army of wingless attendants. Like Lord British, he seems to suppress his rage only through a heroic effort of will. His scaly hand grasps your shoulder, and your Amulet of Submission grows very warm.", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x85
	
	rotate_and_wait()
	
	scroll_img = image_load("end.shp", 0xb)
	image_print(scroll_img, "\"Thy time hath come, Thief,\127 he says.", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x0
	scroll.y = 0xa0
	
	for i=-0xc,0x7d,1  do
		stones_rotate_palette()
		moongate.y = i
		canvas_update()
		input = input_poll()
		if input ~= nil then
			moongate.y = 0x7d
			break
		end
	end

	moongate.visible = false
	
	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xb)
	image_print(scroll_img, "Quickly you reach down to seize the convex lens...", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x0
	scroll.y = 0x98

	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xc)
	image_print(scroll_img, "...and you press it into the hand of the towering Gargoyle king, meeting his sunken eyes. \"Join my Lord in his search for peace, I beg thee.\127", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x97

	wait_for_input()
	
	characters.visible = false
	characters1.clip_w = 320
	red_lens.visible = false
		
	scroll_img = image_load("end.shp", 0xa)
	image_print(scroll_img, "At your urging, King Draxinusom reluctantly raises his lens to catch the light. As Lord British holds up his own lens, every eye in the room, human and Gargoyle alike, fixes upon the image of the Codex which shines upon the wall.", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x85

	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xa)
	image_print(scroll_img, "The ancient book opens. Both kings gaze upon its pages in spellbound silence, as the eloquence of Ultimate Wisdom is revealed in the tongues of each lord's domain. You, too, can read the answers the Codex gives...", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x85
	
	codex_opened.opacity = 0
	codex_opened.visible = true
	
	for i=0,0xff,3  do
		codex_opened.opacity = i
		--FIXME do star field here.
		canvas_update()
		input = input_poll()
		if input ~= nil then
			codex_opened.opacity = 0xff
			break
		end
	end
	codex.visible = false
	
	wait_for_input()
	
	scroll_img = image_load("end.shp", 0xa)
	image_print(scroll_img, "...and when its wisdom is gleaned, when Lord British and King Draxinusom turn to each other as friends, hating no longer, fearing no more, you know that your mission in Britannia has ended at last.", 8, 303, 7, 10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x85
	
	wait_for_input()
	
	scroll.visible = false
	
	for i=0xff,0,-3  do
		codex_opened.opacity = i
		--FIXME do star field here.
		canvas_update()
		input = input_poll()
		if input ~= nil then
			codex_opened.opacity = 0
			break
		end
	end
	
	codex_opened.visible = false
	
	wall.opacity = 0
	wall.visible = true

	for i=0,0xff,3  do
		wall.opacity = i
		--FIXME do star field here.
		canvas_update()
		input = input_poll()
		if input ~= nil then
			wall.opacity = 0xff
			break
		end
	end
	
	wait_for_input()
	

	for i=0xff,0,-3 do
		canvas_set_opacity(i)
		canvas_update()
	end
	
	canvas_hide_all_sprites()
	canvas_set_opacity(0xff)

	
	local x, y
	scroll_img = image_load("end.shp", 0xa)
	x, y = image_print(scroll_img, "CONGRATULATIONS", 8, 303, 7, 10, 0x3e)
	x, y = image_print(scroll_img, "Thou hast completed Ultima VI: The False Prophet, in xxx years, yyyy months, and zzz day.", 8, 303, 7, y+10, 0x3e)
	x, y = image_print(scroll_img, "Report thy feat unto Lord British at Origin Systems!", 8, 303, 7, y+10, 0x3e)
	scroll.image = scroll_img
	scroll.x = 0x1
	scroll.y = 0x44

	scroll.opacity = 0
	scroll.visible = true
	
	for i=0,0xff,3  do
		scroll.opacity = i
		canvas_update()
		input = input_poll()
		if input ~= nil then
			scroll.opacity = 0xff
			break
		end
	end
	
	wait_for_input()

end

play()
