#ifndef user_logic_h__
#define user_logic_h__

#include "base.h"

#include "rpc/JME_RpcServer.h"
#include "google/protobuf/message.h"
#include "init_helper.h"
#include "data_interface.h"
#include "lua_helper.h"

#include "user.h"

class UserLogic :
	public JMEngine::rpc::RpcHandlerInterface,
	public DataManagerInterface<int, User::Ptr>,
	public AutoInitLogicInterface,
	public MessageDispatcher,
	public LuaObj
{
CreateSingletonClass(UserLogic);

public:
	void clientJoin(const string& gate, const string& game, int pid, int net, int server, int channel);
	int clientServerId(int pid);
private:
	void msg_1002(google::protobuf::Message* msg, const string& params);
private:
	//rpc handler
	google::protobuf::Message* rpc_dataLoadComplete(const string& params);

protected:
	void __init();
	auto_init_h(UserLogic);

protected:
	void __bindLua();
	auto_luabind_h(UserLogic);
};
#endif // user_logic_h__
