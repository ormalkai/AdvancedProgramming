

#include <iostream>
#include <fstream>
#include <string>
#include "Game.h"
#include "Utils.h"
#include "Debug.h"

#define INIT_BOARD_ROW_SIZE (BOARD_ROW_SIZE + 2)
#define INIT_BOARD_COL_SIZE (BOARD_COL_SIZE + 2)
#define SHIPS_PER_PLAYER (5)

using namespace std;

bool Game::checkWrongSizeOrShape() const
{
	bool result = true;
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		char player = Utils::instance().getPlayerCharByIndex(i);
		for (int j = 0; j < ShipType::MAX_SHIP; j++)
		{
			if (true == m_wrongSizeOrShapePerPlayer[i][j])
			{
				char ship = Utils::instance().getShipByIndexAndPlayer(j, i);
				cout << "Wrong size or shape for ship " << ship << " for player " << player << endl;
				result = false;
			}
		}
	}
	return result;
}

bool Game::checkNumberOfShips() const
{
	bool result = true;
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		char player = Utils::instance().getPlayerCharByIndex(i);
		if (SHIPS_PER_PLAYER < m_numOfShipsPerPlayer[i])
		{
			cout << "Too many ships for player " << player << endl;
			result = false;
		}
		else if (SHIPS_PER_PLAYER > m_numOfShipsPerPlayer[i])
		{
			cout << "Too few ships for player " << player << endl;
			result = false;
		}
	}
	return result;
}

bool Game::checkAdjacentShips() const
{
	bool result = true;
	if (true == m_foundAdjacentShips)
	{
		cout << "Adjacent Ships on Board" << endl;
		result = false;
	}
	return result;
}

bool Game::checkErrors() const
{
	bool result = true;
	// Wrong size or shape for ship errors
	result = result && checkWrongSizeOrShape();
	// Too many/few ships for player errors
	result = result && checkNumberOfShips();
	// Adjacent Ships on board error
	result = result && checkAdjacentShips();
	return result;
}

void Game::validateBoard(char** initBoard)
{
	// for each char
	for (int i = 1; i <= BOARD_ROW_SIZE; ++i)
	{
		for (int j = 1; j <= BOARD_COL_SIZE; ++j)
		{
			if (SPACE == initBoard[i][j])
			{
				// ignore spaces
				continue;
			}
			char currentShip = initBoard[i][j];
			int horizontalShipLen = getShipLength((char**)initBoard, currentShip, i, j, ShipLengthDirection::HORIZONTAL);
			int verticalShipLen = getShipLength((char**)initBoard, currentShip, i, j, ShipLengthDirection::VERTICAL);
			int expectedLen = m_shipToExpectedLen[currentShip];

			PlayerIndex playerIndex = Utils::instance().getPlayerIdByShip(currentShip);
			int shipIndex = Utils::instance().getIndexByShip(currentShip);
			if ((expectedLen != horizontalShipLen && 1 != verticalShipLen) ||
				(expectedLen != verticalShipLen && 1 != horizontalShipLen))
			{
				// ERROR - wrong size or shape for currentShip(player too)
				m_wrongSizeOrShapePerPlayer[playerIndex][shipIndex] = true;
			}
			else
			{
				// count legal ship
				m_numOfShipsPerPlayer[playerIndex]++;
			}

			//check Adjacency
			if (false == Game::isAdjacencyValid((char**)initBoard, i, j))
			{
				// ERROR Adjacency
				m_foundAdjacentShips = true;
			}
		}
	}
}

void Game::readSBoardFile(std::string filePath, char** initBoard)
{
	ifstream sboard(filePath);
	string line;
	int rowIndex = 1;
	// Line by line up to 10 line and up to 10 chars per line
	while (getline(sboard, line) && rowIndex <= BOARD_ROW_SIZE)
	{
		int colIndex = 1;
		for (std::string::size_type i = 0; i < line.size() && colIndex <= 10; i++)
		{
			char c = line[i];
			// If not allowed chars skip, otherwise insert it to board
			if (PlayerIndex::MAX_PLAYER != Utils::instance().getPlayerIdByShip(c))
			{
				initBoard[colIndex][rowIndex] = c;
			}
			// parse nex char
			colIndex++;
		}
		// parse next line
		rowIndex++;
	}
	// close sboard file
	sboard.close();
}

/**
 * @Details		parses sboard file and initializes the game board
 * @Param		filepath		- path to sboard file
 * @Param		rows			- number of rows
 * @param		cols			- number of columns
 * @Return		ReturnCode
 */
ReturnCode Game::parseBoardFile(std::string filePath)
{
	// Init board is larger by 1 from actual board in every dimension for 
	// traversing within the board more easily
	char initBoard[INIT_BOARD_ROW_SIZE][INIT_BOARD_COL_SIZE];
	
	// Init board file with spaces
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			initBoard[i][j] = SPACE;
		}
	}
	// Read from file 
	readSBoardFile(filePath, (char**)initBoard);
	// input validation
	validateBoard((char**)initBoard);
	// Print errors and return ERROR if needed
	ReturnCode result = RC_SUCCESS;
	if (false == checkErrors())
	{
		result = RC_ERROR;
	}
	return result;
}

/**
 * @Details		Receives init board and specific cell,
 *				verifies that there no overlapped ships
 * @Param		initBoard		- board contains only legal ships characters and spaces
 * @Param		i				- row of the cell to test
 * @Param		j				- column of the cell to test
 */
bool Game::isAdjacencyValid(char** initBoard, int i/*row*/, int j/*col*/)
{
	char expectedShip = initBoard[i][j];
	if (SPACE == expectedShip)
	{
		return true;
	}
	// neighbors can be like me or spaces only!
	if ((SPACE != initBoard[i + 1][j] && expectedShip != initBoard[i + 1][j]) ||
		(SPACE != initBoard[i - 1][j] && expectedShip != initBoard[i - 1][j]) ||
		(SPACE != initBoard[i][j + 1] && expectedShip != initBoard[i][j + 1]) ||
		(SPACE != initBoard[i][j - 1] && expectedShip != initBoard[i][j - 1]))
	{
		return false;
	}

	return true;
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
int Game::getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthDirection direction)
{
	// stop condition
	if (expectedShip != initBoard[i][j])
	{
		return 0;
	}

	if (ShipLengthDirection::HORIZONTAL == direction)
	{
		return (1 + Game::getShipLength(initBoard, expectedShip, i, j - 1, direction) +
			Game::getShipLength(initBoard, expectedShip, i, j + 1, direction));
	}
	else /*GetShipLengthDirection::VERTICAL == direction*/
	{
		return (1 + Game::getShipLength(initBoard, expectedShip, i - 1, j, direction) +
			Game::getShipLength(initBoard, expectedShip, i + 1, j, direction));
	}
}

ReturnCode Game::initListPlayers()
{
	return ReturnCode::RC_SUCCESS;
}

void Game::initErrorDataStructures()
{
	vector<bool> wrongSizeOrShapePerPlayer;
	for (int j = 0; j < ShipType::MAX_SHIP; j++)
	{
		wrongSizeOrShapePerPlayer.push_back(false);
	}
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		m_numOfShipsPerPlayer.push_back(0);
		m_wrongSizeOrShapePerPlayer.push_back(wrongSizeOrShapePerPlayer);
	}
	m_foundAdjacentShips = false;
}

void Game::initExpectedShipLenMap()
{
	m_shipToExpectedLen[PLAYER_A_RUBBER_SHIP] = m_shipToExpectedLen[PLAYER_B_RUBBER_SHIP] = 1;
	m_shipToExpectedLen[PLAYER_A_ROCKET_SHIP] = m_shipToExpectedLen[PLAYER_B_ROCKET_SHIP] = 2;
	m_shipToExpectedLen[PLAYER_A_SUBMARINE] = m_shipToExpectedLen[PLAYER_B_SUBMARINE] = 3;
	m_shipToExpectedLen[PLAYER_A_DESTROYER] = m_shipToExpectedLen[PLAYER_B_DESTROYER] = 4;
}

ReturnCode Game::init(std::string boardPathFile)
{
	// init ERRORs data structures
	initErrorDataStructures();
	// init expected len map
	initExpectedShipLenMap();

	ReturnCode rc = parseBoardFile(boardPathFile);
	if (RC_SUCCESS != rc)
	{
		DBG(Debug::Error, "Failed parsing board file rc[%d]", rc);
		return rc;
	}

	// Create players and boards
	for (size_t i = 0; i < NUM_OF_PLAYERS; i++)
	{
		m_players[i] = PlayerAlgoFactory::instance().create(AlgoType::FILE);
		m_boards[m_players[i]] = 
	
	}
	
	// initListPlayers + init foreach player
	// Set board for Players
	//

	return ReturnCode::RC_SUCCESS;
}

ReturnCode Game::fillBoardOfPlayer(vector<char> playerChars, Board& board)
{
	for (size_t i = 0; i < m_rows; i++)
	{
		for (size_t i = 0; i < m_cols; i++)
		{
			char c = m_board.getSign(i, j);
			if (SPACE != c && Utils::instance().isExistInVec(playerChars, c))
			{
				m_board.getSign(i, j) 
			}
		}
	}
	
}

ReturnCode Game::startGame()
{
	vector<IBattleshipGameAlgo*>::iterator iter = m_players.begin();
	pair<int, int> attackReq;
	IBattleshipGameAlgo* currentPlayer;

	while (true)
	{
		currentPlayer = (*iter);
		attackReq = currentPlayer->attack();
		// Check attack status (hit,miss)
		// Notify for all players
		// If game over break

		// If attack failed - iter.next
		// If iter == last do iter = begin
	}

	// Notify winner

	return ReturnCode::RC_SUCCESS;
}
