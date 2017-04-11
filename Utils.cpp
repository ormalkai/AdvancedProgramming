
#include "Utils.h"
#include <istream>

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

	m_shipToColor[PLAYER_A_RUBBER_SHIP] = FOREGROUND_BLUE;
	m_shipToColor[PLAYER_B_RUBBER_SHIP] = FOREGROUND_BLUE | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	m_shipToColor[PLAYER_A_ROCKET_SHIP] = FOREGROUND_GREEN;
	m_shipToColor[PLAYER_B_ROCKET_SHIP] = FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	m_shipToColor[PLAYER_A_SUBMARINE] = FOREGROUND_BLUE | FOREGROUND_GREEN;
	m_shipToColor[PLAYER_B_SUBMARINE] = FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
	m_shipToColor[PLAYER_A_DESTROYER] = FOREGROUND_RED;
	m_shipToColor[PLAYER_B_DESTROYER] = FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
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

_In_ WORD Utils::getShipColor(char c)
{
	return m_shipToColor[c];
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
	if (player < 0 || player > MAX_PLAYER)
	{
		return "";
	}

	return m_expectedAttackFilePerPlayer[player];
}

/**
 * @Credit		http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
 */
std::istream& Utils::safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

