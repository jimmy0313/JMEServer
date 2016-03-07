function dispatch(user, msg)

	if "number" == (type(user)) then
		UserLogic.instance():join(tostring("gate-1"), tostring("game-1"), user, 1, 1, 1)
		return
	end
	--local user = UserLogic.instance():get_data(0)
	if nil == user then
		--UserLogic.instance():join(tostring("gate-1"), tostring("game-1"), 1, 1, 1, 1)
		return
	end

	local player = user:get_player()
	if nil == player then
		return
	end

	local empty_cell = player:item_box():get_new()
	if nil ~= empty_cell then
		empty_cell.id = 1001
		empty_cell.num = 1
	end
	print(player:item_box().size)
	for i = 0, player:item_box().size - 1 do
		local item = player:item_box():get(i)
		print('item: ' .. item.id .. ' num: ' .. item.num)
	end

	local hero_box = player:hero_box()
	local hero = hero_box:get(1)
	if 0 == hero.id then
		local id = HeroBox.create_new_id()
		hero = hero_box:get_new()
		hero.id = id
	else
		print(hero.id .. ' ' .. hero.level .. ' ' .. hero.exp)
		local exp = math.random(1, 1000)
		hero.exp = hero.exp + exp

		print(hero.id .. " get exp " .. exp)
		while true
		do
			local need_exp = get_level_exp(hero.level)
			if hero.exp >= need_exp then
				hero.exp = hero.exp - need_exp
				hero.level = hero.level + 1
			else
				break
			end
		end
		print(hero.id .. ' ' .. hero.level .. ' ' .. hero.exp)

		if 0 == hero.weapon then
			print(hero.id .. ' doesn\'t equip a weapon')
			local id = WeaponBox.create_new_id()
			local weapon_box = player:weapon_box()
			local weapon = weapon_box:get_new()
			weapon.id = id
			hero.weapon = weapon.id
		else
			print(hero.id .. ' equip weapon ' .. hero.weapon)
		end
	end


	pb.import("protocol/user.proto")
	local proto = pb.new("client_request_join")
	pb.from_string(proto, msg)

	user:send_message(1001, msg)
	print(pb.debug(proto))
end

function get_level_exp(level)

	return math.pow(level, 2) + 100

end
