#include "player.h"
#include "game/JME_SharedMemory.h"
#include "util/JME_String.h"
#include "db_manager.h"
#include "db/JME_MongoConnection.h"

USING_NAMESPACE_JMENGINE;
USING_NAMESPACE_JMENGINE_GAME;

auto_luabind_cpp(Player)
Player::Player()
{
	__used_time = time(NULL);
	__offline = false;

	memset(_name, 0, sizeof(_name));
	_level = 1;
	_exp = 0;
}

Player::~Player()
{

}

Player::Ptr Player::loadSHM(int id)
{
	string name = JMEngine::tools::createString("Player.%d", id);
	auto ptr = SharedMemory<Player>::create(boost::interprocess::open_only, name.c_str(), boost::interprocess::read_write);
	if (nullptr != ptr)
		return ptr;
	return boost::shared_ptr<Player>();
}

Player::Ptr Player::loadDB(int id, int sid)
{
	string name = JMEngine::tools::createString("Player.%d", id);
	auto ptr = SharedMemory<Player>::create(boost::interprocess::create_only, name.c_str(), boost::interprocess::read_write);
	ptr->dbLoad(id, sid);
	return ptr;
}

Player::Ptr Player::create()
{
	auto dbpool = DBManager::getInstance()->mongoPool("world");
	if (nullptr != dbpool)
	{
		__TRY__

		MongoConnHelper conn(dbpool);
		int id = conn->createAutoIncId("server", "player");
		string name = JMEngine::tools::createString("Player.%d", id);

		auto ptr = SharedMemory<Player>::create(boost::interprocess::create_only, name.c_str(), boost::interprocess::read_write);
		ptr->Id(id);
		return ptr;

		__CATCH_DB_EXCEPTION_ERROR__

		__TRY_END__
	}
	return boost::shared_ptr<Player>();
}

bool Player::expire()
{
	return time(NULL) - __used_time > 3600 * 24;
}

void Player::remove(int sid)
{
	dbWrite(_id, sid);
}

void Player::used()
{
	__used_time = time(NULL);
}

bool Player::dbLoad(int pid, int sid)
{
	__dbLoad(pid, sid);
	_item_box.dbLoad(pid, sid);
	_hero_box.dbLoad(pid, sid);
	_weapon_box.dbLoad(pid, sid);
	return true;
}

bool Player::dbWrite(int pid, int sid)
{
	__dbWrite(pid ,sid);
	_item_box.dbWrite(pid, sid);
	_hero_box.dbWrite(pid, sid);
	_weapon_box.dbWrite(pid, sid);
	return true;
}

bool Player::__dbLoad(int pid, int sid)
{
	_id = pid;
	return true;
}

bool Player::__dbWrite(int pid, int sid)
{
	return true;
}

void Player::__bindLua()
{
	using namespace luabind;

	module(LUA_STATE)
	[
		class_<Player, boost::shared_ptr<Player> >("Player")
		.def("id", (int(Player::*)()const)&Player::Id)
 		.def("name", (string(Player::*)()const)&Player::Name)
		.def("name", (void(Player::*)(string))&Player::Name)
		.def("item_box", &Player::getItemBox)
		.def("hero_box", &Player::getHeroBox)
		.def("weapon_box", &Player::getWeaponBox)
	];
	module(LUA_STATE, "Player")
	[
		def("create", Player::create),
		def("load_from_shm", Player::loadSHM)
	];

	LOGT("Export class [Player] to lua");
}
