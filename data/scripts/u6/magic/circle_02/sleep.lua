  local actor = select_actor()
  if actor ~= nil then
  	local caster = magic_get_caster()
  
  	projectile(0x17d, caster.x, caster.y, actor.x, actor.y, 2)
  	local ret = spell_put_actor_to_sleep(caster,actor)
  	
  	if ret == 0xfe then
  		print("\nSuccess\n")
  		--FIXME play sfx 0xe
  	else
  		if ret == 1 then
  			print("\nFailed\n")
  		elseif ret == 2 then
  			print("\nNo effect\n")
  		end
  		--FIXME play_sfx 0xd here.	
  	end
  end
