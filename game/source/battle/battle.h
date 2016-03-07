#ifndef battle_h__
#define battle_h__

#include "event_interface.h"

class ActionUnit;
class BattleGroup;
class CounterManager;
class BattleEvent;
class Warrior;

typedef EventManager<BattleEvent, ActionUnit*, bool> BattleEventManager;

class Battle
{
public:
	Battle();
	virtual ~Battle();

	bool update(float delta); //返回值表示 战斗是否结束, true 结束

	int random();
	
	void use_skill(int unit_id, int skill_id);	//用于释放 职业技能
	void dispel(int unit_id, int num);	//消块
	void skillComplete(int unit_id); //技能释放完毕，可以释放下一个技能

	void skillHit(int unit_id, int skill_id, std::vector<int>& target_vec); //技能命中目标
public:
	BattleGroup* _group_0;
	BattleGroup* _group_1;

	CounterManager* _counter_manager;
	BattleEventManager* _battle_event_manager;

private:
	int _random_seed;

private:
	Warrior* getUnit(int unit_id);
};
#endif // battle_h__
