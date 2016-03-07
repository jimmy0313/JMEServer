#include "game_server.h"
#include "log/JME_GLog.h"
#include "util/JME_String.h"
#include "boost/date_time/posix_time/ptime.hpp"
#include "db_manager.h"

#include "lua_helper.h"
#include "luapb/lua_pb.h"
void GameServer::init()
{
	DBManager::getInstance()->init(_config["db"]);

	luaopen_luapb(LUA_STATE);
}
