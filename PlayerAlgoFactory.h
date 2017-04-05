#pragma once

#include "IBattleshipGameAlgo.h"

enum class AlgoType {
	FILE
};

class PlayerAlgoFactory {

	static int id = 0;

	IBattleshipGameAlgo createInstance(AlgoType type);
	

};