magic_casting_effect()

qty = math.random(4,9)

obj = Obj.new(129,0,0,qty)

Obj.moveToInv(obj, 1)

if obj.qty > 0 then
  print("\nCreate " .. obj.qty .. " food.\n")
end
