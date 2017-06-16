#pragma once

#include "BattleshipAlgoSmart.h"

using namespace std;

/**
 *@Details		Class for managing tournament results table
 */
class PlayerStatistics
{
private:
	int m_playerIndex;
	string m_playerName;
	int m_gamesPlayed;
	int m_wins;
	int m_losses;
	int m_ties;
	int m_ptsFor;
	int m_ptsAgainst;


public:

	PlayerStatistics(int index, string name) : m_playerIndex(index), m_playerName(name), m_gamesPlayed(0), m_wins(0), m_losses(0), m_ties(0), m_ptsFor(0), m_ptsAgainst(0)
	{
	}

	/**
	 * @Details		Update player result for game
	 */
	void update(const pair<int, int> score, WinLoseTie result);

	/**
	 * @Details		Get number of wins for player
	 */
	int getWins() const { return m_wins; }

	/**
	* @Details		Get number of losses for player
	*/
	int getLosses() const { return m_losses; }

	/**
	* @Details		Get number of points for player
	*/
	int getPointsFor() const { return m_ptsFor; }

	/**
	* @Details		Get number of points against player
	*/
	int getPointsAgainst() const { return m_ptsAgainst; }

	/**
	* @Details		Get player name for printing
	*/
	string getPlayerName() const { return m_playerName; }

	/**
	* @Details		Get player index
	*/
	int getPlayerIndex() const { return m_playerIndex; }
};
