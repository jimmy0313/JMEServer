#ifndef skill_h__
#define skill_h__

#include <string>
#include <vector>

#ifdef WIN32
#include "json/json.h"
#else
#include "json.h"
#endif // WIN32

using namespace std;

class EffectCommand;
struct ConditionEvent
{
	string _cmd;
	int _para;
	ConditionEvent();
	ConditionEvent(const std::string& cmd);
};

enum TARGET_TYPE
{
	TARGET_NULL = 0,
	SELF/* = 1*/,
	MATE/* = 2*/,
	TARGET/* = 3*/,
	ENEMY/* = 4*/,
	WE/* = 5*/,
	MATE_HP_LESS/* = 6*/,
};

TARGET_TYPE parseTarget(const string& target);
struct Condition
{
	TARGET_TYPE _source;
	string _cmd;
	string _event;
	int _times;

	ConditionEvent _cond_event;

	vector<int> _params;

	Condition();

	Condition(const Json::Value::iterator it);

	int getConditionTimes() const { return _times; }
};

struct Trigger
{
	TARGET_TYPE _source;
	string _cmd;
	string _event;
	vector<int> _params;

	Trigger();

	Trigger(const Json::Value::iterator it);
};


struct TriggerUnit
{
	Trigger _trigger;
	vector<Condition> _condition;
	vector<EffectCommand*> _contine_effect;
	vector<EffectCommand*> _begin_effect;
	vector<EffectCommand*> _end_effect;

	bool _trigger_end;	// 该变量标明如果触发该技能， 还能否触发其他技能

	TriggerUnit();
	TriggerUnit(int skill_id, const Json::Value& val);
};

class ActionUnit;
class Skill :
	public TriggerUnit
{
public:
	friend class TriggerManager;
	friend class ActionUnit;
public:
	Skill(){}
	Skill(int nid, const Json::Value& val);

	void start_skill(ActionUnit* source, int trigger_id) const;
	void start_skill(ActionUnit* source, vector<int>& target_vec) const;

	void exec_skill(ActionUnit* source, vector<int>& target_vec) const;
	void end_skill(ActionUnit* source, vector<int>& target_vec) const;
protected:
	int _id;
	std::string _name;
	int _dispel_num;	// 释放该技能, 消除的技能块数量, 
						// _dispel_num == -99 表示 该技能不会触发抛出任何消块事件 
						// _dispel_num == 0 表示 释放的是普攻
						// _dispel_num == [ 1, 2, 3 ] 表示 消块的数量
};	
#endif // skill_h__
