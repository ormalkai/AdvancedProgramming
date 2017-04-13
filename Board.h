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

	Cell& get(int r, int c) { return  m_boardData[r][c]; }
	Cell& get(pair<int, int> i) { return get(i.first, i.second); }


	int rows() const { return m_rows; }
	int cols() const { return m_cols; }
	void printBoard() const;
	void printAttack(int i, int j, AttackResult attackResult) const;
	void setIsQuiet(bool isQuiet) { m_isQuiet = isQuiet; }
	void setDelay(int delay) { m_delay = delay; }

	void put(int r, int c);

	char** Board::toCharMat() const;
	char** Board::toCharMat(PlayerIndex playerId) const;
	void buildBoard(char ** initBoard);
	char getSign(int r, int c);
	inline void clearCell(int r, int c)
	{
		m_boardData[r][c].clear();
	}
};
