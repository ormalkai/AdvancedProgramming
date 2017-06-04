#pragma once

#include <string>
#include <atomic>

using namespace std;

class PlayerStatistics
{
private:
	atomic<int>  m_playerIndex;
	string m_playerName;
	atomic<int>  m_gamesPlayed;
	atomic<int>  m_wins;
	atomic<int>  m_loses;
	atomic<int> m_ptsFor;
	atomic<int> m_ptsAgainst;

	
public:

	PlayerStatistics(int index, string name) : m_playerIndex(index), m_playerName(name),m_gamesPlayed(0), m_wins(0), m_loses(0), m_ptsFor(0), m_ptsAgainst(0)
	{}

	bool operator< (const PlayerStatistics &other) const {
		return this->m_wins < other.m_wins;
	}

	void update(pair<int, int> result);

	int getWins() const { return m_wins; }
};
