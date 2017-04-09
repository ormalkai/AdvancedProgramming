#pragma once

#include "IBattleshipGameAlgo.h"
#include "Utils.h"

class PlayerAlgo : public IBattleshipGameAlgo {

private:
	int m_id;

public:
	~PlayerAlgo() = default;

	ReturnCode init();
	int getId() { return m_id; }
	void setId(int id) { m_id = id; }
	

	
};
