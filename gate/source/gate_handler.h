#ifndef gate_handler_h__
#define gate_handler_h__

#include "rpc/JME_RpcHandler.h"
#include "net/JME_NetHandler.h"
#include "base.h"
#include "init_helper.h"

typedef JMEngine::game::DispatcherInterface<JMEngine::net::TcpSession::TcpSessionPtr, JMEngine::net::Message::MessagePtr> GateMessageDispatcher;

struct ServerLoad
{
	string _server;
	int _client_num;

	ServerLoad(const string& server):
		_server(server),
		_client_num(0)
	{}
	bool operator <(const ServerLoad& sl) const
	{
		return _client_num < sl._client_num;
	}
	bool operator ==(const std::string& server) const
	{
		return !_server.compare(server);
	}
};

struct GateClient
{
	JMEngine::net::TcpSession::TcpSessionPtr _session;
	std::string _game_server;
	int _player_id;

	GateClient(const string& server):
		_game_server(server),
		_player_id(0)
	{}
};
class GateHandler :
	public JMEngine::net::NetHandler,
	public GateMessageDispatcher,
	public JMEngine::rpc::RpcHandlerInterface,
	public AutoInitLogicInterface
{
	friend class GateServer;
protected:
	void __init();
	auto_init_h(GateHandler);

public:
	//net handler
	void sessionConnectSucceed(JMEngine::net::TcpSession::TcpSessionPtr session);
	void sessionConnectFailed(JMEngine::net::TcpSession::TcpSessionPtr session, boost::system::error_code e);
	void sessionDisconnect(JMEngine::net::TcpSession::TcpSessionPtr session, boost::system::error_code e);
	void sessionReceiveMessage(JMEngine::net::TcpSession::TcpSessionPtr session, JMEngine::net::Message::MessagePtr msg);
	void sessionReadError(JMEngine::net::TcpSession::TcpSessionPtr session, boost::system::error_code e);
private:
	//rpc handler
	void initRPCHandler();
	google::protobuf::Message* kickoutClient(const std::string& params);
	google::protobuf::Message* sendClient(const std::string& params);
private:
	//client map
	std::map<int, GateClient> _client_map;

	std::string clientGame(int id);
public:
	void initDistributedGame();

private:
	std::string& dispatchClient();
	void clientDisconnect(const string& server);
	std::vector<ServerLoad> _server_load_vec;

private:
	//client msg handler
	void initClientMsgHandler();
	void clientHeartbeat(JMEngine::net::TcpSession::TcpSessionPtr session, const JMEngine::net::Message::MessagePtr params){};
	void sendMessageToServer(JMEngine::net::TcpSession::TcpSessionPtr session, const JMEngine::net::Message::MessagePtr params){};
	void sendMessageToServer(JMEngine::net::TcpSession::TcpSessionPtr session, const JMEngine::net::Message::MessagePtr params, const string server);

	template<class T1, class... T2>
	void sendMessageToServer(JMEngine::net::TcpSession::TcpSessionPtr session, const JMEngine::net::Message::MessagePtr params, const T1 server, const T2... servers)
	{
		sendMessageToServer(session, params, server);
		sendMessageToServer(session, params, servers...);
	}
};
#endif // gate_handler_h__