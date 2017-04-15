// Board.cpp

#include <windows.h> 
#include <iostream>
#include <conio.h>
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
			//m_boardData[i][j].setIndex(i, j);

			// check if the is the start of ship
			if (SPACE != initBoard[i][j] && SPACE == initBoard[i - 1][j] && SPACE == initBoard[i][j - 1])
			{
				// create the ship
				Ship* ship = ShipFactory::instance()->create(i, j, initBoard);
				m_shipsOnBoard.push_back(ship);
				// init ship in relevant cells and cells in the ship
				int shipLen = Utils::getShipLen(initBoard[i][j]);
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


char** Board::toCharMat(PlayerIndex playerId) const
{
	char** ret = new char*[m_rows];

	for (int i = 0; i < m_rows; i++)
	{
		ret[i] = new char[m_cols];

		for (int j = 0; j < m_cols; j++)
		{
			char c = m_boardData[i][j].getSign();
			if (SPACE != c && (Utils::getPlayerIdByShip(c) != playerId))
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

Board::~Board()
{
	for (vector<Ship*>::iterator it = m_shipsOnBoard.begin(); it != m_shipsOnBoard.end(); ++it)
	{
		delete (*it);
	}
}

void Board::printBoard() const
{
	if (true == m_isQuiet)
	{
		return;
	}
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			Utils::gotoxy(i, j);
			if (SPACE == m_boardData[i][j].getSign())
			{
				Utils::setTextColor(BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN);
				cout << " ";
			}
			else
			{
				Utils::setTextColor(m_boardData[i][j].getShip()->getColor());
				cout << m_boardData[i][j].getSign();
			}
		}
		cout << endl;
	}
}

void Board::printAttack(int i, int j, AttackResult attackResult) const
{
	if (true == m_isQuiet)
	{
		return;
	}

	//for (int k = 0; k < 3; k++)
	//{
	//	Utils::gotoxy(i, j);
	//	cout << "@";
	//	Sleep(100);
	//	Utils::gotoxy(i, j);
	//	cout << m_boardData[i][j].getSign();
	//	Sleep(100);
	//}
	switch (attackResult)
	{
	case (AttackResult::Hit):
	case (AttackResult::Sink):
		Utils::gotoxy(i, j);
		cout << "*";
		break;
	case (AttackResult::Miss):
		Utils::gotoxy(i, j);
		cout << SPACE;
		break;
	}
	Sleep(m_delay);
}
