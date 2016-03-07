#include "db_manager.h"


static bool connect_mongo(JMEngine::db::MongoConnection* conn, const string& ip, const string& db, const string& user, const string& pwd)
{
	string err = "";
	if (!conn->connect(ip, &err))
	{
		LOGE(err);
		return false;
	}
	if (!conn->auth(db, user, pwd, &err))
	{
		LOGE(err);
		return false;
	}
	return true;
}
static bool connect_mysql(JMEngine::db::MysqlConnector* conn, const string& ip, const string& user, const string& pwd, const string& db, const unsigned short port)
{
	conn->setConfig(ip, user, pwd, db, port);
	return conn->connect();
}
void DBManager::init(const Json::Value& conf)
{
	for (auto itor = conf["mongo"].begin(); itor != conf["mongo"].end(); ++itor)
	{
		auto name = itor.key().asString();
		auto& cfg = *itor;

		auto pool = MongoDBPool::create(cfg["num"].asUInt(),boost::bind(&connect_mongo, _1, cfg["host"].asString(), cfg["db"].asString(), cfg["user"].asString(), cfg["passwd"].asString()));

		_mongo_pool[name] = pool;
	}
	for (auto itor = conf["mysql"].begin(); itor != conf["mysql"].end(); ++itor)
	{
		auto name = itor.key().asString();
		auto& cfg = *itor;

		auto pool = MysqlDBPool::create(cfg["num"].asUInt(),boost::bind(&connect_mysql, _1, cfg["host"].asString(), cfg["user"].asString(), cfg["passwd"].asString(), cfg["db"].asString(), cfg["port"].asInt()));

		_mysql_pool[name] = pool;
	}
}

MongoDBPool::DBPoolPtr DBManager::mongoPool(const char* name)
{
	auto it = _mongo_pool.find(name);
	if (it == _mongo_pool.end())
	{
		return MongoDBPool::DBPoolPtr();
	}
	return it->second;
}

MysqlDBPool::DBPoolPtr DBManager::mysqlPool(const char* name)
{
	auto it = _mysql_pool.find(name);
	if (it == _mysql_pool.end())
	{
		return MysqlDBPool::DBPoolPtr();
	}
	return it->second;
}
