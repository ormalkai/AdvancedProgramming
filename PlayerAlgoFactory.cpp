#include "PlayerAlgoFactory.h"
#include "BattleshipAlgoFromFile.h"

PlayerAlgo* PlayerAlgoFactory::create(AlgoType type)
{
	PlayerAlgo* ret;

	switch (type)
	{
	case(AlgoType::FILE):
		ret = new BattleshipAlgoFromFile(m_id++);
		break;
	default:
		ret = nullptr;
	}

	ret->setScore(0);

	return ret;
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
