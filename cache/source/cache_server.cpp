#include "cache_server.h"
#include "log/JME_GLog.h"
#include "util/JME_String.h"
#include "boost/date_time/posix_time/ptime.hpp"
#include "data_manager.h"
#include "db_manager.h"

#include "game/JME_TimerTask.h"
void CacheServer::init()
{
	DBManager::getInstance()->init(_config["db"]);
}