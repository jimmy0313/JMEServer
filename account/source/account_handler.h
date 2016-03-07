#ifndef account_handler_h__
#define account_handler_h__

#include "init_helper.h"
#include "rpc/JME_RpcHandler.h"
#include "base.h"

typedef JMEngine::game::DispatcherInterface<google::protobuf::Message*, const string&> AccountMessageDispatcher;

struct LoginClient
{
	int _account_id;
	std::string _gate;
	int _net_id;
	int _server_id;

	LoginClient():
		_account_id(-1),
		_gate(""),
		_net_id(-1),
		_server_id(1)
	{}

	LoginClient(int account, const string& gate, int net_id, int server_id):
		_account_id(account),
		_gate(gate),
		_net_id(net_id),
		_server_id(server_id)
	{}
};

struct LoginClientKey
{
	int _account_id;
	int _server_id;

	bool operator < (const LoginClientKey& key) const;
};
class AccountHandler :
	public AccountMessageDispatcher,
	public JMEngine::rpc::RpcHandlerInterface,
	public AutoInitLogicInterface
{
protected:
	void __init();
	auto_init_h(AccountHandler);

private:
	google::protobuf::Message* postMessage(const string& params);

	void msg_10001(google::protobuf::Message* rpc, const string& msg);

private:
	//logined client
	std::map<LoginClientKey, LoginClient> _logined_client_map;

	LoginClient* lastLoginClient(int account_id, int server_id);
	void clientLogin(int account_id, const string& gate, int net_id, int server_id);
	void kickoutClient(const LoginClient* client);
};
#endif // account_handler_h__
