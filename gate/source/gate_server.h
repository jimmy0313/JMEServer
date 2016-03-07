#ifndef gate_server_h__
#define gate_server_h__

#include "game/JME_ServerInterface.h"
#include "game/JME_Singleton.h"

#include "db/JME_MongoConnection.h"

class GateServer :
	public JMEngine::game::ServerInterface
{
CreateSingletonClass(GateServer);
friend class GateHandler;

private:
	void init();
};
#endif // gate_server_h__
