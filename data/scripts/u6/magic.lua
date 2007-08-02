io.write("Magic init\n");

magic = {}
magic_invocations = {}

run_magic_script = function(invocation)
    if magic_invocations[invocation] == nil then
      io.write("No magic script found for invocation \"" .. invocation .. "\"\n");
      return
    end

io.write("Running script " .. magic_invocations[invocation].script);

    run_script(magic_invocations[invocation].script)

    return
end

magic_init = function(name, invocation, reagents, circle, num, script)
    local spell = {name=name,invocation=invocation,reagents=reagents,circle=circle,num=num,script=script}

    magic[circle * 16 + num] = spell
    magic_invocations[string.lower(invocation)] = spell

    io.write("Init Magic: " .. name .. " I: " .. invocation .. "\n")
end

select_actor = function()
	print("On whom: ");

	local loc = get_target()
	local actor = map_get_actor(loc)

	if actor == nil then
		print("nothing\n");
	else
		print("actor_type\n");
	end

	return actor
end

select_obj = function()
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
	  
do
local init

magic_init("Create Food", "imy", 0x61, 1, 1, "u6/magic/circle_01/create_food.lua");
magic_init("Detect Trap", "wj", 0x82, 1, 3, "u6/magic/circle_01/detect_trap.lua");
magic_init("Douse", "af", 0x24, 1, 5, "u6/magic/circle_01/douse.lua");
magic_init("Harm", "am", 0x84, 1, 6, "u6/magic/circle_01/harm.lua");
magic_init("Heal", "im", 0x84, 1, 7, "u6/magic/circle_01/heal.lua");
magic_init("Ignite", "if", 0x84, 1, 9, "u6/magic/circle_01/ignite.lua");

magic_init("Poison", "inp", 0x0e, 2, 3, "u6/magic/circle_02/poison.lua");
magic_init("Telekinesis", "opy", 0x0d, 2, 6, "u6/magic/circle_02/telekinesis.lua");
magic_init("Trap", "ij", 0x12, 2, 7, "u6/magic/circle_02/trap.lua");
magic_init("Unlock Magic", "ep", 0x88, 2, 8, "u6/magic/circle_02/unlock_magic.lua");
magic_init("Untrap", "aj", 0x88, 2, 9, "u6/magic/circle_02/untrap.lua");

magic_init("Dispel Field", "ag", 0x84, 3, 2, "u6/magic/circle_03/dispel_field.lua");
magic_init("Magic Lock", "ap", 0xa8, 3, 5, "u6/magic/circle_03/magic_lock.lua");

magic_init("Fire Field", "ifg", 0x94, 4, 4, "u6/magic/circle_04/fire_field.lua");
magic_init("Locate", "iw", 0x02, 4, 6, "u6/magic/circle_04/locate.lua");
magic_init("Poison Field", "ing", 0x56, 4, 8, "u6/magic/circle_04/poison_field.lua");
magic_init("Sleep Field", "izg", 0x54, 4, 9, "u6/magic/circle_04/sleep_field.lua");

magic_init("Energy Field", "isg", 0x15, 5, 1, "u6/magic/circle_05/energy_field.lua");

magic_init("Kill", "ic", 0x86, 7, 6, "u6/magic/circle_07/kill.lua");

magic_init("Eclipse", "val", 0xab, 8, 2, "u6/magic/circle_08/eclipse.lua");
magic_init("Resurrect", "imc", 0xf9, 8, 5, "u6/magic/circle_08/resurrect.lua");
magic_init("Summon", "kxc", 0x39, 8, 7, "u6/magic/circle_08/summon.lua");
magic_init("Tremor", "vpy", 0x89, 8, 9, "u6/magic/circle_08/tremor.lua");

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
