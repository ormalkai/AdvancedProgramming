#include "BattleshipAlgoFromFile.h"
#include <fstream>
#include <sstream>
#include <vector>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

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
}

std::pair<int, int> BattleshipAlgoFromFile::attack()
{
	// TODO what if empty
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
			return;
		}
		trim(tokens[0]);
		i = stoi(tokens[0]);
		trim(tokens[1]);
		j = stoi(tokens[1]);
		pair<int, int> aPair(i, j);
		m_attackQueue.push_back(aPair);
	}


}

//int BattleshipAlgoFromFile::checkLine(string str, int k) {
//
//	int i;
//	if (k == 1) {
//		i = stoi(str);
//	}
//
//	if (k == 2) {
//		i = cleanSpace();
//		i = stoi(str[i]);
//	}
//	if (i >= 1 && i <= 10) {
//		if (cleanSpace() == str.length())
//			return i;
//	}
//	return -1;
//
//}
//
//int BattleshipAlgoFromFile::cleanSpace(string str, int index) {
//
//	while (SPACE == index<str.length() && str[index])
//	{
//		index++;
//	}
//	return index;
//}


