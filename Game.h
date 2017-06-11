#pragma once

#include <list>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "Board.h"

#define TIE_WINNER_ID (-1)

using namespace std;


typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();

class Game
{
private:
	
	bool							m_isGameOver;
	bool							m_isQuiet;						// Is quiet run, for print simulation game
	Board							m_board;						// current board of the game
	int								m_rows;							// number of rows in the board
	int								m_cols;							// number of columns in the board
	int								m_depth;						// number of depth in the board
	int								m_currentPlayerIndex;			// he id of the next player to attack
	int								m_otherPlayerIndex;				// the id of the other playre to attack
	int								m_winner;						// the id of the winner
	vector<unique_ptr<IBattleshipGameAlgo>> m_players;			// the players
	int						m_playerScore[NUM_OF_PLAYERS] = { 0 };				// player's score
	// TODO unused?
	int						m_victoriesPerPlayer[NUM_OF_PLAYERS] = { 0 };		// player's num of victories
	bool					m_finishedAttackPlayer[NUM_OF_PLAYERS] = { false }; // finished attack inidicator per player

	// Giant comment
public:
	/**
	 * @Details		game constructor
	 */
	Game(Board& board, unique_ptr<IBattleshipGameAlgo> algoA, unique_ptr<IBattleshipGameAlgo> algoB);
	/**
	* @Details		game destructor
	*/
	~Game();
	
	/**
	* @Details		copy constructor is deleted
	*/
	Game(const Game&) = delete;
	
	/**
	 * @Details		ignore = operator
	 */
	Game &operator=(const Game&) = delete;

	/**
	* @Details		ignore = operator
	*/
	Game(Game&&) = default;

	/**
	* @Details		find sboard file by given directory, search first file with extension *.sboard
	* @param		filesPath - input directory
	* @param		sboardFileName - reference for valid input file (if return code is success)
	* @return		ReturnCode::RC_ERROR - in case file not found
	*				ReturnCode::RC_SUCCESS - return in sboardFileName the file path
	*/
	static ReturnCode getSboardFileNameFromDirectory(const string filesPath, string& sboardFileName);

	/**
	* @Details		finds the sboard and attack files from given directory
	* @param		filesPath - given directory
	* @param		dllPerPlayer - will contain the attack file per player
	* @return		RC_ERROR if path is wrong or files are missing, RC_SUCCESS otherwise
	*/
	static ReturnCode initDLLFilesPath(const string& filesPath, vector<string>& dllPerPlayer);

	/**
	 * @Details		validates current attack
	 * @param		req - curren attacks
	 * @return		ARC_FINISH_REQ if no attack, ARC_ERROR if attack is illegal, ARC_SUCCESS otherwise
	 */
	AttackRequestCode requestAttack(Coordinate& req);

	/**
	 * @Details		proceed To Next Player after attack
	 */
	void proceedToNextPlayer() { m_currentPlayerIndex = (++m_currentPlayerIndex) % MAX_PLAYER;  m_otherPlayerIndex = (++m_otherPlayerIndex) % MAX_PLAYER; }
	
	/**
	 * @Details		print summary of the current game
	 */
	void printSummary() const;
	
	
public:
	/**
	 * @Details		Parse files and build board matrix
	 *
	 */
	void init();
	/**
	 * @Details		start current game
	 */
	void startGame();
	bool isGameOver() const;

	struct GameResult
	{
		int		m_winnerIndex; // this report is for player m_playerId
		int		m_playerAPoints;	// how many points the player won in the game
		int		m_playerBPoints;	// how many points the other player won in the game
		GameResult(int winnerId, int playerAPoints, int playerBPoints) : 
		m_winnerIndex(winnerId), m_playerAPoints(playerAPoints), m_playerBPoints(playerBPoints) {}
	};
	GameResult getGameResult();

	/**
	 * @Details		singleton class, get current instance.
	 */
//	static Game& instance()
//	{
//		static Game gameInstance;
//		return gameInstance;
//	}

	
	/**
	* @Details		finds the sboard and attack files from given directory
	* @return		RC_ERROR if path is wrong or files are missing, RC_SUCCESS otherwise
	*/
	static ReturnCode initSboardFilePath(const string& filesPath, string& sboardFilePath);
};
