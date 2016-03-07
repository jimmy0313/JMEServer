#ifndef JME_Dispatcher_h__
#define JME_Dispatcher_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	消息分发类， 游戏逻辑 继承至该类， 初始化注册消息回调函数， 收到消息后自动回调
	warning:	
*********************************************************************/


#include "boost/shared_ptr.hpp"

namespace JMEngine
{
	namespace game
	{
		template<class T1, class T2>
		class DispatcherInterface
		{
		public:
			typedef boost::shared_ptr<DispatcherInterface> DispatcherPtr;
			typedef boost::function<void(T1 client/*指定客户端的代表，可以是一个session， 也可以是一个id，或者一个字符串*/, T2 params)> MessageHandler;
			typedef map<int, MessageHandler> MessageHandlerMap;
		public:

			//************************************
			// Method:    regMessageHandler
			// FullName:  JMEngine::game::Dispatcher<T>::regMessageHandler
			// Access:    public static 
			// Returns:   void
			// Qualifier: reg handler for msgId
			// Parameter: int msgId
			// Parameter: MessageHandler handler
			//************************************
			static void regMessageHandler(int msg_id, MessageHandler handler);

			//************************************
			// Method:    regMessageHandler
			// FullName:  JMEngine::game::Dispatcher<T>::regMessageHandler
			// Access:    public static 
			// Returns:   void
			// Qualifier: reg handler from beginMsg to endMsg
			// Parameter: int beginMsg
			// Parameter: int endMsg
			// Parameter: MessageHandler handler
			//************************************

			static void regMessageHandler(int begin_msg, int end_msg, MessageHandler handler);

			//************************************
			// Method:    execMessageHandler
			// FullName:  JMEngine::game::Dispatcher<T>::execMessageHandler
			// Access:    public static 
			// Returns:   void
			// Qualifier: call msg handler for msgId
			// Parameter: int msgId
			// Parameter: JMEngine::net::TcpSessionPtr session
			// Parameter: const T & params
			//************************************
			static void execMessageHandler(int msg_id, T1 client, T2 params);

			//************************************
			// Method:    removeMessageHandler
			// FullName:  JMEngine::game::DispatcherInterface<T1, T2>::removeMessageHandler
			// Access:    public static 
			// Returns:   void
			// Qualifier: remove msg handler for shield client msg id
			// Parameter: int msg_id
			//************************************
			static void removeMessageHandler(int msg_id);

			//************************************
			// Method:    setDefaultHandler
			// FullName:  JMEngine::game::DispatcherInterface<T1, T2>::setDefaultHandler
			// Access:    public static 
			// Returns:   void
			// Qualifier: use this function to handle msg when we don't find any handler for this msg
			// Parameter: MessageHandler handler
			//************************************
			static void setDefaultHandler(MessageHandler handler);
		private:
			static MessageHandlerMap& getHandleMap()
			{
				static MessageHandlerMap handler_map;
				return handler_map;
			};
			
			static MessageHandler _default_handler;
		};

		template<class T1, class T2>
		typename JMEngine::game::DispatcherInterface<T1, T2>::MessageHandler JMEngine::game::DispatcherInterface<T1, T2>::_default_handler;

		template<class T1, class T2>
		void JMEngine::game::DispatcherInterface<T1, T2>::setDefaultHandler(MessageHandler handler)
		{
			_default_handler = handler;
		}

		template<class T1, class T2>
		void JMEngine::game::DispatcherInterface<T1, T2>::regMessageHandler( int begin_msg, int end_msg, MessageHandler handler )
		{
			for (int i = begin_msg; i <= end_msg; i++)
			{
				regMessageHandler(i, handler);
			}
		}

		template<class T1, class T2>
		void JMEngine::game::DispatcherInterface<T1, T2>::regMessageHandler( int msg_id, MessageHandler handler )
		{
			auto& handler_map = JMEngine::game::DispatcherInterface<T1, T2>::getHandleMap();
			auto res = handler_map.insert(make_pair(msg_id,handler)); 
			if(!res.second)
			{
				LOGE("Message [ %d ] had been registered", msg_id);
				abort();
			}
		}

		template<class T1, class T2>
		void JMEngine::game::DispatcherInterface<T1, T2>::execMessageHandler( int msg_id, T1 client, T2 params )
		{
			auto& handler_map = JMEngine::game::DispatcherInterface<T1, T2>::getHandleMap();
			auto it = handler_map.find(msg_id);
			if(it != handler_map.end())
			{
				it->second(client, params);
			}
			else
			{
				if (_default_handler)
				{
					_default_handler(client, params);
				}
				else
				{
					LOGW("Can't find message handler for msg [ %d ]", msg_id);
				}
			}
		}


		template<class T1, class T2>
		void JMEngine::game::DispatcherInterface<T1, T2>::removeMessageHandler(int msg_id)
		{
			auto& handler_map = JMEngine::game::DispatcherInterface<T1, T2>::getHandleMap();
			auto it = handler_map.find(msg_id);
			if ( it != handler_map.end())
			{
				handler_map.erase(it);
			}
		}
	}
}
#endif // JME_Dispatcher_h__
