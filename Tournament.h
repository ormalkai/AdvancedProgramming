#pragma once

#include <future>
#include <atomic>
#include <thread>
#include <mutex>
#include "Game.h"
#include "PlayerStatistics.h"


class Tournament
{
private:
		
	bool										m_isTournamentFinished;
	vector<Game>								m_gameList;
	vector<pair<int, int>>						m_gamePlayerIndexes;

	mutex										m_reporterMutex;
	condition_variable							m_cvRound;
	int											m_nextRoundToReport = 0;

	atomic<int>									m_nextGameIndex = 0;
	
	vector<Board>								m_boards;
	vector<PlayerStatistics>					m_playerStat;
	
	vector<tuple<HINSTANCE, GetAlgoFuncType>>	m_algoDLLVec;						// dll tuples <hanfle, algo func>

	int											m_maxNameLen = 0;

	struct PlayerGameResult
	{
		int			m_playerId; // this report is for player m_playerId
		int			m_pointsFor;	// how many points the player won in the game
		int			m_pointsAgainst;	// how many points the other player won in the game
		WinLoseTie	m_isWon;
		PlayerGameResult() : m_playerId(-1), m_pointsFor(0), m_pointsAgainst(0), m_isWon(TIE){}
		PlayerGameResult(int playerId, int pointsFor, int pointsAgainst , WinLoseTie isWon) :
		m_playerId(playerId), m_pointsFor(pointsFor), m_pointsAgainst(pointsAgainst), m_isWon(isWon) {}
	};

	struct RoundResults
	{
		int					m_playersFinished;	// holds how many players finished their games in this round, atomic since this is the index to insert in the round
		vector<PlayerGameResult>	m_results;			// holds all the results in this round
		mutex				m_roundMutex;		// lock for each round between workers and reporter
		RoundResults() : m_playersFinished(0) {}
		RoundResults(const RoundResults &other) : m_roundMutex()
		{
			m_playersFinished = other.m_playersFinished;
			m_results = other.m_results;
		}
	};

	vector<RoundResults>	m_playersResultsPerRound; // the main database of the tournament
	int						m_finishedRounds; // indicates how many rounds are finished for printing results on progress
	int						m_printedRounds; // how many rounds are actually printed 

	// Updating this vector must be against lock and it can not be vector of atomic int
	// take the following example in consider
	// there are only 2 players with one board, meaning there are total 2 games
	// ThreadA finished game between player 1 and player 2
	// ThreadA exucutes inc to number of games of player 1
	// ThreadB finished the game between player 2 and player 1
	// threadB executes inc to number of games of player 2
	// ThreadA continues
	// ThreadB continues
	// The result:  ThreadA game is game1 for player1 and game2 for player2
	//				ThreadB game is game1 for player2 and game2 for player1
	vector<int>		m_gameFinishedByEachPlayer; // needed for updating m_playersResultsPerRound in the correct round
	mutex			m_gameFinishedByEachPlayerMutex;

	struct SortByWins
	{
		bool operator() (PlayerStatistics & L, PlayerStatistics & R) const { return L.getWins() > R.getWins(); }
	};


	
	void initPlayerResultsPerRound();
	
	void printResult() const;
	
	template <class T>
	static void printElement(T t, const int& width);
	
	void executeGame(int workerId);
	void notifyGameResult(Game& game, int gameIndex);

	void updateRoundResultForPlayer(int RoundForPlayer, PlayerGameResult& playerResult);

	static ReturnCode initSboardFiles(const string& directoryPath, vector<string>& sboardFiles);
	static ReturnCode initDLLFilesPath(const string& directoryPath, vector<string>& dllFiles);
	ReturnCode loadAllAlgoFromDLLs(const vector<string>& dllPaths, promise<ReturnCode>&& p);
	ReturnCode loadBoards(vector<string>& boardsPaths, promise<ReturnCode>&& p);
	void buildGameSchedule();
	vector<vector<pair<int, int>>> createSchedule(int numOfAlgos);

	thread reporterThread() {
		return std::thread([=] { reportResult(); });
	}

	vector<pair<int, int>> zip(vector<int> first, vector<int> second);
	void reportResult();

	void updateStatAndPrintRound();

public:
	ReturnCode init(const string& directoryPath);
	void startTournament(int numOfThreads);
	static Tournament& instance()
	{
		static Tournament tournamentInstance;
		return tournamentInstance;
	}


};
