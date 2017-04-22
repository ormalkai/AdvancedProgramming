


#include "BattleshipAlgoSmart.h"
#include "Utils.h"

void BattleshipAlgoSmart::setBoard(int player, const char** board, int numRows, int numCols)
{
	m_id = player;
	m_board.buildBoard(board);

	for (int i = 0; i < m_board.rows(); ++i)
	{
		for (int j = 0; j < m_board.cols(); ++j)
		{
			calcHist(i, j);
		}
	}
}

pair<int, int> BattleshipAlgoSmart::attack()
{
	if (m_currentStatus == HUNT)
	{
		Cell* c = popAttack();
		return pair<int, int>(c->row(), c->col());

	}
	else /*(m_currentStatus == TARGET)*/
	{
		Cell* c = m_targetQueue.;
		return pair<int, int>(c->row(), c->col());
		
		// pop from target queue

		return c;


	}
}

void BattleshipAlgoSmart::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{

	Cell& c = m_board.get(row, col);
	// Set c's status to attacked

	if (m_id == player)
	{
		switch (m_currentStatus)
		{
		case HUNT:
		{
			switch (result)
			{

			case AttackResult::Hit:
			{
				m_currentStatus = TARGET;
				// Insert neighs

			} break;
			}
			
		}
		break;
		case TARGET:
		{
			switch (result)
			{
			case AttackResult::Miss: {} break;
			case AttackResult::Hit: {
			
				// Remove all points in other axis
			
			
			} break;
			case AttackResult::Sink: break;
			default: ;
			}
		} break;
		default:;
		}




	}
	else
	{

	}



}

bool BattleshipAlgoSmart::init(const std::string& path)
{
	m_currentStatus = HUNT;




}

void BattleshipAlgoSmart::calcHist(int i, int j)
{

	auto& cell = m_board.get(i, j);

	if (!cell.isEmpty())
	{
		cell.setHistValue(0);
		return;
	}



	auto numOfPotentialShips = 1; //

	map<Direction, int> maxIndexOfValid = {
		{ Direction::UP, MAX_SHIP_LEN },
		{ Direction::DOWN, MAX_SHIP_LEN },
		{ Direction::LEFT, MAX_SHIP_LEN },
		{ Direction::RIGHT, MAX_SHIP_LEN }
	};


	for (auto d_i = 0; d_i <= NUM_OF_DIRECTIONS; ++d_i)
	{
		for (auto shipLen = 1; shipLen < MAX_SHIP_LEN; ++shipLen)
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
				checkedColIndex -= shipLen;
				break;
			default:;
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

	cell.setHistValue(numOfPotentialShips);
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
		ret = m_board.get(rIndex + 1, cIndex).isEmpty() &&	// Check down
			m_board.get(rIndex, cIndex + 1).isEmpty() &&	// Check right
			m_board.get(rIndex, cIndex - 1).isEmpty();		// Check left
	}
	break;
	case Direction::DOWN: {
		ret = m_board.get(rIndex - 1, cIndex).isEmpty() &&	// Check up
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
	case Direction::LEFT: {
		ret = m_board.get(rIndex - 1, cIndex).isEmpty() &&	// Check up
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