#include "gate_server.h"
#include "gate_handler.h"
#include "log/JME_GLog.h"
#include "util/JME_String.h"
#include "boost/date_time/posix_time/ptime.hpp"

void GateServer::init()
{
	_client_handler = GateHandler::__init_helper._obj;
}
