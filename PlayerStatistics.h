#pragma once

#include <string>
#include <atomic>
#include "BattleshipAlgoSmart.h"

using namespace std;

class PlayerStatistics
{
private:
	int		m_playerIndex;
	string	m_playerName;
	int		m_gamesPlayed;
	int		m_wins;
	int		m_losses;
	int		m_ties;
	int		m_ptsFor;
	int		m_ptsAgainst;

	
public:

	PlayerStatistics(int index, string name) : m_playerIndex(index), m_playerName(name),m_gamesPlayed(0), m_wins(0), m_losses(0), m_ptsFor(0), m_ptsAgainst(0)
	{}

	/*bool operator< (const PlayerStatistics &other) const {
		return this->m_wins < other.m_wins;
	}*/

	void update(const pair<int, int> score, WinLoseTie result);

	int getWins() const { return m_wins; }
	int getLosses() const { return m_losses; }
	int getPointsFor() const { return m_ptsFor; }
	int getPointsAgainst() const { return m_ptsAgainst; }
	string getPlayerName() const { return m_playerName; }
	int getPlayerIndex() const { return m_playerIndex; }



};
