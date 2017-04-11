#pragma once

#include <list>
#include "Cell.h"
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "PlayerAlgoFactory.h"
#include <vector>
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

	Game(int rows = BOARD_ROW_SIZE, int cols = BOARD_ROW_SIZE) : m_rows(rows), m_cols(cols), m_foundAdjacentShips(false)
	{
	}

	~Game()
	{
	}

	bool checkWrongSizeOrShape() const;
	bool checkNumberOfShips() const;
	bool checkAdjacentShips() const;
	bool checkErrors() const;
	void validateBoard(char** initBoard);
	void readSBoardFile(string filePath, char** initBoard);
	ReturnCode getSboardFileNameFromDirectory(string filesPath, string& sboardFileName);
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
public:

	Game(const Game&) = delete;
	void operator=(Game const&) = delete;

	/*
		Parse board file and build board matrix
	*/
	ReturnCode init(string filesPath, bool isQuiet, int delay);
	
	ReturnCode startGame();

	static Game& getInstance()
	{
		static Game gameInstance;
		return gameInstance;
	}
};
