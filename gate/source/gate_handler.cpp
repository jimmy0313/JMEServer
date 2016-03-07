#include "gate_handler.h"
#include "db_manager.h"
#include "util/JME_String.h"
#include "gate_server.h"
#include "server_rpc.pb.h"
#include "trace.h"
#include "boost/date_time/posix_time/ptime.hpp"
#include "game/JME_RPCManager.h"
#include "msg_define.h"
#include "json/JME_JsonFile.h"

auto_init_cpp(GateHandler);

// const static int Msg_RequestLogin = 10001;

void GateHandler::__init()
{
	initDistributedGame();
	initClientMsgHandler();
	initRPCHandler();
}

void GateHandler::sessionConnectSucceed(JMEngine::net::TcpSession::TcpSessionPtr session)
{
	static int client_id = 1;
	session->start(client_id++);

	GateClient client(GateHandler::dispatchClient());
	client._session = session;
	_client_map.insert(make_pair(session->getNetId(),client));

	LOGT("Client from [ %s:%s:%d ] connected", session->getIp(), session->getPort(), session->getNetId());
}

void GateHandler::sessionConnectFailed(JMEngine::net::TcpSession::TcpSessionPtr session, boost::system::error_code e)
{

}

void GateHandler::sessionDisconnect(JMEngine::net::TcpSession::TcpSessionPtr session, boost::system::error_code e)
{
	session->stop();

	auto client_itor = _client_map.find(session->getNetId());
	if (client_itor != _client_map.end())
		clientDisconnect(client_itor->second._game_server);

	_client_map.erase(session->getNetId());

	LOGT("Client from [ %s:%s:%d ] disconnect", session->getIp(), session->getPort(), session->getNetId());
}

void GateHandler::sessionReadError(JMEngine::net::TcpSession::TcpSessionPtr session, boost::system::error_code e)
{
	sessionDisconnect(session, e);
}

void GateHandler::sessionReceiveMessage(JMEngine::net::TcpSession::TcpSessionPtr session, JMEngine::net::Message::MessagePtr msg)
{
	auto client_itor = _client_map.find(session->getNetId());
	if (client_itor != _client_map.end())
		msg->_bindId = client_itor->second._player_id > 0 ? client_itor->second._player_id : msg->_bindId;

	execMessageHandler(msg->_msgId, session, msg);
}

void GateHandler::initClientMsgHandler()
{
#define REG_MSG_0(msg_id, handler) regMessageHandler(msg_id, boost::bind(&GateHandler::handler, this, _1, _2))
#define REG_MSG_1(msg_id, server) regMessageHandler(msg_id, boost::bind(&GateHandler::sendMessageToServer<const string&>, this, _1, _2, server))
#define REG_MSG_2(msg_id, server_1, server_2) regMessageHandler(msg_id, boost::bind(&GateHandler::sendMessageToServer<const string&, const string&>, this, _1, _2, server_1, server_2))
#define REG_MSG_2_2(msg_id_1, msg_id_2, server_1, server_2) regMessageHandler(msg_id_1, msg_id_2, boost::bind(&GateHandler::sendMessageToServer<const string&, const string&>, this, _1, _2, server_1, server_2))

	REG_MSG_0(100, clientHeartbeat);
	REG_MSG_2(101, "social", "game");
	REG_MSG_2_2(12000, 12010, "battle", "game");
	REG_MSG_2(12012, "battle", "game");
	REG_MSG_2_2(12014, 12099, "battle", "game");

	REG_MSG_1(10002, "account");
}

void GateHandler::sendMessageToServer(JMEngine::net::TcpSession::TcpSessionPtr session, const JMEngine::net::Message::MessagePtr params, const string server)
{

	FUNCTIONLOG

	__TRY__

	rpc_post_message proto;
	proto.set_msg_id(params->_msgId);
	proto.set_pid(params->_bindId);
	proto.set_net_id(session->getNetId());

	auto param = params->getMessageStr();
	proto.set_params(param);
	proto.set_gate(GateServer::getInstance()->_server_name);
	proto.set_server_id(GateServer::getInstance()->_server_id);

	if (server.compare("game"))
	{
		if(!GETRPC(server)->callRpcMethod("postMessage", &proto) && !server.compare("social"))
		{
			//kick out
// 			GateServer::getInstance()->kickOut(session->getNetId(), params->_bindId);
		}
	}
	else
	{
// 		//join world msg
// 		if (params->_msgId == 11004)	
// 		{
// 			join_world_proto proto;
// 			string msg = params->getMessageStr();
// 			proto.ParseFromString(msg);
// 			int pid = proto.pid();
// 			bindPlayerSession(session->getNetId(), pid);	
// 		}
		if(!GETRPC(clientGame(session->getNetId()))->callRpcMethod("postMessage", &proto))
		{
			//kick out
// 			GateServer::getInstance()->kickOut(session->getNetId(), params->_bindId);
		}
	}

	__CATCH_RPC_EXCEPTION_ERROR__

		LOGE(e.what());

	__TRY_END__
}

void GateHandler::initDistributedGame()
{
	auto config = JMEngine::file::load_jsonfile_val("config/conf.json");
	const auto& rpc_conf = config["rpc"]["client"];
	for (auto& conf : rpc_conf)
	{
		auto name = conf["name"].asString();
		if (std::string::npos == name.find("game"))
			continue;
		_server_load_vec.emplace_back(ServerLoad(name));
	}
}

std::string& GateHandler::dispatchClient()
{
	std::sort(_server_load_vec.begin(), _server_load_vec.end(), std::less<ServerLoad>());
	auto server = _server_load_vec.begin();
	server->_client_num++;
	return server->_server;
}

void GateHandler::clientDisconnect(const string& server)
{
	auto server_load = std::find(_server_load_vec.begin(), _server_load_vec.end(), server);
	if (server_load != _server_load_vec.end())
		server_load->_client_num--;
}

std::string GateHandler::clientGame(int id)
{
	auto it = _client_map.find(id);
	if (it == _client_map.end())
		return "";
	return it->second._game_server;
}

google::protobuf::Message* GateHandler::kickoutClient(const std::string& params)
{
	__TRY__

	rpc_kickout_client proto;
	proto.ParseFromString(params);

	int client = proto.net_id();

	
	__CATCH_STD_EXCEPTION_ERROR__

	__TRY_END__
	return nullptr;
}

void GateHandler::initRPCHandler()
{
	regRpcHandler("kickout", boost::bind(&GateHandler::kickoutClient, this, _1));
	regRpcHandler("sendClient", boost::bind(&GateHandler::sendClient, this, _1));
}

google::protobuf::Message* GateHandler::sendClient(const std::string& params)
{
	rpc_send_client proto;

	__TRY__

	proto.ParseFromString(params);

	int net = proto.net_id();

	Message msg(proto.msg_id(), net, proto.params());

	auto client_itor = _client_map.find(net);
	if (client_itor != _client_map.end())
	{
		client_itor->second._session->writeMessage(msg);
	}

	__CATCH_STD_EXCEPTION_ERROR__

	__TRY_END__

	return nullptr;
}

