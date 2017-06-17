#include <windows.h> 
#include <iostream>
#include <conio.h>
#include <regex>
#include <fstream>
#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"
#include "Debug.h"

#define INC_ROW(DIR, ROW, OFFSET) (((ShipDirection::VERTICAL) == (DIR)) ? ((ROW) + (OFFSET)) : (ROW))
#define INC_COL(DIR, COL, OFFSET) (((ShipDirection::HORIZONTAL) == (DIR)) ? ((COL) + (OFFSET)) : (COL))
#define INC_DEPTH(DIR, DEP, OFFSET) (((ShipDirection::DEPTH) == (DIR)) ? ((DEP) + (OFFSET)) : (DEP))

Board::Board(const Board & other)
{
	if (&other != this)
	{
		this->buildBoard(other);
	}
}
Board& Board::operator=(const Board & other)
{
	if (&other != this)
	{
		this->buildBoard(other);
	}
	
	return *this;
}


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


void Board::buildBoard(const vector<vector<vector<char>>>& initBoard)
{
	clear();
	m_depth = _depth = static_cast<int>(initBoard.size() - BOARD_PADDING);
	m_rows = _rows = static_cast<int>(initBoard[0].size() - BOARD_PADDING);
	m_cols = _cols = static_cast<int>(initBoard[0][0].size() - BOARD_PADDING);

	// init num of ships per player
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		m_numOfShipsPerPlayer.push_back(0);
	}

	int initDepthSize = m_depth + BOARD_PADDING;
	int initRowSize = m_rows + BOARD_PADDING;
	int initColSize = m_cols + BOARD_PADDING;
	// for each depth:
	//		create matrix
	for (int d = 0; d < initDepthSize; d++)
	{
		vector<vector<shared_ptr<Cell>>> depth;
		m_boardData.push_back(depth);
		for (int i = 0; i < initRowSize; i++)
		{
			vector<shared_ptr<Cell>> col;
			m_boardData[d].push_back(col);
			for (int j = 0; j < initColSize; j++)
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
					PlayerIndex pi = Utils::getPlayerIdByShip(initBoard[d][i][j]);
					m_numOfShipsPerPlayer[pi]++;
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
	int initDepthSize = depth() + BOARD_PADDING;
	int initRowSize = rows() + BOARD_PADDING;
	int initColSize = cols() + BOARD_PADDING;

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
				Coordinate coord(r, c, d);
				char sign = this->charAt(coord);

				PlayerIndex pi = Utils::getPlayerIdByShip(sign);
				switch (pi)
				{
				case PLAYER_A:
				{
					copyBoardA[d][r].push_back(sign);
					copyBoardB[d][r].push_back(SPACE);
				} break;

				case PLAYER_B:
				{
					copyBoardA[d][r].push_back(SPACE);
					copyBoardB[d][r].push_back(sign);
				} break;

				case MAX_PLAYER:
				default:
				{
					copyBoardA[d][r].push_back(SPACE);
					copyBoardB[d][r].push_back(SPACE);
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
				Utils::gotoxy(i + (initRowSize * d) + 2, j * 3);
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

void Board::printHist() const
{
	static int k = 1;
	if (k++ % 2 == 0)
		Utils::gotoxy(13,0);
	else
		Utils::gotoxy(27, 0);
	for (int d = 1; d <= m_depth; ++d)
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
	cout << "-------------------------------------" << endl;
}

void Board::printAttack(int player, int i, int j, int d, AttackResult attackResult) const
{
	int initRowSize = m_rows + BOARD_PADDING;
	if (true == m_isQuiet)
	{
		return;
	}
	Utils::ShowConsoleCursor(false);
	// animation of attack
	for (int k = 0; k < 3; k++)
	{
		Utils::gotoxy(i + (initRowSize * d) + 2, j * 3);
		if (player == PLAYER_A)
			cout << "@  ";
		else
			cout << "&  ";
		Sleep(100);
		Utils::gotoxy(i + (initRowSize * d) + 2, j * 3);
		cout << m_boardData[d][i][j]->getSign();
		Sleep(100);
	}
	switch (attackResult)
	{
	case (AttackResult::Hit):
	case (AttackResult::Sink):
		Utils::gotoxy(i + (initRowSize * d) + 2, j * 3);
		if (player == PLAYER_A)
			cout << "*  ";
		else
			cout << "#  ";
		break;
	case (AttackResult::Miss):
		Utils::gotoxy(i + (initRowSize * d) + 2, j * 3);
		cout << SPACE;
		break;
	}
	Sleep(m_delay);
}



ReturnCode Board::loadBoardFromFile(string& boardPath)
{
	BoardBuilder b(boardPath);
	vector<vector<vector<char>>> v;
	
	ReturnCode rc = b.parseBoardFile(v);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}

	this->buildBoard(v);

	return RC_SUCCESS;
}


Board::BoardBuilder::BoardBuilder(const string sboardPath) :
	m_cols(-1),
	m_rows(-1),
	m_depth(-1),
	m_foundAdjacentShips(false),
	m_wrongSizeOrShapePerPlayer(false),
	m_sboardFilePath(sboardPath)
{
}


Board::BoardBuilder::~BoardBuilder()
{
}


void Board::BoardBuilder::initErrorDataStructures()
{
	vector<bool> wrongSizeOrShapePerPlayer;
	for (int j = 0; j < ShipType::MAX_SHIP; j++)
	{
		wrongSizeOrShapePerPlayer.push_back(false);
	}
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		m_numOfShipsPerPlayer.push_back(0);
		m_shipsPerPlayer.push_back(vector<char>());
		m_wrongSizeOrShapePerPlayer.push_back(wrongSizeOrShapePerPlayer);
	}
	m_foundAdjacentShips = false;
}

/**
* @Details		parses sboard file and initializes the game board
* @Return		ReturnCode
*/
ReturnCode Board::BoardBuilder::parseBoardFile(vector<vector<vector<char>>>& board)
{
	// Read from file 
	ifstream sboard(m_sboardFilePath);
	if (!sboard.is_open())
	{
		DBG(Debug::DBG_ERROR, "Could not open board file: %s", m_sboardFilePath);
		return RC_ERROR;
	}

	ReturnCode result = getBoardDimensionsFromFile(sboard);
	if (RC_SUCCESS != result)
	{
		// close sboard file
		sboard.close();
		return result;
	}

	initInitBoard();

	readSBoardFile(sboard);
	// close sboard file
	sboard.close();

	// init errors DS
	initErrorDataStructures();

	// input validation
	validateBoard();
	// Print errors and return ERROR if needed
	if (false == checkErrors())
	{
		return RC_ERROR;
	}
	board = m_initBoard;
	return RC_SUCCESS;
}

ReturnCode Board::BoardBuilder::getBoardDimensionsFromFile(ifstream& sboard)
{
	string line;
	Utils::safeGetline(sboard, line);
	transform(line.begin(), line.end(), line.begin(), ::tolower);
	regex regex("^([0-9]+)x([0-9]+)x([0-9]+)");
	smatch m;
	regex_match(line, m, regex);
	if (m.size() != 4)
	{
		DBG(Debug::DBG_ERROR, "Failed to parse first line[%s] from: %s", line.c_str(), m_sboardFilePath.c_str());
		return RC_ERROR;
	}
	try
	{
		m_cols = stoi(m[1]);
		m_rows = stoi(m[2]);
		m_depth = stoi(m[3]);
	}
	catch (const std::exception&)
	{
		DBG(Debug::DBG_ERROR, "Invalid values in first line [%s] from: %s", line.c_str(), m_sboardFilePath);
		return RC_ERROR;
	}

	return RC_SUCCESS;
}
void Board::BoardBuilder::readSBoardFile(ifstream& sboard)
{
	string line;
	// absorb empty line
	Utils::safeGetline(sboard, line);

	// Line by line up to 10 line and up to 10 chars per line
	int depthIndex = 1;
	for (int l = 0; l<m_depth; l++)
	{
		int rowIndex = 1;
		// read line by line until EOF, empty line or done reading rows
		while (Utils::safeGetline(sboard, line) && (line != "" && line != "\r") && rowIndex <= m_rows)
		{
			int colIndex = 1;
			for (string::size_type i = 0; i < line.size() && colIndex <= m_cols; i++)
			{
				char c = line[i];
				// If not allowed chars skip, otherwise insert it to board
				if (MAX_PLAYER != Utils::getPlayerIdByShip(c))
				{
					m_initBoard[depthIndex][rowIndex][colIndex] = c;
				}
				// parse nex char
				colIndex++;
			}
			// parse next line
			rowIndex++;
		}

		// if there are lines that are not empty but done reading current matrix, absorb these lines
		while (line != "" && line != "\r")
		{
			Utils::safeGetline(sboard, line);
		}
		depthIndex++;
	}
}

void Board::BoardBuilder::validateBoard()
{
	// for each char
	for (int d = 1; d <= m_depth; ++d)
	{
		for (int i = 1; i <= m_rows; ++i)
		{
			for (int j = 1; j <= m_cols; ++j)
			{
				char currentShip = m_initBoard[d][i][j];
				if (SPACE == currentShip)
				{
					// ignore spaces
					continue;
				}
				int horizontalShipLen = getShipLength(currentShip, d, i, j, ShipDirection::HORIZONTAL);
				int verticalShipLen = getShipLength(currentShip, d, i, j, ShipDirection::VERTICAL);
				int depthShipLen = getShipLength(currentShip, d, i, j, ShipDirection::DEPTH);
				int expectedLen = Utils::getShipLen(currentShip);

				PlayerIndex playerIndex = Utils::getPlayerIdByShip(currentShip);
				int shipIndex = Utils::getIndexByShip(currentShip);
				if (((expectedLen != horizontalShipLen) || (expectedLen == horizontalShipLen && (1 != verticalShipLen || 1 != depthShipLen))) &&
					((expectedLen != verticalShipLen) || (expectedLen == verticalShipLen && (1 != horizontalShipLen || 1 != depthShipLen))) &&
					((expectedLen != depthShipLen) || (expectedLen == depthShipLen && (1 != horizontalShipLen || 1 != verticalShipLen))))
				{
					// ERROR - wrong size or shape for currentShip(player too)
					m_wrongSizeOrShapePerPlayer[playerIndex][shipIndex] = true;
				}
				else
				{
					// we need to count every ship only once.
					// therefore any ship can be represented by its first char, direction and length
					// hence we need to check the cell left(horizontal),  the cell up(vertical) and the cell
					// in the previous depth if one of them is like me
					// that means that we have already counted that ship, otherwise this is a new ship
					if (currentShip != m_initBoard[d][i - 1][j] &&
						currentShip != m_initBoard[d][i][j - 1] &&
						currentShip != m_initBoard[d - 1][i][j])
					{
						m_numOfShipsPerPlayer[playerIndex]++;
						m_shipsPerPlayer[playerIndex].push_back(currentShip);
					}
				}


				//check Adjacency
				if (false == isAdjacencyValid(d, i, j))
				{
					// ERROR Adjacency
					m_foundAdjacentShips = true;
				}
			}
		}
	}
}

/**
* @Details		Receives init board and specific cell,
*				verifies that there no overlapped ships
* @Param		initBoard		- board contains only legal ships characters and spaces
* @Param		i				- row of the cell to test
* @Param		j				- column of the cell to test
*/
bool Board::BoardBuilder::isAdjacencyValid(int d/*depth*/, int i/*row*/, int j/*col*/) const
{
	char expectedShip = m_initBoard[d][i][j];
	if (SPACE == expectedShip)
	{
		return true;
	}
	// neighbors can be like me or spaces only!
	if ((SPACE != m_initBoard[d][i + 1][j] && expectedShip != m_initBoard[d][i + 1][j]) ||
		(SPACE != m_initBoard[d][i - 1][j] && expectedShip != m_initBoard[d][i - 1][j]) ||
		(SPACE != m_initBoard[d][i][j + 1] && expectedShip != m_initBoard[d][i][j + 1]) ||
		(SPACE != m_initBoard[d][i][j - 1] && expectedShip != m_initBoard[d][i][j - 1]) ||
		(SPACE != m_initBoard[d - 1][i][j] && expectedShip != m_initBoard[d - 1][i][j]) ||
		(SPACE != m_initBoard[d + 1][i][j] && expectedShip != m_initBoard[d + 1][i][j]))
	{
		return false;
	}

	return true;
}

int Board::BoardBuilder::getShipLengthHorizontal(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const
{
	// stop condition
	if (expectedShip != m_initBoard[d][i][j])
	{
		return 0;
	}
	if (ShipLengthSecondDirection::FORWARD == direction)
	{
		return (1 + getShipLengthHorizontal(expectedShip, d, i, j + 1, direction));
	}
	else /*GetShipLengthDirection::BACKWORD == direction*/
	{
		return (1 + getShipLengthHorizontal(expectedShip, d, i, j - 1, direction));
	}
}

int Board::BoardBuilder::getShipLengthVertical(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const
{
	// stop condition
	if (expectedShip != m_initBoard[d][i][j])
	{
		return 0;
	}
	if (ShipLengthSecondDirection::FORWARD == direction)
	{
		return (1 + getShipLengthVertical(expectedShip, d, i + 1, j, direction));
	}
	else /*GetShipLengthDirection::BACKWORD == direction*/
	{
		return (1 + getShipLengthVertical(expectedShip, d, i - 1, j, direction));
	}
}

int Board::BoardBuilder::getShipLengthDepth(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const
{
	// stop condition
	if (expectedShip != m_initBoard[d][i][j])
	{
		return 0;
	}
	if (ShipLengthSecondDirection::FORWARD == direction)
	{
		return (1 + getShipLengthDepth(expectedShip, d + 1, i, j, direction));
	}
	else /*GetShipLengthDirection::BACKWORD == direction*/
	{
		return (1 + getShipLengthDepth(expectedShip, d - 1, i, j, direction));
	}
}

/**
* @Details		This is a recursive function which calculates the largest sequence of chars
*				in specific dimmension, basically this is the length of the ship in the dimension.
* @Param		initBoard		- board contains only legal ships characters and spaces
* @Param		expectedShip	- the char of the ship to calculate (for stop condition)
* @Param		i				- row of the cell to test
* @Param		j				- column of the cell to test
* @Param		direction		- direction to calculate in current flow
*/
int Board::BoardBuilder::getShipLength(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipDirection direction)
{
	// sanity
	if (expectedShip != m_initBoard[d][i][j])
	{
		return 0;
	}

	if (ShipDirection::HORIZONTAL == direction)
	{
		return (1 + getShipLengthHorizontal(expectedShip, d, i, j - 1, ShipLengthSecondDirection::BACKWORD) +
			getShipLengthHorizontal(expectedShip, d, i, j + 1, ShipLengthSecondDirection::FORWARD));


	}
	else if (ShipDirection::VERTICAL == direction)
	{
		return (1 + getShipLengthVertical(expectedShip, d, i - 1, j, ShipLengthSecondDirection::BACKWORD) +
			getShipLengthVertical(expectedShip, d, i + 1, j, ShipLengthSecondDirection::FORWARD));
	}
	else /*ShipDirection::DEPTH == direction*/
	{
		return (1 + getShipLengthDepth(expectedShip, d - 1, i, j, ShipLengthSecondDirection::BACKWORD) +
			getShipLengthDepth(expectedShip, d + 1, i, j, ShipLengthSecondDirection::FORWARD));
	}
}

bool Board::BoardBuilder::checkWrongSizeOrShape() const
{
	bool result = true;
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		char player = Utils::getPlayerCharByIndex(i);
		for (int j = 0; j < ShipType::MAX_SHIP; j++)
		{
			if (true == m_wrongSizeOrShapePerPlayer[i][j])
			{
				char ship = Utils::getShipByIndexAndPlayer(j, i);
				DBG(Debug::DBG_WARNING, "Wrong size or shape for ship [%c] for [%c] board[%s]", ship, player, m_sboardFilePath.c_str());
				result = false;
			}
		}
	}
	return result;
}

void Board::BoardBuilder::checkIntegrityOfShips() const
{
	// check number of ships
	if (m_numOfShipsPerPlayer[PLAYER_A] != m_numOfShipsPerPlayer[PLAYER_B])
	{
		// do not set result to false, keep playing!!!
		DBG(Debug::DBG_WARNING, "Inconsistent number of ships per player - playerA[%d], playerB[%d] continue playing anyway",
			m_numOfShipsPerPlayer[PLAYER_A], m_numOfShipsPerPlayer[PLAYER_B]);
		return;
	}
	// check that ships are the same
	auto shipsPlayerACopy(m_shipsPerPlayer[PLAYER_A]);
	auto shipsPlayerBCopy(m_shipsPerPlayer[PLAYER_B]);
	sort(shipsPlayerACopy.begin(), shipsPlayerACopy.end(), less<char>());
	sort(shipsPlayerBCopy.begin(), shipsPlayerBCopy.end(), less<char>());
	for (int i = 0; i < m_numOfShipsPerPlayer[PLAYER_A]; i++)
	{
		if (tolower(shipsPlayerACopy[i]) != tolower(shipsPlayerBCopy[i]))
		{
			DBG(Debug::DBG_WARNING, "Inconsistent ships for players");
			for (int j = 0; j < m_numOfShipsPerPlayer[PLAYER_A]; j++)
			{
				DBG(Debug::DBG_WARNING, "Player A[%c] playerB[%c]", shipsPlayerACopy[j], shipsPlayerBCopy[j]);
			}
			DBG(Debug::DBG_WARNING, "continue playing anyway");
			return;
		}
	}
}

bool Board::BoardBuilder::checkAdjacentShips() const
{
	bool result = true;
	if (true == m_foundAdjacentShips)
	{
		DBG(Debug::DBG_WARNING, "Adjacent Ships on Board board [%s]", m_sboardFilePath.c_str());
		result = false;
	}
	return result;
}

bool Board::BoardBuilder::checkErrors() const
{
	bool result = true;
	// Wrong size or shape for ship errors

	result = checkWrongSizeOrShape() && result;
	// Adjacent Ships on board error
	result = checkAdjacentShips() && result;

	// number and type of ships
	checkIntegrityOfShips();
	return result;
}

void Board::BoardBuilder::initInitBoard()
{
	// for each depth:
	//		create matrix
	for (int i = 0; i<m_depth + BOARD_PADDING; i++)
	{
		vector<vector<char>> depth;
		m_initBoard.push_back(depth);
		for (int j = 0; j<m_rows + BOARD_PADDING; j++)
		{
			vector<char> col;
			m_initBoard[i].push_back(col);
			for (int k = 0; k<m_cols + BOARD_PADDING; k++)
			{
				m_initBoard[i][j].push_back(SPACE);
			}
		}
	}
}

void Board::getOtherPlayerShips(vector<int>& ships)
{
	for (auto const& ship : m_shipsOnBoard)
	{
		ships.push_back(ship->getLength());
	}
}

void Board::getShipsCoord(vector<Coordinate>& shipsCoord) const
{
	for (auto ship : m_shipsOnBoard)
	{
		for (auto cell : ship->getCellsList())
		{
			shipsCoord.push_back(move(cell->getCoord()));
		}
	}
}

void Board::clear()
{
	m_boardData.clear();	// The board is matrix of Cells
	m_shipsOnBoard.clear();	// Pointer to every ship on the board
	m_numOfShipsPerPlayer.clear();
}
