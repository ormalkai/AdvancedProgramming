#include <string>
#include <iostream>
#include <thread>
#include <cassert>
#include <iomanip>
#include "Tournament.h"
#include "Game.h"
#include "Debug.h"
#include "PriorityQueue.h"


ReturnCode Tournament::init(const string& directoryPath)
{
	vector<string> boardsFilesPaths;
	
	// Get all board paths
	ReturnCode rc = initSboardFiles(directoryPath, boardsFilesPaths);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}
	
	vector<string> dllsFilesPaths;

	// Get all dlls paths
	rc = initDLLFilesPath(directoryPath, dllsFilesPaths);
	if (RC_SUCCESS != rc)
	{
		return rc;
	}
	
	// Load boards
	promise<ReturnCode> p1;
	promise<ReturnCode> p2;
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
	thread thrBoardLoading(&Tournament::loadBoards, this, boardsFilesPaths, move(p1));
	thread thrDllLoading(&Tournament::loadAllAlgoFromDLLs, this, dllsFilesPaths, move(p2));

	// Wait boards and dll finish loading
	thrBoardLoading.join();
	thrDllLoading.join();
	ReturnCode rcloadDll = f1.get();
	ReturnCode rcloadBoards = f2.get();
	if (RC_SUCCESS != rcloadDll || RC_SUCCESS != rcloadBoards)
	{
		return RC_ERROR;
	}
	cout << "Number of legal players : " << m_algoDLLVec.size() << endl;
	cout << "Number of legal boards : " << m_boards.size() << endl;

	// Load games to data structure
	buildGameSchedule();

	// init main database - result per player in round
	initPlayerResultsPerRound();

	return RC_SUCCESS;
}


void Tournament::initPlayerResultsPerRound()
{
	// in each round every player plays one game
	auto playersInRound = m_algoDLLVec.size();

	// number of rounds is the number of games mul 2 divided by number of players in round
	int numRounds = (m_gameList.size() * 2) / playersInRound;

	// init player results per round
	for (int round = 0; round < numRounds; round++)
	{
		m_playersResultsPerRound.emplace_back();
		for (int player = 0; player < playersInRound; player++)
		{
			m_playersResultsPerRound[round].m_results.emplace_back();
		}
	}

	// init number of games finished per player
	for (int player = 0; player < playersInRound; player++)
	{
		// this code is done by the control thread, therefore not need lock here
		m_gameFinishedByEachPlayer.push_back(0);
	}
}

void Tournament::startTournament(int numOfThreads)
{
	m_isTournamentFinished = false;
	
	// first create the reporter thread
	thread reporter = reporterThread();

	// vector of all threads - workers - do not open more threads than games
	numOfThreads = (numOfThreads <= m_gameList.size() ? numOfThreads : static_cast<int>(m_gameList.size()));
	vector<thread> vec_threads(numOfThreads);
	int id = 1;
	for (auto & t : vec_threads) 
	{
		t = thread(&Tournament::executeGame, this, id);
		++id;
	}

	// game is running

	// join all threads, first workers 
	for (auto & t : vec_threads) 
	{
		t.join();
	}

	// finally join the reporter
	reporter.join();
}

void Tournament::printResult() const
{
	Utils::gotoxy(3, 0);
	cout << "------ Round " << m_printedRounds+1 << " ------" << endl;
	auto playerStat(m_playerStat);
	sort(playerStat.begin(), playerStat.end(), SortByWins());
	
	const int nameWidth = m_maxNameLen + 2;
	const int colWidth = 8;
	
	printElement("#", colWidth);
	printElement("Team Name", nameWidth);
	printElement("Wins", colWidth);
	printElement("Losses", colWidth);
	printElement("%", colWidth);
	printElement("Pts For", colWidth);
	printElement("Pts Against", colWidth);
	cout << endl;
	
	for (int i = 0; i < playerStat.size(); i++) {

		const PlayerStatistics& ps = playerStat[i];		
		int wins = ps.getWins();
		int losses = ps.getLosses();
		double prec = (static_cast<double>(wins) / (wins + losses)) * 100;
	
		printElement(i + 1, colWidth);
		printElement(ps.getPlayerName(), nameWidth);
		printElement(wins, colWidth);
		printElement(losses, colWidth);
		printElement(prec, colWidth);
		printElement(ps.getPointsFor(), colWidth);
		printElement(ps.getPointsAgainst(), colWidth);
		cout << endl;
	}
	cout << endl;

	/*
#       Team Name               Wins    Losses  %       Pts For Pts Against

1.      oribarel.smart          1530    270     85      34930   18353
2.      galtzafri.smart         1487    313     82.61   41157   28268
3.      danielkozlov.smart      1465    335     81.39   35369   26544
4.      gelbart1.smart          1353    447     75.17   32802   26553
5.      danachernov.smart       1329    471     73.83   34534   22008
6.      shiranmazor.smart       1295    505     71.94   48695   25508
7.      ormalkai.smart          1278    522     71      34798   25337
8.      itamark.smart           1268    532     70.44   42776   33455
9.      nivk.smart              1198    602     66.56   32553   26301
10.     danielf1.smart          1013    787     56.28   38101   34543
11.     ronnysivan.smart        976     824     54.22   37333   36036
12.     amitmarcus.smart        975     825     54.17   28339   28585
13.     itayroll.smart          974     826     54.11   40821   37034
14.     inbarm1.smart           962     838     53.44   41708   38733
15.     ofirg1.smart            940     860     52.22   36013   35334
	 */

}


template<typename T> void Tournament::printElement(T t, const int& width)
{
	cout << left << setw(width) << setfill(' ') << t;
}


void Tournament::executeGame(int workerId) {

	// There is more game to play

	int currentGameIndex = m_nextGameIndex++;
	while (currentGameIndex < m_gameList.size())
	{
		Game& currentGame = m_gameList[currentGameIndex];
		currentGame.startGame();
		
		// Notify result
		notifyGameResult(currentGame, currentGameIndex);

		currentGameIndex = m_nextGameIndex++;
	}

	DBG(Debug::DBG_INFO, "Worker %d complete job", workerId);
}


void Tournament::notifyGameResult(Game& game, int gameIndex)
{
	// TODO ORM why assert?
	assert(game.isGameOver());

	pair <int, int> playersIndexes = m_gamePlayerIndexes[gameIndex];
	int playerAIndex = playersIndexes.first;
	int playerBIndex = playersIndexes.second;

	// critical section updating the game per player
	m_gameFinishedByEachPlayerMutex.lock();
	// TODO ORM what happen here?
	int playerAGame = m_gameFinishedByEachPlayer[playerAIndex]++;
	int playerBGame = m_gameFinishedByEachPlayer[playerBIndex]++;
	m_gameFinishedByEachPlayerMutex.unlock();

	// get every player score from game
	Game::GameResult s = game.getGameResult();
	int playerAScore = s.m_playerAPoints;
	int playerBScore = s.m_playerBPoints;
	int winnerIndex = s.m_winnerIndex;
	WinLoseTie winLoseTieA;
	WinLoseTie winLoseTieB;
	if (winnerIndex == PLAYER_A)
	{
		winLoseTieA = WON;
		winLoseTieB = LOST;
	}
	else if (winnerIndex == PLAYER_B)
	{
		winLoseTieA = LOST;
		winLoseTieB = WON;
	}
	else
	{
		winLoseTieA = TIE;
		winLoseTieB = TIE;
	}
	PlayerGameResult playerAResult(playerAIndex, playerAScore, playerBScore, winLoseTieA);
	PlayerGameResult playerBResult(playerBIndex, playerBScore, playerAScore, winLoseTieB);

	// update round results for each player
	updateRoundResultForPlayer(playerAGame, playerAResult);
	updateRoundResultForPlayer(playerBGame, playerBResult);
}

void Tournament::updateRoundResultForPlayer(int RoundForPlayer, PlayerGameResult& playerResult)
{
	int playerIndexInRound;
	// fetch current index and increment current
	{
		lock_guard<mutex> lock(m_playersResultsPerRound[RoundForPlayer].m_roundMutex);
		playerIndexInRound = m_playersResultsPerRound[RoundForPlayer].m_playersFinished++;
		m_playersResultsPerRound[RoundForPlayer].m_results[playerIndexInRound] = playerResult;
	}

	// if Round is finished notify reporter
	if (playerIndexInRound == m_playersResultsPerRound[RoundForPlayer].m_results.size() - 1)
	{
		// increment finished rounds for reporter
		{
			lock_guard<mutex> lock(m_reporterMutex);
			++m_finishedRounds;
		}

		// wake up reporter
		m_cvRound.notify_one();
	}
}


ReturnCode Tournament::initSboardFiles(const string& directoryPath, vector<string>& sboardFiles)
{
	// load sboard file
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(directoryPath, "sboard", sboardFiles, true);
	// ERROR means that the path exists but there is no file
	if (RC_ERROR == rc || sboardFiles.size() == 0)
	{
		// TODO ORM exercise is different "No board files (*.sboard) looking in path:"
		cout << "Missing board file (*.sboard) looking in path: " << directoryPath << endl;
		DBG(Debug::DBG_ERROR, "Missing board file (*.sboard) looking in path: %s", directoryPath);
		return rc;
	}

	return RC_SUCCESS;
}

ReturnCode Tournament::initDLLFilesPath(const string& directoryPath, vector<string>& dllFiles)
{
	// find dll files
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(directoryPath, "dll", dllFiles);
	if (RC_INVALID_ARG == rc)
	{
		// something bad happens, someone deleted the path between getting sboard and getting dll
		DBG(Debug::DBG_ERROR, "Failed in finding dll files");
		return rc;
	}
	if (RC_ERROR == rc || (RC_SUCCESS == rc && dllFiles.size() < MIN_DLLS_FILES)) // no dlls in path or less then 2 dlls
	{
		cout << "Missing an algorithm (dll) file looking in path : " + directoryPath << " (needs at least two)" << endl;
		DBG(Debug::DBG_ERROR, "Missing an algorithm(dll) file looking in path : %s", directoryPath.c_str());
		return RC_ERROR;
	}

	return RC_SUCCESS;
}

ReturnCode Tournament::loadAllAlgoFromDLLs(const vector<string>& dllPaths, promise<ReturnCode>&& p)
{
	GetAlgoFuncType getAlgoFunc;
	
	int playerIndex = 0;

	for (string path : dllPaths)
	{
		// Load dynamic library
		HINSTANCE hDll = LoadLibraryA(path.c_str()); // Notice: Unicode compatible version of LoadLibrary
		if (!hDll)
		{
			DBG(Debug::DBG_INFO, "Cannot load dll: %s", path);
			continue;
		}

		// Get GetAlgorithm function pointerm
		getAlgoFunc = (GetAlgoFuncType)GetProcAddress(hDll, "GetAlgorithm");
		if (!getAlgoFunc)
		{
			DBG(Debug::DBG_INFO, "Cannot load GetAlgorithm func from dll: %s", path);
			continue;
		}

		// Extract player name for the dll path
		string playerName;
		ReturnCode rc = Utils::getPlayerNameByDllPath(path, playerName);
		if (RC_SUCCESS != rc)
		{
			continue;
		}
		if (m_maxNameLen < playerName.size())
		{
			m_maxNameLen = playerName.size();
		}
		
		m_algoDLLVec.push_back(make_tuple(hDll, getAlgoFunc));
		m_playerStat.emplace_back(playerIndex, playerName);
		playerIndex++;
	}

	if (MIN_DLLS_FILES > m_algoDLLVec.size())
	{
		DBG(Debug::DBG_ERROR, "Num of dlls (%d) is below the minimum (%d)", m_algoDLLVec.size(), MIN_DLLS_FILES);
		p.set_value(RC_ERROR);
		return RC_ERROR;
	}
	
	DBG(Debug::DBG_INFO, "Finished to load dlls, total was loaded: %d", m_algoDLLVec.size());
	p.set_value(RC_SUCCESS);
	return RC_SUCCESS;
}

ReturnCode Tournament::loadBoards(vector<string>& boardsPaths, promise<ReturnCode>&& p)
{

	for (string boardPath : boardsPaths)
	{
		Board b;
		ReturnCode rc = b.loadBoardFromFile(boardPath);
	
		if (RC_SUCCESS != rc)
		{
			DBG(Debug::DBG_WARNING, "Invalid board [%s], skipping", boardPath.c_str());
			continue;
		}
		
		m_boards.push_back(b);
	}

	if (MIN_BOARDS_FILE > m_boards.size())
	{
		DBG(Debug::DBG_ERROR, "Num of boards (%d) is below the minimum (%d)", m_boards.size(), MIN_BOARDS_FILE);
		p.set_value(RC_ERROR);
		return RC_ERROR;
	}
	
	DBG(Debug::DBG_INFO, "Finished to load boards, total was loaded: %d", m_boards.size());
	p.set_value(RC_SUCCESS);
	return RC_SUCCESS;
}

void Tournament::buildGameSchedule()
{
	size_t numOfBoards = m_boards.size();
	size_t numOfAlgos = m_algoDLLVec.size();

	auto allPossibilities = createSchedule(numOfAlgos);
	for (size_t boardIndex = 0; boardIndex < numOfBoards; boardIndex++)
	{
		for (auto round : allPossibilities)
		{
			for (pair<int,int> p : round)
			{

				int playerAIndex = p.first;
				int playerBIndex = p.second;

				if (playerAIndex == -1 || playerBIndex == -1)
					continue;

				Board b = m_boards[boardIndex];
				
				unique_ptr<IBattleshipGameAlgo> ibg1(get<1>(m_algoDLLVec[playerAIndex])());
				unique_ptr<IBattleshipGameAlgo> ibg2(get<1>(m_algoDLLVec[playerBIndex])());
				
				m_gameList.emplace_back(b, move(ibg1), move(ibg2));
				m_gamePlayerIndexes.emplace_back(make_pair(playerAIndex, playerBIndex));
			}
		}
	}
}

vector<vector<pair<int, int>>> Tournament::createSchedule(int numOfAlgos)
{
	// Credit: https://stackoverflow.com/questions/1037057/how-to-automatically-generate-a-sports-league-schedule


	vector<int> list;

	for (int i = 0; i < numOfAlgos; i++)
	{
		list.push_back(i);
	}
	// Padding
	if (numOfAlgos % 2 == 1)
		list.push_back(-1);

	vector<vector<pair<int,int>>> result;
	
	for (int i = 0; i < list.size() - 1; i++)
	{
		size_t mid = list.size() / 2;
		vector<int> l1(list.begin(), list.begin() + mid);
		vector<int> l2(list.begin() + mid, list.end());
		reverse(l2.begin(), l2.end());

		// Switch sides after each round
		if (i % 2 == 1)
		{
			result.push_back(zip(l1, l2));
		}
		else
		{
			result.push_back(zip(l2, l1));

		}

		auto last = list.back();
		list.pop_back();
		list.insert(list.begin() + 1, last);
	}

	auto temp(result);
	for (auto cycleVec : temp)
	{
		vector<pair<int, int>> inverse;

		for (auto pair : cycleVec)
		{
			inverse.push_back(make_pair(pair.second, pair.first));
		}

		result.push_back(inverse);
	}
	return result;
}

vector<pair<int, int>> Tournament::zip(vector<int> first, vector<int> second)
{
	assert(first.size() == second.size());

	vector<pair<int, int>> result;

	for (size_t i = 0; i < first.size(); i++)
	{
		result.push_back(make_pair(first[i], second[i]));
	}

	return result;
}

void Tournament::reportResult()
{
	// while there are running rounds
	while (false == m_isTournamentFinished)
	{
		// NOTE: the lock is only on the cond_var wait
		// The reason is that we want to let the workers increment m_finishedRounds
		// even if the reporter is in reporting progress
		// however we don't want to let the reporter go to sleep and miss a signal
		// between the condition to the sleep.
		unique_lock<std::mutex> lk(m_reporterMutex); //unique lock
		m_cvRound.wait(lk, [this](void) { return m_printedRounds < m_finishedRounds; });
		lk.unlock();
		while (m_printedRounds < m_finishedRounds)
		{
			// print round m_printedRounds
			updateStatAndPrintRound();

			m_printedRounds++;
			// check if tournament finished
			if (m_printedRounds == m_playersResultsPerRound.size())
				m_isTournamentFinished = true;
		}
	}
}

void Tournament::updateStatAndPrintRound()
{
	// update stat acording to the finished round
	for (auto const& result: m_playersResultsPerRound[m_printedRounds].m_results)
	{
		m_playerStat[result.m_playerId].update(pair<int, int>(result.m_pointsFor, result.m_pointsAgainst), result.m_isWon);
	}

	printResult();
}


