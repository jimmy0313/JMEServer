#ifndef battle_data_h__
#define battle_data_h__

#include "skill.h"
#include "battle_unit.h"

class DataManager
{
public:
	static DataManager* getInstance();
	void init();

	const Buff* getBuff(int buff_id);
	Buff* createBuff(int skill_id);
	Skill* getSkill(int buff_id); 
private:
	map<int, Skill> _skill_map;
 	map<int, Buff> _buff_map;
};
#endif // battle_data_h__
