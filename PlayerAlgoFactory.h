#pragma once

#include "IBattleshipGameAlgo.h"

enum class AlgoType {
	FILE
};

class PlayerAlgoFactory {
public:
	IBattleshipGameAlgo createInstance(AlgoType type);
private:
	static int id;
	

};