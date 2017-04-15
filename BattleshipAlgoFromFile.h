#pragma once

#include "PlayerAlgo.h"
#include <queue>

using namespace std;

class BattleshipAlgoFromFile : public PlayerAlgo {

private:
	char** m_board;
	int m_rows;
	int m_cols;
	queue<pair<int, int>> m_attackQueue;
	
public:
	BattleshipAlgoFromFile(int id);
	~BattleshipAlgoFromFile() = default;

	/**
	* @Details		receives path of attack file and build attack queue
	* @Param		attackPath - path of attack file
	*/
	void AttackFileParser(string & attackPath);
	
	
	/**
	* @Details		Receive board matrix and matrix's size, and notify player on his board
	* @Param		board - player's board
	* @Param		board - num of rows in board
	* @Param		board - num of cols in board
	*/
	void setBoard(const char** board, int numRows, int numCols) override;

	/**
	* @Details		Return next attack request
	* @retrun		pair of coordinates - requested attack.
	*				in case the queue is empty returns the pair (-1,-1)
	*/
	std::pair<int, int> attack() override;

	/**
	* @Details		notify player on last move result
	* @Param		player - player's index
	* @Param		row - attacked cell's row index
	* @Param		col - attacked cell's col index
	* @Param		result - attack's result (hit, miss, sink)
	*/
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override {};

};
