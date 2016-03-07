#include "skill_trigger.h"
#include "skill.h"
#include "event_interface.h"
#include "battle_event.h"
#include "battle_unit.h"
#include "battle.h"

auto_init_cpp(TriggerManager);
void TriggerManager::conditionHandler(const char* cond, ConditionHandler handler)
{
	auto& handler_map = getHandler();
	handler_map.insert(make_pair(cond, handler));
}

bool TriggerManager::random(const Condition& cond, ActionUnit* unit)
{
	int ra = unit->father()->_group->_battle->random();
	if (ra >= cond.getConditionTimes())
		return false;
	return true;
}

bool TriggerManager::targetDead(const Condition& cond, ActionUnit* unit)
{
	if (unit->father()->dead())
		return true;
	return false;
}

bool TriggerManager::equipWeapon(const Condition& cond, ActionUnit* unit)
{
	return true;
}

bool TriggerManager::canTrigger(int nid, const Condition& cond, ActionUnit* unit)
{
	if (SELF == cond._source && unit->_id == nid)
		return true;
	else if(MATE == cond._source && unit->_id != nid && unit->father()->_group->unitExist(nid))
		return true;
	else if(ENEMY == cond._source && !unit->father()->_group->unitExist(nid))
		return true;
	else
		return false;
}

bool TriggerManager::canTrigger(int nid, const Trigger& trigger, ActionUnit* unit)
{
	if (SELF == trigger._source && unit->_id == nid)
		return true;
	else if(MATE == trigger._source && unit->_id != nid && unit->father()->_group->unitExist(nid))
		return true;
	else if(ENEMY == trigger._source && !unit->father()->_group->unitExist(nid))
		return true;
	else
		return false;
}

bool TriggerManager::defaultConditionHandler(const Condition& cond, ActionUnit* unit)
{
	auto counter = unit->_group->_battle->_counter_manager->get(unit, cond._event);
	if (counter->_counter < cond.getConditionTimes())
		return false;
	return true;
}

void TriggerManager::__init()
{
	conditionHandler("random", TriggerManager::random);
	conditionHandler("dead", TriggerManager::targetDead);
	conditionHandler("equip_weapon", TriggerManager::equipWeapon);
}

std::map<std::string, TriggerManager::ConditionHandler>& TriggerManager::getHandler()
{
	static std::map<std::string, TriggerManager::ConditionHandler> handler_map;
	return handler_map;
}

void TriggerManager::conditionCompleteEvent(Counter* c, const Condition& cond)
{
	if (!cond._cond_event._cmd.compare("reset"))
	{
		c->reset();
	}
	else if(!cond._cond_event._cmd.compare("del"))
	{
		if (cond._cond_event._para != -1)
		{
			c->del(cond._cond_event._para);
		}
		else
		{
			c->del(cond.getConditionTimes());
		}
	}
}

void TriggerManager::regSkillEvent(const Skill& skill, ActionUnit* unit)
{
	for (auto& cond : skill._condition)
	{
		if (!cond._cmd.compare("interval"))
		{
			auto counter = unit->_group->_battle->_counter_manager->get(unit, cond._event);
			counter->add(cond.getConditionTimes());
		}
		else if(!cond._cmd.compare("random"))
		{
			continue;
		}
		else
		{
			for (auto para : cond._params)
			{
				BattleEvent evt(cond._cmd, para);
				unit->_group->_battle->_battle_event_manager->regEventHandler(evt, unit, std::bind([cond, skill](const BattleEvent& evt, ActionUnit* unit, SignalConnector<BattleEvent, ActionUnit*, bool>::SignalConnPtr conn)
				{
					int nid = evt.get(0); 
					int times = evt.get(1);

					if (canTrigger(nid, cond, unit))
					{
						auto counter = unit->_group->_battle->_counter_manager->get(unit, cond._event);
						counter->add(times);
					}
					return false;
				}, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			}
		}
	}
	for (auto para : skill._trigger._params)
	{
		BattleEvent evt(skill._trigger._cmd, para);
		unit->father()->_group->_battle->_battle_event_manager->regEventHandler(evt, unit, std::bind([skill](const BattleEvent& evt, ActionUnit*unit, SignalConnector<BattleEvent, ActionUnit*, bool>::SignalConnPtr conn)
		{
			if (NEED_DELETE == unit->_state)
				return false;

			int nid = evt.get(0);

			if (canTrigger(nid, skill._trigger, unit))
			{
				if (!skillCanTrigger(skill, unit))
					return false;

				Command cmd;
				cmd._skill_id = skill._id;
				cmd._trigger_id = nid;
				unit->pushCommand(cmd);

				for (auto& cond : skill._condition)
				{
					auto counter = unit->father()->_group->_battle->_counter_manager->get(unit, cond._event);
					conditionCompleteEvent(counter, cond);
				}
				return skill._trigger_end;
			}
			return false;
		}, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
}

bool TriggerManager::skillCanTrigger(const Skill& skill, ActionUnit* unit)
{
	bool dead_trigger = false;	//ÊÇ·ñËÀÍö´¥·¢£¬ false->ÅÐ¶ÏunitÎ´ËÀÍö
	for (auto& cond : skill._condition)
	{
		if (!cond._cmd.compare("dead"))
			dead_trigger = true;
		if (!conditionComplete(cond, unit))
			return false;
	}
	if (dead_trigger)
		return true;
	return !unit->father()->dead();
}

bool TriggerManager::conditionComplete(const Condition& cond, ActionUnit* unit)
{
	auto& handler_map = getHandler();
	auto it = handler_map.find(cond._cmd);
	if ( it == handler_map.end())
		return defaultConditionHandler(cond, unit);
	else
		return it->second(cond, unit);
}
