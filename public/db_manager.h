#ifndef db_manager_h__
#define db_manager_h__

#include "base.h"

#include "db/JME_DBPool.h"
#include "db/JME_MongoConnection.h"
#include "db/JME_MysqlConnection.h"

#include "game/JME_Singleton.h"
#include "json/json.h"

typedef JMEngine::db::DBPool<JMEngine::db::MongoConnection> MongoDBPool;
typedef JMEngine::db::DBHelper<JMEngine::db::MongoConnection> MongoConnHelper;

typedef JMEngine::db::DBPool<JMEngine::db::MysqlConnector> MysqlDBPool;
typedef JMEngine::db::DBHelper<JMEngine::db::MysqlConnector> MysqlConnHelper;

class DBManager
{
CreateSingletonClass(DBManager);

public:
	void init(const Json::Value& conf);

	MongoDBPool::DBPoolPtr mongoPool(const char* name);
	MysqlDBPool::DBPoolPtr mysqlPool(const char* name);

private:
	map<string, MongoDBPool::DBPoolPtr> _mongo_pool;
	map<string, MysqlDBPool::DBPoolPtr> _mysql_pool;
};
#endif // db_manager_h__
