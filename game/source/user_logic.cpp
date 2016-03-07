#include "user_logic.h"
#include "game/JME_RPCManager.h"
#include "util/JME_String.h"
#include "db_manager.h"
#include "game_server.h"

#include "user.pb.h"
#include "server_rpc.pb.h"


auto_init_cpp(UserLogic)
auto_luabind_cpp(UserLogic)
void UserLogic::clientJoin(const string& gate, const string& game, int pid, int net, int server, int channel)
{
	auto user = getData(pid);
	if (nullptr == user)
	{
		user = User::Ptr(new User(gate, game, pid, net, server, channel));
		addData(pid, user);
	}
	if (nullptr == user->_player)
	{
		user->_player = Player::loadSHM(pid);
	}

	if (nullptr == user->_player)
	{
		rpc_load_data rpc;
		rpc.set_server(GameServer::getInstance()->_server_name);
		rpc.set_pid(pid);

		__TRY__

		GETRPC("cache")->callRpcMethod("loadData", &rpc);

		__CATCH_RPC_EXCEPTION_ERROR__

		__TRY_END__
	}
}

google::protobuf::Message* UserLogic::rpc_dataLoadComplete(const string& params)
{
	rpc_load_data rpc;
	rpc.ParseFromString(params);

	int pid = rpc.pid();

	auto user = getData(pid);
	if (nullptr != user)
		user->_player = Player::loadSHM(pid);

	return nullptr;
}

void UserLogic::__init()
{
	regRpcHandler("loadComplete", boost::bind(&UserLogic::rpc_dataLoadComplete, getInstance(), _1));
}

void UserLogic::__bindLua()
{
	using namespace luabind;
	module(LUA_STATE)
	[
		class_<UserLogic>("UserLogic")
		.def("join", &UserLogic::clientJoin)
		.def("get_data", &UserLogic::getData)
	];
	module(LUA_STATE, "UserLogic")
	[
		def("instance", UserLogic::getInstance)
	];

	LOGT("Export class [UserLogic] to lua");
}

void UserLogic::msg_1002(google::protobuf::Message* msg, const string& params)
{
	__TRY__
	
	rpc_post_message *rpc = dynamic_cast<rpc_post_message*>(msg);

	client_request_join request;
	request.ParseFromString(params);

	int account_id = request.account_id();
	int server_id = request.server_id();
	auto key = request.auth_key();

	MongoConnHelper conn(DBManager::getInstance()->mongoPool("world"));
	Json::Value res;

	static string dbname = "server.account_role";
	conn->selectJson(dbname, BSON("account_id"<<account_id<<"server_id"<<server_id<<"auth_key"<<key), &res);
	if (Json::Value::null == res)
	{
		//error
	}
	else
	{
		clientJoin(rpc->gate(), GameServer::getInstance()->_server_name, request.player_id(), rpc->net_id(), server_id, request.channel());
	}
	
	__CATCH_STD_EXCEPTION_ERROR__

	__TRY_END__
}

int UserLogic::clientServerId(int pid)
{
	auto user = getData(pid);
	if (nullptr == user)
		return 0;

	return user->_server;
}
