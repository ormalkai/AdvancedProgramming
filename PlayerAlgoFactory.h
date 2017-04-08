#pragma once

#include "IBattleshipGameAlgo.h"

enum class AlgoType {
	FILE
};

class PlayerAlgoFactory {
public:
	IBattleshipGameAlgo* create(AlgoType type);
	static PlayerAlgoFactory& instance();
private:
	int m_id;
	

};