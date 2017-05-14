#pragma once

#include <list>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "Board.h"

using namespace std;


typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();

class Game
{
private:
	

	/**
	* @Details		Assistent enum for recursive funtions of analayzing input board
	*/
	enum class ShipLengthSecondDirection
	{
		FORWARD,
		BACKWORD
	};


	bool					m_isQuiet;						// Is quiet run, for print simulation game
	Board					m_board;						// current board of the game
	int						m_rows;							// number of rows in the board
	int						m_cols;							// number of columns in the board
	int						m_currentPlayerIndex;			// he id of the next player to attack
	int						m_otherPlayerIndex;				// the id of the other playre to attack
	IBattleshipGameAlgo*	m_players[MAX_PLAYER];			// the players
	vector<int>				m_numOfShipsPerPlayer;			// how many ships each player has
	bool					m_foundAdjacentShips;			// need to print Adjacency error per player
	vector<vector<bool>>	m_wrongSizeOrShapePerPlayer;	// 2 players X 4 ships
	
	int						m_playerScore[NUM_OF_PLAYERS] = { 0 };				// player's score
	int						m_victoriesPerPlayer[NUM_OF_PLAYERS] = { 0 };		// player's num of victories
	bool					m_finishedAttackPlayer[NUM_OF_PLAYERS] = { false }; // finished attack inidicator per player
	vector<tuple<HINSTANCE, GetAlgoFuncType>> m_algoDLLVec;						// dll tuples <hanfle, algo func>

	/**
	 * @Details		game constructor
	 */
	Game(int rows = BOARD_ROW_SIZE, int cols = BOARD_ROW_SIZE);

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
	Game(Game&&) = delete;

	/**
	* @Details		check if there is a ship with inappropriate size
	* @return		true if at least one ship found, o/w false
	*/
	bool checkWrongSizeOrShape() const;

	/**
	* @Details		check each player has exactly number of ship as expected
	* @return		false if at least one player has a unexpected ships amount, o/w true
	*/
	bool checkNumberOfShips() const;
	
	/**
	* @Details		check if there is adjacent ships [ships w/o space cell between them]
	* @return		true if at least two ships are neighboring, o/w false
	*/
	bool checkAdjacentShips() const;
	
	/**
	* @Details		check if all ships in appropriate size, shape not adjacent, and the amount ships of each player is as expected
	* @return		true if all the condtions are accepted, o/w false
	*/
	bool checkErrors() const;
	
	
	/**
	* @Details		check the board by given constaints: ship size, shape, neighbors
	*				and fill error data structure.
	* @param		initBoard - input board to verify
	*/
	void validateBoard(const char** initBoard);

	/**
	* @Details		read board from file and fill initBoard
	* @param		filePath - board file path
	* @param		initBoard - matrix for output
	* @return		ReturnCode::RC_ERROR - in case file not found
	*				ReturnCode::RC_SUCCESS - return in sboardFileName the file path
	*/
	ReturnCode readSBoardFile(const string filePath, char**const initBoard) const;

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
	 * @param		sboardFile - will contain the sboard file name
	 * @param		dllPerPlayer - will contain the attack file per player
	 * @return		RC_ERROR if path is wrong or files are missing, RC_SUCCESS otherwise
	 */
	static ReturnCode initFilesPath(const string& filesPath, string& sboardFile, vector<string>& dllPerPlayer);

	/**
	 * @Details		Parses the sboard file and validates it
	 * @param		sboardFileName - path to input sboard file
	 * @param		initBoard - the parsed board will be filled in initBoard for validations
	 * @return		RC_ERROR if one of the valdiations according to Spec failed, RC_SUCCESS otherwise
	 */
	ReturnCode parseBoardFile(string sboardFileName, char** initBoard);

	/**
	 * @Details		Init errors dataStructures - no errors
	 */
	void initErrorDataStructures();

	/**
	 * @Details		validates current attack
	 * @param		req - curren attacks
	 * @return		ARC_FINISH_REQ if no attack, ARC_ERROR if attack is illegal, ARC_SUCCESS otherwise
	 */
	AttackRequestCode requestAttack(const pair<int, int>& req);

	/**
	 * @Details		Wrapper for recursive function for recieving current ship length in specific cell
	 * @param		initBoard - the board after parsing
	 * @param		expectedShip - ship to test
	 * @param		row - current row to check
	 * @param		col - current col to check
	 * @param		direction - in which direction to go in the next recursive call
	 */
	static int getShipLength(const char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipDirection direction);
	
	/**
	* @Details		recursive function for recieving current ship length in specific cell
	* @param		initBoard - the board after parsing
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	static int getShipLengthHorizontal(const char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction);

	/**
	* @Details		Wrapper for recursive function for recieving current ship length in specific cell
	* @param		initBoard - the board after parsing
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	static int getShipLengthVertical(const char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction);

	/**
	 * @Details		Checks if  adjacency in specific cell is valid
	 * @param		i - row ro check
	 * @param		j - col to check
	 * @return		true if valid, false otherwise
	 */
	static bool isAdjacencyValid(const char** initBoard, int i/*row*/, int j/*col*/);

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
	 * @param		filesPath - path to search for input files
	 * @param		isQuiet - print simulation ot not
	 * @param		delay - delay between attacks in simulations
	 */
	ReturnCode init(const string filesPath, bool isQuiet, int delay);
	

	/**
	 * @Details		start current game
	 */
	void startGame();

	/**
	 * @Details		singleton class, get current instance.
	 */
	static Game& instance()
	{
		static Game gameInstance;
		return gameInstance;
	}

	/**
	* @Details		Load dlls by given pathes to data memberes (tuples of handle and pointer to function)
	* @param		dllPaths - vector of paths of the dlls
	* @Return		ReturnCode::RC_ERROR - in case loading error
	*				ReturnCode::RC_SUCCESS - return in all dlls load successfully  
	*/
	ReturnCode loadAllAlgoFromDLLs(const vector<string>& dllPaths);

	/**
	* @Details		Free board resources
	* @param		board - board to free
	*/
	void freePlayerBoard(char** board) const;
};
