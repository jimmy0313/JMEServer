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

	bool update(float delta); //����ֵ��ʾ ս���Ƿ����, true ����

	int random();
	
	void use_skill(int unit_id, int skill_id);	//�����ͷ� ְҵ����
	void dispel(int unit_id, int num);	//����
	void skillComplete(int unit_id); //�����ͷ���ϣ������ͷ���һ������

	void skillHit(int unit_id, int skill_id, std::vector<int>& target_vec); //��������Ŀ��
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
