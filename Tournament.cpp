#include <string>
#include <iostream>
#include "Tournament.h"
#include "Game.h"
#include "Debug.h"
#include <thread>
#include <cassert>
#include "PriorityQueue.h"
#include <iomanip>


ReturnCode Tournament::init(const string& directoryPath, int numOfThreads)
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
	thread thrBoardLoading(&Tournament::loadBoards, this, boardsFilesPaths);
	thread thrDllLoading(&Tournament::loadAllAlgoFromDLLs, this, dllsFilesPaths);

	// Wait boards and dll finish loading
	thrBoardLoading.join();
	thrDllLoading.join();

	// Load games to data structure
	buildGameSchedule();

	return RC_SUCCESS;
}


ReturnCode Tournament::startTournament(int numOfThreads)
{


	m_isRoundFinished = false;
	m_isTournamentFinished = false;


	return RC_SUCCESS;

}

void Tournament::printResult() const
{
	// Lock
	vector<PlayerStatistics> playerStatVec = m_playerStat;
	// Unlock
	
	sort(playerStatVec.begin(), playerStatVec.end(), SortByAge());
	
	const int nameWidth = 24;
	const int colWidth = 8;
	
	printElement("#", colWidth);
	printElement("Team Name", nameWidth);
	printElement("Wins", colWidth);
	printElement("Losses", colWidth);
	printElement("%", colWidth);
	printElement("Pts For", colWidth);
	printElement("Pts Against", colWidth);
	cout << endl;
	
	for (int i = 0; i < playerStatVec.size(); i++) {

		PlayerStatistics& ps = playerStatVec[i];
		int playerIndex = ps.getPlayerIndex();
		
		int wins = ps.getWins();
		int losses = ps.getLosses();
		double prec = ((double)wins / (wins + losses)) * 100;
	
		printElement(i + 1, colWidth);
		printElement(ps.getPlayerName(), colWidth);
		printElement(wins, colWidth);
		printElement(losses, colWidth);
		printElement(prec, colWidth);
		printElement(ps.getPointsFor(), colWidth);
		printElement(ps.getPointsAgainst(), colWidth);
	}

	// TODO: Print in the format

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
	while (currentGameIndex < m_totalGamesToPlay)
	{
		Game& currentGame = m_gameList[currentGameIndex];
		currentGame.startGame();
		
		// Notify result
		notifyGameResult(currentGame, currentGameIndex);
	

		// TODO: Fix !! Mistake !! - The last game maybe will over before the pervious game,
		// and then the round is not over
		// Notify end of round
		if (lastGameInRound(currentGameIndex))
		{
			m_cvRound.notify_one();
		}


		currentGameIndex = m_nextGameIndex++;
	}

	DBG(Debug::DBG_INFO, "Worker %d complete job", workerId);
}

bool Tournament::lastGameInRound(int gameIndex) const
{
	static size_t gamesInRound = m_algoDLLVec.size() / 2;
	return gameIndex % gamesInRound == 0;
}

void Tournament::notifyGameResult(Game& game, int gameIndex)
{
	assert(game.isGameOver());

	pair <int, int> playersIndexes = m_gamePlayerIndexes[gameIndex];
	int playerAIndex = playersIndexes.first;
	int playerBIndex = playersIndexes.second;

	pair<int, int> s = game.getScore();
	int playerAScore = s.first;
	int playerBScore = s.second;
	
	lock_guard<mutex> guard(m_statMutex);
	m_playerStat[playerAIndex].update(make_pair(playerAScore, playerBScore));
	m_playerStat[playerBIndex].update(make_pair(playerBScore, playerAScore));
}


ReturnCode Tournament::initSboardFiles(const string& directoryPath, vector<string>& sboardFiles)
{
	// load sboard file
	auto rc = Utils::getListOfFilesInDirectoryBySuffix(directoryPath, "sboard", sboardFiles, true);
	// ERROR means that the path exists but there is no file
	if (RC_ERROR == rc || sboardFiles.size() == 0)
	{
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
		cout << "Missing an algorithm (dll) file looking in path : " + directoryPath << endl;
		DBG(Debug::DBG_ERROR, "Missing an algorithm(dll) file looking in path : %s", directoryPath);
		return RC_ERROR;
	}

	return RC_SUCCESS;
}

ReturnCode Tournament::loadAllAlgoFromDLLs(const vector<string>& dllPaths)
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
		
		m_algoDLLVec.push_back(make_tuple(hDll, getAlgoFunc));
		m_playerStat.emplace_back(playerIndex, playerName);
		playerIndex++;
	}

	return RC_SUCCESS;
}

ReturnCode Tournament::loadBoards(vector<string>& boardsPaths)
{

	for (string boardPath : boardsPaths)
	{
		Board b;
		ReturnCode rc = b.loadBoardFromFile(boardPath);
	
		if (RC_SUCCESS != rc)
		{
			return rc;
		}
		
		// TODO: Check if move c'tor or copy c'tor
		m_boards.push_back(b);
	}
	
	DBG(Debug::DBG_INFO, "Finished to load dlls, total wad loaded: %d", m_boards.size());
	return RC_SUCCESS;
}

void Tournament::buildGameSchedule()
{
	size_t numOfBoards = m_boards.size();
	size_t numOfAlgos = m_algoDLLVec.size();

	auto allPossibilities = createSchedule(numOfAlgos);

	for (size_t boardIndex = 0; boardIndex < numOfBoards; boardIndex)
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
				
				// TODO: Change to unique ptr
				unique_ptr<IBattleshipGameAlgo> ibg1(get<1>(m_algoDLLVec[playerAIndex])());
				unique_ptr<IBattleshipGameAlgo> ibg2(get<1>(m_algoDLLVec[playerBIndex])());
				
				//IBattleshipGameAlgo* ibg1 = get<1>(m_algoDLLVec[playerAIndex])();
				//IBattleshipGameAlgo* ibg2 = get<1>(m_algoDLLVec[playerBIndex])();
				m_gameList.emplace_back(b, ibg1, ibg2);
				m_gamePlayerIndexes.emplace_back(make_pair(playerAIndex, playerBIndex));
			}
		}
	}
}

vector<vector<pair<int, int>>> Tournament::createSchedule(int numOfAlgos)
{
	// Credit: https://stackoverflow.com/questions/1037057/how-to-automatically-generate-a-sports-league-schedule


	vector<int> list(numOfAlgos);
	for (int i = 0; i < numOfAlgos; i++)
	{
		list[i] = i;
	}

	vector<vector<pair<int,int>>> result;
	
	// Padding
	if (numOfAlgos % 2 == 1)
		list.push_back(-1);

	for (int i = 0; i < list.size() - 1; i++)
	{
		size_t mid = list.size() / 2;
		vector<int> l1(list.begin(), list.begin() + mid);
		vector<int> l2(list.begin() + mid, list.end());
		l2.reserve(l2.size());

		// Switch sides after each round
		if (i % 2 == 1)
		{
			result.push_back(zip(l1, l2));
		}
		else
		{
			result.push_back(zip(l2, l1));

		}

		auto last = list[list.size() - 1];
		list.pop_back();
		list.insert(list.begin() + 1, last);
	}


	for (auto cycleVec : result)
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
	static size_t gamesInRound = m_algoDLLVec.size() / 2;

	
	while (m_isTournamentFinished)
	{
		int indexLastGameInRound = ((m_nextRoundToReport + 1) * gamesInRound) - 1;
		Game& lastGameInRound = m_gameList[indexLastGameInRound];
	
		unique_lock<mutex> lk(m_statMutex);
		m_cvRound.wait(lk, lastGameInRound.isGameOver());

		if (!m_isTournamentFinished)
			this->printResult();

	}
}


