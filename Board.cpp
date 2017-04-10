// Board.cpp

#include "Board.h"


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