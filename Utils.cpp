
#include "Utils.h"
#include <istream>


map<char, int> Utils::m_shipToLen = {
	{ PLAYER_A_RUBBER_SHIP, 1 },
	{ PLAYER_A_ROCKET_SHIP, 2 },
	{ PLAYER_A_SUBMARINE, 3 },
	{ PLAYER_A_DESTROYER, 4 },
	{ PLAYER_B_RUBBER_SHIP, 1 },
	{ PLAYER_B_ROCKET_SHIP, 2 },
	{ PLAYER_B_SUBMARINE, 3 },
	{ PLAYER_B_DESTROYER, 4 }
};


map<char, int> Utils::m_shipToValue = {
{PLAYER_A_RUBBER_SHIP, 2 },
{PLAYER_A_ROCKET_SHIP, 3},
{PLAYER_A_SUBMARINE, 7},
{PLAYER_A_DESTROYER, 8},
{PLAYER_B_RUBBER_SHIP, 2},
{PLAYER_B_ROCKET_SHIP, 3},
{PLAYER_B_SUBMARINE, 7},
{PLAYER_B_DESTROYER, 8}
};

map<char, _In_ WORD>  Utils::m_shipToColor = {
{ PLAYER_A_RUBBER_SHIP , FOREGROUND_BLUE },
{ PLAYER_B_RUBBER_SHIP, FOREGROUND_BLUE | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED },
{ PLAYER_A_ROCKET_SHIP, FOREGROUND_GREEN },
{ PLAYER_B_ROCKET_SHIP, FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED },
{ PLAYER_A_SUBMARINE, FOREGROUND_BLUE | FOREGROUND_GREEN },
{ PLAYER_B_SUBMARINE, FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED },
{ PLAYER_A_DESTROYER, FOREGROUND_RED },
{ PLAYER_B_DESTROYER, FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED }
};


const vector<char> Utils::m_playerIndexToChar
{
	PLAYER_A_CHAR,
	PLAYER_B_CHAR
};

const vector<char>  Utils::m_playerALegalSign =
{
	PLAYER_A_RUBBER_SHIP,
	PLAYER_A_ROCKET_SHIP,
	PLAYER_A_SUBMARINE,
	PLAYER_A_DESTROYER
};

const vector<char>  Utils::m_playerBLegalSign =
{
	PLAYER_B_RUBBER_SHIP,
	PLAYER_B_ROCKET_SHIP,
	PLAYER_B_SUBMARINE,
	PLAYER_B_DESTROYER
};

const vector<string>  Utils::m_expectedAttackFilePerPlayer =
{
	// if no attack file - empty string
	PLAYER_A_ATTACK_FILE,
	PLAYER_B_ATTACK_FILE
};


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

void Utils::gotoxy(int row, int col)
{
	COORD coord;
	coord.X = col; // y is the lines
	coord.Y = row; // x is the columns
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void Utils::setTextColor(_In_ WORD color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	SetConsoleTextAttribute(hConsole, color);
}