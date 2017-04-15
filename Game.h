#pragma once

#include <list>
#include <vector>
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "PlayerAlgoFactory.h"
#include "Board.h"
#include "PlayerAlgo.h"

using namespace std;

// enum for players
class Game
{
private:
	enum class ShipLengthDirection
	{
		VERTICAL, 
		HORIZONTAL
	};
	enum class ShipLengthSecondDirection
	{
		FORWARD,
		BACKWORD
	};


	static PlayerAlgoFactory m_playerFactory;
	bool m_isQuiet;

	Board m_board;
	int m_currentPlayerIndex;
	int m_otherPlayerIndex;

	PlayerAlgo* m_players[MAX_PLAYER];
	//map<PlayerAlgo*, Board> m_boards;

	map<char, int> m_shipToExpectedLen; // conversion map between ship type to expected length
	vector<int> m_numOfShipsPerPlayer;

	// ERRORS data structures
	vector<vector<bool>> m_wrongSizeOrShapePerPlayer; // 2 players X 4 ships TODO init in game init
	
	int m_rows;
	int m_cols;
	bool m_foundAdjacentShips; // need to print Adjacency error per player TODO init in game init
	int m_numOfAliveCellsOnBoard;

	Game(int rows = BOARD_ROW_SIZE, int cols = BOARD_ROW_SIZE) : m_rows(rows), m_cols(cols), m_foundAdjacentShips(false)
	{
	}

	~Game()
	{
	}

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
	
	
	ReturnCode initFilesPath(string& filesPath, string& sboardFile, vector<string>& attackFilePerPlayer);
	ReturnCode parseBoardFile(string sboardFileName, char** initBoard);
	ReturnCode initListPlayers();
	void initErrorDataStructures();
	AttackRequestCode requestAttack(pair<int, int> req) const;
	int getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthDirection direction);
	int getShipLengthHorizontal(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction);
	int getShipLengthVertical(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction);
	bool isAdjacencyValid(char** initBoard, int i/*row*/, int j/*col*/);
	void proceedToNextPlayer() { m_currentPlayerIndex = (++m_currentPlayerIndex) % MAX_PLAYER;  m_otherPlayerIndex = (++m_otherPlayerIndex) % MAX_PLAYER; }
	void printSummary() const;
	
	
	public:

	Game(const Game&) = delete;
	void operator=(Game const&) = delete;

	/*
		Parse board file and build board matrix
	*/
	ReturnCode init(string filesPath, bool isQuiet, int delay);
	
	void startGame();

	static Game& getInstance()
	{
		static Game gameInstance;
		return gameInstance;
	}
};
