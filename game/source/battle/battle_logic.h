#ifndef battle_logic_h__
#define battle_logic_h__

class Battle;
class BattleLogic
{
public:
	virtual ~BattleLogic();

	static BattleLogic& getInstance();

	void createBattle();

	Battle* operator ->();

private:
	BattleLogic();
	BattleLogic(const BattleLogic&){};
	BattleLogic& operator = (const BattleLogic&) {}
private:
	Battle* _battle;	
};
#endif // battle_logic_h__
