#include "message_handler.h"
#include "server_rpc.pb.h"
#include "trace.h"
#include "lua_helper.h"
#include "user_logic.h"

auto_init_cpp(MessageHandler);

void MessageHandler::__init()
{
	int ret = luaL_dofile(LUA_STATE, "lua/msg.lua");

	PlayerMessageDispatcher::setDefaultHandler(boost::bind(&MessageHandler::messageHandler, this, _1, _2));
}

google::protobuf::Message* MessageHandler::postMessage(const string& params)
{
	__TRY__

	rpc_post_message rpc;
	rpc.ParseFromString(params);

	MessageDispatcher::execMessageHandler(rpc.msg_id(), &rpc, rpc.params());
	PlayerMessageDispatcher::execMessageHandler(rpc.msg_id(), rpc.pid(), rpc.params());

	__CATCH_STD_EXCEPTION_ERROR__
	__TRY_END__

	return nullptr;
}

void MessageHandler::messageHandler(int pid, const string& params)
{
	using namespace luabind;

	auto user = UserLogic::getInstance()->getData(pid);
// 	if (!user)
// 		return;
	
	__TRY__

	int ret = luaL_dofile(LUA_STATE, "lua/msg.lua");

	if (user)
	{
		luabind::call_function<void>(LUA_STATE, "dispatch", user, params);
	}
	else
	{
		luabind::call_function<void>(LUA_STATE, "dispatch", pid, params);
	}

	__CATCH_STD_EXCEPTION_ERROR__

	LOGE(lua_tostring(LUA_STATE, -1));

	__TRY_END__
}
