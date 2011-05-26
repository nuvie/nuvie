io.stderr:write("Magic init\n");

magic = {}
magic_invocations = {}

run_magic_script = function(invocation)
	local spell_num = magic_invocations[invocation]
    if spell_num == nil then
      io.stderr:write("No magic script found for invocation \"" .. invocation .. "\"\n");
      return
    end

--io.stderr:write("Running script \"" .. magic_invocations[invocation].script .."\"\n");
	
    --run_script(magic_invocations[invocation].script)
    magic_cast_spell(spell_num, nil, nil)
    return
end

g_magic_target = nil
g_magic_caster = nil
g_magic_spell_num = nil

function magic_cast_spell(spell_num, caster, target)
	g_magic_target = target
	g_magic_caster = caster
	g_magic_spell_num = spell_num
	if magic[spell_num] ~= nil then
		run_script(magic[spell_num].script)
	end
	g_magic_caster = nil
	g_magic_target = nil
end

function magic_spell_name(spell_num)
	if magic[spell_num] ~= nil then
		return magic[spell_num].name
	end
	
	return "Unknown"
end

magic_init = function(name, invocation, reagents, circle, num, script)
	local spell_num = (circle-1) * 16 + (num-1); 
    local spell = {name=name,invocation=invocation,reagents=reagents,circle=circle,spell_num=spell_num,script=script}

    magic[spell_num] = spell
    magic_invocations[string.lower(invocation)] = spell_num

    io.stderr:write("Init Magic: " .. name .. " I: " .. invocation .. "\n")
end

function select_location_with_prompt(prompt)
if g_magic_target ~= nil then return g_magic_target end

print(prompt)
return get_target()
end

function select_location()
	return select_location_with_prompt("Location: ")
end

select_actor = function()
	if g_magic_target ~= nil then return map_get_actor(g_magic_target) end
	
	print("On whom: ");

	local loc = get_target()
	local actor
	
	if loc ~= nil then
		actor = map_get_actor(loc)
	end
	
	if actor == nil then
		print("nothing\n");
	else
		print(actor.name.."\n");
	end

	return actor
end

select_obj = function()
	if g_magic_target ~= nil then return map_get_obj(g_magic_target) end
	
	print("On what: ");

	local loc = get_target()
	local obj = map_get_obj(loc)
	
	if obj == nil then
		print("nothing\n");
	else
		print(obj.name .. "\n");
	end

	return obj 
end

function select_actor_with_projectile(projectile_tile, caster)

	if caster == nil then caster = magic_get_caster() end

	local loc = select_location_with_prompt("On Whom: ")
	local actor = map_get_actor(loc)
	if actor == nil then
		local obj = map_get_obj(loc)
		if obj ~= nil then
			print(obj.name.."\n")
		else
			print("nothing\n")
		end
	else
		print(actor.name.."\n")
	end
	
	magic_casting_fade_effect(caster)
	if loc == nil then magic_no_effect() return end

	local hit_x, hit_y =  map_line_hit_check(caster.x, caster.y, loc.x, loc.y, loc.z)
	projectile(projectile_tile, caster.x, caster.y, hit_x, hit_y, 2)

	if hit_x ~= loc.x or hit_y ~= loc.y then magic_blocked() return end

	if actor == nil then magic_no_effect() return end

	return actor
end

function select_actor_or_obj_with_projectile(projectile_tile, caster)

	if caster == nil then caster = magic_get_caster() end

	local loc = select_location_with_prompt("On what: ")
	local item = map_get_actor(loc)
	if item == nil then
		item = map_get_obj(loc)
	end
	
	if item ~= nil then
		print(item.name)
	else
		print("nothing")
	end

	print("\n")

	magic_casting_fade_effect(caster)

	if loc == nil then magic_no_effect() return end

	local hit_x, hit_y =  map_line_hit_check(caster.x, caster.y, loc.x, loc.y, loc.z)
	projectile(projectile_tile, caster.x, caster.y, hit_x, hit_y, 2)

	if hit_x ~= loc.x or hit_y ~= loc.y then magic_blocked() return end

	if item == nil then magic_no_effect() return end

	return item
end

function select_obj_with_projectile(projectile_tile, caster)

	if caster == nil then caster = magic_get_caster() end

	local loc = select_location_with_prompt("On What: ")
	local obj = map_get_obj(loc)
	if obj == nil then
		print("nothing\n");
	else
		print(obj.name.."\n");
	end

	magic_casting_fade_effect(caster)
	if loc == nil then magic_no_effect() return end

	local hit_x, hit_y =  map_line_hit_check(caster.x, caster.y, loc.x, loc.y, loc.z)
	projectile(projectile_tile, caster.x, caster.y, hit_x, hit_y, 2)

	if hit_x ~= loc.x or hit_y ~= loc.y then magic_blocked() return end

	if obj == nil then magic_no_effect() return end

	return obj
end

function select_location_with_projectile(projectile_tile, caster)

if caster == nil then caster = magic_get_caster() end

local loc = select_location_with_prompt("Location: ")

magic_casting_fade_effect(caster)
if loc == nil then magic_no_effect() return end

local hit_x, hit_y =  map_line_hit_check(caster.x, caster.y, loc.x, loc.y, loc.z)
projectile(projectile_tile, caster.x, caster.y, hit_x, hit_y, 2)

if hit_x ~= loc.x or hit_y ~= loc.y then magic_blocked() return end

return loc
end

function caster_get_location()	  
	 if g_magic_caster ~= nil then
	  	return {x = g_magic_caster.x, y = g_magic_caster.y, z = g_magic_caster.z}
	  end
	  
	  return player_get_location()
end

function magic_get_caster()
	if g_magic_caster ~= nil then return g_magic_caster end

	return Actor.get_player_actor()
end

function caster_is_player()
	if g_magic_caster == nil then return true end
	
	return false
end

function magic_casting_effect()
	local magic_level = math.floor(g_magic_spell_num / 0x10);

	play_sfx(SFX_CASTING_MAGIC_P1 + magic_level, true)
	play_sfx(SFX_CASTING_MAGIC_P2 + magic_level)
	xor_effect(1700)
end

function magic_casting_fade_effect(caster)
	if caster == nil then caster = magic_get_caster() end
	
	Actor.black_fade_effect(caster, 12, 20) -- 12 = colour red. 20 = fade_speed
end

function magic_remove_actor_enchantments(actor)
	local success = false
	fade_actor_blue(actor)
	if actor.asleep == true then
		actor.asleep = false
		success = true
	end
	if actor.poisoned == true then
		actor.poisoned = false
		success = true
	end
	if actor.paralysed == true then
		actor.paralyzed = false
		success = true
	end

	if actor_remove_charm(actor) == true then
		success = true
	end

	return success
end

function magic_success()
	if caster_is_player() then
		print("\nSuccess\n")
		play_sfx(SFX_SUCCESS)
	end
end

function magic_no_effect()
	if caster_is_player() then
		print("\nNo effect\n")
		play_sfx(SFX_FAILURE)
	end
end

function magic_blocked()
	if caster_is_player() then
		print("\nBlocked!\n")
		play_sfx(SFX_FAILURE)
	end
end

function magic_failed()
	if caster_is_player() then
		print("\nFailed\n")
		play_sfx(SFX_FAILURE)
	end
end

function magic_not_possible()
	if caster_is_player() then
		print("\nNot possible\n")
		play_sfx(SFX_FAILURE)
	end
end

do
local init

magic_init("Create Food", "imy", 0x61, 1, 1, "u6/magic/circle_01/create_food.lua");
magic_init("Detect Magic", "wo", 0x82, 1, 2, "u6/magic/circle_01/detect_magic.lua");
magic_init("Detect Trap", "wj", 0x82, 1, 3, "u6/magic/circle_01/detect_trap.lua");
magic_init("Dispel Magic", "ajo", 0x60, 1, 4, "u6/magic/circle_01/dispel_magic.lua");
magic_init("Douse", "af", 0x24, 1, 5, "u6/magic/circle_01/douse.lua");
magic_init("Harm", "am", 0x84, 1, 6, "u6/magic/circle_01/harm.lua");
magic_init("Heal", "im", 0x84, 1, 7, "u6/magic/circle_01/heal.lua");
magic_init("Help", "kl", 0x00, 1, 8, "u6/magic/circle_01/help.lua");
magic_init("Ignite", "if", 0x84, 1, 9, "u6/magic/circle_01/ignite.lua");
magic_init("Light", "il", 0x80, 1, 10, "u6/magic/circle_01/light.lua");

magic_init("Infravision", "ql", 0x82, 2, 1, "u6/magic/circle_02/infravision.lua");
magic_init("Magic Arrow", "oj", 0x84, 2, 2, "u6/magic/circle_02/magic_arrow.lua");
magic_init("Poison", "inp", 0x0e, 2, 3, "u6/magic/circle_02/poison.lua");
magic_init("Reappear", "iy", 0x1c, 2, 4, "u6/magic/circle_02/reappear.lua");
magic_init("Sleep", "iz", 0x16, 2, 5, "u6/magic/circle_02/sleep.lua");
magic_init("Telekinesis", "opy", 0x0d, 2, 6, "u6/magic/circle_02/telekinesis.lua");
magic_init("Trap", "ij", 0x12, 2, 7, "u6/magic/circle_02/trap.lua");
magic_init("Unlock Magic", "ep", 0x88, 2, 8, "u6/magic/circle_02/unlock_magic.lua");
magic_init("Untrap", "aj", 0x88, 2, 9, "u6/magic/circle_02/untrap.lua");
magic_init("Vanish", "ay", 0x2c, 2, 10, "u6/magic/circle_02/vanish.lua");

magic_init("Curse", "as", 0xa2, 3, 1, "u6/magic/circle_03/curse.lua");
magic_init("Dispel Field", "ag", 0x84, 3, 2, "u6/magic/circle_03/dispel_field.lua");
magic_init("Fireball", "pf", 0x84, 3, 3, "u6/magic/circle_03/fireball.lua");
magic_init("Great Light", "vl", 0x81, 3, 4, "u6/magic/circle_03/great_light.lua");
magic_init("Magic Lock", "ap", 0xa8, 3, 5, "u6/magic/circle_03/magic_lock.lua");
magic_init("Mass Awaken", "avz", 0x60, 3, 6, "u6/magic/circle_03/mass_awaken.lua");
magic_init("Mass Sleep", "vz", 0x52, 3, 7, "u6/magic/circle_03/mass_sleep.lua");
magic_init("Peer", "vwy", 0x03, 3, 8, "u6/magic/circle_03/peer.lua");
magic_init("Protection", "is", 0xe0, 3, 9, "u6/magic/circle_03/protection.lua");
magic_init("Repel Undead", "axc", 0xa0, 3, 10, "u6/magic/circle_03/repel_undead.lua");

magic_init("Animate", "oy", 0x89, 4, 1, "u6/magic/circle_04/animate.lua");
magic_init("Conjure", "kx", 0x11, 4, 2, "u6/magic/circle_04/conjure.lua");
magic_init("Disable", "avm", 0x13, 4, 3, "u6/magic/circle_04/disable.lua");
magic_init("Fire Field", "ifg", 0x94, 4, 4, "u6/magic/circle_04/fire_field.lua");
magic_init("Great Heal", "vm", 0x51, 4, 5, "u6/magic/circle_04/great_heal.lua");
magic_init("Locate", "iw", 0x02, 4, 6, "u6/magic/circle_04/locate.lua");
magic_init("Mass Dispel", "vajo", 0x60, 4, 7, "u6/magic/circle_04/mass_dispel.lua");
magic_init("Poison Field", "ing", 0x56, 4, 8, "u6/magic/circle_04/poison_field.lua");
magic_init("Sleep Field", "izg", 0x54, 4, 9, "u6/magic/circle_04/sleep_field.lua");
magic_init("Wind Change", "rh", 0x88, 4, 10, "u6/magic/circle_04/wind_change.lua");

magic_init("Energy Field", "isg", 0x15, 5, 1, "u6/magic/circle_05/energy_field.lua");
magic_init("Explosion", "vpf", 0x8d, 5, 2, "u6/magic/circle_05/explosion.lua");
magic_init("Insect Swarm", "kbx", 0x98, 5, 3, "u6/magic/circle_05/insect_swarm.lua");
magic_init("Invisibility", "sl", 0x0a, 5, 4, "u6/magic/circle_05/invisibility.lua");
magic_init("Lightning", "og", 0x85, 5, 5, "u6/magic/circle_05/lightning.lua");
magic_init("Paralyze", "axp", 0x96, 5, 6, "u6/magic/circle_05/paralyze.lua");
magic_init("Pickpocket", "py", 0x1a, 5, 7, "u6/magic/circle_05/pickpocket.lua");
magic_init("Reveal", "asl", 0x13, 5, 8, "u6/magic/circle_05/reveal.lua");
magic_init("Seance", "kmc", 0x9b, 5, 9, "u6/magic/circle_05/seance.lua");
magic_init("X-ray", "wy", 0x81, 5, 10, "u6/magic/circle_05/xray.lua");

magic_init("Charm", "axe", 0x16, 6, 1, "u6/magic/circle_06/charm.lua");

magic_init("Kill", "ic", 0x86, 7, 6, "u6/magic/circle_07/kill.lua");

magic_init("Armageddon", "vcbm", 0xff, 8, 1, "u6/magic/circle_08/armageddon.lua");
magic_init("Eclipse", "val", 0xab, 8, 3, "u6/magic/circle_08/eclipse.lua");
magic_init("Resurrect", "imc", 0xf9, 8, 6, "u6/magic/circle_08/resurrect.lua");
magic_init("Slime", "vrx", 0xb, 8, 7, "u6/magic/circle_08/slime.lua");
magic_init("Summon", "kxc", 0x39, 8, 8, "u6/magic/circle_08/summon.lua");
magic_init("Tremor", "vpy", 0x89, 8, 10, "u6/magic/circle_08/tremor.lua");

magic_init("Monster Invisible", "", 0x00, 8, 17, "u6/magic/circle_08/monster_invisible.lua");

--[[
init = nuvie_load("u6/magic/circle_01/create_food.lua"); init()
init = nuvie_load("u6/magic/circle_01/detect_magic.lua"); init()
init = nuvie_load("u6/magic/circle_01/detect_trap.lua"); init()
init = nuvie_load("u6/magic/circle_01/dispel_magic.lua"); init()
init = nuvie_load("u6/magic/circle_01/douse.lua"); init()
init = nuvie_load("u6/magic/circle_01/harm.lua"); init()
init = nuvie_load("u6/magic/circle_01/heal.lua"); init()
init = nuvie_load("u6/magic/circle_01/help.lua"); init()
init = nuvie_load("u6/magic/circle_01/ignite.lua"); init()
init = nuvie_load("u6/magic/circle_01/light.lua"); init()

init = nuvie_load("u6/magic/circle_02/infravision.lua"); init()
init = nuvie_load("u6/magic/circle_02/magic_arrow.lua"); init()
init = nuvie_load("u6/magic/circle_02/poison.lua"); init()
init = nuvie_load("u6/magic/circle_02/reappear.lua"); init()
init = nuvie_load("u6/magic/circle_02/sleep.lua"); init()
init = nuvie_load("u6/magic/circle_02/telekinesis.lua"); init()
init = nuvie_load("u6/magic/circle_02/trap.lua"); init()
init = nuvie_load("u6/magic/circle_02/unlock_magic.lua"); init()
init = nuvie_load("u6/magic/circle_02/untrap.lua"); init()
init = nuvie_load("u6/magic/circle_02/vanish.lua"); init()

init = nuvie_load("u6/magic/circle_03/curse.lua"); init()
init = nuvie_load("u6/magic/circle_03/dispel_field.lua"); init()
init = nuvie_load("u6/magic/circle_03/fireball.lua"); init()
init = nuvie_load("u6/magic/circle_03/great_light.lua"); init()
init = nuvie_load("u6/magic/circle_03/magic_lock.lua"); init()
init = nuvie_load("u6/magic/circle_03/mass_awaken.lua"); init()
init = nuvie_load("u6/magic/circle_03/mass_sleep.lua"); init()
init = nuvie_load("u6/magic/circle_03/peer.lua"); init()
init = nuvie_load("u6/magic/circle_03/protection.lua"); init()
init = nuvie_load("u6/magic/circle_03/repel_undead.lua"); init()

init = nuvie_load("u6/magic/circle_04/animate.lua"); init()
init = nuvie_load("u6/magic/circle_04/conjure.lua"); init()
init = nuvie_load("u6/magic/circle_04/disable.lua"); init()
init = nuvie_load("u6/magic/circle_04/fire_field.lua"); init()
init = nuvie_load("u6/magic/circle_04/great_heal.lua"); init()
--init = nuvie_load("u6/magic/circle_04/locate.lua"); init()
init = nuvie_load("u6/magic/circle_04/mass_dispel.lua"); init()
init = nuvie_load("u6/magic/circle_04/poison_field.lua"); init()
init = nuvie_load("u6/magic/circle_04/sleep_field.lua"); init()
init = nuvie_load("u6/magic/circle_04/wind_change.lua"); init()

init = nuvie_load("u6/magic/circle_05/energy_field.lua"); init()
init = nuvie_load("u6/magic/circle_05/explosion.lua"); init()
init = nuvie_load("u6/magic/circle_05/insect_swarm.lua"); init()
init = nuvie_load("u6/magic/circle_05/invisibility.lua"); init()
init = nuvie_load("u6/magic/circle_05/lightning.lua"); init()
init = nuvie_load("u6/magic/circle_05/paralyze.lua"); init()
init = nuvie_load("u6/magic/circle_05/pickpocket.lua"); init()
init = nuvie_load("u6/magic/circle_05/reveal.lua"); init()
init = nuvie_load("u6/magic/circle_05/seance.lua"); init()
init = nuvie_load("u6/magic/circle_05/xray.lua"); init()

init = nuvie_load("u6/magic/circle_06/charm.lua"); init()
init = nuvie_load("u6/magic/circle_06/clone.lua"); init()
init = nuvie_load("u6/magic/circle_06/confuse.lua"); init()
init = nuvie_load("u6/magic/circle_06/flame_wind.lua"); init()
init = nuvie_load("u6/magic/circle_06/hail_storm.lua"); init()
init = nuvie_load("u6/magic/circle_06/mass_protect.lua"); init()
init = nuvie_load("u6/magic/circle_06/negate_magic.lua"); init()
init = nuvie_load("u6/magic/circle_06/poison_wind.lua"); init()
init = nuvie_load("u6/magic/circle_06/replicate.lua"); init()
init = nuvie_load("u6/magic/circle_06/web.lua"); init()

init = nuvie_load("u6/magic/circle_07/chain_bolt.lua"); init()
init = nuvie_load("u6/magic/circle_07/enchant.lua"); init()
init = nuvie_load("u6/magic/circle_07/energy_wind.lua"); init()
init = nuvie_load("u6/magic/circle_07/fear.lua"); init()
init = nuvie_load("u6/magic/circle_07/gate_travel.lua"); init()
init = nuvie_load("u6/magic/circle_07/kill.lua"); init()
init = nuvie_load("u6/magic/circle_07/mass_curse.lua"); init()
init = nuvie_load("u6/magic/circle_07/mass_invisibility.lua"); init()
init = nuvie_load("u6/magic/circle_07/wing_strike.lua"); init()
init = nuvie_load("u6/magic/circle_07/wizard_eye.lua"); init()

init = nuvie_load("u6/magic/circle_08/death_wind.lua"); init()
init = nuvie_load("u6/magic/circle_08/eclipse.lua"); init()
init = nuvie_load("u6/magic/circle_08/mass_charm.lua"); init()
init = nuvie_load("u6/magic/circle_08/mass_kill.lua"); init()
init = nuvie_load("u6/magic/circle_08/resurrect.lua"); init()
init = nuvie_load("u6/magic/circle_08/slime.lua"); init()
init = nuvie_load("u6/magic/circle_08/summon.lua"); init()
init = nuvie_load("u6/magic/circle_08/time_stop.lua"); init()
init = nuvie_load("u6/magic/circle_08/tremor.lua"); init()
init = nuvie_load("u6/magic/circle_08/armageddon.lua"); init()

--]]

end

magic_load = nil
