
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "Game.h"
#include "Utils.h"
#include "Debug.h"
#include "Board.h"
#include "BattleshipAlgoFromFile.h"
#include <codecvt>

#define SHIPS_PER_PLAYER (5)

using namespace std;

Game::Game(int rows, int cols) : m_rows(rows), m_cols(cols),
m_foundAdjacentShips(false), m_isQuiet(false), m_currentPlayerIndex(MAX_PLAYER), m_otherPlayerIndex(MAX_PLAYER)
{
	for (int i = 0; i< MAX_PLAYER; i++)
	{
		m_players[i] = nullptr;
	}
}

Game::~Game()
{
	for (int i=0; i< MAX_PLAYER ;i++)
	{
		if (nullptr != m_players[i])
		{
			delete m_players[i];
		}
	}
}

bool Game::checkWrongSizeOrShape() const
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

bool Game::checkNumberOfShips() const
{
	bool result = true;
	for (int i = 0; i < PlayerIndex::MAX_PLAYER; i++)
	{
		char player = Utils::getPlayerCharByIndex(i);
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

void Game::validateBoard(const char** initBoard)
{
	// for each char
	for (int i = 1; i <= m_rows; ++i)
	{
		for (int j = 1; j <= m_cols; ++j)
		{
			if (SPACE == initBoard[i][j])
			{
				// ignore spaces
				continue;
			}
			char currentShip = initBoard[i][j];
			int horizontalShipLen = getShipLength(initBoard, currentShip, i, j, ShipDirection::HORIZONTAL);
			int verticalShipLen = getShipLength(initBoard, currentShip, i, j, ShipDirection::VERTICAL);
			int expectedLen = Utils::getShipLen(currentShip);

			PlayerIndex playerIndex = Utils::getPlayerIdByShip(currentShip);
			int shipIndex = Utils::getIndexByShip(currentShip);
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

ReturnCode Game::readSBoardFile(const string filePath, char** const initBoard) const
{
	ifstream sboard(filePath);
	if (!sboard.is_open())
	{
		DBG(Debug::DBG_ERROR, "Could not open board file: %s", filePath);
		return RC_ERROR;
	}
	string line;
	int rowIndex = 1;
	// Line by line up to 10 line and up to 10 chars per line
	while (Utils::safeGetline(sboard, line) && rowIndex <= m_rows)
	{
		int colIndex = 1;
		for (std::string::size_type i = 0; i < line.size() && colIndex <= 10; i++)
		{
			char c = line[i];
			// If not allowed chars skip, otherwise insert it to board
			if (PlayerIndex::MAX_PLAYER != Utils::getPlayerIdByShip(c))
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

	return RC_SUCCESS;
}

ReturnCode Game::getSboardFileNameFromDirectory(const string filesPath, string& sboardFileName)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	string sboardFile = filesPath + "*.sboard";
	hFind = FindFirstFileA(sboardFile.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		cout << "Missing board file (*.sboard) looking in path: " << filesPath << endl;
		return RC_ERROR;
	}
	
	sboardFileName = filesPath + FindFileData.cFileName;
	FindClose(hFind);
	return RC_SUCCESS;
}

/**
 * @Details		parses sboard file and initializes the game board
 * @Param		sboardFileName		- path to sboard file
 * @Param		initBoard			- 2d array for initialize game board
 * @Return		ReturnCode
 */
ReturnCode Game::parseBoardFile(const string sboardFileName, char** initBoard)
{
	// Init board file with spaces
	for (int i = 0; i < m_board.rows() + BOARD_PADDING; ++i)
	{
		for (int j = 0; j < m_cols + BOARD_PADDING; ++j)
		{
			initBoard[i][j] = SPACE;
		}
	}
	// Read from file 
	ReturnCode result = readSBoardFile(sboardFileName, initBoard);
	if (RC_SUCCESS != result)
	{
		return result;
	}

	// input validation
	validateBoard(const_cast<const char**>(initBoard));
	// Print errors and return ERROR if needed
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
bool Game::isAdjacencyValid(const char** initBoard, int i/*row*/, int j/*col*/)
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

int Game::getShipLengthHorizontal(const char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction)
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

int Game::getShipLengthVertical(const char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction)
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
int Game::getShipLength(const char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipDirection direction)
{
	// sanity
	if (expectedShip != initBoard[i][j])
	{
		return 0;
	}

	if (ShipDirection::HORIZONTAL == direction)
	{
		return (1 + getShipLengthHorizontal(initBoard, expectedShip, i, j - 1, ShipLengthSecondDirection::BACKWORD) +
			Game::getShipLengthHorizontal(initBoard, expectedShip, i, j + 1, ShipLengthSecondDirection::FORWARD));


	}
	else /*GetShipLengthDirection::VERTICAL == direction*/
	{
		return (1 + Game::getShipLengthVertical(initBoard, expectedShip, i - 1, j, ShipLengthSecondDirection::BACKWORD) +
			Game::getShipLengthVertical(initBoard, expectedShip, i + 1, j, ShipLengthSecondDirection::FORWARD));


	}
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


ReturnCode Game::initSboardFilePath(const string& filesPath, string& sboardFile)
{
	vector<string> sboardFiles;
	// load sboard file
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(filesPath, "sboard", sboardFiles, true);
	// ERROR means that the path exists but there is no file
	if (RC_ERROR == rc)
	{
		cout << "Missing board file (*.sboard) looking in path: " << filesPath << endl;
		return rc;
	}
	sboardFile = sboardFiles[0];

	return RC_SUCCESS;
}

ReturnCode Game::initDLLFilesPath(const string& filesPath, vector<string>& dllPerPlayer)
{
	// find dll files
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(filesPath, "dll", dllPerPlayer);
	if (RC_INVALID_ARG == rc)
	{
		// something bad happens, someone deleted the path between getting sboard and getting dll
		DBG(Debug::DBG_ERROR, "Failed in finding dll files");
		return rc;
	}
	else if (RC_ERROR == rc || (RC_SUCCESS == rc && dllPerPlayer.size() < MAX_PLAYER)) // no dlls in path or less then 2 dlls
	{
		cout << "Missing an algorithm (dll) file looking in path : " + filesPath << endl;
		return RC_ERROR;
	}

	return RC_SUCCESS;
}


/**
 * @Details		receives path to sboard and attack files and initializes the game:
 *				players, board etc
 * @Param		filesPath - location of sboard and attack files
 */
ReturnCode Game::init(const string filesPath, bool isQuiet, int delay)
{
	string sboardFile, attackAFile, attackBFile;
	vector<string> dllPaths;
	char** initBoard;
	bool isInitBoardInitialized = false;
	ReturnCode sboardRc = initSboardFilePath(filesPath, sboardFile);
	// RC_INVALID_ARG means that path is wrong
	if (RC_INVALID_ARG == sboardRc)
	{
		return sboardRc;
	}
	else if (RC_SUCCESS == sboardRc)
	{
		// set printOut parameters
		m_isQuiet = isQuiet;
		m_board.setDelay(delay);
		m_board.setIsQuiet(isQuiet);

		// init ERRORs data structures
		initErrorDataStructures();


		// Init board is larger by 1 from actual board in every dimension for 
		// traversing within the board more easily
		initBoard = new char*[m_rows + BOARD_PADDING];
		for (int i = 0; i < m_board.rows() + BOARD_PADDING; ++i)
		{
			initBoard[i] = new char[m_board.cols() + BOARD_PADDING];
		}
		isInitBoardInitialized = true;
		sboardRc = parseBoardFile(sboardFile, initBoard);
	}

	// Load algorithms from DLL
	ReturnCode DLLRc = loadAllAlgoFromDLLs(dllPaths);
	if (RC_SUCCESS != DLLRc || RC_SUCCESS != sboardRc)
	{
		if (isInitBoardInitialized)
		{
			for (int i = 0; i < m_board.rows() + BOARD_PADDING; ++i)
			{
				delete[] initBoard[i];
			}
			delete[] initBoard;
		}
		return RC_ERROR;
	}
	
	// now the board is valid lets build our board
	m_board.buildBoard(const_cast<const char**>(initBoard), m_rows, m_cols);
	// initBoard is no longer relevant lets delete it
	for (int i = 0; i < m_board.rows() + BOARD_PADDING; ++i)
	{
		delete[] initBoard[i];
	}
	delete[] initBoard;

	// Init player A
	m_players[PLAYER_A] = get<1>(m_algoDLLVec[PLAYER_A])();
	char** playerABoard = m_board.toCharMat(PLAYER_A);
	m_players[PLAYER_A]->setBoard(PLAYER_A, const_cast<const char **>(playerABoard), m_rows, m_cols);
	freePlayerBoard(playerABoard);
	bool ret = m_players[PLAYER_A]->init(filesPath);
	if (false == ret)
	{
		cout << "Algorithm initialization failed for dll: " << dllPaths[PLAYER_A] << endl;
		return RC_ERROR;
	}

	// Init player B
	m_players[PLAYER_B] = get<1>(m_algoDLLVec[PLAYER_B])();
	char** playerBBoard = m_board.toCharMat(PLAYER_B);
	m_players[PLAYER_B]->setBoard(PLAYER_B, const_cast<const char **>(playerBBoard), m_rows, m_cols);
	freePlayerBoard(playerBBoard);
	ret = m_players[PLAYER_B]->init(filesPath);
	if (false == ret)
	{
		cout << "Algorithm initialization failed for dll: " << dllPaths[PLAYER_B] << endl;
		return RC_ERROR;
	}
	
	Utils::gotoxy(20, 0);
	cout << "PlayerA algo: " << dllPaths[PLAYER_A].substr(dllPaths[PLAYER_A].find_last_of("\\") + 1) << endl;
	cout << "PlayerB algo: " << dllPaths[PLAYER_B].substr(dllPaths[PLAYER_B].find_last_of("\\") + 1) << endl;
	return RC_SUCCESS;
}

void Game::freePlayerBoard(char** board) const
{
	if (nullptr == board)
		return;

	for (int i = 0; i < m_rows; i++)
	{
		delete[] board[i];
	}
	delete[] board;
}

AttackRequestCode Game::requestAttack(const pair<int, int>& req)
{

	if (ARC_FINISH_REQ == req.first && ARC_FINISH_REQ == req.second)
		if (true == m_finishedAttackPlayer[m_otherPlayerIndex])
			return ARC_GAME_OVER;
		else {
			m_finishedAttackPlayer[m_currentPlayerIndex] = true;
			return ARC_FINISH_REQ;
		}
	else if (	req.first < 0 || req.first > m_rows ||
				req.second < 0 || req.second > m_cols)
		return ARC_ERROR;
	else
		return ARC_SUCCESS;
}




void Game::startGame()
{
	m_board.printBoard();

	pair<int, int> attackReq;
	IBattleshipGameAlgo* currentPlayer;
	m_currentPlayerIndex = PLAYER_A;
	m_otherPlayerIndex = PLAYER_B;
	bool gameOver = false;

	// Game loop
	while (!gameOver)
	{
		currentPlayer = m_players[m_currentPlayerIndex];

		attackReq = currentPlayer->attack();

		// Check attack request 
		AttackRequestCode arc = requestAttack(attackReq);
		switch (arc)
		{
		case ARC_GAME_OVER:
			DBG(Debug::DBG_INFO, "For both players - No more attack requests");
			gameOver = true;
			continue;
		case ARC_FINISH_REQ:
			proceedToNextPlayer();
			continue;
		case ARC_ERROR:
			DBG(Debug::DBG_INFO, "Attack failed ! values: %d-%d. Skipping.. arc[%d]", attackReq.first, attackReq.second, arc);
			break;
		default:
			break;
		}

		Cell& attackedCell = m_board.get(attackReq);
		attackedCell.hitCell();

		AttackResult attackResult;

		// Check attack result
		switch (attackedCell.getStatus())
		{
		case Cell::ALIVE:
		{
			attackedCell.setStatus(Cell::DEAD);

			// Notify the user his ridiculous mistake
			if (attackedCell.getPlayerIndexOwner() == m_currentPlayerIndex)
			{
				DBG(Debug::DBG_INFO, "You bombed youself, U R probably an idiot..");
			}

			Ship* pShip = attackedCell.getShip();
			pShip->executeAttack();

			if (pShip->isShipAlive())
			{
				attackResult = AttackResult::Hit;
			}
			else
			{
				attackResult = AttackResult::Sink;

				// Hit myself
				if (attackedCell.getPlayerIndexOwner() == m_currentPlayerIndex)
				{
					m_playerScore[m_otherPlayerIndex] += pShip->getValue();
					m_numOfShipsPerPlayer[m_currentPlayerIndex]--;
				}
				else
				{
					// Update score for player
					m_playerScore[m_currentPlayerIndex] += pShip->getValue();

					// Update number of alive ships
					m_numOfShipsPerPlayer[m_otherPlayerIndex]--;
				}		
			}
		}
		break;
		case Cell::DEAD:
		{
			DBG(Debug::DBG_INFO, "This cell was already bombed, go to sleep bro...");
			if (attackedCell.getShip()->isShipAlive())
				attackResult = AttackResult::Hit;
			else
				attackResult = AttackResult::Miss;
		}
		break;
		case Cell::FREE:
		default:
			attackResult = AttackResult::Miss;
		}

		m_board.printAttack(m_currentPlayerIndex, attackReq.first, attackReq.second, attackResult);

		// Notify for all players
		for (int i = 0; i < NUM_OF_PLAYERS; i++)
		{
			m_players[i]->notifyOnAttackResult(m_currentPlayerIndex, attackReq.first, attackReq.second, attackResult);
		}

		// If game over break
		if (Utils::isExistInVec(m_numOfShipsPerPlayer, 0))
		{
			gameOver = true;
			DBG(Debug::DBG_DEBUG, "Game over - 0 ships remaining");
		}
		// If attack failed - iter.next
		// If iter == last do iter = begin

		// Update next turn
		if (AttackResult::Miss == attackResult ||
			attackedCell.getPlayerIndexOwner() == m_currentPlayerIndex)
		{
			proceedToNextPlayer();
		}

	} // Game Loop
	

	printSummary();
}

ReturnCode Game::loadAllAlgoFromDLLs(const vector<string>& dllPaths)
{
	GetAlgoFuncType getAlgoFunc;

	for (string path : dllPaths)
	{
		// Load dynamic library
		HINSTANCE hDll = LoadLibraryA(path.c_str()); // Notice: Unicode compatible version of LoadLibrary
		if (!hDll)
		{
			cout << "Cannot load dll: " << path << endl;
			return RC_ERROR;
		}
		
		// Get GetAlgorithm function pointerm
		getAlgoFunc = (GetAlgoFuncType)GetProcAddress(hDll, "GetAlgorithm");
		if (!getAlgoFunc)
		{
			cout << "Cannot load dll: " << path << endl;		
			return RC_ERROR;
		}

		m_algoDLLVec.push_back(make_tuple(hDll, getAlgoFunc));
	}

	return RC_SUCCESS;
}

void Game::printSummary() const
{
	if (false == m_isQuiet)
	{
		Utils::gotoxy(13, 0);
	}

	// Notify winner
	int scoreDef = m_playerScore[PLAYER_A] - m_playerScore[PLAYER_B];

	if (0 != scoreDef)
	{
		int winner = (scoreDef > 0) ? PLAYER_A : PLAYER_B;
		cout << "Player " << Utils::getPlayerCharByIndex(winner) << " won" << endl;
	}

	// Print points
	cout << "Points:" << endl;
	for (int i = 0; i < NUM_OF_PLAYERS; i++)
	{
		cout << "Player " << Utils::getPlayerCharByIndex(i) << ": " << m_playerScore[i] << endl;
	}
}
