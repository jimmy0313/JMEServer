#include "battle.h"
#include "battle_event.h"
#include "battle_unit.h"
#include <time.h>
#include "battle_data.h"
Battle::Battle():
	_random_seed(time(NULL))
{
	_group_0 = new BattleGroup(this, BaseId_0);
	_group_1 = new BattleGroup(this, BaseId_1);

	_counter_manager = new CounterManager;
	_battle_event_manager = new BattleEventManager;
}

Battle::~Battle()
{
	delete _group_0;
	delete _group_1;
	delete _battle_event_manager;
	delete _counter_manager;
}

bool Battle::update(float delta)
{
	if (!_group_0->allDead() && !_group_1->allDead())
	{
		_counter_manager->update(delta);
		_group_0->update(delta);
		_group_1->update(delta);

		return false;
	}
	else
	{
		_group_0->print();
		_group_1->print();

		if (_group_0->allDead())
		{
			std::cout << "Group 1 win !!!" << std::endl;
		}
		else
		{
			std::cout << "Group 0 win !!!" << std::endl;
		}
		std::cout << "============================== 战斗结束 ==============================" << std::endl;
	}
	return true;
}

int Battle::random()
{
	auto myrandom = [](int seed)
	{
		return ( ((seed * 214013L + 2531011L) >> 16) & 0x7fff );
	};
	int random = myrandom(_random_seed);

	_random_seed += random == 0 ? 0x7fff : random;	// 设置新种
	return random % 100 + 1;
}

void Battle::use_skill(int unit_id, int skill_id)
{
	ActionUnit* unit = unit_id < BaseId_1 ? unit = _group_0->getUnit(unit_id) : unit = _group_1->getUnit(unit_id);
	if (nullptr == unit || unit->father() != unit)
		return;

	Command cmd;
	cmd._skill_id = skill_id;
	unit->pushCommand(cmd);
}

void Battle::dispel(int unit_id, int num)
{
	auto unit = getUnit(unit_id);
	if (nullptr != unit)
	{
		unit->dispel(num);
	}
}

void Battle::skillComplete(int unit_id)
{
	auto unit = getUnit(unit_id);
	if (nullptr != unit)
	{
		unit->skillComplete();
	}
}

void Battle::skillHit(int unit_id, int skill_id, std::vector<int>& target_vec)
{
	auto skill = DataManager::getInstance()->getSkill(skill_id);
	if (nullptr == skill)
		return;

	auto unit = getUnit(unit_id);
	if (nullptr == unit)
		return;
	
	skill->exec_skill(unit, target_vec);
}

Warrior* Battle::getUnit(int unit_id)
{
	ActionUnit* unit = unit_id < BaseId_1 ? unit = _group_0->getUnit(unit_id) : unit = _group_1->getUnit(unit_id);
	if (nullptr == unit || unit != unit->father())
		return nullptr;
	return unit->father();
}
