#ifndef event_interface_h__
#define event_interface_h__

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <memory>

using namespace std;

template<typename T, typename OW, typename RV = bool>
class SignalConnector : public std::enable_shared_from_this<SignalConnector<T, OW, RV> >
{
public:
	typedef std::shared_ptr<SignalConnector<T, OW, RV> > SignalConnPtr;
	typedef RV(*EventHandler)(const T&, OW);
	typedef std::function<RV(const T&, OW, SignalConnPtr)> EventHandlerFunction;

public:
	SignalConnector():
		_deleted(false),
		_handler(nullptr)
	{

	}
	virtual ~SignalConnector()
	{
	}
	SignalConnector(typename SignalConnector<T, OW, RV>::EventHandler handler):
		_deleted(false),
		_handler(handler)
	{

	}
	SignalConnector(typename SignalConnector<T, OW, RV>::EventHandlerFunction handler):
		_deleted(false),
		_handler(nullptr),
		_handler_function(handler)
	{

	}

	static SignalConnPtr create(typename SignalConnector<T, OW, RV>::EventHandler handler)
	{
		return typename SignalConnector<T, OW, RV>::SignalConnPtr(new SignalConnector<T, OW, RV>(handler)); 
	}
	static SignalConnPtr create(typename SignalConnector<T, OW, RV>::EventHandlerFunction handler)
	{
		return typename SignalConnector<T, OW, RV>::SignalConnPtr(new SignalConnector<T, OW, RV>(handler));
	}

	void disconnect()
	{
		_deleted = true;
	}
	bool isDeleted()
	{
		return _deleted;
	}

	RV callback(const T& evt, OW ow) // if the return value is bool, it means weather to continue post the event
	{
		if (_deleted)
			return true;

		if (_handler)
			return _handler(evt, ow);
		else
			return _handler_function(evt, ow, SignalConnector<T, OW, RV>::shared_from_this());
	}
public:
	bool _deleted;
	typename SignalConnector<T, OW, RV>::EventHandler _handler;
	typename SignalConnector<T, OW, RV>::EventHandlerFunction _handler_function;
};

template<typename T, typename OW, typename RV = bool>
class EventSignal
{
public:
	typedef std::shared_ptr<EventSignal<T, OW, RV> > EventSignalPtr;
public:
	EventSignal(OW ow){ _ow = ow; };
	virtual ~EventSignal()
	{
	}

	static EventSignalPtr create(OW ow)
	{
		return EventSignalPtr(new EventSignal<T, OW, RV>(ow));
	}

	bool ismine(OW ow) { return _ow == ow; }
	void connect( typename SignalConnector<T, OW, RV>::EventHandler handler)
	{
		if (eventHandlerConnected(handler))
			return;
		auto signal = SignalConnector<T, OW, RV>::create(handler);
		_handlers.push_back(signal);
	}
	void connect( typename SignalConnector<T, OW, RV>::EventHandlerFunction handler)
	{
		auto signal = SignalConnector<T, OW, RV>::create(handler);
		_handlers.push_back(signal);
	}

	void disconnect(typename SignalConnector<T, OW, RV>::EventHandler handler)
	{
		for (auto it = _handlers.begin(); it != _handlers.end(); ++it)
		{
			if ((*it)->_handler == handler)
			{
				(*it)->disconnect();
			}
		}
	}
	void callback(const T& evt)
	{
		for (auto cb : _handlers)
		{
			if(cb->callback(evt, _ow))
				break;
		}
	}

	bool empty()
	{
		for (auto it = _handlers.begin(); it != _handlers.end();)
		{
			if ((*it)->isDeleted())
			{
				it = _handlers.erase(it);
			}
			else
			{
				++it;
			}
		}
		return _handlers.empty();
	}
private:
	OW _ow;
	vector< typename SignalConnector<T, OW, RV>::SignalConnPtr > _handlers;

	bool eventHandlerConnected(typename SignalConnector<T, OW, RV>::EventHandler handler)
	{
		for (auto it = _handlers.begin(); it != _handlers.end(); ++it)
		{
			if ((*it)->_handler == handler)
			{
				return true;
			}
		}
		return false;
	}
};

template<typename T, typename OW, typename RV = bool>
class EventManager
{
public:
	EventManager(){};
	virtual ~EventManager(void){};

	void regEventHandler(const T& evt, OW ow, typename SignalConnector<T, OW, RV>::EventHandler handler)
	{
		auto it = _events.find(evt);
		if (it != _events.end())
		{
			auto signal = getOwSignal(ow, it->second);
			if (signal)
				return signal->connect(handler);
			else
			{
				auto signal = EventSignal<T, OW, RV>::create(ow);
				signal->connect(handler);
				it->second.push_back(signal);
			}
		}
		else
		{
			auto signal = EventSignal<T, OW, RV>::create(ow);
			signal->connect(handler);
			_events[evt].push_back(signal);
		}
	}
	void regEventHandler(const T& evt, OW ow, typename SignalConnector<T, OW, RV>::EventHandlerFunction handler)
	{
		auto it = _events.find(evt);
		if (it != _events.end())
		{
			auto signal = getOwSignal(ow, it->second);
			if (signal)
				return signal->connect(handler);
			else
			{
				auto signal = EventSignal<T, OW, RV>::create(ow);
				signal->connect(handler);
				it->second.push_back(signal);
			}
		}
		else
		{
			auto signal = EventSignal<T, OW, RV>::create(ow);
			signal->connect(handler);
			_events[evt].push_back(signal);
		}
	}

	void removeEventHandler(const T& evt, OW ow, typename SignalConnector<T, OW, RV>::EventHandler handler)
	{
		auto it = _events.find(evt);
		if (it != _events.end())
		{
			auto signal = getOwSignal(ow, it->second);
			if (signal)
				signal->disconnect(handler);
		}
	}
	void eventCallback(const T& evt)
	{
		auto it = _events.find(evt);
		if (it != _events.end())
		{
			for (auto signal : it->second)
			{
				signal->callback(evt);
			}
		}
	}
	void removeUnitEvent(OW ow)
	{
		for (auto it = _events.begin(); it != _events.end(); ++it)
		{
			removeOwSignal(ow, it->second);
		}
	}
private:
	map<T, vector<typename EventSignal<T, OW, RV>::EventSignalPtr> > _events;

	typename EventSignal<T, OW, RV>::EventSignalPtr getOwSignal(OW ow, vector<typename EventSignal<T, OW, RV>::EventSignalPtr>& signals)
	{
		for (auto signal : signals)
		{
			if (signal->ismine(ow))
				return signal;
		}
		return nullptr;
	}

	void removeOwSignal(OW ow, vector<typename EventSignal<T, OW, RV>::EventSignalPtr>& signals)
	{
		for (auto it = signals.begin(); it != signals.end(); ++it)
		{
			if(!(*it)->ismine(ow))
				continue;
			signals.erase(it);
			return;
		}
	}
};
#endif // event_interface_h__