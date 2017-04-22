


#include "BattleshipAlgoSmart.h"
#include "Utils.h"

int BattleshipAlgoSmart::calcHist(int i, int j)
{

	auto& cell = m_board.get(i, j);

	if (!cell.isEmpty())
		return 0;


	auto numOfPotentialShips = 1;
	map<Direction, int> maxIndexOfValid = {
		{ Direction::UP, MAX_SHIP_LEN },
		{ Direction::DOWN, MAX_SHIP_LEN },
		{ Direction::LEFT, MAX_SHIP_LEN },
		{ Direction::RIGHT, MAX_SHIP_LEN }
	};

	
	for (auto d_i = 0; d_i <= NUM_OF_DIRECTIONS; ++d_i)
	{
		for (auto shipLen = 1; shipLen <= MAX_SHIP_LEN; ++shipLen)
		{
			auto d = static_cast<Direction>(d_i);
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

			if (!m_board.isValidCell(checkedRowIndex, checkedColIndex))
				break;

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
	numOfPotentialShips += calcNumOfOptionalShipsInOffset(maxIndexOfValid.at(Direction::UP), maxIndexOfValid.at(Direction::DOWN));
	numOfPotentialShips += calcNumOfOptionalShipsInOffset(maxIndexOfValid.at(Direction::LEFT), maxIndexOfValid.at(Direction::RIGHT));

	return numOfPotentialShips;
}


bool BattleshipAlgoSmart::isOtherNeighborValid(const Cell& cell, Direction d)
{
	// Not mandatory
	//if (m_board.isPaddingCell(cell))
	//	return false;

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


int BattleshipAlgoSmart::calcNumOfOptionalShipsInOffset(int i, int j) const
{
	auto min = min(i, j);

	if (min == 0)
		return 0;
	if (min == 1)
		return (i + j == 2 ? 1 : 2); // If j==1 only one ship with len 3 is valid
	
	// At least 2 cells each direction
	return min * 2;
}