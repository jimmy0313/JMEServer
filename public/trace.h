#ifndef Trace_h__
#define Trace_h__

#include "log/JME_GLog.h"

#define TRACE_FUNCTION
#ifdef TRACE_FUNCTION

class FunctionLog final
{
public:
	FunctionLog(const char* func):
		_func(func)
	{
		LOGT("Start function [ %s ]", func);
		_startTime = boost::get_system_time();
	}
	~FunctionLog()
	{
		boost::system_time endTime = boost::get_system_time();
		int elapse = (int)(endTime - _startTime).total_milliseconds();
		if (elapse >= 3)
		{
			LOGT("End function [ %s ], consume [ %s ] ms", _func, elapse);
		}
	}
private:
	const char* _func;
	boost::system_time _startTime;
};

#define FUNCTIONLOG \
	auto log = FunctionLog(__FUNCTION__);

#else

#define FUNCTIONLOG

#endif

class MsgLog final
{
public:
	MsgLog(int msgId):
		_msgId(msgId)
	{
		_startTime = boost::get_system_time();
	}
	~MsgLog()
	{
		boost::system_time endTime = boost::get_system_time();
		int elapse = (int)(endTime - _startTime).total_milliseconds();
		if (elapse >= 3)
		{
			LOGT("Msg [ %d ] consume [ %s ] ms", _msgId, elapse);
		}
	}
private:
	int _msgId;
	boost::system_time _startTime;
};
#define MSGLOG(id) auto log = MsgLog(id);
#define DEFAULTMSGLOG auto log = MsgLog(proto.msg_id());

#endif // Trace_h__
