#include "skill_effect.h"
#include "skill_trigger.h"
#include "battle_data.h"
#include "battle_event.h"
#include "battle.h"
#include "util.h"
#include <algorithm>
#include "battle_unit.h"

EffectCommand::EffectCommand(int skill_id, const Json::Value& val):
	_skill_id(skill_id),
	_target_type(TARGET_NULL),
	_index(0)
{
	_cmd = val[_index++].asString();
	_probability = val[_index++].asInt();
	auto target = val[_index++].asString();

	vector<std::string> vs;
	split(target, vs, ".");
	_target = parseTarget(vs[0]);
	if (vs.size() > 1)
		_target_type = parseTarget(vs[1]);

}

vector<ActionUnit*> EffectCommand::getTarget(ActionUnit* source, const vector<int>& target_vec)
{
	vector<ActionUnit*> target_list;
	if (MATE_HP_LESS == _target)
	{
		target_list.push_back(source->_group->getHpLessMate());
	}
	else if (TARGET ==  _target)
	{
		for (auto nid : target_vec)
		{
			auto target_unit = getTarget(source, nid);
			if (nullptr == target_unit)
				continue;
			if (TARGET_NULL == _target_type)
			{
				target_list.push_back(target_unit);
			}
			else if(MATE == _target_type && source->isMates(nid))
			{
				target_list.push_back(target_unit);
			}
			else if(ENEMY == _target_type && source->isEnemy(nid))
			{
				target_list.push_back(target_unit);
			}
			else if(WE == _target_type && !source->isEnemy(nid))
			{
				target_list.push_back(target_unit);
			}
			else if(SELF == _target_type && source->isMe(nid))
			{
				target_list.push_back(target_unit);
			}	
		}
	}
	else
	{
		target_list = getTarget(source);
	}
	return target_list;
}

vector<ActionUnit*> EffectCommand::getTarget(ActionUnit* source)
{
	vector<ActionUnit*> target_list;
	ActionUnit* target_unit = nullptr;
	if (SELF == _target)
	{
		target_list.push_back(source);
	}
	else if(MATE == _target)
	{
		target_list = source->_group->_group_unit;
		target_list.erase(std::remove(target_list.begin(), target_list.end(), source), target_list.end());
	}
	else if(WE == _target)
	{
		target_list = source->_group->_group_unit;
	}
	else if(ENEMY == _target)
	{
		source->getEnemy(target_list);
	}
	return target_list;
}

ActionUnit* EffectCommand::getTarget(ActionUnit* source, int nid)
{
	if (source->isEnemy(nid))
		return source->getEnemy(nid);
	else
		return source->getMate(nid);
}

void EffectCommand::parseEffect(int skill_id, const Json::Value& val, vector<EffectCommand*>& effect_list)
{
	for (auto it = val.begin(); it != val.end(); ++it)
	{
		auto& cmd = *it;
		if (!cmd[0u].asString().compare("do_damage"))
		{
			effect_list.push_back(new DamageEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("add_buff"))
		{
			effect_list.push_back(new AddBuffEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("attribute"))
		{
			effect_list.push_back(new AttributeEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("use_skill"))
		{
			effect_list.push_back(new UseSkillEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("do_cure"))
		{
			effect_list.push_back(new CureEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("add_sp"))
		{
			effect_list.push_back(new AddSPEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("counter"))
		{
			effect_list.push_back(new CounterEffect(skill_id, cmd));
		}
		else if(!cmd[0u].asString().compare("del_buff"))
		{
			effect_list.push_back(new DelBuffEffect(skill_id, cmd));
		}
	}
}

bool EffectCommand::effectConditionEnable(Warrior* source)
{
	//概率
	int ra = source->_group->_battle->random();
	if (ra > _probability)
		return false;

	for (auto& cond : _condition)
	{
	}
	return true;
}

DamageEffect::DamageEffect(int skill_id, const Json::Value& val) :
	EffectCommand(skill_id, val),
	_ext_crit_damage(0.f),
	_ext_phys_penetration(0.f),
	_ext_mana_penetration(0.f),
	_ext_crit_prob(0.f),
	_ext_absorb(0.f)
{
	{
		auto damage = val[_index++].asString();
		std::vector<std::string> vs;
		split(damage, vs, ".");

		_attr_source = parseTarget(vs[0]);
		_attr_type = parseAttribute(vs[1]);
		_attr_value = ::atof(vs[2].c_str()) / ::atof(vs[3].c_str()) / 100;
		_damage_type = parseDamageType(vs[4]);
	}
	for (int i = _index; i < val.size(); i++)
	{
		auto ext_attr = val[i].asString();
		std::vector<std::string> vs;
		split(ext_attr, vs, ".");
		if (!vs[0].compare("phys_penetration"))
		{
			_ext_phys_penetration = ::atof(vs[1].c_str());
		}
		else if (!vs[0].compare("mana_penetration"))
		{
			_ext_mana_penetration = ::atof(vs[1].c_str());
		}
		else if (!vs[0].compare("crit"))
		{
			_ext_crit_prob = ::atof(vs[1].c_str());
		}
		else if (!vs[0].compare("crit_damage"))
		{
			_ext_crit_damage = ::atof(vs[1].c_str()) / 100;
		}
		else if (!vs[0].compare("absorb"))
		{
			_ext_absorb = ::atof(vs[1].c_str()) / 100;
		}
	}
}

void DamageEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	ActionUnit* attr_unit = source;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		if (source->father()->dead() || target_unit->father()->dead())
			continue;

		if (TARGET == _attr_source)
			attr_unit = target_unit; 

		if (!attackHit(source->father(), target_unit->father()))
			continue;

		float damage = attr_unit->father()->attribute(_attr_type) * _attr_value;

		if (DAMAGE_PHYS == _damage_type)
			damage -= target_unit->father()->attribute(PHYS_DEFENSE);
		else if(DAMAGE_MANA == _damage_type)
			damage -= target_unit->father()->attribute(MANA_DEFENSE);

		{
			//暴击
			float crit = source->father()->attribute(CRIT) + _ext_crit_prob;
			int ra = source->_group->_battle->random(); 
			if (ra < crit)
			{
				damage *= 1 + source->father()->attribute(CRIT_DAMAGE) + _ext_crit_damage;

				{
					BattleEvent evt("crit", -1);
					evt.push(source->father()->_id, 1);
					source->_group->_battle->_battle_event_manager->eventCallback(evt);
				}
			}
		}

		{
			//吸血
			float absorb = source->father()->attribute(ABSORB) + _ext_absorb;
			if (absorb)
			{
				float absorb_value = absorb * damage;
				source->father()->cure(absorb_value);
			}
		}

		target_unit->damage(damage);

		std::cout << source->_name << " 对 " << target_unit->_name << " 造成 " << damage << " 点伤害" << std::endl;

		if (!target_unit->father()->dead())
		{
			damageRebound(source->father(), target_unit->father(), damage, _damage_type);
		}
	}
}

bool DamageEffect::attackHit(Warrior* source, Warrior* target)
{
	float hit = source->attribute(HIT);
	float miss = target->attribute(MISS);
	float true_hit = hit - miss;

	return true;
}

void DamageEffect::damageRebound(Warrior* source, Warrior* target, float damage, DAMAGE_TYPE damage_type)
{
	if (source->existEffectBuff(INVINCIBLE))
		return;

	float rebound = target->attribute(DAMAGE_REBOUND);
	if ( rebound <= 0.f)
		return;
	
	float rebound_damage = rebound * damage * ( 1 - max(0.f, DAMAGE_PHYS == damage_type ? target->attribute(PHYS_DAMAGE_REDUCE) : target->attribute(MANA_DAMAGE_REDUCE)));
	if (rebound_damage <= 0.f)
		return;

	std::cout << target->_name << " 反弹 " << source->_name << " 造成 " << rebound_damage << " 点伤害" << std::endl;

	source->damage(rebound_damage);
}

AddBuffEffect::AddBuffEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	_buff_id = val[_index++].asInt();
}

void AddBuffEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	if (source->father()->dead())
		return;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		auto target = dynamic_cast<Warrior*>(target_unit);
		if (nullptr == target)
			continue;

		auto cfg_buff = DataManager::getInstance()->getBuff(_buff_id);
		if (!cfg_buff || !buffAppendEnable(target, cfg_buff->_buff_effect))
			continue;

		Buff* buff = DataManager::getInstance()->createBuff(_buff_id);
		buff->_appender = source;
		buff->_father = target;
		buff->exec();

		target->pushBuff(buff);

		std::cout << source->_name << " 向 " << target->_name << " 添加buff " << buff->_name << std::endl;
		for (auto nid : buff->_skill_list)
		{
			auto skill = DataManager::getInstance()->getSkill(nid);
			TriggerManager::regSkillEvent(*skill, buff);
		}
	}
}

bool AddBuffEffect::buffAppendEnable(Warrior* target, int effect)
{
	//debuff,目标免疫减益效果
	auto effect_type = BUFF_EFFECT(effect);
	if ((STUN == effect_type || FROZEN == effect_type || CURE_RESISTANCE == effect_type) && target->existEffectBuff(DEBUFF_IMMUNE))
		return false;

	if (STUN == effect_type && target->existEffectBuff(STUN_IMMUNE))
		return false;
	
	return true;
}

DelBuffEffect::DelBuffEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	_type = val[_index++].asInt();
}

void DelBuffEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	if (source->father()->dead())
		return;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		auto target = dynamic_cast<Warrior*>(target_unit);
		if (nullptr == target)
			continue;
		
		target->removeTypeBuff(BUFF_TYPE(_type));
	}
}

UseSkillEffect::UseSkillEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	_skill = val[_index++].asInt();
}

void UseSkillEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	if (source->father()->dead())
		return;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		target_unit->useSkill(_skill, target_unit->_id);
	}
}

AttributeEffect::AttributeEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	auto source = val[_index++].asString();
	std::vector<std::string> vs;
	split(source, vs, ".");

	_attr_source = parseTarget(vs[0]);
	_attr = parseAttribute(vs[1]);
	_type = vs[2];
	_value = ::atof(vs[3].c_str());

	_tar_attr = parseAttribute(val[_index++].asString());
}

void AttributeEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	if (source->father()->dead())
		return;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		auto target = dynamic_cast<Warrior*>(target_unit);
		if (nullptr == target || target->father()->dead())
			continue;

		AttributeBuff buff;
		buff._time = _time;
		buff._buff_id = _buff_id;
		if (!_type.compare("perc"))
		{
			float val = target->baseAttribute(_attr);
			if (SELF == _attr_source)
				val = source->father()->baseAttribute(_attr);

			val *= _value / 100;
			buff._value = val;
		}
		else
		{
			buff._value = _value;
		}
		target->attributeValue(_tar_attr)->pushBuff(buff);
	}
}

CureEffect::CureEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	auto cure = val[_index++].asString();
	std::vector<std::string> vs;
	split(cure, vs, ".");

	_attr_source = parseTarget(vs[0]);
	_attr_type = parseAttribute(vs[1]);
	_attr_value = ::atof(vs[2].c_str()) / 100;
}

void CureEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	if (source->father()->dead())
		return;

	ActionUnit* attr_unit = source;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		if (TARGET == _attr_source)
			attr_unit = target_unit;
		if (attr_unit->father()->dead())
			continue;

		float cure = attr_unit->father()->attribute(_attr_type) * _attr_value;

		target_unit->cure(cure);
	}
}

AddSPEffect::AddSPEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	_value = val[_index++].asInt();	
}

void AddSPEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	if (source->father()->dead())
		return;

	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		auto target = dynamic_cast<Warrior*>(target_unit);
		if (nullptr == target || target->father()->dead())
			continue;
		target->_sp += _value;
	}
}

CounterEffect::CounterEffect(int skill_id, const Json::Value& val):
	EffectCommand(skill_id, val)
{
	_event = val[_index++].asString();
	_value = val[_index++].asDouble();
}

void CounterEffect::exec(ActionUnit* source, const vector<int>& target_vec)
{
	vector<ActionUnit*> target_list = getTarget(source, target_vec);

	for (auto target_unit : target_list)
	{
		auto counter = source->_group->_battle->_counter_manager->get(target_unit, _event);
		counter->add(_value);
	}
}

DAMAGE_TYPE parseDamageType(const string& type)
{
	if (!type.compare("phys"))
		return DAMAGE_PHYS;
	else if(!type.compare("mana"))
		return DAMAGE_MANA;
	else
		return DAMAGE_NULL;
}
