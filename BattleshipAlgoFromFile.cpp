#include "BattleshipAlgoFromFile.h"
#include <fstream>
#include <sstream>
#include <vector>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include "Debug.h"

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

BattleshipAlgoFromFile::BattleshipAlgoFromFile(int id)
{
	this->setId(id);
}

void BattleshipAlgoFromFile::setBoard(const char** board, int numRows, int numCols)
{
	// noting to do in player algo from file
}

std::pair<int, int> BattleshipAlgoFromFile::attack()
{
	if (0 == m_attackQueue.size())
	{
		static bool isFirst = true;
		if (true == isFirst)
		{
			DBG(Debug::DBG_DEBUG, "Attacks of player %c done", getId());
		}
		return std::pair<int, int>(-1, -1);
	}
	std::pair<int, int> attack = m_attackQueue[m_attackQueue.size() - 1];
	m_attackQueue.pop_back();
	return attack;
}

void BattleshipAlgoFromFile::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
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

void BattleshipAlgoFromFile::AttackFileParser(string& attackPath) {
	vector<pair<int, int>> attackMoves;
	vector<string> tokens;
	int i, j;
	ifstream fin(attackPath);
	string line;
	while (Utils::instance().safeGetline(fin, line))
	{
		tokens = split(line, ',');
		if (tokens.size() != 2)
		{
			// skip bad lines
			return;
		}
		trim(tokens[0]);
		i = stoi(tokens[0]);
		trim(tokens[1]);
		j = stoi(tokens[1]);

		// input validation
		if (i < 1 || j < 1 || i > BOARD_ROW_SIZE || j > BOARD_COL_SIZE)
		{
			return;
		}

		pair<int, int> aPair(i, j);
		m_attackQueue.push_back(aPair);
	}
}



