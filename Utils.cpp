
#include "Utils.h"


Utils& Utils::instance()
{
	static Utils utils;
	return utils;
}

/**
* @Details		receives char and returns for which player the ship belongs to
* @Param		c - query ship
* @Return		PlayerIndex::PLAYER_A if c belongs to m_playerALegalSign,
*				PlayerIndex::PLAYER_B if c belongs to m_playerBLegalSign,
*				PlayerIndex::MAX_PLAYER otherwise
*/
 PlayerIndex Utils::getPlayerIdByShip(char c)
{

	if (std::find(m_playerALegalSign.begin(), m_playerALegalSign.end(), c) != m_playerALegalSign.end())
	{
		return PlayerIndex::PLAYER_A;
	}
	else if (std::find(m_playerBLegalSign.begin(), m_playerBLegalSign.end(), c) != m_playerBLegalSign.end())
	{
		return PlayerIndex::PLAYER_B;
	}
	return PlayerIndex::MAX_PLAYER;
}

int Utils::getIndexByShip(char c)
{
	switch (c) {
	case 'b':
	case 'B':
		return RUBBER_SHIP;
	case 'p':
	case 'P':
		return ROCKET_SHIP;
	case 'm':
	case 'M':
		return SUBMRINE;
	case 'd':
	case 'D':
		return DESTROYER;
	default:
		return (int)ReturnCode::RC_ERROR;
	}
}

int Utils::getShipByIndexAndPlayer(int ship, int player)
{
	if (MAX_SHIP < ship || 0 > ship)
	{
		return (int)ReturnCode::RC_ERROR;
	}

	if (MAX_PLAYER < player || 0 > player)
	{
		return (int)ReturnCode::RC_ERROR;
	}

	switch (player) {
	case PLAYER_A:
		return m_playerALegalSign[ship];
	case PLAYER_B:
		return m_playerBLegalSign[ship];
	default:
		return (int)ReturnCode::RC_ERROR;
	}
}

int Utils::getPlayerCharByIndex(int player)
{
	if (MAX_PLAYER < player || 0 > player)
	{
		return (int)ReturnCode::RC_ERROR;
	}

	return m_playerIndexToChar[player];
}