#ifndef JME_ServerInterface_h__
#define JME_ServerInterface_h__

#include "json/json.h"

#include "net/JME_TcpSession.h"
#include "net/JME_NetHandler.h"
#include "net/JME_Message.h"
#include "rpc/JME_RpcClient.h"
#include "rpc/JME_RpcServer.h"
#include "game/JME_Dispatcher.h"
#include "game/JME_RPCManager.h"


using namespace JMEngine;
using namespace JMEngine::net;
using namespace JMEngine::rpc;
using namespace JMEngine::game;

namespace JMEngine
{
	namespace game
	{
		class ServerInterface
		{
		public:
			void onInit();

		private:
			//************************************
			// Method:    init
			// FullName:  JMEngine::game::ServerInterface::init
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: 逻辑服务器进行一些初始化， 例如数据库等
			//************************************
			virtual void init();

			//************************************
			// Method:    init_
			// FullName:  JMEngine::game::ServerInterface::init_
			// Access:    public 
			// Returns:   void
			// Qualifier: 初始化各种连接， 必须在 各个handler初始化完毕后调用
			//************************************
			void init_();
		public:

			size_t _server_id;
			string _server_name;	//服务器名 形如 name + server_id + idx, ps: "game-server-1-01", "chat-server-1"

		protected:
			Json::Value _config;

			// 主要用于网关服务器， 接受客户端连接
			TcpAcceptor::TcpAcceptorPtr _acceptor_ptr;
			NetHandler::NetHandlerPtr _client_handler;	//主动连接我的“客户端”的请求消息处理
		};
	}
}
#endif // JME_ServerInterface_h__
