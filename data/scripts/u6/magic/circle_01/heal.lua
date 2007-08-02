actor = select_actor()

if actor ~= nil then
  actor.hp = actor.hp + math.random(1,10)
  print("\nSuccess\n")
else
  print("\nNo effect\n")
end
