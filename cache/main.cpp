#include "log/JME_GLog.h"
#include "net/JME_Core.h"
#include "cache_server.h"
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
	CacheServer::getInstance()->onInit();

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
	}

	JMECore.stop();
	return 0;
}