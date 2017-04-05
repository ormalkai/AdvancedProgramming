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
static class Game {

private:
	static PlayerAlgoFactory m_playerFactory;

	Board m_board;

	std::vector<IBattleshipGameAlgo*> m_players;
	std::map<IBattleshipGameAlgo, Board> m_boards;
	std::map<char, int> m_shipToExpectedLen;
	vector<int> m_numOfShipsPerPlayer;

	// ERRORS data structures
	vector<vector<bool>> m_wrongSizeOrShapePerPlayer; // 2 players X 4 ships TODO init in game init
	vector<bool> m_adjacency; // need to print Adjacency error per player TODO init in game init
	int m_rows;
	int m_cols;

	Game();
	~Game();
	
	ReturnCode parseBoardFile(std::string filePath, int rows, int cols);
	ReturnCode initListPlayers();
	
	int getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, GetShipLengthDirection direction);

public:
	
	Game(Game& const) = delete;
	void operator=(Game const&) = delete;

	/*
		Parse board file and build board matrix
	*/
	
	ReturnCode init(std::string boardPathFile, int row, int col);
	ReturnCode startGame();

	static Game& getInstance()
	{
		static Game gameInstance;
		return gameInstance;
	}


};
