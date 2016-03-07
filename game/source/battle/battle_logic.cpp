#include "battle_logic.h"
#include "battle.h"
#include <iostream>
BattleLogic::BattleLogic():
	_battle(nullptr)
{

}

BattleLogic::~BattleLogic()
{
	if (nullptr != _battle)
	{
		delete _battle;
	}
}

BattleLogic& BattleLogic::getInstance()
{
	static BattleLogic _pInstance;
	return _pInstance;
}

void BattleLogic::createBattle()
{
	if (nullptr != _battle)
	{
		delete _battle;
	}

	std::cout << "============================== ´´½¨Õ½¶· ==============================" << std::endl;
	_battle = new Battle;
}

Battle* BattleLogic::operator->()
{
	return _battle;
}
