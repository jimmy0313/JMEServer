#include "data_manager.h"
#include "db_manager.h"
#include "game/JME_TimerTask.h"

void CacheDataManager::cache(Player::Ptr ptr)
{
	auto res = _cache_player.insert(make_pair(ptr->Id(), ptr));
	if (!res.second)
	{
		LOGE("Push data for player [ %d ] to cache failed", ptr->Id());
	}

	int pid = ptr->Id();
	JMEngine::game::TimerTaskCenter::callbackFromNow(10, boost::bind(&CacheDataManager::checkCache, this, _1, pid));	
}

Player::Ptr CacheDataManager::cached(int pid) const
{
	auto it = _cache_player.find(pid);
	return it != _cache_player.end() ? it->second : Player::Ptr();
}

void CacheDataManager::remove(int pid)
{
	auto it = _cache_player.find(pid);
	if (it != _cache_player.end())
	{
		_cache_player.erase(it);
	}
}

void CacheDataManager::remvoeCache(Player::Ptr ptr)
{
	auto dbpool = DBManager::getInstance()->mongoPool("world");
	if (nullptr == dbpool)
		return;

	__TRY__

	static auto dbname = "server.account_role";
	Json::Value res;

	MongoConnHelper conn(dbpool);
	conn->selectJson(dbname, BSON("pid"<<ptr->Id()), &res);
	if (Json::Value::null == res)
		return;

	int server_id = res["server_id"].asInt();
	ptr->remove(server_id);

	remove(ptr->Id());

	__CATCH_DB_EXCEPTION_ERROR__

	__CATCH_STD_EXCEPTION_ERROR__

	__TRY_END__
}

void CacheDataManager::checkCache(boost::shared_ptr<boost::asio::deadline_timer> dt, int pid)
{
	auto ptr = cached(pid);
	if (ptr && (!ptr->__offline || !ptr->expire()))
	{
		JMEngine::game::TimerTaskCenter::callbackAtTime(ptr->__used_time + 10, boost::bind(&CacheDataManager::checkCache, this, _1, pid));
		return;
	}
	if (ptr)
	{
		LOGT("Remove unused cache data for player [ %d ]", pid);

		remvoeCache(ptr);
	}
}
