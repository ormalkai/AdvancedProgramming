

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "Game.h"
#include "Utils.h"
#include "Debug.h"
#include "Board.h"

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

	result = checkWrongSizeOrShape() && result;
	// Too many/few ships for player errors

	result = checkNumberOfShips() && result;
	// Adjacent Ships on board error

	result = checkAdjacentShips() && result;
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
			int horizontalShipLen = getShipLength(initBoard, currentShip, i, j, ShipLengthDirection::HORIZONTAL);
			int verticalShipLen = getShipLength(initBoard, currentShip, i, j, ShipLengthDirection::VERTICAL);
			int expectedLen = Utils::instance().getShipLen(currentShip);

			PlayerIndex playerIndex = Utils::instance().getPlayerIdByShip(currentShip);
			int shipIndex = Utils::instance().getIndexByShip(currentShip);
			if (((expectedLen != horizontalShipLen) || (expectedLen == horizontalShipLen && 1 != verticalShipLen)) &&
				((expectedLen != verticalShipLen)   || (expectedLen == verticalShipLen && 1 != horizontalShipLen)))
			{
				// ERROR - wrong size or shape for currentShip(player too)
				m_wrongSizeOrShapePerPlayer[playerIndex][shipIndex] = true;
			}
			else
			{
				// we need to count every ship only once.
				// therefore any ship can be represented by its first char, direction and length
				// hence we need to check the cell left and the cell up if one of them is like me
				// that means that we have already counted that ship, otherwise this is a new ship
				if (currentShip != initBoard[i-1][j] && currentShip != initBoard[i][j-1])
				{
					m_numOfShipsPerPlayer[playerIndex]++;
				}
			}
			

			//check Adjacency
			if (false == Game::isAdjacencyValid(initBoard, i, j))
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
				initBoard[rowIndex][colIndex] = c;
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

ReturnCode Game::getSboardFileNameFromDirectory(string filesPath, string& sboardFileName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	string sboardFile = filesPath + "*.sboard";
	DBG(Debug::DBG_DEBUG, "sboardFile [%s]", sboardFile.c_str());
	hFind = FindFirstFile(sboardFile.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		cout << "Missing board file (*.sboard) looking in path: " << filesPath << endl;
		return RC_ERROR;
	}
	else
	{
		DBG(Debug::DBG_INFO, "The first file found is %s\n", FindFileData.cFileName);
		sboardFileName = filesPath + FindFileData.cFileName;
		FindClose(hFind);
		return RC_SUCCESS;
	}
}

ReturnCode Game::getattackFilesNameFromDirectory(string filesPath, vector<string>& attackFilePerPlayer)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	ReturnCode rc = RC_SUCCESS;
	for (int i = PLAYER_A; i < PlayerIndex::MAX_PLAYER; i++)
	{
		string attackFileExtensionPerPlayer = Utils::instance().getAttackFileByPlayer(i);
		if (NO_ATTACK_FILE == attackFileExtensionPerPlayer)
		{
			attackFilePerPlayer.push_back(NO_ATTACK_FILE);
			continue;
		}
		string attackFile = filesPath + attackFileExtensionPerPlayer;
		hFind = FindFirstFile(attackFile.c_str(), &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			cout << "Missing attack file for player " << Utils::instance().getPlayerCharByIndex(i) << " looking in path: " << filesPath << endl;
			rc = RC_ERROR;
		}
		else
		{
			DBG(Debug::DBG_INFO, "The first file found is %s\n", FindFileData.cFileName);
			attackFilePerPlayer.push_back(filesPath + FindFileData.cFileName);
			FindClose(hFind);
		}
	}
	return rc;
}

/**
 * @Details		parses sboard file and initializes the game board
 * @Param		filepath		- path to sboard file
 * @Param		rows			- number of rows
 * @param		cols			- number of columns
 * @Return		ReturnCode
 */
ReturnCode Game::parseBoardFile(string sboardFileName, char** initBoard)
{
	// Init board file with spaces
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			initBoard[i][j] = SPACE;
		}
	}
	// Read from file 
	readSBoardFile(sboardFileName, initBoard);

	/**
	 *DEBUG
	 */
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			if (SPACE == initBoard[i][j])
			{
				printf("@");
			}
			else
			{
				printf("%c", initBoard[i][j]);
			}
		}
		printf("\n");
	}
	/**
	 *
	 */

	// input validation
	validateBoard(initBoard);
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

int Game::getShipLengthHorizontal(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction)
{
	// stop condition
	if (expectedShip != initBoard[i][j])
	{
		return 0;
	}
	if (ShipLengthSecondDirection::FORWARD == direction)
	{
		return (1 + Game::getShipLengthHorizontal(initBoard, expectedShip, i, j + 1, direction));
	}
	else /*GetShipLengthDirection::BACKWORD == direction*/
	{
		return (1 + Game::getShipLengthHorizontal(initBoard, expectedShip, i, j - 1, direction));
	}
}

int Game::getShipLengthVertical(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction)
{
	// stop condition
	if (expectedShip != initBoard[i][j])
	{
		return 0;
	}
	if (ShipLengthSecondDirection::FORWARD == direction)
	{
		return (1 + Game::getShipLengthVertical(initBoard, expectedShip, i + 1, j, direction));
	}
	else /*GetShipLengthDirection::BACKWORD == direction*/
	{
		return (1 + Game::getShipLengthVertical(initBoard, expectedShip, i - 1, j, direction));
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
int Game::getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthDirection direction)
{
	// sanity

	if (expectedShip != initBoard[i][j])
	{
		return 0;
	}

	if (ShipLengthDirection::HORIZONTAL == direction)
	{
		return (1 + Game::getShipLengthHorizontal(initBoard, expectedShip, i, j - 1, ShipLengthSecondDirection::BACKWORD) +
			Game::getShipLengthHorizontal(initBoard, expectedShip, i, j + 1, ShipLengthSecondDirection::FORWARD));


	}
	else /*GetShipLengthDirection::VERTICAL == direction*/
	{
		return (1 + Game::getShipLengthVertical(initBoard, expectedShip, i - 1, j, ShipLengthSecondDirection::BACKWORD) +
			Game::getShipLengthVertical(initBoard, expectedShip, i + 1, j, ShipLengthSecondDirection::FORWARD));


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

ReturnCode Game::initFilesPath(string& filesPath, string& sboardFile, vector<string>& attackFilePerPlayer)
{
	// first check that the folder exists
	DWORD ftyp = GetFileAttributesA(filesPath.c_str());
	if ("" != filesPath && 
		(ftyp == INVALID_FILE_ATTRIBUTES || false == (ftyp & FILE_ATTRIBUTE_DIRECTORY)))
	{
		cout << "Wrong path: " << filesPath << endl;
		return RC_ERROR;  //something is wrong with your path!
	}

	// path is OK
	if (RC_SUCCESS != getSboardFileNameFromDirectory(filesPath, sboardFile))
	{
		return RC_ERROR;
	}

	if (RC_SUCCESS != getattackFilesNameFromDirectory(filesPath, attackFilePerPlayer))
	{
		return RC_ERROR;
	}

	return RC_SUCCESS;
}


/**
 * @Details		receives path to sboard and attack files and initializes the game:
 *				players, board etc
 * @Param		filesPath - location of sboard and attack files
 */
ReturnCode Game::init(std::string filesPath)
{
	string sboardFile, attackAFile, attackBFile;
	vector<string> attackFilePerPlayer;
	ReturnCode rc = initFilesPath(filesPath, sboardFile, attackFilePerPlayer);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}
	// init ERRORs data structures
	initErrorDataStructures();

	// Init board is larger by 1 from actual board in every dimension for 
	// traversing within the board more easily
	//char initBoard[INIT_BOARD_ROW_SIZE][INIT_BOARD_COL_SIZE];
	char** initBoard = new char*[INIT_BOARD_ROW_SIZE];
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		initBoard[i] = new char[INIT_BOARD_COL_SIZE];
	}
	rc = parseBoardFile(sboardFile, initBoard);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}

	// now the board is valid lets build our board
	m_board.buildBoard(initBoard);
	/* DEBUG */
	m_board.dump();
	/*********/

	// Create players and boards
	for (size_t i = 0; i < NUM_OF_PLAYERS; i++)
	{
		m_players[i] = PlayerAlgoFactory::instance().create(AlgoType::FILE);
		//m_boards[m_players[i]] = 
	m_players[PLAYER_A] = PlayerAlgoFactory::instance().create(AlgoType::FILE);
	//rc = fillBoardOfPlayer(PLAYER_A, m_boards[m_players[PLAYER_A]]);
	char** playerABoard = m_board.toCharMat(PLAYER_A);
	//m_players[PLAYER_A]->setBoard(playerABoard, m_rows, m_cols);

	m_players[PLAYER_B] = PlayerAlgoFactory::instance().create(AlgoType::FILE);
	char** playerBBoard = m_board.toCharMat(PLAYER_B);
	//m_players[PLAYER_B]->setBoard(playerBBoard, m_rows, m_cols);
	
	}
	
	// initListPlayers + init foreach player
	// Set board for Players
	//
	// For more players - add new section

	return RC_SUCCESS;
}

//ReturnCode Game::fillBoardOfPlayer(vector<char> playerChars, Board& board)
//{
//	for (size_t i = 0; i < m_rows; i++)
//	{
//		for (size_t i = 0; i < m_cols; i++)
//		{
//			char c = m_board.getSign(i, j);
//			if (SPACE != c && Utils::instance().isExistInVec(playerChars, c))
//			{
//				m_board.getSign(i, j) 
//			}
//		}
//	}
//	
//}
ReturnCode Game::fillBoardOfPlayer(PlayerIndex player, Board& board)
{
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_cols; j++)
		{
			board.get(i, j) = m_board.get(i, j);

			char c = m_board.getSign(i, j);
			if (SPACE == c || Utils::instance().getIndexByShip(c) != player)
			{
				board.get(i,j).clear();
			}
		}
	}
	
	return RC_SUCCESS;
}

AttackRequestCode Game::requestAttack(pair<int, int> req)
{
	if (ARC_NO_REQ == req.first)
		return ARC_NO_REQ;
	else if (req.first < 0 || req.first >= m_rows ||
		req.second < 0 || req.second >= m_cols)
		return ARC_ERROR;
	else
		return ARC_SUCCESS;
}

ReturnCode Game::startGame()
{
	//vector<IBattleshipGameAlgo*>::iterator iter = m_players.begin();
	//pair<int, int> attackReq;
	//IBattleshipGameAlgo* currentPlayer;
	////vector<IBattleshipGameAlgo*>::iterator iter = m_players.begin();
	//
	//// Just for Ex1 
	//if (m_players.size() != NUM_OF_PLAYERS)
	//{
	//	DBG(Debug::DBG_ERROR, "Wrong number of players - [%d]", m_players.size());
	//	return RC_ERROR;
	//}


	//pair<int, int> attackReq;
	//PlayerAlgo* currentPlayer = m_players[PLAYER_A];
	//int currentPlayerIndex = PLAYER_A;
	//
	//while (true)
	//{
	//	currentPlayer = (*iter);
	//	currentPlayer = m_players[currentPlayerIndex];
	//	attackReq = currentPlayer->attack();

	//	AttackRequestCode arc = requestAttack(attackReq);
	//	switch (arc)
	//	{
	//	case ARC_NO_REQ:
	//		// TODO: Maybe print info
	//		break;
	//	case ARC_ERROR:
	//		DBG(Debug::DBG_ERROR, "Attack failed ! values: %d-%d. Skipping.. arc[%d]", attackReq.first, attackReq.second, arc);
	//		break;
	//	default:
	//		break;
	//	}
	//	

	//	Cell attackedCell = m_board.get(attackReq);
	//	AttackResult ar;

	//	if (Cell::DEAD == attackedCell.getStatus())
	//	{
	//		DBG(Debug::DBG_INFO, "This cell already attacked, go to sleep...");
	//		// TODO: ar = ?
	//	}
	//	else
	//	{
	//		if (attackedCell.getSign())
	//		{
	//			
	//		}
	//		
	//	}

	//	// Check attack status (hit,miss)
	//	// Notify for all players
	//	// If game over break

	//	// If attack failed - iter.next
	//	// If iter == last do iter = begin
	//}

	//// Notify winner

	return ReturnCode::RC_SUCCESS;
}
