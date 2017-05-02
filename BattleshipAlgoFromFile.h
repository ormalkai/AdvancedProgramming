#pragma once

#include <queue>		// for attack queue
#include "IBattleshipGameAlgo.h"
#include "Utils.h"

using namespace std;

class BattleshipAlgoFromFile : public IBattleshipGameAlgo
{
private:
	char** m_board; // The board of the player (currently not in use, for future prposes)
	int m_rows; // number of rows in the board
	int m_cols; // number of columns in the board
	int m_id; // unique identifier of the player
	queue<pair<int, int>> m_attackQueue;
	int m_sss;

public:
	/**
	 * @Details		Constructor receives id of the player
	 */
	BattleshipAlgoFromFile() = default;

	/**
	 * @Details		Default destructor
	 */
	~BattleshipAlgoFromFile() = default;

	BattleshipAlgoFromFile(const BattleshipAlgoFromFile&) = delete;
	BattleshipAlgoFromFile& operator=(const BattleshipAlgoFromFile&) = delete;
	BattleshipAlgoFromFile(BattleshipAlgoFromFile&&) = delete;

	/**
	* @Details		receives path of attack file and build attack queue
	* @Param		attackPath - path of attack file
	*/
	ReturnCode AttackFileParser(const string& attackPath);


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
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override
	{
	};

	/**
	* @Details		getter for id
	*/
	int getId() const { return m_id; }

	/**
	* @Details		setter for id
	*/
	void setId(int id) { m_id = id; }

	bool init(const std::string& path) override;

};