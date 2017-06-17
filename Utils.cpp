#include <istream>
#include <iostream>
#include <locale>
#include <algorithm>
#include <regex>
#include "Utils.h"
#include "Debug.h"


map<char, int> Utils::m_shipLenBySign = {
	{PLAYER_A_RUBBER_SHIP, 1},
	{PLAYER_A_ROCKET_SHIP, 2},
	{PLAYER_A_SUBMARINE, 3},
	{PLAYER_A_DESTROYER, 4},
	{PLAYER_B_RUBBER_SHIP, 1},
	{PLAYER_B_ROCKET_SHIP, 2},
	{PLAYER_B_SUBMARINE, 3},
	{PLAYER_B_DESTROYER, 4}
};


const map<ShipType, int> Utils::m_shipLenByType = {
	{RUBBER_SHIP, 1},
	{ROCKET_SHIP, 2},
	{SUBMARINE, 3},
	{DESTROYER, 4}
};

map<char, int> Utils::m_shipToValue = {
	{PLAYER_A_RUBBER_SHIP, 2},
	{PLAYER_A_ROCKET_SHIP, 3},
	{PLAYER_A_SUBMARINE, 7},
	{PLAYER_A_DESTROYER, 8},
	{PLAYER_B_RUBBER_SHIP, 2},
	{PLAYER_B_ROCKET_SHIP, 3},
	{PLAYER_B_SUBMARINE, 7},
	{PLAYER_B_DESTROYER, 8}
};

map<char, _In_ WORD> Utils::m_shipToColor = {
	{PLAYER_A_RUBBER_SHIP , FOREGROUND_BLUE | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_B_RUBBER_SHIP, FOREGROUND_BLUE | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_A_ROCKET_SHIP, FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_B_ROCKET_SHIP, FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_A_SUBMARINE, FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_B_SUBMARINE, FOREGROUND_BLUE | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_A_DESTROYER, FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED},
	{PLAYER_B_DESTROYER, FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED}
};


const vector<char> Utils::m_playerIndexToChar
{
	PLAYER_A_CHAR,
	PLAYER_B_CHAR
};

const vector<char> Utils::m_playerALegalSign =
{
	PLAYER_A_RUBBER_SHIP,
	PLAYER_A_ROCKET_SHIP,
	PLAYER_A_SUBMARINE,
	PLAYER_A_DESTROYER
};

const vector<char> Utils::m_playerBLegalSign =
{
	PLAYER_B_RUBBER_SHIP,
	PLAYER_B_ROCKET_SHIP,
	PLAYER_B_SUBMARINE,
	PLAYER_B_DESTROYER
};

const vector<string> Utils::m_expectedAttackFilePerPlayer =
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
	switch (c)
	{
	case 'b':
	case 'B':
		return RUBBER_SHIP;
	case 'p':
	case 'P':
		return ROCKET_SHIP;
	case 'm':
	case 'M':
		return SUBMARINE;
	case 'd':
	case 'D':
		return DESTROYER;
	default:
		return static_cast<int>(ReturnCode::RC_ERROR);
	}
}


int Utils::getShipLen(char c)
{
	return m_shipLenBySign[c];
}

int Utils::getShipLen(ShipType s)
{
	return m_shipLenByType.at(s);
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
		return static_cast<int>(ReturnCode::RC_ERROR);
	}

	if (MAX_PLAYER < player || 0 > player)
	{
		return static_cast<int>(ReturnCode::RC_ERROR);
	}

	switch (player)
	{
	case PLAYER_A:
		return m_playerALegalSign[ship];
	case PLAYER_B:
		return m_playerBLegalSign[ship];
	default:
		return static_cast<int>(ReturnCode::RC_ERROR);
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

	std::streambuf* sb = is.rdbuf();

	for (;;)
	{
		int c = sb->sbumpc();
		switch (c)
		{
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
			t += static_cast<char>(c);
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
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Get handle to standard output
	SetConsoleTextAttribute(hConsole, color);
}

Direction Utils::getOppositeDirection(Direction d)
{
	switch (d)
	{
	case Direction::UP: return Direction::DOWN;
	case Direction::DOWN: return Direction::UP;
	case Direction::RIGHT: return Direction::LEFT;
	case Direction::LEFT: return Direction::RIGHT;
	default: return Direction::DOWN;
	}
}

ReturnCode Utils::getListOfFilesInDirectoryBySuffix(const string& path, const string& suffix, vector<string>& files, bool printWrongPath/*false*/)
{
	DWORD ftyp = GetFileAttributesA(path.c_str());
	if ("" != path &&
		(ftyp == INVALID_FILE_ATTRIBUTES || false == (ftyp & FILE_ATTRIBUTE_DIRECTORY)))
	{
		if (printWrongPath)
			cout << "Wrong path: " << path << endl;
		return RC_INVALID_ARG; //something is wrong with your path!
	}


	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	string sboardFile = path + "*." + suffix;
	hFind = FindFirstFileA(sboardFile.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return RC_ERROR;
	}

	do
	{
		files.push_back(path + FindFileData.cFileName);
	}
	while (FindNextFileA(hFind, &FindFileData) != 0);

	FindClose(hFind);

	// sort files in lex order
	sort(files.begin(), files.end(), greater<string>());

	return RC_SUCCESS;
}

void Utils::ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

ReturnCode Utils::getPlayerNameByDllPath(const string& path, string& playerName)
{
	regex regex(".*ex3\\.(\\S+\\.smart)\\.dll");
	smatch m;
	regex_match(path, m, regex);
	if (m.size() != 2)
	{
		DBG(Debug::DBG_ERROR, "Failed to extract player name from dll path: %s", path.c_str());
		return RC_ERROR;
	}
	
	playerName = m[1];

	return RC_SUCCESS;
}
