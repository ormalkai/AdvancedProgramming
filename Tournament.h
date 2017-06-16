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

	Tournament() :
		m_isTournamentFinished(false),
		m_nextRoundToReport(0),
		m_nextGameIndex(0),
		m_maxNameLen(0),
		m_finishedRounds(0),
		m_printedRounds(0)
	{
	}

	Tournament(const Tournament&) = delete;
	Tournament(Tournament&&) = delete;
	Tournament& operator=(const Tournament &) = delete;
	~Tournament() = default;

	bool m_isTournamentFinished; // is tournament finished
	vector<Game> m_gameList; // list of all games in the tournament
	vector<pair<int, int>> m_gamePlayerIndexes; // Ids of every player in each game
	mutex m_reporterMutex; // reporter mutex for conditional variable
	condition_variable m_cvRound; // conditional variable for reporter in end of round
	int m_nextRoundToReport; // the number of the next reound to report
	atomic<int> m_nextGameIndex; // next index in game list to play
	vector<Board> m_boards; // all boards in the tournament
	vector<PlayerStatistics> m_playerStat; // table of managing statistics of the tournament
	vector<tuple<HINSTANCE, GetAlgoFuncType>> m_algoDLLVec; // dll tuples <hanfle, algo func>
	int m_maxNameLen; // for printing pretty table

	/**
	 * @Details		struct for managing games results
	 */
	struct PlayerGameResult
	{
		int m_playerId; // this report is for player m_playerId
		int m_pointsFor; // how many points the player won in the game
		int m_pointsAgainst; // how many points the other player won in the game
		WinLoseTie m_isWon;

		PlayerGameResult() : m_playerId(-1), m_pointsFor(0), m_pointsAgainst(0), m_isWon(TIE)
		{
		}

		PlayerGameResult(int playerId, int pointsFor, int pointsAgainst, WinLoseTie isWon) :
			m_playerId(playerId), m_pointsFor(pointsFor), m_pointsAgainst(pointsAgainst), m_isWon(isWon)
		{
		}
	};

	/**
	 * @Details		struct for managing round results
	 */
	struct RoundResults
	{
		int m_playersFinished; // holds how many players finished their games in this round, atomic since this is the index to insert in the round
		vector<PlayerGameResult> m_results; // holds all the results in this round
		mutex m_roundMutex; // lock for each round between workers and reporter
		RoundResults() : m_playersFinished(0)
		{
		}

		RoundResults(const RoundResults& other) : m_roundMutex()
		{
			m_playersFinished = other.m_playersFinished;
			m_results = other.m_results;
		}
	};

	vector<RoundResults> m_playersResultsPerRound; // the main database of the tournament
	int m_finishedRounds; // indicates how many rounds are finished for printing results on progress
	int m_printedRounds; // how many rounds are actually printed 

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
	vector<int> m_gameFinishedByEachPlayer; // needed for updating m_playersResultsPerRound in the correct round
	mutex m_gameFinishedByEachPlayerMutex; // locak between workers for incrementing game per player atomically in the context of game

	struct SortByWins
	{
		bool operator()(PlayerStatistics& L, PlayerStatistics& R) const { return L.getWins() > R.getWins(); }
	};


	/**
	 * @Details		init main DB of managing game results in rounds
	 */
	void initPlayerResultsPerRound();

	/**
	 * @Details		prints round result
	 */
	void printResult() const;

	/**
	 * @Details		prints one element in the table
	 */
	template <class T>
	static void printElement(T t, const int& width);

	/**
	 * @Details		main function of workers, executes one or more games
	 */
	void executeGame(int workerId);

	/**
	 * @Details		After every game update the main DB for reporter
	 */
	void notifyGameResult(Game& game, int gameIndex);

	/**
	 * @Details		updates the main DB for one player
	 */
	void updateRoundResultForPlayer(int RoundForPlayer, PlayerGameResult& playerResult);

	/**
	 * @Details		gets all sboard files from directory
	 */
	static ReturnCode initSboardFiles(const string& directoryPath, vector<string>& sboardFiles);

	/**
	 * @Details		gets all dll files from directory
	 */
	static ReturnCode initDLLFilesPath(const string& directoryPath, vector<string>& dllFiles);

	/**
	 * @Details		loads all algos
	 */
	void loadAllAlgoFromDLLs(const vector<string>& dllPaths, promise<ReturnCode>&& p);

	/**
	 * @Details		loads all boards
	 */
	void loadBoards(vector<string>& boardsPaths, promise<ReturnCode>&& p);

	/**
	 * @Details		creates game schedule in leage order
	 */
	void buildGameSchedule();
	static vector<vector<pair<int, int>>> createSchedule(int numOfAlgos);

	/**
	 * @Details		reporter thread
	 */
	thread reporterThread()
	{
		return std::thread([=] { reportResult(); });
	}

	/**
	 * @Details		gets two vectors and returns vector of pairs
	 *				where the the ith pair is the ith var in vector 1
	 *				and the ith var in vector 2
	 */
	static vector<pair<int, int>> zip(vector<int> first, vector<int> second);

	/**
	 * @Details		main function of reproter, reports result
	 */
	void reportResult();

	/**
	 * @Details		reporter prints round
	 */
	void updateStatAndPrintRound();

public:
	/**
	 * @Details		init all internal DS
	 */
	ReturnCode init(const string& directoryPath);

	/**
	 * @Details		starts the tournament
	 */
	void startTournament(int numOfThreads);

	/**
	 * @Details		singleton implementation
	 */
	static Tournament& instance()
	{
		static Tournament tournamentInstance;
		return tournamentInstance;
	}
};
