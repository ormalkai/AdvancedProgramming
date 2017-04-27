

#include <climits>
#include "BattleshipAlgoSmart.h"
#include "Utils.h"
#include "ShipFactory.h"

Cell* BattleshipAlgoSmart::popAttack()
{
	m_attackedQueue.update();
	Cell* c = m_attackedQueue.top();
	m_attackedQueue.pop();
	return c;
}

BattleshipAlgoSmart::BattleshipAlgoSmart(int id) : m_rows(BOARD_ROW_SIZE), m_cols(BOARD_COL_SIZE)
{
	this->setId(id);
	m_currentStatus = HUNT;
}

void BattleshipAlgoSmart::setBoard(int player, const char** board, int numRows, int numCols)
{
	m_id = player;
	char** initBoard = new char*[INIT_BOARD_ROW_SIZE];
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		initBoard[i] = new char[INIT_BOARD_COL_SIZE];
	}
	for (int i=0; i<INIT_BOARD_ROW_SIZE; i++)
	{
		for (int j = 0; j<INIT_BOARD_COL_SIZE; j++)
		{
			initBoard[i][j] = SPACE;
		}
	}
	for (int i = 1; i <= m_board.rows(); ++i)
	{
		for (int j = 1; j <= m_board.cols(); ++j)
		{
			initBoard[i][j] = board[i-1][j-1];
		}
	}
	m_board.buildBoard(const_cast<const char**>(initBoard));
	Cell c = m_board.get(9, 4);
	for (int i = 0; i < INIT_BOARD_ROW_SIZE; ++i)
	{
		delete[] initBoard[i];
	}
	delete[] initBoard;

	for (int i = 1; i <= m_board.rows(); ++i)
	{
		for (int j = 1; j <= m_board.cols(); ++j)
		{
			calcHist(i, j);
			//m_attackedQueue.push(m_board.getCellPointer(i, j));
			Cell* c = m_board.getCellPointer(i, j);
			m_attackedQueue.push(c);
		}
	}
}

pair<int, int> BattleshipAlgoSmart::attack()
{
	m_board.printHist();
	if (m_currentStatus == HUNT)
	{
		Cell* c = popAttack();

		if (c->getHistValue() == INT_MAX)
		{
			m_currentStatus = TARGET;
			m_currentAttackedShipCells.push_back(c);

			// Insert cell's neighbors
			updateTargetAttackQueue(c, ShipDirection::ALL, false);

			c = popTargetAttack();
			return pair<int, int>(c->row(), c->col());
		}

		return pair<int, int>(c->row(), c->col());
	}
	else /*(m_currentStatus == TARGET)*/
	{
		Cell* c = popTargetAttack();
		return pair<int, int>(c->row(), c->col());
	}
}

void BattleshipAlgoSmart::handleUntargetShipSunk(Cell* attackedCell)
{
	vector<Cell*> attackedShipCells = getSunkShipByCell(attackedCell);

	// Update hist
	updateHist(attackedShipCells);
}

void BattleshipAlgoSmart::handleTargetShipSunk(Cell* attackedCell)
{
	m_currentStatus = HUNT;

	m_targetQueue.clear();

	// Add the cell was attacked for update the neighbors
	m_currentAttackedShipCells.push_back(attackedCell);

	// Update hist
	updateHist(m_currentAttackedShipCells);

	// update hist of neighs
	m_currentAttackedShipCells.clear();
}

void BattleshipAlgoSmart::notifyOnAttackResult(int player, int row, int col, AttackResult attackResult)
{

	Cell* attackedCell = m_board.getCellPointer(row, col);
	attackedCell->hitCell();
	if (AttackResult::Hit == attackResult || AttackResult::Sink == attackResult)
		attackedCell->setStatus(Cell::DEAD);
	attackedCell->setHistValue(0);



	if (AttackResult::Miss == attackResult && !attackedCell->hasShip())
	{
		// Update hist
		updateHist(vector<Cell*>{attackedCell}, false);
	}
	

	if (m_id == player)
	{
		if (HUNT == m_currentStatus)
		{
			switch (attackResult)
			{
			case AttackResult::Sink:
			{
				// Add the cell was attacked for update the neighbors
				m_currentAttackedShipCells.push_back(attackedCell);

				// Update hist
				updateHist(m_currentAttackedShipCells);

				m_currentAttackedShipCells.clear();
			} break;
			case AttackResult::Hit:
			{
				m_currentStatus = TARGET;
				m_currentAttackedShipCells.push_back(attackedCell);

				// Insert cell's neighbors
				updateTargetAttackQueue(attackedCell, ShipDirection::ALL, false);

			} break;
			}
		}
		else // TARGET == m_currentStatus
		{
			switch (attackResult)
			{
			case AttackResult::Hit: 
			{
				m_currentAttackedShipCells.push_back(attackedCell);

				bool toRemoveWrongAxis = false;
				if (m_currentAttackedShipCells.size() == 2)
					toRemoveWrongAxis = true;
				if (m_currentAttackedShipCells.at(0)->row() == m_currentAttackedShipCells.at(1)->row())
					updateTargetAttackQueue(attackedCell, ShipDirection::HORIZONTAL, toRemoveWrongAxis);
				else
					updateTargetAttackQueue(attackedCell, ShipDirection::VERTICAL, toRemoveWrongAxis);


			} break;
			case AttackResult::Sink:
			{
				handleTargetShipSunk(attackedCell);
			}
			}
		}
	}
	else if (attackedCell->getPlayerIndexOwner() != getId())
	{
		if (HUNT == m_currentStatus)
		{
			switch (attackResult)
			{
			case AttackResult::Sink:
			{
				handleUntargetShipSunk(attackedCell);
			}
			break;
			case AttackResult::Hit:
			{
				attackedCell->setHistValue(INT_MAX);
			}
			break;
			}
		}
		else // TARGET == m_currentStatus
		{
			switch (attackResult)
			{
			case AttackResult::Hit:
			{
				attackedCell->setHistValue(INT_MAX);
			}
			break;
			case AttackResult::Sink:
			{
				// If the other player shot the last cell in the ship that current player tried to hit
				if (isCellNeighborToTargetShip(attackedCell))
				{
					handleTargetShipSunk(attackedCell);
				}
				else
				{
					handleUntargetShipSunk(attackedCell);
				}
			}
			}
		}
	}
	
	m_board.printHist();
}

bool BattleshipAlgoSmart::isCellNeighborToTargetShip(Cell* cell)
{
	for (auto it = m_currentAttackedShipCells.begin(); it != m_currentAttackedShipCells.end(); ++it)
	{
		// If the cell is neighbor of *it cell
		if (cell->squaredDistance(*it) == 1)
			return true;
	}

	return false;
}

vector<Cell*> BattleshipAlgoSmart::getSunkShipByCell(Cell* c)
{
	int row = c->row();
	int col = c->col();
	vector<Cell*> result{};

	ShipDirection d = ShipDirection::HORIZONTAL;
	if (!m_board.getCellPointer(row + 1, col)->isEmpty() || !m_board.getCellPointer(row - 1, col)->isEmpty())
	{
		d = ShipDirection::VERTICAL;
	}

	Cell* p = c;
	while (!p->isEmpty())
	{
		if (d == ShipDirection::HORIZONTAL)
			p = m_board.getCellPointer(row + 1, col);
		else /*(d == ShipDirection::VERTICAL)*/
			p = m_board.getCellPointer(row, col + 1);
		result.push_back(p);
	}

	p = c;
	while (!p->isEmpty())
	{
		if (d == ShipDirection::HORIZONTAL)
			p = m_board.getCellPointer(row - 1, col);
		else /*(d == ShipDirection::VERTICAL)*/
			p = m_board.getCellPointer(row, col - 1);
		result.push_back(p);
	}

	return result;
}

void BattleshipAlgoSmart::updateHist(vector<Cell*> cells, bool createDummyShip /*= true*/)
{
	if (createDummyShip) {
		// create dummy ship
		m_board.addDummyNewShipToBoard(cells);
	}

	// update ships neighbors hist

	for (auto it = cells.begin(); it != cells.end(); ++it)
	{
		Cell* pc = *it;

		if (!createDummyShip)
			pc->setHistValue(0);

		int r = pc->row();
		int c = pc->col();

		for (auto i = 1; i <= MAX_SHIP_LEN; ++i)
		{
			calcHist(r + i, c);
			calcHist(r - i, c);
			calcHist(r, c + i);
			calcHist(r, c - i);
		}
	}
}

void BattleshipAlgoSmart::updateTargetAttackQueue(Cell* attackedCell, ShipDirection direction, bool toRemoveWrongAxis)
{
	int rowIndex = attackedCell->row();
	int colIndex = attackedCell->col();

	if (toRemoveWrongAxis)
	{
		for (list<Cell*>::iterator it = m_targetQueue.begin(); it != m_targetQueue.end();)
		{
			int rIndex = (*it)->row();
			int cIndex = (*it)->col();

			if (ShipDirection::HORIZONTAL == direction && rowIndex != rIndex)
				it = m_targetQueue.erase(it);
			if (ShipDirection::VERTICAL == direction && colIndex != cIndex)
				it = m_targetQueue.erase(it);
			else if (it != m_targetQueue.end())
				++it;
		}

	}

	int newRow = rowIndex, newCol = colIndex;

	if (ShipDirection::ALL == direction || ShipDirection::VERTICAL == direction) {

		// Down
		newRow = rowIndex + 1;
		while (m_board.get(newRow, colIndex).isPendingCell()) newRow++;
		if (isAttackable(m_board.get(newRow, colIndex)))
			m_targetQueue.push_back(&(m_board.get(newRow, colIndex)));

		// Up
		newRow = rowIndex - 1;
		while (m_board.get(newRow, colIndex).isPendingCell()) newRow--;
		if (isAttackable(m_board.get(newRow, colIndex)))
			m_targetQueue.push_back(&(m_board.get(newRow, colIndex)));
	}
	if (ShipDirection::ALL == direction || ShipDirection::HORIZONTAL == direction)
	{
		// Right
		newCol = colIndex + 1;
		while (m_board.get(rowIndex, newCol).isPendingCell()) newCol++;
		if (isAttackable(m_board.get(rowIndex, newCol)))
			m_targetQueue.push_back(&(m_board.get(rowIndex, newCol)));

		// Left
		newCol = colIndex - 1;
		while (m_board.get(rowIndex, newCol).isPendingCell()) newCol--;
		if (isAttackable(m_board.get(rowIndex, newCol)))
			m_targetQueue.push_back(&(m_board.get(rowIndex, newCol)));
	}
}

bool BattleshipAlgoSmart::isAttackable(Cell& c) const
{
	return (!m_board.isPaddingCell(c)) && c.isEmpty();

}

bool BattleshipAlgoSmart::init(const std::string& path)
{
	m_currentStatus = HUNT;



	return false;

}

void BattleshipAlgoSmart::calcHist(int i, int j)
{

	if (i <= 0 || i > m_rows || j <= 0 || j > m_cols)
		return;

	auto& cell = m_board.get(i, j);

	if (!cell.isEmpty())
	{
		cell.setHistValue(0);
		return;
	}

	// Check there are no neighbors ship
	if (!m_board.get(i + 1, j).isEmpty() ||
		!m_board.get(i - 1, j).isEmpty() ||
		!m_board.get(i, j + 1).isEmpty() ||
		!m_board.get(i, j - 1).isEmpty())
	{
		cell.setHistValue(0);
		return;
	}



	auto numOfPotentialShips = 1; 

	map<Direction, int> maxIndexOfValid = {
		{ Direction::UP, MAX_SHIP_LEN - 1 },
		{ Direction::DOWN, MAX_SHIP_LEN - 1 },
		{ Direction::LEFT, MAX_SHIP_LEN - 1 },
		{ Direction::RIGHT, MAX_SHIP_LEN - 1 }
	};


	for (auto d_i = 0; d_i < NUM_OF_DIRECTIONS; ++d_i)
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
			{
				maxIndexOfValid[d] = shipLen - 1;
				break;
			}
				
			auto& checkedCell = m_board.get(checkedRowIndex, checkedColIndex);

			if ((checkedCell.isHitted() && !checkedCell.hasShip()) || 
				!isOtherNeighborsValid(checkedCell, od))
			{
				maxIndexOfValid[d] = shipLen - 1;
				break;
			}
		} // ShipLen
	} // Direction


	// Calc 
	numOfPotentialShips += calcNumOfOptionalShipsInOffset(maxIndexOfValid.at(Direction::UP), maxIndexOfValid.at(Direction::DOWN));
	numOfPotentialShips += calcNumOfOptionalShipsInOffset(maxIndexOfValid.at(Direction::LEFT), maxIndexOfValid.at(Direction::RIGHT));

	cell.setHistValue(numOfPotentialShips);
}


bool BattleshipAlgoSmart::isOtherNeighborsValid(const Cell& cell, Direction d)
{

	if (m_board.isPaddingCell(cell))
		return false;

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
	auto minBottleneck = min(i, j);

	// If one of the sides is 0 -> the cell in edge of ship 
	// Num of ship will be exacly the same as the other side's len
	if (0 == minBottleneck)
		return max(i, j);


	int ret = 0;

	for (int shipLen = 2; shipLen <= MAX_SHIP_LEN; ++shipLen)
	{
		ret += min(minBottleneck, shipLen);

	}

	return ret;
}