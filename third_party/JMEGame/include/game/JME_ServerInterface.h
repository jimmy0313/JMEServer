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
			// Qualifier: �߼�����������һЩ��ʼ���� �������ݿ��
			//************************************
			virtual void init();

			//************************************
			// Method:    init_
			// FullName:  JMEngine::game::ServerInterface::init_
			// Access:    public 
			// Returns:   void
			// Qualifier: ��ʼ���������ӣ� ������ ����handler��ʼ����Ϻ����
			//************************************
			void init_();
		public:

			size_t _server_id;
			string _server_name;	//�������� ���� name + server_id + idx, ps: "game-server-1-01", "chat-server-1"

		protected:
			Json::Value _config;

			// ��Ҫ�������ط������� ���ܿͻ�������
			TcpAcceptor::TcpAcceptorPtr _acceptor_ptr;
			NetHandler::NetHandlerPtr _client_handler;	//���������ҵġ��ͻ��ˡ���������Ϣ����
		};
	}
}
#endif // JME_ServerInterface_h__
