#ifndef data_manager_h__
#define data_manager_h__

#include "base.h"
#include "player.h"

class CacheDataManager
{
CreateSingletonClass(CacheDataManager);

public:
	void cache(Player::Ptr ptr);
	Player::Ptr cached(int pid) const;
	void remove(int pid);

	void remvoeCache(Player::Ptr ptr);
	void checkCache(boost::shared_ptr<boost::asio::deadline_timer> dt, int pid);
private:
	map<int, Player::Ptr> _cache_player;
};
#endif // data_manager_h__
