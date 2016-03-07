#include "battle_event.h"

BattleEvent::BattleEvent(const string& evt, int tar) :
	_event(evt),
	_target_id(tar)
{

}

int BattleEvent::get(unsigned char index) const
{
	if (index >= _params.size())
		return -1;
	return _params[index];
}

bool BattleEvent::operator<(const BattleEvent& evt) const
{
	if (_event < evt._event) 
		return true;
	if (_event > evt._event) 
		return false;

	if (_target_id < evt._target_id) 
		return true;
	if (_target_id > evt._target_id) 
		return false;

	return false;
}

void Counter::reset()
{
	_counter = 0;
}

void Counter::add(float v)
{
	_counter += v;
}

void Counter::del(float v)
{
	_counter-= v;
}

Counter* CounterManager::get(ActionUnit* unit, const string& cmd)
{
	for (auto c : _counter)
	{
		if (!c->_cmd.compare(cmd) && c->_father == unit)
		{
			return c;
		}
	}
	Counter* c = new Counter;
	c->_father = unit;
	c->_cmd = cmd;
	c->_counter = 0;

	_counter.push_back(c);
	return c;
}

CounterManager::~CounterManager()
{
	for (auto c : _counter)
	{
		delete c;
	}
}

void CounterManager::update(float delta)
{
	for (auto c : _counter)
	{
		if ( c->_cmd.find("interval") == std::string::npos)
			continue;
		c->add(delta);
	}
}

void CounterManager::removeUnitCounter(ActionUnit* unit)
{
	for (auto it = _counter.begin(); it != _counter.end(); )
	{
		if ((*it)->_father == unit)
		{
			delete (*it);
			it = _counter.erase(it);
		}
		else
			++it;
	}
}
