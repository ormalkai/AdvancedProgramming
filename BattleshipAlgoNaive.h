#pragma once

#include <queue>		// for attack queue
#include "Board.h"

using namespace std;

class BattleshipAlgoNaive : public IBattleshipGameAlgo {

private:
	int m_id;				// unique identifier of the player (PlayerA/PlayerB)
	char			m_board[INIT_BOARD_ROW_SIZE][INIT_BOARD_COL_SIZE];	// The board of the player (currently not in use, for future prposes)
	int				m_rows;			// number of rows in the board
	int				m_cols;			// number of columns in the board
	pair<int,int>	m_nextAttack;	// generator for next attack

	bool isAlreadyAttacked(char c);
	bool isNeighborsMine(int i, int j);
	bool hasShipOnTheLeft(int i, int j);
	bool hasShipOnTheTop(int i, int j);
public:
	/**
	* @Details		Constructor receives id of the player
	* @param		id - Unique identifier of the player
	*/
	BattleshipAlgoNaive(int id);

	/**
	* @Details		Default destructor
	*/
	~BattleshipAlgoNaive() = default;

	BattleshipAlgoNaive(const BattleshipAlgoNaive &) = default;
	BattleshipAlgoNaive &operator=(const BattleshipAlgoNaive &) = default;
	
	/**
	* @Details		getter for id
	*/
	int getId() const { return m_id; }

	/**
	* @Details		setter for id
	*/
	void setId(int id) { m_id = id; }

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
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;

	bool init(const std::string& path) override;


};
