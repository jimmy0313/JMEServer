#include "skill.h"
#include "skill_effect.h"
#include "util.h"
ConditionEvent::ConditionEvent() :
	_cmd("del"),
	_para(-1)
{

}

ConditionEvent::ConditionEvent(const std::string& cmd)
{
	vector<string> vs;
	split(cmd, vs, ".");

	_cmd = vs[0];
	_para = ::atoi(vs[1].c_str());
}

Condition::Condition() :
	_source(SELF),
	_cmd(""),
	_event("")
{

}

Condition::Condition(const Json::Value::iterator it)
{
	_cmd = it.key().asString();
	_event = _cmd;

	vector<string> vs;
	split(_cmd, vs, ".");

	_source = parseTarget(vs[0]);
	_cmd = vs[1];

	auto e_target = (*it)[0u];
	if (e_target.empty())
	{
		_params.push_back(-1);
	}
	else
	{
		for (auto& para : e_target)
		{
			_params.push_back(para.asInt());
		}
	}
	_times = (*it)[1].asInt();

	if ((*it).size() >= 3)
	{
		_cond_event = ConditionEvent((*it)[2].asString());
	}
}

Trigger::Trigger() :
	_source(SELF),
	_cmd(""),
	_event("")
{

}

Trigger::Trigger(const Json::Value::iterator it)
{
	_cmd = it.key().asString();
	_event = _cmd;

	vector<string> vs;
	split(_cmd, vs, ".");

	_source = parseTarget(vs[0]);
	_cmd = vs[1];

	for (auto& para : (*it))
	{
		_params.push_back(para.asInt());
	}
}

Skill::Skill(int nid, const Json::Value& val):
	TriggerUnit(nid, val),
	_id(nid),
	_dispel_num(-99)
{
	if (val.isMember("dispel_num"))
		_dispel_num = val["dispel_num"].asInt();

#ifdef WIN32
	auto name = val["name"].asString();
	UTF_8ToGB2312(_name, (char*)name.data(), name.length());
#else
	_name = val["name"].asString();
#endif // WIN32
}

void Skill::start_skill(ActionUnit* source, int trigger_id) const
{
	vector<int> target_vec;
	target_vec.push_back(trigger_id);
	start_skill(source, target_vec);
}

void Skill::start_skill(ActionUnit* source, vector<int>& target_vec) const
{
	for (auto effect : _begin_effect)
	{
		effect->exec(source, target_vec);
	}
}

void Skill::exec_skill(ActionUnit* source, vector<int>& target_vec) const
{
	for (auto effect : _contine_effect)
	{
		effect->exec(source, target_vec);
	}
}

void Skill::end_skill(ActionUnit* source, vector<int>& target_vec) const
{
	for (auto effect : _end_effect)
	{
		effect->exec(source, target_vec);
	}
}

TriggerUnit::TriggerUnit()
{

}
TriggerUnit::TriggerUnit(int skill_id, const Json::Value& val)
{
	_trigger = Trigger(val["trigger"].begin());
	for (auto it = val["condition"].begin(); it != val["condition"].end(); ++it)
	{
		_condition.push_back(Condition(it));
	}

	EffectCommand::parseEffect(skill_id, val["continue_effect"], _contine_effect);
	EffectCommand::parseEffect(skill_id, val["begin_effect"], _begin_effect);
	EffectCommand::parseEffect(skill_id, val["end_effect"], _end_effect);

	_trigger_end = bool(val["trigger_end"].asInt());
}

TARGET_TYPE parseTarget(const string& target)
{
	if (!target.compare("self"))
		return SELF;
	else if(!target.compare("mate"))
		return MATE;
	else if(!target.compare("target"))
		return TARGET;
	else if(!target.compare("enemy"))
		return ENEMY;
	else if(!target.compare("we"))
		return WE;
	else if(!target.compare("mate_hp_less"))
		return MATE_HP_LESS;
	else
		return TARGET_NULL;
}
