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

	string _gate;	//�����������
	string _game;	//�������game server
	string _ip_addr;
	string _ip_port; //��ҵ�ip��ַ
	string _udp_addr;
	string _udp_port; //��ҵ�udp��ַ


	int _pid;
	int _net;			//���ش���net_id,��ID ����client��gate_server��session
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
