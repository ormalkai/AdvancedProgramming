

#include <climits>
#include "BattleshipAlgoSmart.h"
#include "Utils.h"
#include "Debug.h"

Cell* BattleshipAlgoSmart::popAttack()
{
	m_attackedQueue.update();
	Cell* c = m_attackedQueue.top();
	m_attackedQueue.pop();
	return c;
}


void BattleshipAlgoSmart::setPlayer(int player) {
	m_id = player;
}

void BattleshipAlgoSmart::setBoard(const BoardData& board)
{
	m_rows = board.rows();
	m_cols = board.cols();
	m_depth = board.depth();
	int initBoardRows = m_rows + BOARD_PADDING;
	int initBoardCols = m_cols + BOARD_PADDING;
	char** initBoard = new char*[initBoardRows];
	for (int i = 0; i < initBoardRows; ++i)
	{
		initBoard[i] = new char[initBoardCols];
	}
	for (int i = 0; i < initBoardRows; i++)
	{
		for (int j = 0; j < initBoardCols; j++)
		{
			initBoard[i][j] = SPACE;
		}
	}
	for (int i = 1; i <= m_rows; ++i)
	{
		for (int j = 1; j <= m_cols; ++j)
		{
			initBoard[i][j] = board[i - 1][j - 1];
		}
	}
	m_board.buildBoard(const_cast<const char**>(initBoard), m_rows, m_cols);
	for (int i = 0; i < initBoardRows; ++i)
	{
		delete[] initBoard[i];
	}
	delete[] initBoard;

	initStripSizeToNumPotentialShips();

	for (int i = 1; i <= m_board.rows(); ++i)
	{
		for (int j = 1; j <= m_board.cols(); ++j)
		{
			calcHist(i, j);
			Cell* c = m_board.getCellPointer(i, j);
			m_attackedQueue.push(c);
		}
	}
}

Coordinate BattleshipAlgoSmart::attack()
{
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
			return std::move(Coordinate(c->row(), c->col(), c->depth()));
		}

		return make_pair(c->row(), c->col());
	}
	else /*(m_currentStatus == TARGET)*/
	{
		Cell* c = popTargetAttack();
		return make_pair(c->row(), c->col());
	}
}

void BattleshipAlgoSmart::handleUntargetShipSunk(Cell* const attackedCell)
{
	vector<Cell*> attackedShipCells = getSunkShipByCell(attackedCell);

	// Update hist
	updateHist(attackedShipCells);
}

void BattleshipAlgoSmart::handleTargetShipSunk(Cell* const attackedCell)
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
			default: ;
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", attackResult, __func__);
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
			default: 
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", attackResult, __func__);
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
			default: 
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", attackResult, __func__);
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
			default: 
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", attackResult, __func__);
			}
		}
	}
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

vector<Cell*> BattleshipAlgoSmart::getSunkShipByCell(Cell* const c) const
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

void BattleshipAlgoSmart::updateHist(const vector<Cell*>& cells, bool createDummyShip /*= true*/)
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

void BattleshipAlgoSmart::updateTargetAttackQueue(const Cell* attackedCell, ShipDirection direction, bool toRemoveWrongAxis)
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

	int newRow, newCol;

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

bool BattleshipAlgoSmart::isAttackable(const Cell& c) const
{
	return (!m_board.isPaddingCell(c)) && c.isEmpty();

}

bool BattleshipAlgoSmart::init(const string& path)
{
	m_currentStatus = HUNT;
	return true;
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
	numOfPotentialShips += m_stripToPotentialShips[{maxIndexOfValid.at(Direction::UP), maxIndexOfValid.at(Direction::DOWN)}];
	numOfPotentialShips += m_stripToPotentialShips[{maxIndexOfValid.at(Direction::LEFT), maxIndexOfValid.at(Direction::RIGHT)}];

	cell.setHistValue(numOfPotentialShips);
}


bool BattleshipAlgoSmart::isOtherNeighborsValid(const Cell& cell, Direction d) const
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


bool BattleshipAlgoSmart::isShipValidInOffset(int shipLen, int offset, int hasLeft, int hasRight)
{
	int needLeft = offset;
	int needRight = shipLen - (offset + 1);
	if (needLeft <= hasLeft && needRight <= hasRight)
	{
		return true;
	}
	return false;
}

void BattleshipAlgoSmart::initStripSizeToNumPotentialShips()
{
	for (int i = 0; i < MAX_SHIP_LEN; i++)
	{
		for (int j = 0; j < MAX_SHIP_LEN; j++)
		{
			int numPotentialShips = 0;
			for (int shipLen = 2; shipLen <= MAX_SHIP_LEN; shipLen++)
			{
				for (int offset = 0; offset < shipLen; offset++)
				{
					if (isShipValidInOffset(shipLen, offset, i, j))
					{
						numPotentialShips++;
					}
				}
			}
			m_stripToPotentialShips.emplace(make_pair(make_pair(i, j), numPotentialShips));
		}
	}
}

IBattleshipGameAlgo* GetAlgorithm()
{
	return (new BattleshipAlgoSmart());
}