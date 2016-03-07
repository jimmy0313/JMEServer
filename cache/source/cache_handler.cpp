#include "cache_handler.h"
#include "db_manager.h"
#include "server_rpc.pb.h"
#include "trace.h"
#include "game/JME_RPCManager.h"
#include "player.h"
#include "data_manager.h"

auto_init_cpp(CacheHandler);

void CacheHandler::__init()
{
	regRpcHandler("loadData", boost::bind(&CacheHandler::loadData, this, _1));
}

google::protobuf::Message* CacheHandler::loadData(const std::string& params)
{
	__TRY__

	rpc_load_data rpc;
	rpc.ParseFromString(params);
	
	int pid = rpc.pid();
	int sid = 1;
	auto ptr = Player::loadDB(pid, sid);

	CacheDataManager::getInstance()->cache(ptr);

	LOGT("Cache data for player [ %d ] from db", pid);

	GETRPC(rpc.server())->callRpcMethod("loadComplete", &rpc);

	__CATCH_RPC_EXCEPTION_ERROR__

	__CATCH_STD_EXCEPTION_ERROR__

	__TRY_END__
	return nullptr;
}
