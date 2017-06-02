// Board.cpp

#include <windows.h> 
#include <iostream>
#include <conio.h>
#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"

#define INC_ROW(DIR, ROW, OFFSET) (((ShipDirection::VERTICAL) == (DIR)) ? ((ROW) + (OFFSET)) : (ROW))
#define INC_COL(DIR, COL, OFFSET) (((ShipDirection::HORIZONTAL) == (DIR)) ? ((COL) + (OFFSET)) : (COL))
#define INC_DEPTH(DIR, DEP, OFFSET) (((ShipDirection::DEPTH) == (DIR)) ? ((DEP) + (OFFSET)) : (DEP))


void Board::buildBoard(const BoardData& initBoard)
{
	int initDepthSize = initBoard.depth() + BOARD_PADDING;
	int initRowSize = initBoard.rows() + BOARD_PADDING;
	int initColSize = initBoard.cols() + BOARD_PADDING;

	vector<vector<vector<char>>> copyBoard;
	for (int d = 0; d < initDepthSize; d++)
	{
		vector<vector<char>> depth;
		copyBoard.push_back(depth);
		for (int i = 0; i < initRowSize; i++)
		{
			vector<char> col;
			copyBoard[d].push_back(col);
			for (int j = 0; j < initColSize; j++)
			{
				copyBoard[d][i].push_back(initBoard.charAt(Coordinate(i,j,d)));
			}
		}
	}

	buildBoard(copyBoard);
}


void Board::buildBoard(const vector<vector<vector<Cell>>>& initBoard)
{

	size_t initDepthSize	= initBoard.size();
	size_t initRowSize		= initBoard[0].size();
	size_t initColSize		= initBoard[0][0].size();

	vector<vector<vector<char>>> copyBoard;
	for (int d = 0; d < initDepthSize; d++)
	{
		vector<vector<char>> depth;
		copyBoard.push_back(depth);
		for (int i = 0; i < initRowSize; i++)
		{
			vector<char> col;
			copyBoard[d].push_back(col);
			for (int j = 0; j < initColSize; j++)
			{
				copyBoard[d][i].push_back(initBoard[d][i][j].getSign());
			}
		}
	}

	buildBoard(copyBoard);
}

void Board::buildBoard(const vector<vector<vector<char>>>& initBoard)
{
	m_depth = static_cast<int>(initBoard.size());
	m_rows = static_cast<int>(initBoard[0].size());
	m_cols = static_cast<int>(initBoard[0][0].size());

	int initDepthSize = m_depth + BOARD_PADDING;
	int initRowSize = m_rows + BOARD_PADDING;
	int initColSize = m_cols + BOARD_PADDING;
	// for each depth:
	//		create matrix
	for (int d = 0; d, initDepthSize; d++)
	{
		vector<vector<shared_ptr<Cell>>> depth;
		m_boardData.push_back(depth);
		for (int i = 0; i<initRowSize; i++)
		{
			vector<shared_ptr<Cell>> col;
			m_boardData[d].push_back(col);
			for (int j = 0; j<initColSize; j++)
			{
				m_boardData[d][i].push_back(make_shared<Cell>());
			}
		}
	}

	for (int d = 0; d < initDepthSize; d++)
	{
		for (int i = 0; i < initRowSize; i++)
		{
			for (int j = 0; j < initColSize; j++)
			{
				m_boardData[d][i][j]->setStatus(Cell::FREE);
				m_boardData[d][i][j]->setIndexes(d, i, j);
			}
		}
	}

	// scan from top left to right and bottom
	// if my upper and left cells are empty I'm new ship
	for (int d = 1; d <= m_depth; d++)
	{
		for (int i = 1; i <= m_rows; i++)
		{
			for (int j = 1; j <= m_cols; j++)
			{

				// check if the is the start of ship
				if (SPACE != initBoard[d][i][j] && 
					SPACE == initBoard[d][i - 1][j] &&
					SPACE == initBoard[d][i][j - 1] &&
					SPACE == initBoard[d - 1][i][j])
				{
					// create the ship
					shared_ptr<Ship> ship = ShipFactory::instance().create(initBoard[d][i][j]);
					m_shipsOnBoard.push_back(ship);
					// init ship in relevant cells and cells in the ship
					int shipLen = Utils::getShipLen(initBoard[d][i][j]);


					// get ship direction 
					ShipDirection dir;
					if (initBoard[d][i][j] == initBoard[d][i + 1][j])
						dir = ShipDirection::VERTICAL;
					else if (initBoard[d][i][j] == initBoard[d][i][j + 1])
						dir = ShipDirection::HORIZONTAL;
					else /* initBoard[d][i][j] == initBoard[d + 1][i][j ] */
						dir = ShipDirection::DEPTH;

					for (int k = 0; k < shipLen; k++)
					{
						int depth = INC_DEPTH(dir, d, k);
						int row = INC_ROW(dir, i, k);
						int col = INC_COL(dir, j, k);
						m_boardData[depth][row][col]->setShip(ship);
						m_boardData[depth][row][col]->setStatus(Cell::ALIVE);
						ship->addCell(m_boardData[depth][row][col]);
					}
				}
			}
		}
	}
}


void Board::splitToPlayersBoards(Board& boardA, Board& boardB) const
{
	int initDepthSize = depth();
	int initRowSize = rows();
	int initColSize = cols();

	vector<vector<vector<char>>> copyBoardA;
	vector<vector<vector<char>>> copyBoardB;
	for (int d = 0; d < initDepthSize; d++)
	{
		vector<vector<char>> depth;
		copyBoardA.push_back(depth);
		copyBoardB.push_back(depth);
		for (int r = 0; r < initRowSize; r++)
		{
			vector<char> col;
			copyBoardA[d].push_back(col);
			copyBoardB[d].push_back(col);
			for (int c = 0; c < initColSize; c++)
			{
				Coordinate coord(d, r, c);
				char sign = this->charAt(coord);

				PlayerIndex pi = Utils::getPlayerIdByShip(sign);
				switch (pi)
				{
				case PLAYER_A:
				{
					copyBoardA[d][r][c] = sign;
					copyBoardB[d][r][c] = SPACE;
				} break;

				case PLAYER_B:
				{
					copyBoardA[d][r][c] = SPACE;
					copyBoardB[d][r][c] = sign;
				} break;

				case MAX_PLAYER:
				default:
				{
					copyBoardA[d][r][c] = SPACE;
					copyBoardB[d][r][c] = SPACE;
				}
				} // Switch
			}
		}
	}

	boardA.buildBoard(copyBoardA);
	boardB.buildBoard(copyBoardB);
}

char Board::getSign(int d, int r, int c) const
{
	return get(d, r, c)->getSign();
}

void Board::addDummyNewShipToBoard(const vector<shared_ptr<Cell>>& shipCells)
{
	shared_ptr<Ship> ship = ShipFactory::instance().createDummyShipByCellsVector(shipCells);

	m_shipsOnBoard.push_back(ship);
	// init ship in relevant cells and cells in the ship

	for(auto it = shipCells.begin(); it != shipCells.end(); ++it)
	{
		(*it)->setShip(ship);
		(*it)->setStatus(Cell::DEAD);
	}

	ship->addCells(shipCells);
}

vector<int>& Board::shipsPerPlayer()
{
	return m_numOfShipsPerPlayer;
}


Board::~Board()
{ }

void Board::printBoard() const
{
	Utils::ShowConsoleCursor(false);
	if (true == m_isQuiet)
	{
		return;
	}
	int initDepthSize = m_depth + BOARD_PADDING;
	int initRowSize = m_rows + BOARD_PADDING;
	int initColSize = m_cols + BOARD_PADDING;
	for (int d = 0; d < initDepthSize; ++d)
	{
		for (int i = 0; i < initRowSize; ++i)
		{
			for (int j = 0; j < initColSize; ++j)
			{
				Utils::gotoxy(i, j * 3);
				if (SPACE == m_boardData[d][i][j]->getSign())
				{
					Utils::setTextColor(BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN);
					if (0 == i && j >= 1 && j <= m_cols)
					{
						if (m_cols == j)
							cout << j << " ";
						else
							cout << j << "  ";
					}
					else if (0 == j && i >= 1 && i <= m_cols)
					{
						if (m_rows == i)
							cout << i << " ";
						else
							cout << i << "  ";
					}
					else
						cout << "   ";
				}
				else
				{
					Utils::setTextColor(m_boardData[d][i][j]->getShip()->getColor());
					cout << m_boardData[d][i][j]->getSign() << "  ";
				}
			}
			cout << endl;
		}
		cout << endl << endl;
	}
	
}

void Board::printHist()
{
	static int k = 1;
	if (k++ % 2 == 0)
		Utils::gotoxy(13,0);
	else
		Utils::gotoxy(27, 0);
	for (int d = 0; d < m_depth; ++d)
	{
		for (int i = 1; i <= m_rows; ++i)
		{
			for (int j = 1; j <= m_cols; ++j)
			{

				shared_ptr<Cell> c = get(d, i, j);
				int h = c->getHistValue();

				printf("%2d ", h);
			}
			cout << endl;
		}
		cout << endl << endl;
	}
}

void Board::printAttack(int player, int i, int j, int d, AttackResult attackResult) const
{
	if (true == m_isQuiet)
	{
		return;
	}
	Utils::ShowConsoleCursor(false);
	// animation of attack
	for (int k = 0; k < 3; k++)
	{
		Utils::gotoxy(i, j * 3);
		if (player == PLAYER_A)
			cout << "@  ";
		else
			cout << "&  ";
		Sleep(100);
		Utils::gotoxy(i, j * 3);
		cout << m_boardData[d][i][j]->getSign();
		Sleep(100);
	}
	switch (attackResult)
	{
	case (AttackResult::Hit):
	case (AttackResult::Sink):
		Utils::gotoxy(i, j * 3);
		if (player == PLAYER_A)
			cout << "*  ";
		else
			cout << "#  ";
		break;
	case (AttackResult::Miss):
		Utils::gotoxy(i, j * 3);
		cout << SPACE;
		break;
	}
	Sleep(m_delay);
}
