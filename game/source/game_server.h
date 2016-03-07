#ifndef game_server_h__
#define game_server_h__

#include "game/JME_ServerInterface.h"
#include "game/JME_Singleton.h"

#include "db/JME_MongoConnection.h"

class GameServer :
	public JMEngine::game::ServerInterface
{
CreateSingletonClass(GameServer);
friend class MessageHandler;

private:
	void init();
};
#endif // game_server_h__
