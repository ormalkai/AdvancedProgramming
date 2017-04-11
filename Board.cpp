// Board.cpp

#include <windows.h> 
#include <iostream>
#include <conio.h>
#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"

#define INC_ROW(IS_VERTICAL, ROW, OFFSET) (((true) == (IS_VERTICAL)) ? ((ROW) + (OFFSET)) : (ROW))
#define INC_COL(IS_VERTICAL, COL, OFFSET) (((false) == (IS_VERTICAL)) ? ((COL) + (OFFSET)) : (COL))


void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setTextColor(_In_ WORD color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	SetConsoleTextAttribute(hConsole, color);
}

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
				m_shipsOnBoard.push_back(ship);
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

Board::~Board()
{
	for (std::vector<Ship*>::iterator it = m_shipsOnBoard.begin(); it != m_shipsOnBoard.end(); ++it)
	{
		delete (*it);
	}
}

void Board::printBoard()
{
	if (true == m_isQuiet)
	{
		return;
	}
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			gotoxy(i, j);
			if (SPACE == m_boardData[i][j].getSign())
			{
				setTextColor(BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN);
				cout << " ";
			}
			else
			{
				setTextColor(m_boardData[i][j].getShip()->getColor());
				cout << m_boardData[i][j].getSign();
			}
		}
		cout << endl;
	}
}

void Board::printAttack(int i, int j, AttackResult attackResult)
{
	if (true == m_isQuiet)
	{
		return;
	}

	for (int k = 0; k < 5; k++)
	{
		gotoxy(i, j);
		cout << "@";
		Sleep(100);
		gotoxy(i, j);
		cout << m_boardData[i][j].getSign();
		Sleep(100);
	}
	switch (attackResult)
	{
	case (AttackResult::Hit):
	case (AttackResult::Sink):
		gotoxy(i, j);
		cout << "*";
		break;
	case (AttackResult::Miss):
		gotoxy(i, j);
		cout << SPACE;
		break;
	}
	Sleep(m_delay);
}
