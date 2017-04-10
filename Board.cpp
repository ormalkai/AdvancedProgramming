
#include "Board.h"
#include "Utils.h"
#include "ShipFactory.h"

void Board::buildBoard(char ** initBoard)
{
	// scan from top left to right and bottom
	// if my upper and left cells are empty I'm new ship
	for (int i=1; i<= BOARD_ROW_SIZE; i++)
	{
		for (int j = 1; j <= BOARD_COL_SIZE; j++)
		{
			if (SPACE == initBoard[i-1][j] && SPACE == initBoard[i][j-1])
			{
				Ship* ship = ShipFactory::instance()->create(i, j, initBoard);
			}
		}
	}


}
