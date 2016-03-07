#ifndef core_server_h__
#define core_server_h__

#include "game/JME_ServerInterface.h"
#include "game/JME_Singleton.h"

#include "db/JME_MongoConnection.h"

class CacheServer :
	public JMEngine::game::ServerInterface
{
CreateSingletonClass(CacheServer);
friend class CacheHandler;

private:
	void init();

	void update();
};
#endif // core_server_h__
