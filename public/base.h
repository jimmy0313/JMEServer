#ifndef Common_h__
#define Common_h__

#include <map>
#include <vector>
#include <string>
#include "boost/shared_ptr.hpp"
#include "google/protobuf/message.h"
#include "json/json.h"

#include "net/JME_TcpSession.h"
#include "net/JME_Core.h"
#include "net/JME_Message.h"
#include "log/JME_GLog.h"
#include "game/JME_Dispatcher.h"
#include "game/JME_Singleton.h"
#include "game/JME_RPCManager.h"

#include "trace.h"

using namespace std;

#define USING_NAMESPACE_JMENGINE \
using namespace JMEngine;

#define USING_NAMESPACE_JMENGINE_NET \
using namespace JMEngine::net;

#define USING_NAMESPACE_JMENGINE_RPC \
using namespace JMEngine::rpc;

#define USING_NAMESPACE_JMENGINE_GAME \
using namespace JMEngine::game;

#define USING_NAMESPACE_JMENGINE_DB \
using namespace JMEngine::db;

typedef JMEngine::game::DispatcherInterface<int, const string&> PlayerMessageDispatcher;
typedef JMEngine::game::DispatcherInterface<google::protobuf::Message*, const string&> MessageDispatcher;

#define __TRY__ \
	try {

#define __CATCH_DB_EXCEPTION_ERROR__ \
	} \
	catch(const JMEngine::db::DBException& e) \
	{ \
		LOGE(e.what());

#define __CATCH_RPC_EXCEPTION_ERROR__ \
	} \
	catch(const JMEngine::game::RPCException& e) \
	{ \
		LOGE(e.what());

#define __CATCH_STD_EXCEPTION_ERROR__ \
	} \
	catch(const std::exception& e) \
	{ \
		LOGE(e.what());

#define __TRY_END__ \
	}
#endif // Common_h__
