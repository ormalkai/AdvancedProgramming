// Board.cpp

#include <windows.h> 
#include <iostream>
#include <conio.h>
#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"

#define INC_ROW(IS_VERTICAL, ROW, OFFSET) (((true) == (IS_VERTICAL)) ? ((ROW) + (OFFSET)) : (ROW))
#define INC_COL(IS_VERTICAL, COL, OFFSET) (((false) == (IS_VERTICAL)) ? ((COL) + (OFFSET)) : (COL))


void Board::buildBoard(const char ** initBoard, int numRows, int numCols)
{
	m_rows = numRows;
	m_cols = numCols;

	// first allocate board TODO nullptr in constructor
	int initRowSize = m_rows + BOARD_PADDING;
	int initColSize = m_cols + BOARD_PADDING;
	m_boardData = new Cell*[initRowSize];
	for (int i = 0; i < initRowSize; ++i)
	{
		m_boardData[i] = new Cell[initColSize];
	}

	for (int i = 0; i < initRowSize; i++)
	{
		for (int j = 0; j < initColSize; j++)
		{
			m_boardData[i][j].setStatus(Cell::FREE);
			m_boardData[i][j].setIndexes(i, j);
		}
	}

	// scan from top left to right and bottom
	// if my upper and left cells are empty I'm new ship
	for (int i = 1; i <= m_rows; i++)
	{
		for (int j = 1; j <= m_cols; j++)
		{
			
			// check if the is the start of ship
			if (SPACE != initBoard[i][j] && SPACE == initBoard[i - 1][j] && SPACE == initBoard[i][j - 1])
			{
				// create the ship
				Ship* ship = ShipFactory::instance().create(i, j, initBoard);
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
			char c = m_boardData[i+1][j+1].getSign();
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

void Board::addDummyNewShipToBoard(vector<Cell*> shipCells)
{
	Ship* ship = ShipFactory::instance().createDummyShipByCellsVector(shipCells);

	m_shipsOnBoard.push_back(ship);
	// init ship in relevant cells and cells in the ship

	for(auto it = shipCells.begin(); it != shipCells.end(); ++it)
	{
		(*it)->setShip(ship);
		(*it)->setStatus(Cell::DEAD);
	}

	ship->addCells(shipCells);
}

Board::~Board()
{
	for (vector<Ship*>::iterator it = m_shipsOnBoard.begin(); it != m_shipsOnBoard.end(); ++it)
	{
		delete (*it);
	}

	if (nullptr != m_boardData)
	{
		int initRowSize = m_rows + BOARD_PADDING;
		for (int i = 0; i < initRowSize; ++i)
		{
			delete[] m_boardData[i];
		}
		delete[] m_boardData;
	}
}

void Board::printBoard() const
{
	if (true == m_isQuiet)
	{
		return;
	}
	int initRowSize = m_rows + BOARD_PADDING;
	int initColSize = m_cols + BOARD_PADDING;
	for (int i = 0; i < initRowSize; ++i)
	{
		for (int j = 0; j < initColSize; ++j)
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

void Board::printHist()
{
	static int k = 1;
	if (k++ % 2 == 0)
		Utils::gotoxy(13,0);
	else
		Utils::gotoxy(27, 0);

	for (int i = 1; i <= m_rows; ++i)
	{
		for (int j = 1; j <= m_cols; ++j)
		{

			Cell* c = getCellPointer(i, j);
			int h = c->getHistValue();
			
			printf("%2d ", h);
		}
		cout << endl;
	}
}

void Board::printAttack(int player, int i, int j, AttackResult attackResult) const
{
	if (true == m_isQuiet)
	{
		return;
	}
	// animation of attack
	for (int k = 0; k < 3; k++)
	{
		Utils::gotoxy(i, j);
		// TODO: Replace to cout << "@";
		if (player == PLAYER_A)
			cout << "@";
		else
			cout << "&";
		Sleep(100);
		Utils::gotoxy(i, j);
		cout << m_boardData[i][j].getSign();
		Sleep(100);
	}
	switch (attackResult)
	{
	case (AttackResult::Hit):
	case (AttackResult::Sink):
		Utils::gotoxy(i, j);
		// TODO: Replace to cout << "*";
		if (player == PLAYER_A)
			cout << "*";
		else
			cout << "#";
		break;
	case (AttackResult::Miss):
		Utils::gotoxy(i, j);
		cout << SPACE;
		break;
	}
	Sleep(m_delay);
}
