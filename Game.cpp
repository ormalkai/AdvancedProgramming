
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

ReturnCode Game::initSboardFilePath(const string& filesPath, string& sboardFilePath)
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
	sboardFilePath = sboardFiles[0];

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
