#ifndef cache_handler_h__
#define cache_handler_h__

#include "rpc/JME_RpcHandler.h"
#include "base.h"
#include "init_helper.h"

class CacheHandler :
	public JMEngine::rpc::RpcHandlerInterface,
	public AutoInitLogicInterface
{
protected:
	void __init();
	auto_init_h(CacheHandler);

private:
	//rpc handler

	google::protobuf::Message* loadData(const std::string& params);
};
#endif // cache_handler_h__