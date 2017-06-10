#include <iostream>
#include <fstream>
#include <regex>
#include "BoardBuilder.h"
#include "Debug.h"

#define SHIPS_PER_PLAYER (5)

BoardBuilder::BoardBuilder(const string sboardPath) : 
			m_cols(-1), 
			m_rows(-1),
			m_depth(-1),
			m_foundAdjacentShips(false), 
			m_wrongSizeOrShapePerPlayer(false), 
			m_sboardFilePath(sboardPath)
{
}


BoardBuilder::~BoardBuilder()
{
}


void BoardBuilder::initErrorDataStructures()
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
ReturnCode BoardBuilder::parseBoardFile(vector<vector<vector<char>>>& board)
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

ReturnCode BoardBuilder::getBoardDimensionsFromFile(ifstream& sboard)
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
void BoardBuilder::readSBoardFile(ifstream& sboard)
{
	string line;
	// absorb empty line
	Utils::safeGetline(sboard, line);

	// Line by line up to 10 line and up to 10 chars per line
	int depthIndex = 1;
	for (int l=0; l<m_depth; l++)
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

void BoardBuilder::validateBoard()
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
				if (((expectedLen != horizontalShipLen) || (expectedLen == horizontalShipLen && 1 != verticalShipLen && 1 != depthShipLen)) &&
					((expectedLen != verticalShipLen) || (expectedLen == verticalShipLen && 1 != horizontalShipLen && 1 != depthShipLen)) &&
					((expectedLen != depthShipLen) || (expectedLen == depthShipLen && 1 != horizontalShipLen && 1 != verticalShipLen)))
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
bool BoardBuilder::isAdjacencyValid(int d/*depth*/, int i/*row*/, int j/*col*/) const
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

int BoardBuilder::getShipLengthHorizontal(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const
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

int BoardBuilder::getShipLengthVertical(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const
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

int BoardBuilder::getShipLengthDepth(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const
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
int BoardBuilder::getShipLength(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipDirection direction)
{
	// sanity
	if (expectedShip != m_initBoard[d][i][j])
	{
		return 0;
	}

	if (ShipDirection::HORIZONTAL == direction)
	{
		return (1 + getShipLengthHorizontal(expectedShip,d , i, j - 1, ShipLengthSecondDirection::BACKWORD) +
				getShipLengthHorizontal(expectedShip, d, i, j + 1, ShipLengthSecondDirection::FORWARD));


	}
	else if(ShipDirection::VERTICAL == direction)
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

bool BoardBuilder::checkWrongSizeOrShape() const
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
				cout << "Wrong size or shape for ship " << ship << " for player " << player << endl;
				result = false;
			}
		}
	}
	return result;
}

void BoardBuilder::checkIntegrityOfShips() const
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
	for (int i=0; i < m_numOfShipsPerPlayer[PLAYER_A]; i++)
	{
		if (shipsPlayerACopy[i] != shipsPlayerBCopy[i])
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

bool BoardBuilder::checkAdjacentShips() const
{
	bool result = true;
	if (true == m_foundAdjacentShips)
	{
		cout << "Adjacent Ships on Board" << endl;
		result = false;
	}
	return result;
}

bool BoardBuilder::checkErrors() const
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

void BoardBuilder::initInitBoard()
{
	// for each depth:
	//		create matrix
	for (int i=0; i<m_depth + BOARD_PADDING; i++)
	{
		vector<vector<char>> depth;
		m_initBoard.push_back(depth);
		for (int j=0; j<m_rows + BOARD_PADDING; j++)
		{
			vector<char> col;
			m_initBoard[i].push_back(col);
			for (int k=0; k<m_cols + BOARD_PADDING; k++)
			{
				m_initBoard[i][j].push_back(SPACE);
			}
		}
	}
}
