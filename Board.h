#pragma once

#include "Cell.h"
#include <vector>
#include "IBattleshipGameAlgo.h"

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
	Board(const Board &) = default;

	Cell get(int r, int c) { return  m_boardData[r][c]; }
	Cell get(pair<int, int> i);
	void put(int r, int c);

	char** Board::toCharMat();
	char** Board::toCharMat(PlayerIndex playerId);
	char getSign(int r, int c);
	inline void clearCell(int r, int c)
	{
		m_boardData[r][c].clear();
	}

	AttackResult executeAttack(int r, int c);
};
