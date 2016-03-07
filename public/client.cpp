#include "client.h"
#include "server_rpc.pb.h"
#include "game/JME_RPCManager.h"

// 	bool ClientManagerInterface::client_login( const string& gate, const string& game, int pid, int net_id, int server_id, int channel )
// 	{
// 		bool repeated = false;
// 		Client role(gate, game, pid, net_id, server_id, channel);
// 
// 		RecursiveMutexLock(_client_mutex);
// 
// 		auto it = _client_roles.find(pid);
// 		if ( it != _client_roles.end() )
// 		{
// 			repeated = true;
// #ifdef GAMESERVER
// 			
// 			RPC_Kick rpc;
// 			rpc.set_net(it->second._net_id);
// 			rpc.set_pid(it->second._pid);
// 			GETRPC(it->second._gate)->callRpcMethod("kickOut", &rpc);
// 			
// #endif // GAMESERVER
// 
// 			_client_roles.erase(it);
// 		}
// 		_client_roles.insert(make_pair(pid,role));
// 		
// 		return repeated;
// 	}
// 
// 	void ClientManagerInterface::send_single_client_msg( int role_id, int msgId, google::protobuf::Message* msg )
// 	{
// 		__TRY__
// 
// 		RecursiveMutexLock(_client_mutex);
// 
// 		auto it = _client_roles.find(role_id);
// 		if ( it != _client_roles.end() )
// 		{
// 			RPC_SendClient proto;
// 			proto.set_msg_id(msgId);
// 			proto.set_net_id(it->second._net_id);
// 			proto.set_params(msg ? msg->SerializeAsString() : "");
// 
// 			GETRPC(it->second._gate)->callRpcMethod("sendClient", &proto);
// 		}
// 
// 		__CATCH_RPC_EXCEPTION_ERROR__
// 
// 			LOGE(e.what());
// 
// 		__TRY_END__
// 	}
// 
// 	void ClientManagerInterface::send_single_client_msg( int role_id, int msgId, const Json::Value& msg )
// 	{		
// 		__TRY__
// 
// 			RecursiveMutexLock(_client_mutex);
// 
// 		auto it = _client_roles.find(role_id);
// 		if ( it != _client_roles.end() )
// 		{
// 			RPC_SendClient proto;
// 			proto.set_msg_id(msgId);
// 			proto.set_net_id(it->second._net_id);
// 			proto.set_params(msg.toSimpleString());
// 
// 			GETRPC(it->second._gate)->callRpcMethod("sendClient", &proto);
// 		}
// 
// 		__CATCH_RPC_EXCEPTION_ERROR__
// 
// 			LOGE(e.what());
// 
// 		__TRY_END__
// 	}
// 
// 	void ClientManagerInterface::send_all_client_msg( int msgId, google::protobuf::Message* msg )
// 	{
// 		map<string, vector<int> > gate_player;
// 		set<string> gate_list;
// 
// 		__TRY__
// 
// 		RPC_SendMoreClient proto;
// 		proto.set_msg_id(msgId);
// 		proto.set_params(msg ? msg->SerializeAsString() : "");
// 
// 		RecursiveMutexLock(_client_mutex);
// 
// 		for (const auto& client : _client_roles)
// 		{			
// 			gate_player[client.second._gate].push_back(client.second._net_id);
// 		}
// 		for (const auto& gate : gate_player)
// 		{
// 			for (auto& net : gate.second)
// 			{
// 				proto.add_net_id(net);
// 			}
// 			__TRY__
// 
// 			GETRPC(gate.first)->callRpcMethod("sendClient", &proto);
// 
// 			__CATCH_RPC_EXCEPTION_ERROR__
// 
// 			LOGE(e.what());
// 
// 			__TRY_END__
// 		}
// 
// 
// 		__CATCH_STD_EXCEPTION_ERROR__
// 
// 		LOGE(e.what());
// 
// 		__TRY_END__
// 	}
// 
// 	void ClientManagerInterface::send_client_system_msg( int pid ,int msg_id, system_msg_pata_vec_t& para_list )
// 	{
// 		system_msg_proto proto;
// 		proto.set_system_id( msg_id );
// 		for ( system_msg_pata_vec_t::iterator it = para_list.begin(); it != para_list.end(); ++it )
// 		{
// 			if ( it->_type == para_proto_para_type_VALUE_INT )
// 			{
// 				para_proto* p = proto.add_para_list();
// 				p->set_type( para_proto_para_type_VALUE_INT );
// 				p->set_int_value( it->_int_value );
// 			}
// 			else if ( it->_type == para_proto_para_type_VALUE_STR )
// 			{
// 				para_proto* p = proto.add_para_list();
// 				p->set_type( para_proto_para_type_VALUE_STR );
// 				p->set_str_value( it->_str_value );
// 			}
// 		}
// 
// 		send_single_client_msg( pid, msg_header::MSG_SYS_MESSAGE, &proto );
// 	}
// 
// 	void ClientManagerInterface::send_client_system_msg( int pid ,int msg_id )
// 	{
// 		system_msg_proto proto;
// 		proto.set_system_id( msg_id );
// 
// 		send_single_client_msg( pid, msg_header::MSG_SYS_MESSAGE, &proto );
// 	}
// 
// 
// 	void ClientManagerInterface::send_client_open_ui( int pid, int uid )
// 	{
// 		open_ui_proto proto;
// 		proto.set_uid( uid );
// 
// 		send_single_client_msg( pid, msg_header::MSG_SEND_CLIENT_UI, &proto );
// 	}
// 
// 
// 	void ClientManagerInterface::send_single_client_msg(const string& gate, int net, int msgId, google::protobuf::Message* msg)
// 	{
// 		__TRY__
// 
// 			RPC_SendClient proto;
// 		proto.set_msg_id(msgId);
// 		proto.set_net_id(net);
// 		proto.set_params(msg ? msg->SerializeAsString() : "");
// 
// 		GETRPC(gate)->callRpcMethod("sendClient", &proto);
// 
// 		__CATCH_RPC_EXCEPTION_ERROR__
// 
// 			LOGE(e.what());
// 
// 		__TRY_END__
// 	}
// 
// 	bool ClientManagerInterface::remove_client( const string& gate, int role_id, int net_id )
// 	{
// 		RecursiveMutexLock(_client_mutex);
// 
// 		auto it = _client_roles.find(role_id);
// 		if ( it != _client_roles.end() )
// 		{
// 			if (!it->second._gate.compare(gate) && it->second._pid == role_id && it->second._net_id == net_id)
// 			{
// 				_client_roles.erase(it);
// 				return true;
// 			}
// 		}
// 		return false;
// 	}
// 
// 	bool ClientManagerInterface::client_correct(const string& gate, int role_id, int net_id)
// 	{
// 		RecursiveMutexLock(_client_mutex);
// 
// 		auto it = _client_roles.find(role_id);
// 		if ( it != _client_roles.end() )
// 		{
// 			if (!it->second._gate.compare(gate) && it->second._pid == role_id && it->second._net_id == net_id)
// 			{
// 				return true;
// 			}
// 		}
// 		return false;
// 	}
// 
// 	std::string ClientManagerInterface::clientGame(int pid)
// 	{
// 		auto it = _client_roles.find(pid);
// 		if (it != _client_roles.end())
// 			return it->second._game;
// 		return "error";
// 	}
// 

auto_luabind_cpp(Client)
void Client::sendMessage(int msg_id, const google::protobuf::Message* proto)
{
	sendMessage(msg_id, proto ? boost::move(proto->SerializeAsString()) : "");
}

void Client::sendMessage(int msg_id, const Json::Value& proto)
{
	sendMessage(msg_id, boost::move(proto.toSimpleString()));
}

void Client::sendMessage(int msg_id, const string proto)
{
	__TRY__

	rpc_send_client rpc;
	rpc.set_msg_id(msg_id);
	rpc.set_net_id(_net);
	rpc.set_params(proto);

	GETRPC(_gate)->callRpcMethod("sendClient", &rpc);

	__CATCH_RPC_EXCEPTION_ERROR__

	__TRY_END__
}

Client::Client()
{

}

void Client::__bindLua()
{
	using namespace luabind;
	module(LUA_STATE)
	[
		class_<Client>("Client")
		.def("send_message", (void(Client::*)(int, const string))&Client::sendMessage)
	];

	LOGT("Export class [Client] to lua");
}
