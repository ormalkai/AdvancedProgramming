// Board.cpp

#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"

void Board::buildBoard(char ** initBoard)
{
	// scan from top left to right and bottom
	// if my upper and left cells are empty I'm new ship
	for (int i = 1; i <= BOARD_ROW_SIZE; i++)
	{
		for (int j = 1; j <= BOARD_COL_SIZE; j++)
		{
			if (SPACE == initBoard[i - 1][j] && SPACE == initBoard[i][j - 1])
			{
				Ship* ship = ShipFactory::instance()->create(i, j, initBoard);
			}
		}
	}


}



char** Board::toCharMat()
{
	char** ret = new char*[m_rows];

	for (int i = 0; i < m_rows; i++)
	{
		ret[i] = new char[m_cols];

		for (int j = 0; j < m_cols; j++)
		{
			ret[i][j] = m_boardData[i][j].getSign();
		}
	}

	return ret;
}


char** Board::toCharMat(PlayerIndex playerId)
{
	char** ret = new char*[m_rows];

	for (int i = 0; i < m_rows; i++)
	{
		ret[i] = new char[m_cols];

		for (int j = 0; j < m_cols; j++)
		{
			char c = m_boardData[i][j].getSign();
			if (SPACE != c && (Utils::instance().getPlayerIdByShip(c) != playerId))
				c = SPACE;

			ret[i][j] = c;
		}
	}

	return ret;
}

AttackResult Board::executeAttack(int r, int c)
{
	//if ()
	return  AttackResult::Hit;
}


Cell Board::get(pair<int, int> i)
{
	return get(i.first, i.second);
}

char Board::getSign(int r, int c)
{
	return get(r, c).getSign();
}