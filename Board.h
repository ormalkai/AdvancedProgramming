#pragma once

#include "Cell.h"
#include <vector>
#include "IBattleshipGameAlgo.h"

using std::vector;

class Board
{
private:
	Cell m_boardData[INIT_BOARD_ROW_SIZE][INIT_BOARD_COL_SIZE];
	//vector<vector<Cell>> m_boardData;
	vector<Ship*> m_shipsOnBoard;
	int m_rows;
	int m_cols;
	bool m_isQuiet;
	int m_delay;

public:
	Board(int rows = BOARD_ROW_SIZE, int cols = BOARD_COL_SIZE) : m_rows(rows), m_cols(cols) {};
	~Board();
	Board(const Board &) = default;

	/**
	* @Details		Receive row and col indexes and returns reference for the cell in the board
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	* @return		ref of cell in row r and col c
	*/
	Cell& get(int r, int c) { return  m_boardData[r][c]; }

	/**
	* @Details		Receive pair of row and col indexes and returns reference for the cell in the board
	* @Param		i - requested cell's indexes
	* @return		ref of cell in row r and col c
	*/
	Cell& get(pair<int, int> i) { return get(i.first, i.second); }

	/**
	* @Details		Returns num of rows in the board (int)
	* @return		num of rows in the board
	*/
	int rows() const { return m_rows; }
	
	/**
	* @Details		Returns num of cols in the board (int)
	* @return		num of cols in the board
	*/
	int cols() const { return m_cols; }
	
	/**
	* @Details		Prints the board in with sign color for each player's ship
	*/
	void printBoard() const;

	/**
	* @Details		Update GUI board after attack
	* @Param		i - attacked cell's row index
	* @Param		j - attacked cell's col index
	* @return		attackResult - attack restult (hit, miss, sink)
	*/
	void printAttack(int i, int j, AttackResult attackResult) const;

	/**
	* @Details		set true for disable game simulation
	* @Param		isQuiet - disable simulation mode
	*/
	void setIsQuiet(bool isQuiet) { m_isQuiet = isQuiet; }

	/**
	* @Details		set delay between attacks
	* @Param		delay - delay attacks value
	*/
	void setDelay(int delay) { m_delay = delay; }
	
	/**
	* @Details		revieves player index and returns player's board as matrix of chars
	* @Param		playerId - player's index
	*/
	char** Board::toCharMat(PlayerIndex playerId) const;

	/**
	* @Details		build board by given input (matrix of chars)
	* @Param		initBoard - input board matrix 
	*/
	void buildBoard(char ** initBoard);
	
	/**
	* @Details		receive cell's coords and returns sign of requested cell
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	*/
	char getSign(int r, int c);

	/**
	* @Details		receive cell's coords and clear the cell (status=FREE and sign=SPACE)
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	*/
	inline void clearCell(int r, int c)
	{
		m_boardData[r][c].clear();
	}
};
