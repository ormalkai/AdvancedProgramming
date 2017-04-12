#pragma once

#include "IBattleshipGameAlgo.h"
#include "Utils.h"

class PlayerAlgo : public IBattleshipGameAlgo {

private:
	int m_id;
	int m_score;
	int m_numOfVictories;

public:
	~PlayerAlgo() = default;


	int getId() const { return m_id; }
	void setId(int id) { m_id = id; }
	int getScore() const { return m_score; }
	void setScore(int val) { m_score = val; }
	void addToScore(int val) { m_score += val; }
	void setNumOfVictories(int val) { m_numOfVictories = val; }
	void updateVictory() { m_numOfVictories++; }
};
