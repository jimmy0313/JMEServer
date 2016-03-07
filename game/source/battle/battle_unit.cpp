#include "battle_unit.h"
#include "battle_event.h"
#include "event_interface.h"
#include "battle.h"
#include <iostream>
#include "battle_data.h"
#include "skill_effect.h"
#include "util.h"
#include <algorithm>

ActionUnit::ActionUnit():
	_father(nullptr),
	_group(nullptr),
	_state(NORMAL),
	_auto_attack(0.f)
{

}

ActionUnit::ActionUnit(const Json::Value& val):
	ActionUnit()
{
#ifdef WIN32
	auto name = val["name"].asString();
	UTF_8ToGB2312(_name, (char*)name.data(), name.length());
#else
	_name = val["name"].asString();
#endif // WIN32

	for (auto it : val["skill"])
	{
		_skill_list.push_back(it.asInt());
	}
}

ActionUnit::~ActionUnit()
{
	if (_group)
	{
		_group->_battle->_battle_event_manager->removeUnitEvent(this);
		_group->_battle->_counter_manager->removeUnitCounter(this);
	}
}

void ActionUnit::init()
{

}

void ActionUnit::update(float delta)
{
	if (NORMAL != _state)
		return;

	_auto_attack += delta;

	if (!_command.empty())
	{
		auto cmd = _command.front();
		_command.pop_front();
		exeCommand(cmd);
	}
}

void ActionUnit::useSkill(int skill_id, int trigger_id)
{
	Command cmd;
	cmd._skill_id = skill_id;
	cmd._trigger_id = trigger_id;
	exeCommand(cmd);
}

void ActionUnit::dispel(int num)
{
	{
		BattleEvent evt("clear", num);
		evt.push(_id, 1);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
	{
		BattleEvent evt("clear", -1);
		evt.push(_id, num);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
}

void ActionUnit::attack()
{
	_auto_attack = 0.f;

	BattleEvent evt("clear", 0);
	evt.push(_id, 1);
	_group->_battle->_battle_event_manager->eventCallback(evt);
}

void ActionUnit::parry()
{
	BattleEvent evt("parry", -1);
	evt.push(_id, 1);
	_group->_battle->_battle_event_manager->eventCallback(evt);
}
void ActionUnit::skillComplete()
{
	if (_state != DEAD || _state != NEED_DELETE)
	{
		_state = NORMAL;
		_auto_attack = 0.f;
	}
}

void ActionUnit::setGroup(BattleGroup* group)
{
	_id = group->createId();
	_group = group;
}

bool ActionUnit::isMe(ActionUnit* unit) const
{
	return unit == this;
}

bool ActionUnit::isMe(int unit_id) const
{
	return _id == unit_id;
}

bool ActionUnit::isMates(ActionUnit* unit) const
{
	if (unit != this && _group->unitExist(unit))
		return true;
	return false;
}

bool ActionUnit::isMates(int unit_id) const
{
	if (_id != unit_id && _group->unitExist(unit_id))
		return true;
	return false;
}

bool ActionUnit::isEnemy(ActionUnit* unit) const
{
	return !_group->unitExist(unit);
}

bool ActionUnit::isEnemy(int unit_id) const
{
	return !_group->unitExist(unit_id);
}

bool ActionUnit::isNeedDelete() const
{
	return NEED_DELETE == _state;
}

ActionUnit* ActionUnit::getEnemy(int nid)
{
	return _group->getEnemy(nid);
}

void ActionUnit::getEnemy(vector<ActionUnit*>& target_vec)
{
	_group->getEnemy(target_vec);
}

void ActionUnit::getUnit(vector<ActionUnit*>& target_vec)
{
	_group->getUnit(target_vec);
}

void ActionUnit::clearCommand()
{
	_command.clear();
}

ActionUnit* ActionUnit::getMate(int nid)
{
	return _group->getUnit(nid);
}

void ActionUnit::pushCommand(const Command& cmd)
{
	_command.push_back(cmd);
}

void ActionUnit::exeCommand(const Command& cmd)
{
	auto skill = DataManager::getInstance()->getSkill(cmd._skill_id);
	if (nullptr == skill)
		return;

	_auto_attack = 0.f;

	std::cout << _name << "(" << father()->_name << ") " << " 释放 ================================== [ " << skill->_name << " ]" << std::endl;

	{
		BattleEvent evt("skill", skill->_id);
		evt.push(_id, 1);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
	skill->start_skill(father(), cmd._trigger_id);

	_state = USE_SKILL;

// 	vector<ActionUnit*> target;
// 	_group->getEnemy(target);
// 
// 	vector<int> tatget_list;
// 	for_each(target.begin(), target.end(), [&tatget_list](const ActionUnit* unit)
// 	{
// 		tatget_list.push_back(unit->_id);
// 	});
// 	skill->exec_skill(this, tatget_list);

	if (skill->_dispel_num < -1)
		return;
	{
		BattleEvent evt("dispel", skill->_dispel_num);
		evt.push(_id, 1);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
	{
		BattleEvent evt("dispel", -1);
		evt.push(_id, skill->_dispel_num);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
}

Warrior* ActionUnit::father()
{
	if (_father)
		return _father->father();
	return dynamic_cast<Warrior*>(this);
}

BattleGroup::~BattleGroup()
{
	for (auto unit : _group_unit)
	{
		delete unit;
	}
	for (auto unit : _add_unit)
	{
		delete unit;
	}
	for (auto unit : _del_unit)
	{
		delete unit;
	}
}

void BattleGroup::update(float delta)
{
	for (auto unit : _group_unit)
	{
		unit->update(delta);
	}
	for (auto unit : _add_unit)
	{
		_group_unit.push_back(unit);
	}
	_add_unit.clear();

	for (auto unit : _del_unit)
	{
		_group_unit.erase(std::remove(_group_unit.begin(), _group_unit.end(), unit), _group_unit.end());
		delete unit;
	}
	_del_unit.clear();
}

void BattleGroup::addUnit(ActionUnit* unit)
{
	if (unitExist(unit))
		return;
	unit->setGroup(this);
	_group_unit.push_back(unit);
}

ActionUnit* BattleGroup::getUnit(int nid)
{
	for (auto it : _group_unit)
	{
		if (it->_id == nid)
			return &(*it);
	}
	return nullptr;
}

void BattleGroup::getUnit(vector<ActionUnit*>& target_vec)
{
	for (auto it : _group_unit)
	{
		target_vec.push_back(it);
	}
}

ActionUnit* BattleGroup::getHpLessMate()
{
	float perc = 1.f;
	float cur_perc = 1.f;
	Warrior* target_unit = nullptr;
	for (auto unit : _group_unit)
	{
		Warrior* warrior = dynamic_cast<Warrior*>(unit);
		if (nullptr == warrior)
			continue;
		cur_perc = warrior->_hp / warrior->_max_hp;
		if (cur_perc < perc)
		{
			perc = cur_perc;
			target_unit = warrior;
		}
	}
	return target_unit;
}

ActionUnit* BattleGroup::getTankMate()
{
	for (auto unit : _group_unit)
	{
		Warrior* warrior = dynamic_cast<Warrior*>(unit);
		if (nullptr == warrior)
			continue;

		if (warrior->existEffectBuff(DAMAGE_TANK))
			return warrior;
	}
	return nullptr;
}

ActionUnit* BattleGroup::getEnemy(int nid)
{
	if (this == _battle->_group_0)
		return _battle->_group_1->getUnit(nid);
	else
		return _battle->_group_0->getUnit(nid);
}

void BattleGroup::getEnemy(vector<ActionUnit*>& target_vec)
{
	if (this == _battle->_group_0)
		return _battle->_group_1->getUnit(target_vec);
	else
		return _battle->_group_0->getUnit(target_vec);

}

void BattleGroup::pushAddUnit(ActionUnit* unit)
{
	_add_unit.push_back(unit);
}

void BattleGroup::delUnit(ActionUnit* unit)
{
	_del_unit.push_back(unit);
}

bool BattleGroup::unitExist(ActionUnit* unit)
{
	auto it = std::find(_group_unit.begin(), _group_unit.end(), unit);
	if (it != _group_unit.end())
		return true;
	return false;
}

bool BattleGroup::unitExist(int nid)
{
	for (auto it : _group_unit)
	{
		if (it->_id == nid)
			return true;
	}
	return false;
}

int BattleGroup::createId()
{
	return _base_id++;
}

bool BattleGroup::allDead()
{
	for (auto unit : _group_unit)
	{
		if (unit == unit->father() && !unit->father()->dead())
			return false;
	}
	return true;
}

void BattleGroup::print()
{
	for (auto unit : _group_unit)
	{
		if (unit != unit->father())
			continue;

		std::cout << unit->_name << "(" << unit->_id << ") 剩余HP: " << unit->father()->_hp << std::endl;
	}
}

Warrior::Warrior(const Json::Value& val):
	Attribute(val["attribute"]),
	ActionUnit(val)
{

}

Warrior::~Warrior()
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end(); ++it)
	{
		delete *it;
	}
}

void Warrior::update(float delta)
{
	Attribute::update(delta);
	ActionUnit::update(delta);

	if (!dead() && _auto_attack >= attribute(ATTACK_SPEED))	//没有释放技能, 且普通间隔满足
	{
		this->attack();
		_auto_attack = 0.f;
	}

	for (auto buff : _buff_vec)
	{
		buff->update(delta);
	}
	for (auto it = _buff_vec.begin(); it != _buff_vec.end();)
	{
		if ((*it)->isNeedDelete())
		{
			delete *it;
			it = _buff_vec.erase(it);
		}
		else
			++it;
	}
}

void Warrior::damage(float damage)
{
	_hp -= damage;

	{
		BattleEvent evt("damage", -1);
		evt.push(_id, 1);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}

	if(_hp <= 0.f)
	{
		_hp = 0.f;

		std::cout << _name << " 死亡" << std::endl;
		clearCommand();
		clearBuff();

		BattleEvent evt("dead", -1);
		evt.push(_id, 1);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
}

void Warrior::cure(float cure)
{
	if (dead())
		return;

	float real_cure = _hp < _max_hp ? min(cure, _max_hp - _hp) : 0;
	if (real_cure)
		_hp += cure;

	{
		BattleEvent evt("cure", -1);
		evt.push(_id, 1);
		_group->_battle->_battle_event_manager->eventCallback(evt);
	}
}

void Warrior::clearBuff()
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end(); ++it)
	{
		delete *it;
	}
	_buff_vec.clear();
}

void Warrior::pushBuff(Buff* buff)
{
	removeBuff(buff->_id);

	buff->_father = this;
	buff->setGroup(this->_group);
	_buff_vec.push_back(buff);
}

void Warrior::removeTypeBuff(BUFF_TYPE type)
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end();)
	{
		auto buff = *it;
		if (buff->_type == type && buff->_can_removed)
		{
			delete *it;
			it = _buff_vec.erase(it);
		}
		else 
			++it;
	}
}

void Warrior::removeBuff(int nid)
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end();)
	{
		auto buff = *it;
		if (buff->_id == nid)
		{
			delete buff;
			it = _buff_vec.erase(it);
		}
		else 
			++it;
	}
}

bool Warrior::existBuff(int nid)
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end(); ++it)
	{
		auto buff = *it;
		if (buff->_id == nid)
		{
			buff->effectAction();
			return true;
		}
	}
	return false;
}

bool Warrior::existBuff(BUFF_TYPE type)
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end(); ++it)
	{
		auto buff = *it;
		if (buff->_type == type)
			return true;
	}
	return false;
}

bool Warrior::existEffectBuff(BUFF_EFFECT effect)
{
	for (auto it = _buff_vec.begin(); it != _buff_vec.end(); ++it)
	{
		auto buff = *it;
		if (buff->_buff_effect == effect)
		{
			buff->effectAction();
			return true;
		}
	}
	return false;
}

Attribute::Attribute():
	_hp(0.f),
	_max_hp(0.f),
	_sp(0)
{

}

Attribute::Attribute(const Json::Value& val)
{
	_hp = val["hp"].asDouble();
	_max_hp = _hp;
	_sp = 0;
	_attack.value(val["attack"].asDouble());
	_phys_defense.value(val["phys_defense"].asDouble());
	_mana_defense.value(val["mana_defense"].asDouble());
	_phys_penetration.value(val["phys_penetration"].asDouble());
	_mana_penetration.value(val["mana_penetration"].asDouble());
	_crit.value(val["crit"].asDouble());
	_crit_damage.value(val["crit_damage"].asDouble());
	_hit.value(val["hit"].asDouble());
	_miss.value(val["miss"].asDouble());
	_attack_speed.value(val["attack_speed"].asDouble());
	_absorb.value(val["absorb"].asDouble());
	_rebound.value(val["rebound"].asDouble());
	_phys_damage_reduce.value(val["phys_damage_reduce"].asDouble());
	_mana_damage_reduce.value(val["mana_damage_reduce"].asDouble());
}

AttributeValue* Attribute::attributeValue(ATTRIBUTE_TYPE attr)
{
	switch (attr)
	{
	case ATTACK:
		return &_attack;
		break;
	case PHYS_DEFENSE:
		return &_phys_defense;
		break;
	case MANA_DEFENSE:
		return &_mana_defense;
		break;
	case PHYS_PENETRATION:
		return &_phys_penetration;
		break;
	case MANA_PENETRATION:
		return &_mana_penetration;
		break;
	case CRIT:
		return &_crit;
		break;
	case CRIT_DAMAGE:
		return &_crit_damage;
		break;
	case HIT:
		return &_hit;
		break;
	case MISS:
		return &_miss;
		break;
	case ATTACK_SPEED:
		return &_attack_speed;
		break;
	case ABSORB:
		return &_absorb;
		break;
	case DAMAGE_REBOUND:
		return &_rebound;
		break;
	case PHYS_DAMAGE_REDUCE:
		return &_phys_damage_reduce;
		break;
	case MANA_DAMAGE_REDUCE:
		return &_mana_damage_reduce;
		break;
	default:
		return nullptr;
		break;
	}
}

float Attribute::baseAttribute(ATTRIBUTE_TYPE attr) const
{
	switch (attr)
	{
	case HP:
		return _hp;
		break;
	case MAX_HP:
		return _max_hp;
		break;
	case ATTACK:
		return _attack._value;
		break;
	case PHYS_DEFENSE:
		return _phys_defense._value;
		break;
	case MANA_DEFENSE:
		return _mana_defense._value;
		break;
	case PHYS_PENETRATION:
		return _phys_penetration._value;
		break;
	case MANA_PENETRATION:
		return _mana_penetration._value;
		break;
	case CRIT:
		return _crit._value;
		break;
	case CRIT_DAMAGE:
		return _crit_damage._value;
		break;
	case HIT:
		return _hit._value;
		break;
	case MISS:
		return _miss._value;
		break;
	case ATTACK_SPEED:
		return _attack_speed._value;
		break;
	case ABSORB:
		return _absorb._value;
		break;
	case DAMAGE_REBOUND:
		return _rebound._value;
		break;
	case PHYS_DAMAGE_REDUCE:
		return _phys_damage_reduce._value;
		break;
	case MANA_DAMAGE_REDUCE:
		return _mana_damage_reduce._value;
		break;
	default:
		return 0.f;
		break;
	}
}

float Attribute::attribute(ATTRIBUTE_TYPE attr) const
{
	switch (attr)
	{
	case HP:
		return _hp;
		break;
	case MAX_HP:
		return _max_hp;
		break;
	case ATTACK:
		return _attack.value();
		break;
	case PHYS_DEFENSE:
		return _phys_defense.value();
		break;
	case MANA_DEFENSE:
		return _mana_defense.value();
		break;
	case PHYS_PENETRATION:
		return _phys_penetration.value();
		break;
	case MANA_PENETRATION:
		return _mana_penetration.value();
		break;
	case CRIT:
		return _crit.value();
		break;
	case CRIT_DAMAGE:
		return _crit_damage.value() / 100;
		break;
	case HIT:
		return _hit.value();
		break;
	case MISS:
		return _miss.value();
		break;
	case ATTACK_SPEED:
		return 1 / _attack_speed.value();
		break;
	case ABSORB:
		return _absorb.value() / 100;
		break;
	case DAMAGE_REBOUND:
		return _rebound.value() / 100;
		break;
	case PHYS_DAMAGE_REDUCE:
		return _phys_damage_reduce.value() / 100;
		break;
	case MANA_DAMAGE_REDUCE:
		return _mana_damage_reduce.value() / 100;
		break;
	default:
		return 0.f;
		break;
	}
}

void Attribute::update(float delta)
{
	_attack.update(delta);
	_phys_defense.update(delta);
	_mana_defense.update(delta);
	_crit.update(delta);
	_crit_damage.update(delta);
	_phys_penetration.update(delta);
	_mana_penetration.update(delta);
	_attack_speed.update(delta);
	_hit.update(delta);
	_miss.update(delta);
	_absorb.update(delta);
	_rebound.update(delta);
	_phys_damage_reduce.update(delta);
	_mana_damage_reduce.update(delta);
}

float Attribute::hpPerc() const
{
	return _hp / _max_hp;
}

bool Attribute::dead() const
{
	return _hp <= 0.f;
}

void Buff::update(float delta)
{
	_alive_time -= delta;
	_cur_interval += delta;
	if (_alive_time <= 0.f && _alive_time > -99.f)
	{
		_state = NEED_DELETE;
		return;
	}

	if (!_command.empty())
	{
		auto cmd = _command.front();
		_command.pop_front();
		exeCommand(cmd);
	}

	if (_effect_interval > 0.f && _cur_interval >= _effect_interval)
	{
		exec();
	}
}

Buff::Buff():
	_alive_time(-99.f),
	_times(-1),
	_effect_interval(0.f),
	_cur_interval(0.f)
{

}

Buff::Buff(int nid, const Json::Value& val) :
	ActionUnit(val),
	_id(nid),
	_alive_time(-99.f),
	_times(-1),
	_effect_interval(0.f),
	_cur_interval(0.f)
{
	if (val.isMember("time"))
		_alive_time = val["time"].asDouble();

	if (val.isMember("times"))
		_times = val["times"].asInt();

	_type = BUFF_TYPE(val["type"].asInt());

	int effect_times = val["effect_times"].asInt();
	if (effect_times)
		_effect_interval = _alive_time / effect_times;

	EffectCommand::parseEffect(1, val["effect"], _effect_command);
	for (auto effect : _effect_command)
	{
		AttributeEffect* attribute_buff = dynamic_cast<AttributeEffect *>(effect);
		if (NULL != attribute_buff)
		{
			attribute_buff->_time = _alive_time;
			attribute_buff->_buff_id = _id;
		}
	}
}

void Buff::effectAction()
{
	if (_times > 0)
		_times--;
	if (_times == 0)
		_state = NEED_DELETE;
}

void Buff::exec()
{
	vector<int> target_vec;
	target_vec.push_back(_father->_id);
	for (auto effect : _effect_command)
	{
		effect->exec(_appender, target_vec);
	}
}

AttributeBuff::AttributeBuff():
	_buff_id(0),
	_time(0.f),
	_value(0.f)
{

}

float AttributeValue::value() const
{
	float value = _value;
	for (auto& buff : _val_buff)
	{
		value += buff._value;
	}
	return value;
}

void AttributeValue::value(float val)
{
	_value = val;
}

AttributeValue::AttributeValue():
	_value(0.f)
{

}

void AttributeValue::update(float delta)
{
	auto cb = [](float delta, vector<AttributeBuff>& buff_vec)
	{
		for (auto it = buff_vec.begin(); it != buff_vec.end();)
		{
			(*it)._time -= delta;
			if ((*it)._time <= 0.f)
			{
				it = buff_vec.erase(it);
			}
			else
				++it;
		}
	};
	cb(delta, _val_buff);
}

void AttributeValue::pushBuff(const AttributeBuff& buff)
{
	for (auto it = _val_buff.begin(); it != _val_buff.end();)
	{
		if ((*it)._buff_id == buff._buff_id)
			it = _val_buff.erase(it);
		else
			++it;
	}
	_val_buff.push_back(buff);
}

void WarriorCopy::update(float delta)
{
	Warrior::update(delta);

	_alive_time -= delta;
	if (_alive_time <= 0.f)
		_state = NEED_DELETE;
}

ATTRIBUTE_TYPE parseAttribute(const std::string& attr)
{
	if (!attr.compare("attack"))
		return ATTACK;
	else if (!attr.compare("phys_defense"))
		return PHYS_DEFENSE;
	else if (!attr.compare("mana_defense"))
		return MANA_DEFENSE;
	else if (!attr.compare("crit"))
		return CRIT;
	else if (!attr.compare("crit_damage"))
		return CRIT_DAMAGE;
	else if (!attr.compare("phys_penetration"))
		return PHYS_PENETRATION;
	else if (!attr.compare("mana_penetration"))
		return MANA_PENETRATION;
	else if (!attr.compare("speed"))
		return ATTACK_SPEED;
	else if (!attr.compare("hp"))
		return HP;
	else if (!attr.compare("max_hp"))
		return MAX_HP;
	else if (!attr.compare("hit"))
		return HIT;
	else if (!attr.compare("miss"))
		return MISS;
	else if (!attr.compare("absorb"))
		return ABSORB;
	else if (!attr.compare("rebound"))
		return DAMAGE_REBOUND;
	else if (!attr.compare("phys_damage_reduce"))
		return PHYS_DAMAGE_REDUCE;
	else if (!attr.compare("mana_damage_reduce"))
		return MANA_DAMAGE_REDUCE;
}
