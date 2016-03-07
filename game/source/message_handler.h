#ifndef message_handler_h__
#define message_handler_h__

#include "rpc/JME_RpcHandler.h"
#include "base.h"
#include "init_helper.h"

class MessageHandler :
	public JMEngine::rpc::RpcHandlerInterface,
	public AutoInitLogicInterface
{
protected:
	void __init();
	auto_init_h(MessageHandler);

private:
	//rpc handler

	google::protobuf::Message* postMessage(const string& params);

	void messageHandler(int pid, const string& params);
};
#endif // message_handler_h__