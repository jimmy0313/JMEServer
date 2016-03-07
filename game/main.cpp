#include "log/JME_GLog.h"
#include "net/JME_Core.h"
#include "game_server.h"

#include "lua_helper.h"
#include "base.h"

#include "user.pb.h"
std::map<string, JMEngine::log::GLogLevel> log_level;
void initLogLevel()
{
	log_level["trace"] = JMEngine::log::GLog_TRACE;
	log_level["debug"] = JMEngine::log::GLog_DEBUG;
	log_level["info"] = JMEngine::log::GLog_INFO;
	log_level["off"] = JMEngine::log::GLog_OFF;
	log_level["warn"] = JMEngine::log::GLog_WARN;
	log_level["error"] = JMEngine::log::GLog_ERROR;
}

bool logCmd(const string& c)
{
	auto it = log_level.find(c);
	if (it != log_level.end())
	{
		logger.setLogLevel(it->second);
		return true;
	}
	return false;
}

int main()
{
	JMECore.start();
	logger.readConfig();
	GameServer::getInstance()->onInit();

	initLogLevel();
	while(1)
	{
		string c;
		std::cin >> c;
		std::cout << "get input cmd [ " << c << " ]" << std::endl;
		if (logCmd(c))
			continue;

		if (!c.compare("quit")) 
		{
			break;
		}
		else if(!c.compare("test"))
		{
			string id;
			std::cin >> id;
			int pid = boost::lexical_cast<int>(id);
			client_request_join proto;
			proto.set_account_id(1);
			proto.set_server_id(1);
			proto.set_player_id(pid);
			proto.set_auth_key("testetse");
			proto.set_channel(3);

			auto params = proto.SerializeAsString();
			PlayerMessageDispatcher::execMessageHandler(1001, pid, params);
		}
	}

	JMECore.stop();
	return 0;
}