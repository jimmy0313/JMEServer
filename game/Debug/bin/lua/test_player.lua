
UserLogic.instance():join(tostring("gate-1"), tostring("game-1"), 1, 1, 1, 1)

print("========== 1")
local user = UserLogic.instance():get_data(1)

--[[
local player = user.player

print(player:name())
player:name("test2")

b = player:get_bag()

local c3 = b:get_cell(0)
print(c3.item_id)
print(c3.item_num)

print(b.bag_size)
for i = 0, b.bag_size - 1 do
	local c2 = b:get_cell(i)
	print(i .. "==========>" .. c2.item_id .. " " .. c2.item_num)
end

local empty_cell = b:get_empty_cell()
if nil ~= empty_cell then
	empty_cell.item_id = 1001
	empty_cell.item_num = 1
end

local c = b:get_cell(0)
print(c.item_id)
print(c.item_num)

local c1 = b:get_cell(1)
print(c1.item_id)
print(c1.item_num)


for i = 1, #a:cell() do
	print(a.cell[i])
	print(a.cell[i].item_id)
end

]]--

pb.import("protocol/account.proto")
local proto = pb.new("server_response_player_info")
proto.player_id = 0

msg = pb.to_string(proto)

user:send_message(1001, msg)

print(user.player:name())
