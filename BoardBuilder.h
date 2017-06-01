#pragma once
#include "Utils.h"
#include <memory>

class BoardBuilder
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

	string					m_filesPath;
	string					m_sboardFilePath;

	vector<int>				m_numOfShipsPerPlayer;			// how many ships each player has
	bool					m_foundAdjacentShips;			// need to print Adjacency error per player
	vector<vector<bool>>	m_wrongSizeOrShapePerPlayer;	// 2 players X 4 ships

	vector<vector<vector<char>>>	m_initBoard;
	int						m_rows;
	int						m_cols;
	int						m_depth;


public:
	BoardBuilder(const string filesPath);
	~BoardBuilder();

	/**
	* @Details		Init errors dataStructures - no errors
	*/
	void initErrorDataStructures();

	/**
	* @Details		Parses the sboard file and validates it
	* @return		RC_ERROR if one of the valdiations according to Spec failed, RC_SUCCESS otherwise
	*/
	ReturnCode parseBoardFile();

	/**
	* @Details		read board from file and fill initBoard
	*/
	void readSBoardFile(ifstream& sboard);

	/**
	 *
	 */
	ReturnCode getBoardDimensionsFromFile(ifstream& sboard);

	/**
	* @Details		check the board by given constaints: ship size, shape, neighbors
	*				and fill error data structure.
	* @param		initBoard - input board to verify
	*/
	void validateBoard();

	/**
	* @Details		Checks if  adjacency in specific cell is valid
	* @param		i - row ro check
	* @param		j - col to check
	* @return		true if valid, false otherwise
	*/
	bool isAdjacencyValid(int d/*depth*/, int i/*row*/, int j/*col*/) const;

	/**
	* @Details		Wrapper for recursive function for recieving current ship length in specific cell
	* @param		initBoard - the board after parsing
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	int getShipLength(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipDirection direction);

	/**
	* @Details		recursive function for recieving current ship length in specific cell
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	int getShipLengthHorizontal(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const;

	/**
	* @Details		Wrapper for recursive function for recieving current ship length in specific cell
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	int getShipLengthVertical(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const;

	/**
	* @Details		Wrapper for recursive function for recieving current ship length in specific cell
	* @param		expectedShip - ship to test
	* @param		row - current row to check
	* @param		col - current col to check
	* @param		direction - in which direction to go in the next recursive call
	*/
	int getShipLengthDepth(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const;
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

	void initInitBoard();
};

