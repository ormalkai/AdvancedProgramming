#pragma once

#include "PlayerAlgo.h"
#include <queue>
#include "Board.h"

class BattleshipAlgoSmart : public PlayerAlgo
{

private:
	//char**	m_board;	// The board of the player (currently not in use, for future prposes)
	Board	m_board;
	std::priority_queue<std::pair<int,int>, int, cmp> m_histData;
	int		m_rows;		// number of rows in the board
	int		m_cols;		// number of columns in the board
	queue<pair<int, int>> m_attackQueue;

	struct cmp
	{
		bool operator()(int a, int b)
		{
			return occurrences[a] < occurrences[b];
		}
	};


public:
	/**
	* @Details		Constructor receives id of the player
	* @param		id - Unique identifier of the player
	*/
	BattleshipAlgoSmart(int id);

	/**
	* @Details		Default destructor
	*/
	~BattleshipAlgoSmart() = default;

	BattleshipAlgoSmart(const BattleshipAlgoSmart &) = default;
	BattleshipAlgoSmart &operator=(const BattleshipAlgoSmart &) = default;

	/**
	* @Details		Receive board matrix and matrix's size, and notify player on his board
	* @Param		board - player's board
	* @Param		board - num of rows in board
	* @Param		board - num of cols in board
	*/
	void setBoard(int player, const char** board, int numRows, int numCols) override;

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

	bool init(const std::string& path) override { return false; }
	int calcHist(int i, int j);
	bool isOtherNeighborValid(const Cell& cell, Direction d);
	int calcNumOfOptionalShipsInOffset(int i, int j) const;
};


