#include "user.h"

auto_luabind_cpp(User)
User::User(const string& gate, const string& game, int pid, int net, int server, int channel):
	Client(gate, game, pid, net, server, channel)
{

}

User::User()
{

}

Player::Ptr User::getPlayer()
{
	if (_player)
		_player->used();
	return _player;
}

void User::__bindLua()
{
	using namespace luabind;
	module(LUA_STATE)
	[
		class_<User, Client, boost::shared_ptr<User> >("User")
		.def("get_player", &User::getPlayer)
	];

	LOGT("Export class [User] to lua");
}
