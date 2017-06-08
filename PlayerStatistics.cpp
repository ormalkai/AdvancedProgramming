
#include "PlayerStatistics.h"
#include <cassert>


void PlayerStatistics::update(std::pair<int, int> result)
{
	int myScore = result.first;
	int otherScore = result.second;

	assert(myScore != otherScore);

	++m_gamesPlayed;

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
