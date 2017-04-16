#pragma once

#include <list>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "PlayerAlgoFactory.h"
#include "Board.h"
#include "PlayerAlgo.h"

using namespace std;

class Game
{
private:
	/**
	 * @Details		Assistent enum for recursive funtions of analayzing input board 
	 */
	enum class ShipLengthDirection
	{
		VERTICAL, 
		HORIZONTAL
	};

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
	PlayerAlgo*				m_players[MAX_PLAYER];			// the players
	vector<int>				m_numOfShipsPerPlayer;			// how many ships each player has
	bool					m_foundAdjacentShips;			// need to print Adjacency error per player
	vector<vector<bool>>	m_wrongSizeOrShapePerPlayer;	// 2 players X 4 ships

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
	Game &operator=(Game const&) { return *this; }

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
	void validateBoard(char** initBoard);

	/**
	* @Details		read board from file and fill initBoard
	* @param		filePath - board file path
	* @param		initBoard - matrix for output
	*/
	void readSBoardFile(string filePath, char** initBoard);

	/**
	* @Details		find sboard file by given directory, search first file with extension *.sboard
	* @param		filesPath - input directory
	* @param		sboardFileName - reference for valid input file (if return code is success)
	* @return		ReturnCode::RC_ERROR - in case file not found
	*				ReturnCode::RC_SUCCESS - return in sboardFileName the file path
	*/
	ReturnCode getSboardFileNameFromDirectory(string filesPath, string& sboardFileName);

	/**
	* @Details		find attack files by given directory, search files with extension *.attack-a and *.attack-b
	* @param		filesPath - input directory
	* @param		attackFilePerPlayer - reference for vector of valid input file (if return code is success)
	* @return		ReturnCode::RC_ERROR - in case file not found
	*				ReturnCode::RC_SUCCESS - return in attackFilePerPlayer the vector of files paths
	*/
	ReturnCode getattackFilesNameFromDirectory(string filesPath, vector<string>& attackFilePerPlayer);
	
	/**
	 * @Details		finds the sboard and attack files from given directory
	 * @param		filesPath - given directory
	 * @param		sboardFile - will contain the sboard file name
	 * @param		attackFilePerPlayer - will contain the attack file per player
	 * @return		RC_ERROR if path is wrong or files are missing, RC_SUCCESS otherwise
	 */
	ReturnCode initFilesPath(string& filesPath, string& sboardFile, vector<string>& attackFilePerPlayer);

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
	AttackRequestCode requestAttack(pair<int, int> req) const;

	/**
	 * @Details		Wrapper for recursive function for recieving current ship length in specific cell
	 * @param		initBoard - the board after parsing
	 * @param		expectedShip - ship to test
	 * @param		row - current row to check
	 * @param		col - current col to check
	 * @param		direction - in which direction to go in the next recursive call
	 */
	int getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthDirection direction);
	
	/**
	* @Details		recursive function for recieving current ship length in specific cell
	* @param		initBoard - the board after parsing
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	int getShipLengthHorizontal(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction);

	/**
	* @Details		Wrapper for recursive function for recieving current ship length in specific cell
	* @param		initBoard - the board after parsing
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	int getShipLengthVertical(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction);

	/**
	 * @Details		Checks if  adjacency in specific cell is valid
	 * @param		i - row ro check
	 * @param		j - col to check
	 * @return		true if valid, false otherwise
	 */
	bool isAdjacencyValid(char** initBoard, int i/*row*/, int j/*col*/);

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
	ReturnCode init(string filesPath, bool isQuiet, int delay);
	
	/*
	 * @Details		dtart current game
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
};
