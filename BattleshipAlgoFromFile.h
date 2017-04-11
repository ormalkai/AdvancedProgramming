#pragma once

#include "PlayerAlgo.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>

using namespace std;

class BattleshipAlgoFromFile : public PlayerAlgo {
private:
	char** m_board;
	int m_rows;
	int m_cols;
	queue<pair<int, int>> m_attackQueue;

	int checkLine(string str, int k);
	int cleanSpace(string str, int index);



public:
	BattleshipAlgoFromFile(int id);
	~BattleshipAlgoFromFile() = default;

	/*
	Initialize attack queue
	*/
	int parseAttackFile();
	void AttackFileParser(string & attackPath);
	/*
	Initialize data members
	*/
	void setBoard(const char** board, int numRows, int numCols);

	/*
	Simulate attack- pop attack from attack queue
	*/
	std::pair<int, int> attack();

	/*
	Empty implementation
	*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result);





};
