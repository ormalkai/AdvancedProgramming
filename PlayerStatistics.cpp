
#include "PlayerStatistics.h"
#include <cassert>


void PlayerStatistics::update(const pair<int, int> result)
{
	int myScore = result.first;
	int otherScore = result.second;

	assert(myScore != otherScore);

	++m_gamesPlayed;
	// TODO ORM what happens in tie
	if (myScore > otherScore)
	{
		++m_wins;
	}
	else
	{
		++m_losses;
	}
	
	assert(m_gamesPlayed == m_wins + m_losses);

	m_ptsFor += myScore;
	m_ptsAgainst += otherScore;
}
