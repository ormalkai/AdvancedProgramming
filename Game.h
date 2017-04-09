#pragma once

#include <list>
#include "Cell.h"
#include "IBattleshipGameAlgo.h"
#include "Utils.h"
#include "PlayerAlgoFactory.h"
#include <vector>
#include "Board.h"

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

	static PlayerAlgoFactory m_playerFactory;

	Board m_board;

	vector<IBattleshipGameAlgo*> m_players;
	map<IBattleshipGameAlgo, Board> m_boards;

	map<char, int> m_shipToExpectedLen; // conversion map between ship type to expected length
	vector<int> m_numOfShipsPerPlayer;

	// ERRORS data structures
	vector<vector<bool>> m_wrongSizeOrShapePerPlayer; // 2 players X 4 ships TODO init in game init
	bool m_foundAdjacentShips; // need to print Adjacency error per player TODO init in game init
	int m_rows;
	int m_cols;

	Game(int rows = BOARD_ROW_SIZE, int cols = BOARD_ROW_SIZE) : m_rows(rows), m_cols(cols)
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
	void readSBoardFile(std::string filePath, char** initBoard);
	ReturnCode parseBoardFile(std::string filePath);
	ReturnCode initListPlayers();
	void initErrorDataStructures();
	void initExpectedShipLenMap();

	int getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, ShipLengthDirection direction);
	bool isAdjacencyValid(char** initBoard, int i/*row*/, int j/*col*/);

public:

	Game(const Game&) = delete;
	void operator=(Game const&) = delete;

	/*
		Parse board file and build board matrix
	*/

	ReturnCode init(string boardPathFile);
	ReturnCode startGame();

	static Game& getInstance()
	{
		static Game gameInstance;
		return gameInstance;
	}
};
