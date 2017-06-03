#pragma once

#include "Game.h"
#include <atomic>
#include <thread>


class Tournament
{
private:
	
	
	vector<Game> m_gameList;
	atomic<int> m_nextGameIndex = 0;
	atomic<int> m_totalGamesToPlay = 0;
	
	vector<Board> m_boards;

	vector<tuple<HINSTANCE, GetAlgoFuncType>> m_algoDLLVec;						// dll tuples <hanfle, algo func>


	ReturnCode init(const string& directoryPath, int numOfThreads);
	ReturnCode startTournament(int numOfThreads);
	void executeGame();

	ReturnCode initSboardFiles(const string& directoryPath, vector<string>& sboardFiles);
	ReturnCode initDLLFilesPath(const string& directoryPath, vector<string>& dllFiles);
	ReturnCode loadAllAlgoFromDLLs(const vector<string>& dllPaths);
	ReturnCode loadBoards(vector<string>& boardsPaths);
	ReturnCode buildGameSchedule();
	vector<vector<pair<int, int>>> createSchedule(int numOfAlgos);

	thread reporterThread() {
		return std::thread([=] { reportResult(); });
	}

	vector<pair<int, int>> zip(vector<int> first, vector<int> second);
	void reportResult();

public:

	static Tournament& instance()
	{
		static Tournament tournamentInstance;
		return tournamentInstance;
	}


};
