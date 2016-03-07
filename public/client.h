#ifndef client_h__
#define client_h__

#include "base.h"
#include "lua_helper.h"

namespace Json
{
	class Value;
}

struct Client :
	public LuaObj
{
	Client(const string& gate, const string& game, int pid, int net, int server, int channel):
		_gate(gate),
		_game(game),
		_pid(pid),
		_net(net),
		_server(server),
		_channel(channel)
	{

	}

	string _gate;	//玩家所在网关
	string _game;	//玩家所在game server
	string _ip_addr;
	string _ip_port; //玩家的ip地址
	string _udp_addr;
	string _udp_port; //玩家的udp地址


	int _pid;
	int _net;			//网关处的net_id,此ID 关联client与gate_server的session
	int _server;
	int _channel;

	void sendMessage(int msg_id, const string proto);
	void sendMessage(int msg_id, const google::protobuf::Message* proto);
	void sendMessage(int msg_id, const Json::Value& proto);
protected:
	Client();
	void __bindLua();
	auto_luabind_h(Client)
};

#endif // client_h__
