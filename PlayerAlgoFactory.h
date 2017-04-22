#pragma once

#include "PlayerAlgo.h"

enum class AlgoType {
	FILE,
	NAIVE
};

class PlayerAlgoFactory {
public:

	PlayerAlgo* create(AlgoType type);
	static PlayerAlgoFactory& instance();
private:
	int m_id;

	PlayerAlgoFactory()
		: m_id(0) {}
	PlayerAlgoFactory(const PlayerAlgoFactory &) = delete;
	PlayerAlgoFactory &operator=(const PlayerAlgoFactory &) { return *this; }
	

};