#include <string>
#include <iostream>
#include "Tournament.h"
#include "Game.h"
#include "Debug.h"
#include <thread>
#include <cassert>


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

	// TODO 
	// Load games to data structure
	// And sort, Build games order


	return RC_SUCCESS;
}


ReturnCode Tournament::startTournament(int numOfThreads)
{

	

	return RC_SUCCESS;

}

void Tournament::executeGame() {

	// There is more game to play

	int currentGameIndex = m_nextGameIndex++;
	while (currentGameIndex < m_totalGamesToPlay)
	{
		m_gameList[currentGameIndex].startGame();
		currentGameIndex = m_nextGameIndex++;
	}


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

		m_algoDLLVec.push_back(make_tuple(hDll, getAlgoFunc));
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

ReturnCode Tournament::buildGameSchedule()
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

				Board b = m_boards[boardIndex];
				
				// TODO: Change to unique ptr
				unique_ptr<IBattleshipGameAlgo> ibg1(get<1>(m_algoDLLVec[playerAIndex])());
				unique_ptr<IBattleshipGameAlgo> ibg2(get<1>(m_algoDLLVec[playerBIndex])());
				
				//IBattleshipGameAlgo* ibg1 = get<1>(m_algoDLLVec[playerAIndex])();
				//IBattleshipGameAlgo* ibg2 = get<1>(m_algoDLLVec[playerBIndex])();
				m_gameList.emplace_back(b, ibg1, ibg2);
				
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
		int mid = list.size() / 2;
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

}


