

#include <climits>
#include <iostream>
#include <conio.h>
#include <regex>
#include <fstream>
#include "BattleshipAlgoSmart.h"
#include "Utils.h"
#include "Debug.h"
#include <set>

#define PLAYERS_DELIMETER "----------"

shared_ptr<Cell> BattleshipAlgoSmart::popAttack()
{
	m_attackedQueue.update();
	shared_ptr<Cell> c = m_attackedQueue.top();
	m_attackedQueue.pop();
	//removeOperationCellIfNeed(c);
	return c;
}

shared_ptr<Cell> BattleshipAlgoSmart::popTargetAttack()
{
	if (0 == m_targetQueue.size())
	{
		assert(false);
		return nullptr;
	}
	shared_ptr<Cell> c = m_targetQueue.front();
	m_targetQueue.pop_front();

	//removeOperationCellIfNeed(c);
	return c;
}


void BattleshipAlgoSmart::setPlayer(int player) {
	m_id = player;
}

void BattleshipAlgoSmart::clear()
{
	m_shipAwarenessStatus = true;
	m_saveNewBoardAwerness = false;
	m_attackedQueue.clear();
	m_targetQueue.clear();
	m_currentStatus = HUNT;
	m_currentAttackedShipCells.clear();
	m_otherPlayerShips.clear();
	m_stripToPotentialShips.clear();
	m_stripToPotentialShipsPerShip.clear(); // ship len to strip to potential ships
}

void BattleshipAlgoSmart::recoverBoardAwareness()
{
	for (shared_ptr<Cell> cell : m_operationCell)
	{
		int cellHistVal = cell->getHistValue();
		cell->setHistValue(cellHistVal - BOARD_AWARENESS_HIST_VAL);
		cell->setOperationCellStatus(false);
	}

	m_operationCell.clear();
}

void BattleshipAlgoSmart::setBoard(const BoardData& board)
{
	clear();

	m_rows = board.rows();
	m_cols = board.cols();
	m_depth = board.depth();
	m_board.buildBoard(board);

	m_board.getOtherPlayerShips(m_otherPlayerShips);
	initStripSizeToNumPotentialShips();

	for (int d = 1; d <= m_board.depth(); d++)
	{
		for (int i = 1; i <= m_board.rows(); ++i)
		{
			for (int j = 1; j <= m_board.cols(); ++j)
			{
				calcHist(Coordinate(i, j, d));
				shared_ptr<Cell> c = m_board.get(d, i, j);
				m_attackedQueue.push(c);
			}
		}
	}

	// board Awerness
	getAwarenessBoards();

}

void BattleshipAlgoSmart::getAwarenessBoards()
{
	//     read the data (coord list) (for both players) with delimiter
	//     compare to our board - calc similarity ratio
	//				if (size not equal) ratio = 0
	//		if (100% - stop search)
	//		find the max ratio
	//		return the highest ratio vector contains other player's cells

	vector<vector<vector<bool>>> legalCell;

	vector<Coordinate> myShipsCoord;
	m_board.getShipsCoord(myShipsCoord);

	for (int d = 0; d < m_board.depth() + BOARD_PADDING; d++)
	{
		vector<vector<bool>> dvec;
		for (int r = 0; r < m_board.rows() + BOARD_PADDING; r++)
		{
			vector<bool> rvec;
			for (int c = 0; c < m_board.cols() + BOARD_PADDING; c++)
			{
				rvec.push_back(true);
			}
			dvec.push_back(rvec);
		}
		legalCell.push_back(dvec);
	}



	for (Coordinate coord : myShipsCoord)
	{
		int r = coord.row;
		int c = coord.col;
		int d = coord.depth;

		legalCell[d + 1][r][c] = false;
		legalCell[d - 1][r][c] = false;
		legalCell[d][r + 1][c] = false;
		legalCell[d][r - 1][c] = false;
		legalCell[d][r][c + 1] = false;
		legalCell[d][r][c - 1] = false;
	}

	for (Coordinate coord : myShipsCoord)
	{
		int r = coord.row;
		int c = coord.col;
		int d = coord.depth;

		legalCell[d][r][c] = true;
	}

	

	// fix my ships legal cells

	// Get temp directory path
	wstring strTempPath;
	wchar_t wchPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wchPath))
		strTempPath = wchPath;
	
	const string strTmp(strTempPath.begin(), strTempPath.end());

	vector<string> boardFiles;
	// get all sboards file names in tempdir path
	Utils::getListOfFilesInDirectoryBySuffix(strTmp, "ohgsmart", boardFiles);

	// Sort for choosing the last boards before
	sort(boardFiles.begin(), boardFiles.end(), std::greater<string>());

	int maxRatio = -1;
	vector<Coordinate> maxRatioCoord;
	
	

	// foreach file 
	for (string boardPath : boardFiles)
	{
		vector<Coordinate> playerACoord;
		vector<Coordinate> playerBCoord;
		ReturnCode rc = parseBoardDataFile(boardPath, playerACoord, playerBCoord, legalCell);
		if (RC_SUCCESS != rc)
		{
			continue;
		}
		
		int ratioA = calcSimilarityRatio(playerACoord, myShipsCoord);
		if (ratioA <= 100/* can be greater than 100*/)
		{
			if (ratioA > maxRatio)
			{
				maxRatio = ratioA;
				// attack the other player
				if (ratioA > 90/*threshold*/)
					maxRatioCoord = playerBCoord;

				if (100 == maxRatio)
					break;
			}
		}

		int ratioB = calcSimilarityRatio(playerBCoord, myShipsCoord);
		if (ratioB <= 100/* can be greater than 100*/)
		{
			if (ratioB > maxRatio)
			{
				maxRatio = ratioB;
				// attack the other player
				if (ratioB > 90/*threshold*/)
					maxRatioCoord = playerACoord;

				if (100 == maxRatio)
					break;
			}
		}
	}
	
	// foreach coord in the vector
	//		update flag
	//		add shared_ptr<cell> to the set
	//		update hist value of the cells by MAX_INT - 1

	for (Coordinate coor : maxRatioCoord)
	{
		int histVal = m_board.get(coor)->getHistValue();
		shared_ptr<Cell> c = m_board.get(coor);

		c->setOperationCellStatus(true);
		c->setHistValue(histVal + BOARD_AWARENESS_HIST_VAL);
		m_operationCell.insert(c);
	}

	// TODO: Create the board file and save my coord!
	char randName[256] = {'\0'};
	tmpnam_s(randName, 256);
	m_newBoardAwernessFile = string(randName) + ".ohgsmart";
	//open the file
	ofstream newBoardAwerness(m_newBoardAwernessFile);
	if (!newBoardAwerness.is_open())
	{
		m_saveNewBoardAwerness = false;
		return;
	}
	//print dim
	newBoardAwerness << m_cols << "x" << m_rows << "x" << m_depth << endl;
	//print coordinates
	for (auto coord : myShipsCoord)
	{
		newBoardAwerness << coord.row << "," << coord.col << "," << coord.depth << endl;
	}
	//print delimeter
	newBoardAwerness << PLAYERS_DELIMETER << endl;
	// continue saving coordinates during the game
	m_saveNewBoardAwerness = true;
}

int BattleshipAlgoSmart::calcSimilarityRatio(vector<Coordinate>& boardCoord, const vector<Coordinate> myShipsCoord)
{
	// TODO: Make better by check if coord is bot neighbor of my cell
	// Will cause penalty in performance!
	
	int hits = 0;
	for (Coordinate coor : boardCoord)
	{
		// TODO: complete the if with the orminator !
		for (auto myCoor : myShipsCoord)
		{
			if (coor.depth == myCoor.depth && coor.row == myCoor.row && coor.col == myCoor.col)
			{
				hits++;
				break;
			}
			// cant check this condition i'm shooting in my leg when checking the ships!
			//else if (1 == m_board.get(myCoor)->squaredDistance(coor)) // this coord is neighbor to me can't be my board
			//{
			//	return 0;
			//}
		}
	}
	if (0 == hits)
		return 0;
	return static_cast<int>(myShipsCoord.size()) / hits * 100;
}

ReturnCode BattleshipAlgoSmart::parseBoardDataFile(string& boardPath ,vector<Coordinate>& playerACoord, vector<Coordinate>& playerBCoord, vector<vector<vector<bool>>>&  legalCells) const
{
	// Read from file 
	int cols, rows, depth;
	ifstream sboard(boardPath);
	if (!sboard.is_open())
	{
		return RC_ERROR;
	}

	string line;
	Utils::safeGetline(sboard, line);
	transform(line.begin(), line.end(), line.begin(), ::tolower);
	regex regexDim("^([0-9]+)x([0-9]+)x([0-9]+)");
	smatch m;
	regex_match(line, m, regexDim);
	if (m.size() != 4)
	{
		return RC_ERROR;
	}
	try
	{
		cols = stoi(m[1]);
		rows = stoi(m[2]);
		depth = stoi(m[3]);
	}
	catch (const std::exception&)
	{
		return RC_ERROR;
	}

	// do not read file if dimensions are different
	if(m_depth != depth || m_rows != rows || m_cols != cols)
		return RC_ERROR;

	vector<Coordinate>* currentPlayerCoord = &playerACoord;
	while (Utils::safeGetline(sboard, line))
	{
		if (sboard.eof())
			break;

		regex regexCoord("^([0-9]+),([0-9]+),([0-9]+)");
		smatch m;
		regex_match(line, m, regexCoord);
		if (m.size() != 4)
		{
			if (line != PLAYERS_DELIMETER)
			{
				return RC_ERROR;
			}
			currentPlayerCoord = &playerBCoord;
			continue;
		}
		try
		{
			int r = stoi(m[1]);
			int c = stoi(m[2]);
			int d = stoi(m[3]);
			
			if (!legalCells[d][r][c])
				return RC_ERROR;

			currentPlayerCoord->push_back(move(Coordinate(r,c,d)));
		}
		catch (const std::exception&)
		{
			return RC_ERROR;
		}
	}
	
	return RC_SUCCESS;
}


void BattleshipAlgoSmart::removeOperationCellIfNeed(shared_ptr<Cell>& c)
{
	if (c->isOperationCell())
	{
		// cant update this in attack we need this information in notify
		c->setOperationCellStatus(false);
		m_operationCell.erase(c);
	}
}

Coordinate BattleshipAlgoSmart::attack()
{
	if (m_currentStatus == HUNT)
	{
		shared_ptr<Cell> c = popAttack();
	
		if (c->getHistValue() == INT_MAX)
		{
			m_currentStatus = TARGET;
			m_currentAttackedShipCells.push_back(c);

			// Insert cell's neighbors
			updateTargetAttackQueue(c, ShipDirection::ALL, false);

			c = popTargetAttack();
			if (nullptr == c)
			{
				return move(Coordinate(-1, -1, -1));
			}
			return move(Coordinate(c->row(), c->col(), c->depth()));
		}

		return move(Coordinate(c->row(), c->col(), c->depth()));
	}
	else /*(m_currentStatus == TARGET)*/
	{
		shared_ptr<Cell> c = popTargetAttack();
		if (nullptr == c)
		{
			return move(Coordinate(-1, -1, -1));
		}

		return move(Coordinate(c->row(), c->col(), c->depth()));
	}
}

void BattleshipAlgoSmart::handleUntargetShipSunk(shared_ptr<Cell> const attackedCell)
{
	vector<shared_ptr<Cell>> attackedShipCells = getSunkShipByCell(attackedCell);

	// erase this ship from others list
	removeOtherPlayerSunkShip(static_cast<int>(attackedShipCells.size()));

	// Update hist
	updateHist(attackedShipCells);
}

void BattleshipAlgoSmart::handleTargetShipSunk(shared_ptr<Cell> const attackedCell)
{
	m_currentStatus = HUNT;

	m_targetQueue.clear();

	// Add the cell was attacked for update the neighbors
	m_currentAttackedShipCells.push_back(attackedCell);

	// erase this ship from others list
	removeOtherPlayerSunkShip(static_cast<int>(m_currentAttackedShipCells.size()));

	// Update hist
	updateHist(m_currentAttackedShipCells);

	// update hist of neighs
	m_currentAttackedShipCells.clear();
}


void BattleshipAlgoSmart::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	int depth = move.depth;
	int row = move.row;
	int col = move.col;
	shared_ptr<Cell> attackedCell = m_board.get(depth, row, col);
	
	if ((result == AttackResult::Miss && attackedCell->isOperationCell()) || 
		result !=  AttackResult::Miss && !attackedCell->isOperationCell() && attackedCell->getPlayerIndexOwner() != m_id)
	{
		recoverBoardAwareness();
	}
	removeOperationCellIfNeed(attackedCell);

	if (m_saveNewBoardAwerness)
	{
		if (result != AttackResult::Miss && attackedCell->getPlayerIndexOwner() != m_id)
		{
			ofstream newBoardAwerness(m_newBoardAwernessFile, fstream::app);
			newBoardAwerness << move.row << "," << move.col << "," << move.depth << endl;
		}
	}

	attackedCell->hitCell();
	if (AttackResult::Hit == result || AttackResult::Sink == result)
		attackedCell->setStatus(Cell::DEAD);
	attackedCell->setHistValue(0);

	if (AttackResult::Miss == result && !attackedCell->hasShip())
	{
		// Update hist
		updateHist(vector<shared_ptr<Cell>>{attackedCell}, false);
	}
	// attack is mine
	if (m_id == player)
	{
		if (HUNT == m_currentStatus)
		{
			switch (result)
			{
			case AttackResult::Sink:
			{
				// Add the cell was attacked for update the neighbors
				m_currentAttackedShipCells.push_back(attackedCell);
				
				// erase this ship from others list
				removeOtherPlayerSunkShip(static_cast<int>(m_currentAttackedShipCells.size()));
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
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", result, __func__);
			}
		}
		else // TARGET == m_currentStatus
		{
			switch (result)
			{
			case AttackResult::Hit:
			{
				m_currentAttackedShipCells.push_back(attackedCell);

				bool toRemoveWrongAxis = false;
				if (m_currentAttackedShipCells.size() == 2)
					toRemoveWrongAxis = true;
				if (m_currentAttackedShipCells.at(0)->row() == m_currentAttackedShipCells.at(1)->row() && 
					m_currentAttackedShipCells.at(0)->depth() == m_currentAttackedShipCells.at(1)->depth())
					updateTargetAttackQueue(attackedCell, ShipDirection::HORIZONTAL, toRemoveWrongAxis);
				else if (m_currentAttackedShipCells.at(0)->col() == m_currentAttackedShipCells.at(1)->col() &&
					m_currentAttackedShipCells.at(0)->depth() == m_currentAttackedShipCells.at(1)->depth())
					updateTargetAttackQueue(attackedCell, ShipDirection::VERTICAL, toRemoveWrongAxis);
				else /* row and col are equal*/
					updateTargetAttackQueue(attackedCell, ShipDirection::DEPTH, toRemoveWrongAxis);

			} break;
			case AttackResult::Sink:
			{
				handleTargetShipSunk(attackedCell);
			}
			default: 
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", result, __func__);
			}
		}
	}
	else if (attackedCell->getPlayerIndexOwner() != getId()) // other player attacked himself
	{
		if (HUNT == m_currentStatus)
		{
			switch (result)
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
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", result, __func__);
			}
		}
		else // TARGET == m_currentStatus
		{
			switch (result)
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
				DBG(Debug::DBG_WARNING, "Unknown attack result %d in %s", result, __func__);
			}
		}
	}
}

bool BattleshipAlgoSmart::isCellNeighborToTargetShip(shared_ptr<Cell> cell)
{
	for (auto it = m_currentAttackedShipCells.begin(); it != m_currentAttackedShipCells.end(); ++it)
	{
		// If the cell is neighbor of *it cell
		if (cell->squaredDistance(*it) == 1)
			return true;
	}

	return false;
}

vector<shared_ptr<Cell>> BattleshipAlgoSmart::getSunkShipByCell(shared_ptr<Cell> const c) const
{
	int depth = c->depth();
	int row = c->row();
	int col = c->col();
	vector<shared_ptr<Cell>> result{};

	ShipDirection d = ShipDirection::HORIZONTAL;
	if (!m_board.get(depth, row + 1, col)->isEmpty() || !m_board.get(depth, row - 1, col)->isEmpty())
	{
		d = ShipDirection::VERTICAL;
	}
	else if (!m_board.get(depth, row, col + 1)->isEmpty() || !m_board.get(depth, row, col - 1)->isEmpty())
	{
		d = ShipDirection::HORIZONTAL;
	}
	else
	{
		d = ShipDirection::DEPTH;
	}

	shared_ptr<Cell> p = c;
	while (!p->isEmpty())
	{
		if (d == ShipDirection::HORIZONTAL)
			p = m_board.get(depth ,row + 1, col);
		else if (d == ShipDirection::VERTICAL)
			p = m_board.get(depth ,row, col + 1);
		else /*(d == ShipDirection::DEPTH)*/
			p = m_board.get(depth + 1, row, col);
		result.push_back(p);
	}

	p = c;
	while (!p->isEmpty())
	{
		if (d == ShipDirection::HORIZONTAL)
			p = m_board.get(depth ,row - 1, col);
		else if (d == ShipDirection::VERTICAL)
			p = m_board.get(depth, row, col - 1);
		else /*(d == ShipDirection::DEPTH)*/
			p = m_board.get(depth - 1, row, col);
		result.push_back(p);
	}

	return result;
}

void BattleshipAlgoSmart::updateHist(const vector<shared_ptr<Cell>>& cells, bool createDummyShip /*= true*/)
{
	if (createDummyShip) {
		// create dummy ship
		m_board.addDummyNewShipToBoard(cells);
	}

	// update ships neighbors hist

	for (auto it = cells.begin(); it != cells.end(); ++it)
	{
		shared_ptr<Cell> pc = *it;

		if (!createDummyShip)
			pc->setHistValue(0);

		int d = pc->depth();
		int r = pc->row();
		int c = pc->col();

		for (auto i = 1; i <= MAX_SHIP_LEN; ++i)
		{
			calcHist(Coordinate(r, c, d + i));
			calcHist(Coordinate(r, c, d - i));
			calcHist(Coordinate(r + i, c, d));
			calcHist(Coordinate(r - i, c, d));
			calcHist(Coordinate(r, c + i, d));
			calcHist(Coordinate(r, c - i, d));
		}
	}
}

void BattleshipAlgoSmart::updateTargetAttackQueue(const shared_ptr<Cell> attackedCell, ShipDirection direction, bool toRemoveWrongAxis)
{
	int depthIndex = attackedCell->depth();
	int rowIndex = attackedCell->row();
	int colIndex = attackedCell->col();

	if (toRemoveWrongAxis)
	{
		for (list<shared_ptr<Cell>>::iterator it = m_targetQueue.begin(); it != m_targetQueue.end();)
		{
			int dIndex = (*it)->depth();
			int rIndex = (*it)->row();
			int cIndex = (*it)->col();

			if (ShipDirection::HORIZONTAL == direction && (rowIndex != rIndex || depthIndex != dIndex))
				it = m_targetQueue.erase(it);
			if (ShipDirection::VERTICAL == direction && (colIndex != cIndex || depthIndex != dIndex))
				it = m_targetQueue.erase(it);
			if (ShipDirection::DEPTH == direction && (rowIndex != rIndex || colIndex != cIndex))
				it = m_targetQueue.erase(it);
			else if (it != m_targetQueue.end())
				++it;
		}

	}

	int newDepth, newRow, newCol;

	if (ShipDirection::ALL == direction || ShipDirection::VERTICAL == direction) {

		// Down
		newRow = rowIndex + 1;
		while (m_board.get(depthIndex, newRow, colIndex)->isPendingCell()) newRow++;
		if (isAttackable(m_board.get(depthIndex, newRow, colIndex)))
		{
			shared_ptr<Cell> cell = m_board.get(depthIndex, newRow, colIndex);
			if (cell->isOperationCell())
				m_targetQueue.push_front(cell);
			else
				m_targetQueue.push_back(cell);
		}

		// Up
		newRow = rowIndex - 1;
		while (m_board.get(depthIndex, newRow, colIndex)->isPendingCell()) newRow--;
		if (isAttackable(m_board.get(depthIndex, newRow, colIndex)))
		{
			shared_ptr<Cell> cell = m_board.get(depthIndex, newRow, colIndex);
			if (cell->isOperationCell())
				m_targetQueue.push_front(cell);
			else
				m_targetQueue.push_back(cell);
		}
	}
	if (ShipDirection::ALL == direction || ShipDirection::HORIZONTAL == direction)
	{
		// Right
		newCol = colIndex + 1;
		while (m_board.get(depthIndex, rowIndex, newCol)->isPendingCell()) newCol++;
		if (isAttackable(m_board.get(depthIndex, rowIndex, newCol)))
		{
			shared_ptr<Cell> cell = m_board.get(depthIndex, rowIndex, newCol);
			if (cell->isOperationCell())
				m_targetQueue.push_front(cell);
			else
				m_targetQueue.push_back(cell);
		}

		// Left
		newCol = colIndex - 1;
		while (m_board.get(depthIndex, rowIndex, newCol)->isPendingCell()) newCol--;
		if (isAttackable(m_board.get(depthIndex, rowIndex, newCol)))
		{
			shared_ptr<Cell> cell = m_board.get(depthIndex, rowIndex, newCol);
			if (cell->isOperationCell())
				m_targetQueue.push_front(cell);
			else
				m_targetQueue.push_back(cell);
		}
	}
	if (ShipDirection::ALL == direction || ShipDirection::DEPTH == direction)
	{
		// Right
		newDepth = depthIndex + 1;
		while (m_board.get(newDepth, rowIndex, colIndex)->isPendingCell()) newDepth++;
		if (isAttackable(m_board.get(newDepth, rowIndex, colIndex)))
		{
			shared_ptr<Cell> cell = m_board.get(newDepth, rowIndex, colIndex);
			if (cell->isOperationCell())
				m_targetQueue.push_front(cell);
			else
				m_targetQueue.push_back(cell);
		}

		// Left
		newDepth = depthIndex - 1;
		while (m_board.get(newDepth, rowIndex, colIndex)->isPendingCell()) newDepth--;
		if (isAttackable(m_board.get(newDepth, rowIndex, colIndex)))
		{
			shared_ptr<Cell> cell = m_board.get(newDepth, rowIndex, colIndex);
			if (cell->isOperationCell())
				m_targetQueue.push_front(cell);
			else
				m_targetQueue.push_back(cell);
		}
	}
}

bool BattleshipAlgoSmart::isAttackable(const shared_ptr<Cell> c) const
{
	return (!m_board.isPaddingCell(c)) && c->isEmpty();

}

void BattleshipAlgoSmart::calcHist(Coordinate c)
{
	int d = c.depth;
	int i = c.row;
	int j = c.col;


	if (d <= 0 || d > m_depth || i <= 0 || i > m_rows || j <= 0 || j > m_cols)
		return;

	auto cell = m_board.get(d, i, j);

	if (!cell->isEmpty())
	{
		cell->setHistValue(0);
		return;
	}

	// Check there are no neighbors ship
	if (!m_board.get(d + 1, i, j)->isEmpty() ||
		!m_board.get(d - 1, i, j)->isEmpty() ||
		!m_board.get(d ,i + 1, j)->isEmpty() ||
		!m_board.get(d, i - 1, j)->isEmpty() ||
		!m_board.get(d, i, j + 1)->isEmpty() ||
		!m_board.get(d, i, j - 1)->isEmpty())
	{
		cell->setHistValue(0);
		return;
	}
	
	auto numOfPotentialShips = 1;
	if (cell->isOperationCell())
	{
		numOfPotentialShips += BOARD_AWARENESS_HIST_VAL;
	}

	map<Direction, int> maxIndexOfValid = {
		{ Direction::INSIDE, MAX_SHIP_LEN - 1 },
		{ Direction::OUTSIDE, MAX_SHIP_LEN - 1 },
		{ Direction::UP, MAX_SHIP_LEN - 1 },
		{ Direction::DOWN, MAX_SHIP_LEN - 1 },
		{ Direction::LEFT, MAX_SHIP_LEN - 1 },
		{ Direction::RIGHT, MAX_SHIP_LEN - 1 }
	};


	for (auto d_i = 0; d_i < NUM_OF_DIRECTIONS; ++d_i)
	{
		for (auto shipLen = 1; shipLen < MAX_SHIP_LEN; ++shipLen)
		{
			auto dir = static_cast<Direction>(d_i);
			auto od = Utils::getOppositeDirection(dir);

			auto checkedDepthIndex = d;
			auto checkedRowIndex = i;
			auto checkedColIndex = j;

			switch (dir)
			{
			case Direction::INSIDE:
				checkedDepthIndex -= shipLen;
				break;
			case Direction::OUTSIDE:
				checkedDepthIndex += shipLen;
				break;
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
			
			if (!m_board.isValidCell(checkedDepthIndex, checkedRowIndex, checkedColIndex))
			{
				maxIndexOfValid[dir] = shipLen - 1;
				break;
			}

			auto checkedCell = m_board.get(checkedDepthIndex, checkedRowIndex, checkedColIndex);

			if ((checkedCell->isHitted() && !checkedCell->hasShip()) ||
				!isOtherNeighborsValid(checkedCell, od))
			{
				maxIndexOfValid[dir] = shipLen - 1;
				break;
			}
		} // ShipLen
	} // Direction


	// Calc
	numOfPotentialShips += getPotentialShipsByStrip({maxIndexOfValid.at(Direction::INSIDE), maxIndexOfValid.at(Direction::OUTSIDE)});
	numOfPotentialShips += getPotentialShipsByStrip({maxIndexOfValid.at(Direction::UP), maxIndexOfValid.at(Direction::DOWN)});
	numOfPotentialShips += getPotentialShipsByStrip({maxIndexOfValid.at(Direction::LEFT), maxIndexOfValid.at(Direction::RIGHT)});

	cell->setHistValue(numOfPotentialShips);
}


bool BattleshipAlgoSmart::isOtherNeighborsValid(const shared_ptr<Cell> cell, Direction d) const
{

	if (m_board.isPaddingCell(cell))
		return false;

	auto ret = false;
	auto dIndex = cell->depth();
	auto rIndex = cell->row();
	auto cIndex = cell->col();

	switch (d)
	{
	case Direction::INSIDE:
	{
		ret = m_board.get(dIndex - 1, rIndex, cIndex)->isEmpty() &&	// Check outside 
			m_board.get(dIndex, rIndex - 1, cIndex)->isEmpty() &&	// Check up
			m_board.get(dIndex, rIndex + 1, cIndex)->isEmpty() &&	// Check down
			m_board.get(dIndex, rIndex, cIndex + 1)->isEmpty() &&	// Check right
			m_board.get(dIndex, rIndex, cIndex - 1)->isEmpty();		// Check left
	}
	break;
	case Direction::OUTSIDE: {
		ret = 
			m_board.get(dIndex + 1, rIndex, cIndex)->isEmpty() &&	// Check inside
			m_board.get(dIndex, rIndex - 1, cIndex)->isEmpty() &&	// Check up
			m_board.get(dIndex, rIndex + 1, cIndex)->isEmpty() &&	// Check down
			m_board.get(dIndex, rIndex, cIndex + 1)->isEmpty() &&	// Check right
			m_board.get(dIndex, rIndex, cIndex - 1)->isEmpty();		// Check left
	}
	break;
	case Direction::UP:
	{
		ret = m_board.get(dIndex, rIndex + 1, cIndex)->isEmpty() &&	// Check down
			m_board.get(dIndex + 1, rIndex, cIndex)->isEmpty() &&	// Check inside
			m_board.get(dIndex - 1, rIndex, cIndex)->isEmpty() &&	// Check outside
			m_board.get(dIndex, rIndex, cIndex + 1)->isEmpty() &&	// Check right
			m_board.get(dIndex, rIndex, cIndex - 1)->isEmpty();		// Check left
	}
	break;
	case Direction::DOWN: {
		ret = m_board.get(dIndex, rIndex - 1, cIndex)->isEmpty() &&	// Check up
			m_board.get(dIndex + 1, rIndex, cIndex)->isEmpty() &&	// Check inside
			m_board.get(dIndex - 1, rIndex, cIndex)->isEmpty() &&	// Check outside
			m_board.get(dIndex, rIndex, cIndex + 1)->isEmpty() &&	// Check right
			m_board.get(dIndex, rIndex, cIndex - 1)->isEmpty();		// Check left
	}
	break;
	case Direction::RIGHT: {
		ret = m_board.get(dIndex, rIndex - 1, cIndex)->isEmpty() &&	// Check up
			m_board.get(dIndex + 1, rIndex, cIndex)->isEmpty() &&	// Check inside
			m_board.get(dIndex - 1, rIndex, cIndex)->isEmpty() &&	// Check outside
			m_board.get(dIndex, rIndex + 1, cIndex)->isEmpty() &&	// Check down
			m_board.get(dIndex, rIndex, cIndex - 1)->isEmpty();		// Check left
	}
	break;
	case Direction::LEFT: {
		ret = m_board.get(dIndex, rIndex - 1, cIndex)->isEmpty() &&	// Check up
			m_board.get(dIndex + 1, rIndex, cIndex)->isEmpty() &&	// Check inside
			m_board.get(dIndex - 1, rIndex, cIndex)->isEmpty() &&	// Check outside
			m_board.get(dIndex, rIndex + 1, cIndex)->isEmpty() &&	// Check down
			m_board.get(dIndex, rIndex, cIndex + 1)->isEmpty();		// Check right
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
	// enable ship awerness
	initStripSizeToNumPotentialShipsPerShip();
}

void BattleshipAlgoSmart::initStripSizeToNumPotentialShipsPerShip()
{
	// init maps
	for (int shipLen = 0; shipLen <= MAX_SHIP_LEN; shipLen++)
	{
		m_stripToPotentialShipsPerShip.push_back(map<pair<int, int>, int>());
	}

	// calc maps
	for (int shipLen = 2; shipLen <= MAX_SHIP_LEN; shipLen++)
	{
		// has right
		for (int i = 0; i < MAX_SHIP_LEN; i++)
		{
			// has left
			for (int j = 0; j < MAX_SHIP_LEN; j++)
			{
				int numPotentialShips = 0;
				for (int offset = 0; offset < shipLen; offset++)
				{
					if (isShipValidInOffset(shipLen, offset, i, j))
					{
						numPotentialShips++;
					}
				}
				m_stripToPotentialShipsPerShip[shipLen].emplace(make_pair(make_pair(i, j), numPotentialShips));
			}
		}
	}
}

int BattleshipAlgoSmart::getPotentialShipsByStrip(pair<int, int>strip)
{
	int numOfPotentialShips = 0;
	if (m_shipAwarenessStatus)
	{
		for (auto const& ship : m_otherPlayerShips)
		{
			// ignore ships of size 1, calculated separetely
			if (1 != ship)
				numOfPotentialShips += m_stripToPotentialShipsPerShip[ship][strip];
		}
	}
	else
	{
		numOfPotentialShips = m_stripToPotentialShips[strip];
	}
	
	return numOfPotentialShips;
}

void BattleshipAlgoSmart::removeOtherPlayerSunkShip(int len)
{
	for (auto it = m_otherPlayerShips.begin(); it != m_otherPlayerShips.end(); ++it)
	{
		if (*it == len)
		{
			m_otherPlayerShips.erase(it);
			// TODO Board Awerness
			return;
		}
	}
	// NOTE: if we got here that means that someone delivered illegal board
	// but the game manager decided to continue playing. illegal board means
	// that the number of ships and the type of ships is not equal for both players
	// in order to continue playing we need to disable ship awareness feature
	m_shipAwarenessStatus = false;
}

IBattleshipGameAlgo* GetAlgorithm()
{
	return (new BattleshipAlgoSmart());
}