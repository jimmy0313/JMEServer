#include "account_server.h"
#include "log/JME_GLog.h"
#include "db_manager.h"

#include "util/JME_String.h"
#include "boost/date_time/posix_time/ptime.hpp"

void AccountServer::init()
{
	DBManager::getInstance()->init(_config["db"]);
}
