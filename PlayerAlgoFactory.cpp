#include "PlayerAlgoFactory.h"
#include "BattleshipAlgoFromFile.h"

IBattleshipGameAlgo* PlayerAlgoFactory::create(AlgoType type)
{
	switch (type)
	{
	case(AlgoType::FILE):
		return new BattleshipAlgoFromFile(m_id++);
	default:
		return nullptr;
	}
}

PlayerAlgoFactory& PlayerAlgoFactory::instance()
{
	static PlayerAlgoFactory factory;
	static bool isFirstTime = true;
	if (isFirstTime)
	{
		factory.m_id = 0;
		isFirstTime = false;
	}
	return factory;
}
