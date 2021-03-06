

#include "Debug.h"
#include "BattleshipAlgoNaive.h"


#define NAIVE_ATTACK_HIT '*'
#define NAIVE_ATTACK_SINK '$'
#define NAIVE_ATTACK_MISS '&'

void BattleshipAlgoNaive::setBoard(int player, const char** board, int numRows, int numCols)
{
	m_id = player;

	if (NULL == board)
		return;

	m_id = player;
	m_rows = numRows;
	m_cols = numCols;

	for (int i=0; i<m_rows + BOARD_PADDING; i++)
	{
		for (int j=0; j<m_cols + BOARD_PADDING; j++)
		{
			m_board[i][j] = SPACE;
		}
	}

	for (int i = 1; i<= m_rows; i++)
	{
		for (int j = 1; j<= m_cols; j++)
		{
			m_board[i][j] = board[i-1][j-1];
		}
	}
}

void BattleshipAlgoNaive::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	if (player != this->getId())
	{
		// nothing to update this is my attack
		return;
	}
	if (AttackResult::Hit == result)
	{
		m_board[row][col] = NAIVE_ATTACK_HIT;
	}
	else if (AttackResult::Sink == result)
	{
		m_board[row][col] = NAIVE_ATTACK_SINK;
	}
	else /* AttackResult::Miss == result */
	{
		m_board[row][col] = NAIVE_ATTACK_MISS;
	}
}

bool BattleshipAlgoNaive::init(const std::string & path)
{
	m_nextAttack.first = 1;
	m_nextAttack.second = 1;

	return true;
}

bool BattleshipAlgoNaive::isAlreadyAttacked(char c)
{
	if (NAIVE_ATTACK_HIT != c &&
		NAIVE_ATTACK_SINK != c &&
		NAIVE_ATTACK_MISS != c)
	{
		DBG(Debug::DBG_DEBUG, "Not attacked yet");
		return false;
	}
	DBG(Debug::DBG_DEBUG, "Already Attacked");
	return true;
}

bool BattleshipAlgoNaive::isNeighborsMine(int i, int j)
{
	PlayerIndex myId = static_cast<PlayerIndex>(this->getId());
	if (myId == Utils::getPlayerIdByShip(m_board[i][j]) ||
		myId == Utils::getPlayerIdByShip(m_board[i-1][j]) ||
		myId == Utils::getPlayerIdByShip(m_board[i][j-1]) ||
		myId == Utils::getPlayerIdByShip(m_board[i+1][j]) ||
		myId == Utils::getPlayerIdByShip(m_board[i][j+1]))
	{
		DBG(Debug::DBG_DEBUG, "Has neihbors mine");
		return true;
	}
	DBG(Debug::DBG_DEBUG, "No neighbors mine myId[%d] cell id[%c][%d]", myId, m_board[i][j], Utils::getPlayerIdByShip(m_board[i][j]));
	return false;
}

bool BattleshipAlgoNaive::hasShipOnTheLeft(int i, int j)
{
	// ship on the left can be vertical or horizontal:
	//	NAIVE_ATTACK_HIT
	//  NAIVE_ATTACK_HIT  <attack is here>
	// or
	//  NAIVE_ATTACK_SINK <attack is here>
	if (NAIVE_ATTACK_HIT != m_board[i][j - 1] && NAIVE_ATTACK_SINK != m_board[i][j - 1])
	{
		DBG(Debug::DBG_DEBUG, "no sink or hit on the left");
		return false;
	}
	// now we know that the cell on my left was hitted
	// if the cell on my left is sink no matter what we cant attack here
	// if my cell on top left is HIT that means that there is a vertical ship on my left
	// cell on my left is sink
	if (NAIVE_ATTACK_SINK == m_board[i][j - 1] ||
		NAIVE_ATTACK_HIT  == m_board[i - 1][j - 1])
	{
		DBG(Debug::DBG_DEBUG, "top left sink or hit");
		return true;
	}
	DBG(Debug::DBG_DEBUG, "no ship on the left");
	return false;
}

bool BattleshipAlgoNaive::hasShipOnTheTop(int i, int j)
{
	// ship on the top can be vertical or horizontal:
	//	NAIVE_ATTACK_HIT	NAIVE_ATTACK_HIT	NAIVE_ATTACK_HIT
	//						<attack is here>
	// or
	//  NAIVE_ATTACK_SINK 
	//	<attack is here>
	if (NAIVE_ATTACK_HIT != m_board[i - 1][j] && NAIVE_ATTACK_SINK != m_board[i - 1][j])
	{
		DBG(Debug::DBG_DEBUG, "no hit or sink on top");
		return false;
	}
	// now we know that the cell on my top was hitted
	// if the cell on my top is sink no matter what we cant attack here
	// if my cell on top left or top right is HIT that means that there is a horizontal ship on my top
	// cell on my left is sink
	if (NAIVE_ATTACK_SINK == m_board[i - 1][j] ||
		NAIVE_ATTACK_HIT == m_board[i - 1][j - 1] ||
		NAIVE_ATTACK_HIT == m_board[i - 1][j + 1])
	{
		DBG(Debug::DBG_DEBUG, "sink on top or hit on top left or right");
		return true;
	}
	DBG(Debug::DBG_DEBUG, "No ship on top");
	return false;
}

pair<int, int> BattleshipAlgoNaive::attack()
{
	for (;m_nextAttack.first <= m_rows; m_nextAttack.first++)
	{
		for (; m_nextAttack.second <= m_cols; m_nextAttack.second++)
		{
			char c = m_board[m_nextAttack.first][m_nextAttack.second];
			// if one of the conditions met -> continue

			// check if this cell has already been attacked
			if (true == isAlreadyAttacked(c) ||
				true == isNeighborsMine(m_nextAttack.first, m_nextAttack.second))
				/* for future use:
				 * the naive algorithm can take care of information he gathers during the game
				 * currently the naive algorithm looks only at his ships
				true == hasShipOnTheLeft(m_nextAttack.first, m_nextAttack.second) ||*/
				/*true == hasShipOnTheTop(m_nextAttack.first, m_nextAttack.second)*/
			{
				continue;
			}
			// else attack
			DBG(Debug::DBG_DEBUG, "Attack (%d,%d)", m_nextAttack.first, m_nextAttack.second);
			return m_nextAttack;
		}
		m_nextAttack.second = 1;
	}
	DBG(Debug::DBG_DEBUG, "No more attacks, (%d, %d)", m_nextAttack.first, m_nextAttack.second);
	return make_pair(-1, -1);
}

IBattleshipGameAlgo* GetAlgorithm()
{
	return (new BattleshipAlgoNaive());
}