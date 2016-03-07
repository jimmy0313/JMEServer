#ifndef user_h__
#define user_h__

#include "base.h"
#include "player.h"
#include "client.h"
#include "lua_helper.h"

class User :
	public Client
{
public:
	typedef boost::shared_ptr<User> Ptr;
	friend class UserLogic;
public:
	User(const string& gate, const string& game, int pid, int net, int server, int channel);

	Player::Ptr getPlayer();

protected:
	Player::Ptr _player;

protected:
	User();

	void __bindLua();
	auto_luabind_h(User)
};
#endif // user_h__
