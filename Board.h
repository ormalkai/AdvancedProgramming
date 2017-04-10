#pragma once

#include "Cell.h"
#include <vector>

using std::vector;

class Board
{
private:
	vector<vector<Cell>> m_boardData;
	int m_rows;
	int m_cols;

public:
	Board(int rows = BOARD_ROW_SIZE, int cols = BOARD_COL_SIZE) : m_rows(rows), m_cols(cols) {};
	~Board() = default;

	Cell get(pair<int, int> i);
	char getSign(int r, int c);
	inline void clearCell(int r, int c)
	{
		m_boardData[r][c].clear();
	}
};
