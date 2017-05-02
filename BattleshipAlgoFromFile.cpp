#include "BattleshipAlgoFromFile.h"
#include <fstream>
#include <sstream>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include "Debug.h"
#include "Utils.h"
#include <iostream>

// trim from start (in place)
static inline void ltrim(string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
	                                std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
	                     std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(string& s)
{
	ltrim(s);
	rtrim(s);
}

void BattleshipAlgoFromFile::setBoard(int player, const char** board, int numRows, int numCols)
{
	this->setId(player);
	m_rows = numRows;
	m_cols = numCols;
}

std::pair<int, int> BattleshipAlgoFromFile::attack()
{
	if (0 == m_attackQueue.size())
	{
		static bool isFirst = true;
		if (true == isFirst)
		{
			DBG(Debug::DBG_DEBUG, "Attacks of player %c done", Utils::getPlayerCharByIndex(getId()));
		}
		return make_pair(-1, -1);
	}
	
	pair<int, int> attack = m_attackQueue.front();
	m_attackQueue.pop();
	return attack;
}


//splits string s to a vector
static std::vector<std::string> split(const string& s, char delim)
{
	vector<string> elems;
	stringstream strSt(s);
	string item;

	while (getline(strSt, item, delim)) //we could ommit the 'std::' notion from here.
	{
		elems.push_back(item); //push_back is a function of vector class, it inserted to the back of the vector.
	}

	return elems;
}

ReturnCode BattleshipAlgoFromFile::AttackFileParser(const string& attackFile)
{
	vector<pair<int, int>> attackMoves;
	vector<string> tokens;
	int i, j;
	ifstream fin(attackFile);
	if (!fin.is_open())
	{
		DBG(Debug::DBG_ERROR, "Could not open attack file: %s", attackFile);
		return RC_ERROR;
	}
	string line;
	DBG(Debug::DBG_INFO, "Parsing file [%s]", attackFile.c_str());
	int lineNumber = 0;
	while (Utils::safeGetline(fin, line))
	{
		if (fin.eof())
			break;

		++lineNumber;

		tokens = split(line, ',');
		if (tokens.size() != 2)
		{
			// skip bad lines
			DBG(Debug::DBG_WARNING, "Ignore invalid line[%d] [%s]", lineNumber, line.c_str());
			continue;
		}
		trim(tokens[0]);
		trim(tokens[1]);
		try
		{
			i = stoi(tokens[0]);
			j = stoi(tokens[1]);
		}
		catch (const std::exception&)
		{
			DBG(Debug::DBG_WARNING, "Ignore invalid line[%d] [%s]", lineNumber, line.c_str());
			continue;
		}

		// input validation
		if (i < 1 || j < 1 || i > m_rows || j > m_cols)
		{
			DBG(Debug::DBG_WARNING, "Failed to parse attack requet: line[%d] [%s]", lineNumber, line.c_str());
			continue;;
		}

		pair<int, int> aPair(i, j);
		m_attackQueue.push(aPair);
	}

	return RC_SUCCESS;
}

bool BattleshipAlgoFromFile::init(const string& path)
{
	// first lets find the attack file
	vector<string> attackFiles;
	// load sboard file
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(path, "attack", attackFiles);
	if (RC_SUCCESS != rc)
	{
		cout << "Missing attack file for player " << Utils::getPlayerCharByIndex(m_id) << " (*.attack) looking in path: " << path << endl;
		return false;
	}
	string attackFile;
	// get the correct file according to player id and number of files
	if (PLAYER_A == getId())
	{
		// I'm player A lets take the first file
		attackFile = attackFiles[0];
	}
	else // PLAYER_B == getId()
	{
		if (2 <= attackFiles.size())
		{
			// there are two files lets use the second file
			attackFile = attackFiles[1];
		}
		else
		{
			// there is only one file
			attackFile = attackFiles[0];
		}
	}
	// parse attack file
	ReturnCode ret = AttackFileParser(attackFile);
	if (RC_SUCCESS != ret)
	{
		return false;
	}

	return true;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	return (new BattleshipAlgoFromFile());
}
