#pragma once

#include <list>
#include "PriorityQueue.h"
#include "IBattleshipGameAlgo.h"
#include "Board.h"
#include <iostream>

/**
 *
 * Description of the algorithm:
 * Basically the player has two modes of attacks, the first is Hunt, the second is Target.
 * In Hunt mode, the player wants to find a cell which contains a ship or part of a ship, then
 * switch to target mode. In target mode the player is pointed the attacks until the ship is sunk,
 * and so on.
 * 
 * Hunt mode:
 *		In Hunt mode the player creates histogram of how many ships can use the cell for each cell.
 *		For example, at the corners, the value can be at most 7:
 *			1. One for size of size 1
 *			2. Two for ships of size 2 (one vertical and one horizontal)
 *			3. Two for ships of size 3 (one vertical and one horizontal)
 *			4. Two for ships of size 4 (one vertical and one horizontal)
 *		The same calculation can be executed for cells in the middle of the board.
 *		During the calculation, the player taking care of game limitations:
 *			1. There are no Adjacent ships.
 *			2. Ship can not be outside the border of the board.
 *		The histogram is updated every attack (will be explained later).
 *		The player holds a priority queue which maps between the cell to its value in the histogram.
 *		That way the player in hunt mode will attack the cells with the highest probabillity it will contain
 *		a ship.
 *		
 *	Target mode:
 *		In Target mode the player holds a queue of potential cells which can contain the part of the targeted ship.
 *		In the first hit, all 4 neighbors(if exists) are inserted to the queue, in the second hit we can determine
 *		the direction of the ship, horizontal or vertical, therefore we can make corrections in the queue and remove, 
 *		the potential cells from the other axis. from that point we will add neighbors to the queue in every hit only
 *		if the are in the same direction of the ship.
 *		The player will exit target mode only after the ship is sunk.
 *		While exiting from target mode in returning to hunt mode the player updates the histogram according to the ship
 *		sunk.
 *		
 *	Creating and Updating the Histogram:
 *		Creation of the histogram is basically calculation of each cell in the board while considering my ships.
 *		Updating of the histogram is calculation of the relevant cells accrordiong to the change was triggerd the update event.
 *		The histogram value of each cell is the number of options (potential ships) that this cell could be part of them.
 *		Cell that part of a ship (ship cell) will be with value 0.
 *		Cell that neighbor of ship cell will be with value 0.
 *		We calculate the farthest cell each direction (Up,Down,Left,Right) and save the values.
 *		The farthest cell is the cell is in the board (not exceeds the limits) and not a neighbor of another ship. 
 *		Then, we calculate for each ship's length: how many ships can be at this cell and sum all of that.
 *		
 *		
 *	Handling attack results:
 *		We need to define between several situations:
 *		1. The player made an attack and it was successfull and the player is in hunt mode:
 *				That means that the player found a part of the ship, we need to add its
 *				neighbors to the target queue and switch to target mode.
 *		2. The player made an attack and it was successfull and the player is in target mode:
 *				If this is the first hit in target mode we can determine the direction
 *				of the ship vertical or horizontal, and remove irrelevant cells from the 
 *				attacks queue, if it is not the first hit in target mode we need to add only one
 *				cell to attack queue, the one that is a neighbor to the attacked cell in the 
 *				same direction of the ship(if it exists).
 *		
 *		3. The player made an attack and missed and the player is in hunt mode:
 *				The player need to update the histogram  in the area of the missed cell
 *				and stay in hunt mode.
 *		4. The player made an attack and missed and the player is in target mode:
 *				Bassically, nothing to do, stay in target mode, next time the player will
 *				take another cell from the queue.
 *		
 *		5. The player made an attack and ship sunk and the player is in hunt mode:
 *				This case can happen only if the player sunk a ship of size 1.
 *				Therefore the handling is the same as sunk in target mode. the player
 *				needs to update the histogram accroding to the new ship, and stay in hunt mode.
 *		6. The player made an attack and ship sunk and the player is in target mode:
 *				In this case the player needs to update the histogram according to the new ship
 *				and switch to hunt mode.
 *		
 *		7. The other player made an attack and it was successfull and the player is in hunt mode:
 *				Nothing to do
 *		8. The other player made an attack and it was successfull and the player is in target mode:
 *				Nothing to do
 *				
 *		9. The other player made an attack and it was SELF successfull and the player is in hunt mode:
 *				The player should act exactly like it was him discovering the ship, he needs to switch
 *				to Target mode and add the neighbors of the attacked cell to the target queue.
 *		10. The other player made an attack and it was SELF successfull and the player is in target mode:
 *				The player can not exit from target mode unless the ship is sunk, therefore that attacked
 *				value of the attacked cell will be updated to MAX_INT in the histogram, that way it will
 *				be the first cell the player will choose in the next attack in hunt mode, the player 
 *				should recognize the mark of MAX_INT and switch to target mode again with its neighbors
 *				and attack one of them.
 *				
 *		11. The other player made an attack and missed and the player is in hunt mode:
 *				In this case that means that there is no ship in the attacked cell, therfore
 *				the histogram should be updated, the attacked cell value should be 0, and its
 *				area should be updated accordingly.
 *		12. The other player made an attack and missed and the player is in target mode:
 *				In this case that means that there is no ship in the attacked cell, therfore
 *				the histogram should be updated, the attacked cell value should be 0, and its
 *				area should be updated accordingly.
 *		
 *		13. The other player made an attack and ship sunk and the player is in hunt mode:
 *				nothing to do
 *		14. The other player made an attack and ship sunk and the player is in hunt mode:
 *				nothing to do
 *		
 *		15. The other player made an attack and SELF ship sunk and the player is in hunt mode:
 *				This case can happen only if the other player sunk a ship of size 1.
 *				Therefore the handling is the same as sunk in target mode. the player
 *				needs to update the histogram accroding to the new ship, and stay in hunt mode.
 *		16. The other player made an attack and SELF ship sunk and the player is in target mode:
 *				This case can happen only if the other player sunk its own ship while the player
 *				is in target mode, the trivial example is sinking ship of size 1, but non-trivial
 *				example will be hitting ship of size 2 twice in a row, while the target mode
 *				is pointing to ship of size 4. In this case the player should find out the size of 
 *				the sunk ship by looking that MAX_INT marks next to the sunk cell, this sequence 
 *				should be the ship, then the histogram should be updated as it should be updated 
 *				while sinking in target mode.
 *
 */

class BattleshipAlgoSmart : public IBattleshipGameAlgo
{

	enum SmartAlgoStautus {
		HUNT,
		TARGET
	};

	struct cmp
	{
		bool operator()(shared_ptr<Cell> a, shared_ptr<Cell> b) const
		{
			return a->getHistValue() < b->getHistValue();
		}
	};
private:
	int		m_id;			// unique identifier of the player (PlayerA/PlayerB)
	int		m_rows;			// number of rows in the board
	int		m_cols;			// number of columns in the board
	int		m_depth;		// depth level of the board
	Board	m_board;
	PriorityQueue<shared_ptr<Cell>, vector<shared_ptr<Cell>>, cmp> m_attackedQueue;
	list<shared_ptr<Cell>> m_targetQueue;
	SmartAlgoStautus m_currentStatus;
	vector<shared_ptr<Cell>> m_currentAttackedShipCells;
	map<pair<int, int>, int> m_stripToPotentialShips;
	
	/**
	* @Details		Return the next attack request in hunt mode - the cell with the highest hist value
	* @Return		The next cell to be attacked
	*/
	shared_ptr<Cell> popAttack();

	/**
	* @Details		Return the next attack request in target mode - the cell is negihbor on the last attack
	* @Return		The next cell to be attacked
	*/
	shared_ptr<Cell> popTargetAttack()
	{
		shared_ptr<Cell> c = m_targetQueue.front();
		m_targetQueue.pop_front();
		return c;
	}

	/**
	* @Details		Handle case (update hist value for relevent cells) sink not it target mode - for example: we hit ship of one cell
	* @param		attackedCell - The attacked cell's pointer
	*/
	void handleUntargetShipSunk(shared_ptr<Cell> const attackedCell);
	
	/**
	* @Details		Handle case sink not it target mode - for example: we hit ship of one cell
	* @param		attackedCell - The attacked cell's pointer
	*/
	void handleTargetShipSunk(shared_ptr<Cell> const attackedCell);

	/**
	* @Details		check if a given cell is neighbor of one of the cells in attack target queue
	* @Param		cell - given cell to check
	*/
	bool isCellNeighborToTargetShip(shared_ptr<Cell> cell);

	/**
	* @Details		get vector of all cells of sunk ship by given cell is part of the ship
	* @Param		cell - given cell in the sunk ship
	*/
	vector<shared_ptr<Cell>> getSunkShipByCell(shared_ptr<Cell> const c) const;

	/**
	* @Details		This function update the hist value only for the relevant cells by given ship as vector of cells
	* @Param		cells - vector of ship cells to update their neigbors
	* @Param		createDummyShip - true if create dummy ship
	*/
	void updateHist(const vector<shared_ptr<Cell>>& cells, bool createDummyShip = true);

	/**
	* @Details		This function update the attack target queue by given cell and ship direction
	* @Param		attackedCell - attacked cell is part of ship
	* @Param		direction - ship is direction vertical \ horizontal
	* @Param		toRemoveWrongAxis - true if need to remove irrelevant cells from the attacked queue
	*/
	void updateTargetAttackQueue(const shared_ptr<Cell> attackedCell, ShipDirection direction, bool toRemoveWrongAxis);

	/**
	* @Details		Return true if the cell is attackable - not padding, empty and legal cell
	* @Param		Cell - cell to check attackable
	*/
	bool isAttackable(const shared_ptr<Cell> c) const;


	/**
	* @Details		Calculate and update hist value of cell by given 2 indexes
	* @Param		i - row index
	* @Param		j - col index
	*/
	void calcHist(Coordinate c);

	/**
	* @Details		Check if all neighbors of given cell except the neighbor in the direction d, are lagal.
	* @Param		cell - cell to check his neighbors
	* @Param		d - the only direction we do not have to check
	*/
	bool isOtherNeighborsValid(const shared_ptr<Cell> cell, Direction d) const;

	/**
	* @Details		Internal function for check is ship is legal is specific offset.
	* @Param		shipLen - ship's length
	* @Param		offset - the checked offset
	* @Param		hasLeft - cell to the left
	* @Param		hasLeft - cell to the right
	*/
	static bool isShipValidInOffset(int shipLen, int offset, int hasLeft, int hasRight);

	/**
	* @Details		Initialize the board by number of potential ships.
	*/
	void initStripSizeToNumPotentialShips();

public:
	/**
	* @Details		Constructor receives id of the player
	*/
	BattleshipAlgoSmart(): m_id(-1), m_rows(-1), m_cols(-1), m_depth(-1), m_currentStatus(HUNT){}

	/**
	* @Details		Default destructor
	*/
	~BattleshipAlgoSmart() = default;

	BattleshipAlgoSmart(const BattleshipAlgoSmart &) = delete;
	BattleshipAlgoSmart &operator=(const BattleshipAlgoSmart &) = delete;
	BattleshipAlgoSmart(BattleshipAlgoSmart &&) = delete;

	/**
	* @Details		getter for id
	*/
	int getId() const { return m_id; }

	/**
	* @Details		setter for id
	*/
	void setId(int id) { m_id = id; }


	/**
	* @Details		Receive board matrix and matrix's size, and notify player on his board
	* @Param		board - player's board
	*/
	void setBoard(const BoardData& board) override;

	/**
	* @Details		Return next attack request
	* @retrun		Coordinate - requested attack.
	*				in case the queue is empty returns nullptr
	*/
	Coordinate attack() override;

	/**
	* @Details		notify player on last move result
	* @Param		player - player's index
	* @Param		row - attacked cell's row index
	* @Param		col - attacked cell's col index
	* @Param		result - attack's result (hit, miss, sink)
	*/
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;
	
	/**
	* @Details		Set player id
	* @Param		player - player id
	*/
	void setPlayer(int player) override;


};


