#pragma once

#include "IBattleshipGameAlgo.h"
#include "Utils.h"

class PlayerAlgo : public IBattleshipGameAlgo {

private:
	int m_id;				// unique identifier of the player
	int m_score;			// The score of the player
	int m_numOfVictories;	// Number of victories of the player

public:
	/**
	 * @Details		constructor for PlayerAlgo
	 */
	PlayerAlgo() : m_id(MAX_PLAYER), m_score(0), m_numOfVictories(0){}

	/**
	 * @Details		default destructor
	 */
	virtual ~PlayerAlgo() = default;

	PlayerAlgo(const PlayerAlgo &) = default;
	PlayerAlgo &operator=(const PlayerAlgo &) = default;

	/**
	 * @Details		getter for id
	 */
	int getId() const { return m_id; }

	/**
	 * @Details		setter for id
	 */
	void setId(int id) { m_id = id; }

	/**
	 * @Details		getter for score
	 */
	int getScore() const { return m_score; }

	/**
	 * @Details		setter for score
	 */
	void setScore(int val) { m_score = val; }

	/**
	 * @Details		add points to current score
	 */
	void addToScore(int val) { m_score += val; }

	/**
	 * @Details		setter for number of victories
	 */
	void setNumOfVictories(int val) { m_numOfVictories = val; }

	/**
	 * @Details		Add victory to number of victories
	 */
	void updateVictory() { m_numOfVictories++; }
};
