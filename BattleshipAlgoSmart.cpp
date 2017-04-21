


#include "BattleshipAlgoSmart.h"
#include "Utils.h"

int BattleshipAlgoSmart::calcHist(int i, int j)
{

	auto& cell = m_board.get(i, j);
	auto numOfPotentialShips = 1;
	map<Direction, int> maxIndexOfValid = {
		{ Direction::UP, MAX_SHIP_LEN },
		{ Direction::DOWN, MAX_SHIP_LEN },
		{ Direction::LEFT, MAX_SHIP_LEN },
		{ Direction::RIGHT, MAX_SHIP_LEN }
};

	if (!cell.isEmpty())
		return 0;

	for (auto d = 0; d < 4; ++d)
	{
		for (auto shipLen = 1; shipLen <= MAX_SHIP_LEN; ++shipLen)
		{
			Direction d;
			auto od = Utils::getOppositeDirection(d);

			auto checkedRowIndex = i;
			auto checkedColIndex = j;

			switch (d)
			{
			case Direction::UP: 
				checkedRowIndex -= shipLen;
				break;
			case Direction::DOWN: 
				checkedRowIndex += shipLen; 
				break;
			case Direction::RIGHT: 
				checkedColIndex += shipLen;
				break;
			case Direction::LEFT: 
				checkedColIndex += shipLen; 
				break;
			default: ;
			}

			auto& checkedCell = m_board.get(checkedRowIndex, checkedColIndex);
			
			if (isOtherNeighborValid(checkedCell, od))
				numOfPotentialShips++;
			else
			{
				maxIndexOfValid[d] = shipLen - 1;
				break;
			}
		} // ShipLen
	} // Direction

	
	// If the cell is in the middle of ship



}


bool BattleshipAlgoSmart::isOtherNeighborValid(const Cell& cell, Direction d)
{
	auto ret = false;
	auto rIndex = cell.row();
	auto cIndex = cell.col();

	switch (d)
	{
	case Direction::UP:
	{
		ret =	m_board.get(rIndex + 1, cIndex).isEmpty() &&	// Check down
				m_board.get(rIndex, cIndex + 1).isEmpty() &&	// Check right
				m_board.get(rIndex, cIndex - 1).isEmpty();		// Check left
	}
	break;
	case Direction::DOWN: {
		ret =	m_board.get(rIndex - 1, cIndex).isEmpty() &&	// Check up
				m_board.get(rIndex, cIndex + 1).isEmpty() &&	// Check right
				m_board.get(rIndex, cIndex - 1).isEmpty();		// Check left
	}
	break;
	case Direction::RIGHT: {
		ret = m_board.get(rIndex - 1, cIndex).isEmpty() &&	// Check up
			m_board.get(rIndex + 1, cIndex).isEmpty() &&	// Check down
			m_board.get(rIndex, cIndex - 1).isEmpty();		// Check left
	}
	break;
	case Direction::LEFT:  {
		ret =	m_board.get(rIndex - 1, cIndex).isEmpty() &&	// Check up
				m_board.get(rIndex + 1, cIndex).isEmpty() &&	// Check down
				m_board.get(rIndex, cIndex + 1).isEmpty();		// Check right
	}
	break;
	default:;
	}


	return ret;
}
