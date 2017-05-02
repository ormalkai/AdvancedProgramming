

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
#include "BattleshipAlgoInteractive.h"

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

void Game::validateBoard(char** initBoard)
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

void Game::readSBoardFile(std::string filePath, char** initBoard)
{
	ifstream sboard(filePath);
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
}

ReturnCode Game::getSboardFileNameFromDirectory(string filesPath, string& sboardFileName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	string sboardFile = filesPath + "*.sboard";
	std::wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	hFind = FindFirstFile(converter.from_bytes(sboardFile).c_str(), &FindFileData);
	/*hFind = FindFirstFile(sboardFile.c_str(), &FindFileData);*/
	if (INVALID_HANDLE_VALUE == hFind)
	{
		cout << "Missing board file (*.sboard) looking in path: " << filesPath << endl;
		return RC_ERROR;
	}
	else
	{
		sboardFileName = filesPath + converter.to_bytes(FindFileData.cFileName);
		FindClose(hFind);
		return RC_SUCCESS;
	}
}

ReturnCode Game::getattackFilesNameFromDirectory(string filesPath, vector<string>& attackFilePerPlayer)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	ReturnCode rc = RC_SUCCESS;
	std::wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	for (int i = PLAYER_A; i < PlayerIndex::MAX_PLAYER; i++)
	{
		string attackFileExtensionPerPlayer = Utils::getAttackFileByPlayer(i);
		if (NO_ATTACK_FILE == attackFileExtensionPerPlayer)
		{
			attackFilePerPlayer.push_back(NO_ATTACK_FILE);
			continue;
		}
		string attackFile = filesPath + attackFileExtensionPerPlayer;
		hFind = FindFirstFile(converter.from_bytes(attackFile).c_str(), &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			cout << "Missing attack file for player " << Utils::getPlayerCharByIndex(i) << " looking in path: " << filesPath << endl;
			rc = RC_ERROR;
		}
		else
		{
			attackFilePerPlayer.push_back(filesPath + converter.to_bytes(FindFileData.cFileName));
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
	for (int i = 0; i < m_rows + BOARD_PADDING; ++i)
	{
		for (int j = 0; j < m_cols + BOARD_PADDING; ++j)
		{
			initBoard[i][j] = SPACE;
		}
	}
	// Read from file 
	readSBoardFile(sboardFileName, initBoard);

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
int Game::getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipDirection direction)
{
	// sanity
	if (expectedShip != initBoard[i][j])
	{
		return 0;
	}

	if (ShipDirection::HORIZONTAL == direction)
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

ReturnCode Game::initFilesPath(string& filesPath, string& sboardFile, vector<string>& dllPerPlayer)
{
	vector<string> sboardFiles;
	// load sboard file
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(filesPath, "sboard", sboardFiles);
	if (RC_SUCCESS != rc)
	{
		DBG(Debug::DBG_ERROR, "Failed in finding sboard file");
		return rc;
	}
	sboardFile = sboardFiles[0];

	// find dll files
	rc = Utils::getListOfFilesInDirectoryBySuffix(filesPath, "dll", dllPerPlayer);
	if (RC_INVALID_ARG == rc)
	{
		// something bad happens, someone deleted the path between getting sboard and getting dll
		DBG(Debug::DBG_ERROR, "Failed in finding dll files");
		return rc;
	}
	else if (RC_ERROR == rc || (RC_SUCCESS == rc && dllPerPlayer.size() < 2)) // no dlls in path or less then 2 dlls
	{
		cout << "Missing an algorithm(dll) file looking in path : " + filesPath;
		return RC_ERROR;
	}

	return RC_SUCCESS;
}


/**
 * @Details		receives path to sboard and attack files and initializes the game:
 *				players, board etc
 * @Param		filesPath - location of sboard and attack files
 */
ReturnCode Game::init(std::string filesPath, bool isQuiet, int delay)
{
	// TEST ORM TODO
	string sboardFile, attackAFile, attackBFile;
	vector<string> dllPaths;
	ReturnCode rc = initFilesPath(filesPath, sboardFile, dllPaths);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}

	// set printOut parameters
	m_isQuiet = isQuiet;
	m_board.setDelay(delay);
	m_board.setIsQuiet(isQuiet);

	// init ERRORs data structures
	initErrorDataStructures();


	// Init board is larger by 1 from actual board in every dimension for 
	// traversing within the board more easily
	char** initBoard = new char*[m_rows + BOARD_PADDING];
	for (int i = 0; i < m_rows + BOARD_PADDING; ++i)
	{
		initBoard[i] = new char[m_cols + BOARD_PADDING];
	}
	rc = parseBoardFile(sboardFile, initBoard);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}

	// now the board is valid lets build our board
	m_board.buildBoard(const_cast<const char**>(initBoard), m_rows, m_cols);
	// initBoard is no longer relevant lets delete it
	for (int i = 0; i < m_rows + BOARD_PADDING; ++i)
	{
		delete[] initBoard[i];
	}
	delete[] initBoard;



	// Load algorithms from DLL
	rc = loadAllAlgoFromDLLs(dllPaths);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}

	// Init player A
	m_players[PLAYER_A] = get<1>(m_algoDLLVec[PLAYER_A])();
	char** playerABoard = m_board.toCharMat(PLAYER_A);
	m_players[PLAYER_A]->setBoard(PLAYER_A, const_cast<const char **>(playerABoard), m_rows, m_cols);
	m_players[PLAYER_A]->init(filesPath);

	// Init player B
	m_players[PLAYER_B] = get<1>(m_algoDLLVec[PLAYER_B])();
	char** playerBBoard = m_board.toCharMat(PLAYER_B);
	m_players[PLAYER_B]->setBoard(PLAYER_B, const_cast<const char **>(playerABoard), m_rows, m_cols);
	m_players[PLAYER_B]->init(filesPath);
	
		
	
	
	/* testing smart*/
	/*m_players[PLAYER_A] = PlayerAlgoFactory::instance().create(AlgoType::SMART);
	char** playerABoard = m_board.toCharMat(PLAYER_A);
	m_players[PLAYER_A]->init("");
	m_players[PLAYER_A]->setBoard(PLAYER_A, const_cast<const char **>(playerABoard), m_rows, m_cols);*/
	/*==============*/


	// Or
	/*m_players[PLAYER_B] = new BattleshipAlgoInteractive(1);
	char** playerBBoard = m_board.toCharMat(PLAYER_B);
	m_players[PLAYER_B]->setBoard(PLAYER_B, const_cast<const char **>(playerBBoard), m_rows, m_cols);*/


	

	/* testing naive#1#*/
	/*m_players[PLAYER_B] = PlayerAlgoFactory::instance().create(AlgoType::NAIVE);
	char** playerBBoard = m_board.toCharMat(PLAYER_B);
	m_players[PLAYER_B]->setBoard(PLAYER_B, const_cast<const char **>(playerBBoard), m_rows, m_cols);*/
	/*==============#1#*/

	for (int i = 0; i < m_board.rows(); i++)
	{
		delete[] playerABoard[i];
		delete[] playerBBoard[i];
	}
	
	delete[] playerABoard;
	delete[] playerBBoard;


	// For more players - add new section

	return RC_SUCCESS;
}

AttackRequestCode Game::requestAttack(pair<int, int> req) const
{

	if (ARC_FINISH_REQ == req.first && ARC_FINISH_REQ == req.second)
		if (true == m_finishedAttackPlayer[m_otherPlayerIndex])
			return ARC_GAME_OVER;
		else
			return ARC_FINISH_REQ;
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
	IBattleshipGameAlgo* currentPlayer = m_players[PLAYER_A];
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
			DBG(Debug::DBG_INFO, "Both players are epmty attack queue");
			break;
		case ARC_FINISH_REQ:
			// TODO:Fix both players with empty attack queue
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
			DBG(Debug::DBG_ERROR, "Failed to load dll %s", path);
			return RC_ERROR;
		}

		// Get GetAlgorithm function pointer
		getAlgoFunc = (GetAlgoFuncType)GetProcAddress(hDll, "GetAlgorithm");
		if (!getAlgoFunc)
		{
			DBG(Debug::DBG_ERROR, "Could not load function GetAlgorithm", path);
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

	if (0 == scoreDef)
	{
		cout << "Draw" << endl;
	}
	else 
	{
		int winner = (scoreDef > 0) ? PLAYER_A : PLAYER_B;
		cout << "Player " << Utils::getPlayerCharByIndex(winner) << " Won" << endl;
	}

	// Print points
	cout << "Points:" << endl;
	for (int i = 0; i < NUM_OF_PLAYERS; i++)
	{
		cout << "Player " << Utils::getPlayerCharByIndex(i) << ": " << m_playerScore[i] << endl;
	}
}
