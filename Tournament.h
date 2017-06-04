#pragma once

#include "Game.h"
#include <atomic>
#include <thread>
#include "PlayerStatistics.h"
#include <mutex>


class Tournament
{
private:
		
	bool m_isTournamentFinished;
	vector<Game> m_gameList;
	vector<pair<int, int>> m_gamePlayerIndexes;
	mutex m_statMutex;

	mutex m_roundMutex;
	bool m_isRoundFinished;
	condition_variable m_cvRound;
	int m_nextRoundToReport = 0;

	atomic<int> m_nextGameIndex = 0;
	atomic<int> m_totalGamesToPlay = 0;
	
	vector<Board> m_boards;
	vector<PlayerStatistics> m_playerStat;
	
	vector<tuple<HINSTANCE, GetAlgoFuncType>> m_algoDLLVec;						// dll tuples <hanfle, algo func>

	struct SortByAge
	{
		bool operator() (PlayerStatistics const & L, PlayerStatistics const & R) const { return L.getWins() < R.getWins(); }
	};


	ReturnCode init(const string& directoryPath, int numOfThreads);
	ReturnCode startTournament(int numOfThreads);
	void printResult() const;
	void executeGame(int workerId);
	bool lastGameInRound(int gameIndex) const;
	void notifyGameResult(Game& game, int gameIndex);

	static ReturnCode initSboardFiles(const string& directoryPath, vector<string>& sboardFiles);
	static ReturnCode initDLLFilesPath(const string& directoryPath, vector<string>& dllFiles);
	ReturnCode loadAllAlgoFromDLLs(const vector<string>& dllPaths);
	ReturnCode loadBoards(vector<string>& boardsPaths);
	void buildGameSchedule();
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
