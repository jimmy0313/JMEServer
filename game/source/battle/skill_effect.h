#ifndef skill_effect_h__
#define skill_effect_h__

#include <string>
#include <vector>
#include "skill.h"
#include "battle_unit.h"

#ifdef WIN32
#include "json/json.h"
#else
#include "json.h"
#endif // WIN32


// class Skill;
// class ActionUnit;
// class Warrior;

struct EffectCommand
{
	int _skill_id;
	TARGET_TYPE _target;
	TARGET_TYPE _target_type;
	std::string _cmd;
	int _probability;

	int _index;
	std::vector<Condition> _condition;

	EffectCommand(int skill_id, const Json::Value& val);

	virtual void exec(ActionUnit* source, const vector<int>& target_vec) = 0;

	vector<ActionUnit*> getTarget(ActionUnit* source, const vector<int>& target_vec);
	vector<ActionUnit*> getTarget(ActionUnit* source);
	ActionUnit* getTarget(ActionUnit* source, int nid);

	static void parseEffect(int skill_id, const Json::Value& val, vector<EffectCommand*>& effect_list);

	bool effectConditionEnable(Warrior* source);
};

enum DAMAGE_TYPE
{
	DAMAGE_NULL = 0,
	DAMAGE_PHYS/* = 1*/,
	DAMAGE_MANA/* = 2*/,
};
DAMAGE_TYPE parseDamageType(const string& type);
struct DamageEffect :
	public EffectCommand
{
	TARGET_TYPE _attr_source;	//伤害属性来源
	ATTRIBUTE_TYPE _attr_type;		//伤害参照数值
	DAMAGE_TYPE _damage_type;	//伤害类型
	float _attr_value;			//伤害数值 = 伤害总数 / 伤害段数

	float _ext_phys_penetration;
	float _ext_mana_penetration;
	float _ext_crit_prob;
	float _ext_crit_damage;
	float _ext_absorb;

	DamageEffect(int skill_id, const Json::Value& val);
	virtual void exec(ActionUnit* source, const vector<int>& target_vec);

	bool attackHit(Warrior* source, Warrior* target);

	void damageRebound(Warrior* source, Warrior* target, float damage, DAMAGE_TYPE damage_type);
};

struct AddBuffEffect :
	public EffectCommand
{
	int _buff_id;

	AddBuffEffect(int skill_id, const Json::Value& val);

	virtual void exec(ActionUnit* source, const vector<int>& target_vec);

	bool buffAppendEnable(Warrior* target, int effect);
};

struct DelBuffEffect :
	public EffectCommand
{
	int _type;

	DelBuffEffect(int skill_id, const Json::Value& val);

	virtual void exec(ActionUnit* source, const vector<int>& target_vec);
};

struct UseSkillEffect :
	public EffectCommand
{
	int _skill;

	UseSkillEffect(int skill_id, const Json::Value& val);

	virtual void exec(ActionUnit* source, const vector<int>& target_vec);
};

struct AttributeEffect :
	public EffectCommand
{
	TARGET_TYPE _attr_source;
	ATTRIBUTE_TYPE _attr;
	std::string _type;
	float _value;

	ATTRIBUTE_TYPE _tar_attr;

	int _buff_id;
	float _time;

	AttributeEffect(int skill_id, const Json::Value& val);

	virtual void exec(ActionUnit* source, const vector<int>& target_vec);
};

struct CureEffect :
	public EffectCommand
{
	TARGET_TYPE _attr_source;	//治疗属性来源
	ATTRIBUTE_TYPE _attr_type;		//治疗参照数值
	float _attr_value;			//治疗数值 = 治疗总数 / 治疗段数

	CureEffect(int skill_id, const Json::Value& val);
	virtual void exec(ActionUnit* source, const vector<int>& target_vec);
};

struct AddSPEffect :
	public EffectCommand
{
	int _value;

	AddSPEffect(int skill_id, const Json::Value& val);
	virtual void exec(ActionUnit* source, const vector<int>& target_vec);
};

struct CounterEffect :
	public EffectCommand
{
	std::string _event;
	float _value;

	CounterEffect(int skill_id, const Json::Value& val);
	virtual void exec(ActionUnit* source, const vector<int>& target_vec);
};
#endif // skill_effect_h__
