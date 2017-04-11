// Board.cpp

#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"

#define INC_ROW(IS_VERTICAL, ROW, OFFSET) (((true) == (IS_VERTICAL)) ? ((ROW) + (OFFSET)) : (ROW))
#define INC_COL(IS_VERTICAL, COL, OFFSET) (((false) == (IS_VERTICAL)) ? ((COL) + (OFFSET)) : (COL))

void Board::buildBoard(char ** initBoard)
{
	// scan from top left to right and bottom
	// if my upper and left cells are empty I'm new ship
	for (int i = 1; i <= BOARD_ROW_SIZE; i++)
	{
		for (int j = 1; j <= BOARD_COL_SIZE; j++)
		{
			// initalize index of Cell
			m_boardData[i][j].setIndex(i, j);
			// check if the is the start of ship
			if (SPACE != initBoard[i][j] && SPACE == initBoard[i - 1][j] && SPACE == initBoard[i][j - 1])
			{
				// create the ship
				Ship* ship = ShipFactory::instance()->create(i, j, initBoard);

				// init ship in relevant cells and cells in the ship
				int shipLen = Utils::instance().getShipLen(initBoard[i][j]);
				bool isVertical = initBoard[i][j] == initBoard[i + 1][j];
				for (int k = 0; k < shipLen; k++)
				{
					int row = INC_ROW(isVertical, i, k);
					int col = INC_COL(isVertical, j, k);
					m_boardData[row][col].setShip(ship);
					m_boardData[row][col].setStatus(Cell::ALIVE);
					ship->addCell(&(m_boardData[row][col]));
				}
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

char Board::getSign(int r, int c)
{
	return get(r, c).getSign();
}

void Board::dump() const
{
	printf("Dumping Board\n");
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			if (SPACE == m_boardData[i][j].getSign())
			{
				printf("@");
			}
			else
			{
				printf("%c", m_boardData[i][j].getSign());
			}
		}
		printf("\n");
	}
}
