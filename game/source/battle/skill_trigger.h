#ifndef skill_condition_h__
#define skill_condition_h__

#include <map>
#include <functional>
#include <string>
#include "battle_event.h"

class ActionUnit;
class ConditionEvent;
class Condition;
class Trigger;
class Skill;

#include "init_helper.h"
class TriggerManager :
	public AutoInitLogicInterface
{
public:
	friend class EffectCommand;
	typedef std::function<bool(const Condition& cond, ActionUnit* unit)> ConditionHandler;

	static void regSkillEvent(const Skill& skill, ActionUnit* unit);
private:
	static bool skillCanTrigger(const Skill& skill, ActionUnit* unit);
	static void conditionHandler(const char* cond, ConditionHandler handler);
	static bool conditionComplete(const Condition& cond, ActionUnit* unit);

	static bool selfGetBuffer(const Condition& cond, ActionUnit* unit);
	static bool enemyGetBuffer(const Condition& cond, ActionUnit* unit);
	static bool random(const Condition& cond, ActionUnit* unit);
	static bool targetDead(const Condition& cond, ActionUnit* unit);
	static bool equipWeapon(const Condition& cond, ActionUnit* unit);

	static bool canTrigger(int nid, const Condition& cond, ActionUnit* unit);
	static bool canTrigger(int nid, const Trigger& trigger, ActionUnit* unit);

	static bool defaultConditionHandler(const Condition& cond, ActionUnit* unit);
protected:
	void __init();
	auto_init_h(TriggerManager);

	static std::map<std::string, TriggerManager::ConditionHandler>& getHandler();

	static void conditionCompleteEvent(Counter* c, const Condition& cond);
};
#endif // skill_condition_h__
