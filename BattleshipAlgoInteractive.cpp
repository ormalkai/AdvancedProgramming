#include "BattleshipAlgoFromFile.h"
#include <fstream>
#include <sstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include "Debug.h"
#include "BattleshipAlgoInteractive.h"
#include <iostream>

// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

BattleshipAlgoInteractive::BattleshipAlgoInteractive(int id)
{
	this->setId(id);
}

void BattleshipAlgoInteractive::setBoard(int player, const char** board, int numRows, int numCols)
{
	// noting to do in player algo from file
}

std::pair<int, int> BattleshipAlgoInteractive::attack()
{

	Utils::gotoxy(41, 0);
	pair<int, int> attack(-1,-1);
	cout << "Enter attack bro: ";

	cin >> attack.first >> attack.second;

	cout << endl;	
	return attack;
}


//splits string s to a vector
static std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream strSt(s);
	std::string item;

	while (std::getline(strSt, item, delim)) //we could ommit the 'std::' notion from here.
	{
		elems.push_back(item); //push_back is a function of vector class, it inserted to the back of the vector.
	}

	return elems;
}

void BattleshipAlgoInteractive::AttackFileParser(string& attackPath) {
	vector<pair<int, int>> attackMoves;
	vector<string> tokens;
	int i, j;
	ifstream fin(attackPath);
	string line;
	DBG(Debug::DBG_INFO, "Parsing file [%s]", attackPath.c_str());
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
		if (i < 1 || j < 1 || i > BOARD_ROW_SIZE || j > BOARD_COL_SIZE)
		{
			DBG(Debug::DBG_WARNING, "Failed to parse attack requet: line[%d] [%s]", lineNumber, line.c_str());
			continue;;
		}

		pair<int, int> aPair(i, j);
		m_attackQueue.push(aPair);
	}
}



