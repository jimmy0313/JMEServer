#ifndef account_server_h__
#define account_server_h__

#include "game/JME_ServerInterface.h"
#include "game/JME_Singleton.h"

#include "db/JME_MongoConnection.h"

class AccountServer :
	public JMEngine::game::ServerInterface
{
CreateSingletonClass(AccountServer);

private:
	void init();
};
#endif // account_server_h__
