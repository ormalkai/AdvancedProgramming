#pragma once

#include "IBattleshipGameAlgo.h"
#include "Utils.h"

class PlayerAlgo : public IBattleshipGameAlgo {

private:
	int m_id;
	int m_score;

public:
	~PlayerAlgo() = default;

	ReturnCode init();
	int getId() const { return m_id; }
	void setId(int id) { m_id = id; }
	int getScore() const { return m_score; }
	void setScore(int s) { m_score = s; }
	void addToScore(int val) { m_score += val; }
};
