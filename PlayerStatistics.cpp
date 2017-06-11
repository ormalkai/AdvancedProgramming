
#include "PlayerStatistics.h"
#include <cassert>


void PlayerStatistics::update(const pair<int, int> score, WinLoseTie result)
{
	int myScore = score.first;
	int otherScore = score.second;

	++m_gamesPlayed;
	if (WON == result)
	{
		++m_wins;
	}
	else if (LOST == result)
	{
		++m_losses;
	}
	else /*TIE == result*/
	{
		++m_ties;
	}
	
	m_ptsFor += myScore;
	m_ptsAgainst += otherScore;
}
