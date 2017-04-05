

#include <limits.h>
#include "Game.h"
#include "Utils.h"


#define INIT_BOARD_ROW_SIZE (BOARD_ROW_SIZE + 2)
#define INIT_BOARD_COL_SIZE (BOARD_COL_SIZE + 2)
#define SHIPS_PER_PLAYER (5)

enum class GetShipLengthDirection {
    VERTICAL, HORIZONTAL
};


ReturnCode Game::parseBoardFile(std::string filePath, int rows, int cols)
{
	char initBoard[INIT_BOARD_ROW_SIZE][INIT_BOARD_COL_SIZE] = {' '};
	bool wrongSizeShip[UCHAR_MAX] = {false};
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		for (int j = 0; j < INIT_BOARD_COL_SIZE; ++j)
		{
			initBoard[i][j] = ' ';
		}
	}


	// Read from file 
	// Line by line up to 10 line and up to 10 chars per line
	// If not allowed chars ignore
	//
	// for each char
	for (int i = 1; i <= BOARD_ROW_SIZE; ++i)
	{
	    for (int j = 1; j <= BOARD_COL_SIZE; ++j)
	    {
	        if (SPACE == initBoard[i][j])
	        {
	            // ignore spaces
	            continue;
	        }
	        char currentShip = initBoard[i][j];
	        int horizontalShipLen = getShipLength(initBoard, currentShip, i, j, GetShipLengthDirection::HORIZONTAL);
	        int verticalShipLen   = getShipLength(initBoard, currentShip, i, j, GetShipLengthDirection::VERTICAL);
	        int expectedLen = m_shipToExpectedLen[currentShip];
	        if ((expectedLen != horizontalShipLen && 1 != verticalShipLen) ||
	            (expectedLen != verticalShipLen   && 1 != horizontalShipLen))
	        {
	            // ERROR - wrong size or shape for currentShip(player too)


	        }
	        else
	        {
	            // count legal ship
	        }

	        //check Adjacency
	        if (false == isAdjacencyValid(initBoard, i, j))
	        {
	            // ERROR Adjacency
	        }

	    }

	    // check count of ships per player - should be SHIPS_PER_PLAYER
	}


	return ReturnCode::RC_SUCCESS;
}

/**
 * @Details
 */
bool Game::isAdjacencyValid(char** initBoard, int i/*row*/, int j/*col*/)
{
    char expectedShip = initBoard[i][j];
    if (SPACE == expectedShip)
    {
        return true;
    }

    if ((SPACE != initBoard[i+1][j] && expectedShip != initBoard[i+1][j]) ||
        (SPACE != initBoard[i-1][j] && expectedShip != initBoard[i-1][j]) ||
        (SPACE != initBoard[i][j+1] && expectedShip != initBoard[i][j+1]) ||
        (SPACE != initBoard[i][j-1] && expectedShip != initBoard[i][j-1]))
    {
        return false;
    }

    return true;
}

/**
 * @Details
 */
int Game::getShipLength(char** initBoard, char expectedShip, int i/*row*/, int j/*col*/, GetShipLengthDirection direction)
{
	// stop condition
	if (expectedShip != initBoard[i][j])
	{
	    return 0;
	}

	if (GetShipLengthDirection::HORIZONTAL == direction)
	{
	    return (1 + Game::getShipLength(initBoard, expectedShip, i, j-1, direction) +
	                Game::getShipLength(initBoard, expectedShip, i, j+1, direction));
	}
	else /*GetShipLengthDirection::VERTICAL == direction*/
	{
	    return (1 + Game::getShipLength(initBoard, expectedShip, i-1, j, direction) +
	                Game::getShipLength(initBoard, expectedShip, i+1, j, direction));
	}
}

ReturnCode Game::initListPlayers()
{
	return ReturnCode::RC_SUCCESS;
}

ReturnCode Game::init(std::string boardPathFile, int row, int col)
{
	ReturnCode rc = this->parseBoardFile(boardPathFile, row, col);
	// init expected len map
	//TODO macros
	m_shipToExpectedLen['B'] = m_shipToExpectedLen['b'] = 1;
	m_shipToExpectedLen['P'] = m_shipToExpectedLen['p'] = 2;
	m_shipToExpectedLen['M'] = m_shipToExpectedLen['m'] = 3;
	m_shipToExpectedLen['D'] = m_shipToExpectedLen['d'] = 4;
	// initListPlayers + init foreach player
	// Set board for Players
	//

	return ReturnCode::RC_SUCCESS;
}

ReturnCode Game::startGame()
{
	vector<IBattleshipGameAlgo*>::iterator iter = m_players.begin();
	pair<int, int> attackReq;
	IBattleshipGameAlgo* currentPlayer;

	while (true)
	{
		currentPlayer = (*iter);
		attackReq = currentPlayer->attack();
		// Check attack status (hit,miss)
		// Notify for all players
		// If game over break

		// If attack failed - iter.next
		// If iter == last do iter = begin
	}
	
	// Notify winner

	return ReturnCode::RC_SUCCESS;
}
