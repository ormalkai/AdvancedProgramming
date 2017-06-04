
#include <codecvt>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "Game.h"
#include "Utils.h"
#include "Debug.h"
#include "Board.h"


using namespace std;


Game::Game(Board& board, unique_ptr<IBattleshipGameAlgo> algoA, unique_ptr<IBattleshipGameAlgo> algoB) : m_isQuiet(false),
m_currentPlayerIndex(MAX_PLAYER), m_otherPlayerIndex(MAX_PLAYER), m_isGameOver(false)
{
	init(board, move(algoA), move(algoB));
}

Game::~Game()
{
	/*for (int i=0; i< MAX_PLAYER ;i++)
	{
		if (nullptr != m_players[i])
		{
			delete m_players[i];
		}
	}*/
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
	if (RC_ERROR == rc || (RC_SUCCESS == rc && dllPerPlayer.size() < MAX_PLAYER)) // no dlls in path or less then 2 dlls
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
void Game::init(const vector<vector<vector<char>>> board, unique_ptr<IBattleshipGameAlgo> algoA, unique_ptr<IBattleshipGameAlgo> algoB)
{
	// now the board is valid lets build our board
	m_board.buildBoard(board);
	
	init(m_board, move(algoA), move(algoB));
}

void Game::init(const Board& board, unique_ptr<IBattleshipGameAlgo> algoA, unique_ptr<IBattleshipGameAlgo> algoB)
{

	m_board = board;
	
	Board boardA, boardB;
	m_board.splitToPlayersBoards(boardA, boardB);

	// Init player A
	m_players.push_back(move(algoA));
	m_players.push_back(move(algoB));

	m_players[PLAYER_A]->setBoard(boardA);
	m_players[PLAYER_B]->setBoard(boardB);

	m_players[PLAYER_A]->setPlayer(PLAYER_A);
	m_players[PLAYER_B]->setPlayer(PLAYER_B);
}


AttackRequestCode Game::requestAttack(Coordinate& req)
{

	if (ARC_FINISH_REQ == req.depth && ARC_FINISH_REQ == req.row && ARC_FINISH_REQ == req.col)
		if (true == m_finishedAttackPlayer[m_otherPlayerIndex])
			return ARC_GAME_OVER;
		else {
			m_finishedAttackPlayer[m_currentPlayerIndex] = true;
			return ARC_FINISH_REQ;
		}
	else if (	req.depth < 0 || req.depth > m_depth ||
				req.row   < 0 || req.row   > m_rows ||
				req.col   < 0 || req.col   > m_cols)
		return ARC_ERROR;
	else
		return ARC_SUCCESS;
}


void Game::startGame()
{

	auto& shipsPerPlayer = m_board.shipsPerPlayer();

	m_board.printBoard();

	m_currentPlayerIndex = PLAYER_A;
	m_otherPlayerIndex = PLAYER_B;

	// Game loop
	while (!m_isGameOver)
	{
		Coordinate attackReq = m_players[m_currentPlayerIndex]->attack();

		// Check attack request 
		AttackRequestCode arc = requestAttack(attackReq);
		switch (arc)
		{
		case ARC_GAME_OVER:
			DBG(Debug::DBG_INFO, "For both players - No more attack requests");
			m_isGameOver = true;
			continue;
		case ARC_FINISH_REQ:
			proceedToNextPlayer();
			continue;
		case ARC_ERROR:
			DBG(Debug::DBG_INFO, "Attack failed ! values: %d-%d-%d. Skipping.. arc[%d]", attackReq.depth, attackReq.row, attackReq.col, arc);
			break;
		default:
			break;
		}

		shared_ptr<Cell> attackedCell = m_board.get(attackReq);
		attackedCell->hitCell();

		AttackResult attackResult;

		// Check attack result
		switch (attackedCell->getStatus())
		{
		case Cell::ALIVE:
		{
			attackedCell->setStatus(Cell::DEAD);

			// Notify the user his ridiculous mistake
			if (attackedCell->getPlayerIndexOwner() == m_currentPlayerIndex)
			{
				DBG(Debug::DBG_INFO, "You bombed youself, U R probably an idiot..");
			}

			shared_ptr<Ship> pShip = attackedCell->getShip();
			pShip->executeAttack();

			if (pShip->isShipAlive())
			{
				attackResult = AttackResult::Hit;
			}
			else
			{
				attackResult = AttackResult::Sink;

				// Hit myself
				if (attackedCell->getPlayerIndexOwner() == m_currentPlayerIndex)
				{
					m_playerScore[m_otherPlayerIndex] += pShip->getValue();
					shipsPerPlayer[m_currentPlayerIndex]--;
				}
				else
				{
					// Update score for player
					m_playerScore[m_currentPlayerIndex] += pShip->getValue();

					// Update number of alive ships
					shipsPerPlayer[m_otherPlayerIndex]--;
				}		
			}
		}
		break;
		case Cell::DEAD:
		{
			DBG(Debug::DBG_INFO, "This cell was already bombed, go to sleep bro...");
			if (attackedCell->getShip()->isShipAlive())
				attackResult = AttackResult::Hit;
			else
				attackResult = AttackResult::Miss;
		}
		break;
		case Cell::FREE:
		default:
			attackResult = AttackResult::Miss;
		}

		m_board.printAttack(m_currentPlayerIndex, attackReq.row, attackReq.col, attackReq.depth, attackResult);

		// Notify for all players
		for (int i = 0; i < NUM_OF_PLAYERS; i++)
		{
			m_players[i]->notifyOnAttackResult(m_currentPlayerIndex, attackReq, attackResult);
		}

		// If game over break
		if (Utils::isExistInVec(shipsPerPlayer, 0))
		{
			m_isGameOver = true;
			DBG(Debug::DBG_DEBUG, "Game over - 0 ships remaining");
		}
		// If attack failed - iter.next
		// If iter == last do iter = begin

		// Update next turn
		if (AttackResult::Miss == attackResult ||
			attackedCell->getPlayerIndexOwner() == m_currentPlayerIndex)
		{
			proceedToNextPlayer();
		}

	} // Game Loop
	
	printSummary();
}

bool Game::isGameOver() const
{
	return m_isGameOver;
}

pair<int,int> Game::getScore()
{
	return make_pair(m_playerScore[PLAYER_A], m_playerScore[PLAYER_B]);
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
