
#include "Utils.h"

Utils::Utils()
{
	m_shipToLen[PLAYER_A_RUBBER_SHIP] = m_shipToLen[PLAYER_B_RUBBER_SHIP] = 1;
	m_shipToLen[PLAYER_A_ROCKET_SHIP] = m_shipToLen[PLAYER_B_ROCKET_SHIP] = 2;
	m_shipToLen[PLAYER_A_SUBMARINE]   = m_shipToLen[PLAYER_B_SUBMARINE] = 3;
	m_shipToLen[PLAYER_A_DESTROYER]   = m_shipToLen[PLAYER_B_DESTROYER] = 4;

	m_shipToValue[PLAYER_A_RUBBER_SHIP] = m_shipToValue[PLAYER_B_RUBBER_SHIP] = 2;
	m_shipToValue[PLAYER_A_ROCKET_SHIP] = m_shipToValue[PLAYER_B_ROCKET_SHIP] = 3;
	m_shipToValue[PLAYER_A_SUBMARINE] = m_shipToValue[PLAYER_B_SUBMARINE] = 7;
	m_shipToValue[PLAYER_A_DESTROYER] = m_shipToValue[PLAYER_B_DESTROYER] = 8;
}

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

	if (isExistInVec(m_playerALegalSign, c))
	{
		return PlayerIndex::PLAYER_A;
	}
	else if (isExistInVec(m_playerBLegalSign, c))
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
		return static_cast<int>(ReturnCode::RC_ERROR);
	}
}

int Utils::getShipLen(char c)
{
	return m_shipToLen[c];
}

int Utils::getShipValue(char c)
{
	return m_shipToValue[c];
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

char Utils::getPlayerCharByIndex(int player)
{
	if (MAX_PLAYER < player || 0 > player)
	{
		return '\0';
	}

	return m_playerIndexToChar[player];
}

string Utils::getAttackFileByPlayer(int player)
{
	// TODO: insert return statement here
	if (player < 0 || player > MAX_PLAYER)
	{
		return "";
	}

	return m_expectedAttackFilePerPlayer[player];
}


template<class T>
bool Utils::isExistInVec(vector<T> vec, T val)
{
	if (find(vec.begin(), vec.end(), val) != vec.end()) {
		return true;
	}
	return false;
}
