#pragma once

#include <vector>
#include "Cell.h"
#include "Utils.h"

using std::vector;

class Board
{
private:
	Cell m_boardData[INIT_BOARD_ROW_SIZE][INIT_BOARD_COL_SIZE];
	vector<Ship*> m_shipsOnBoard;
	//vector<vector<Cell>> m_boardData;
	int m_rows;
	int m_cols;

public:
	Board(int rows = BOARD_ROW_SIZE, int cols = BOARD_COL_SIZE) : m_rows(rows), m_cols(cols) {};
	~Board() = default;
	Board(const Board &);

	Cell get(int r, int c);
	Cell get(pair<int, int> i);
	char getSign(int r, int c);
	void buildBoard(char ** initBoard);
	inline void clearCell(int r, int c)
	{
		m_boardData[r][c].clear();
	}
};
