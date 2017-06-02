#pragma once

#include "Ship.h"
#include "Utils.h"
#include "IBattleshipGameAlgo.h"

class Cell {
	
public:

	/**
	 * Details		enum CellStatus indicates The status of the ship in it, or whether it is free.
	 */
	typedef  enum CellStatus {
		ALIVE,	// Ship has never hitted
		DEAD,	// Ship was hitted
		FREE	// No ship is this cell
	}CellStatus;

private:
	CellStatus			m_status;				// The status of the cell according to the enum above
	shared_ptr<Ship>	m_pShip;				// pointer to the ship in this cell, NULL if no ship
	PlayerIndex			m_playerIndexOwner;		// The unique identifier of the player own the ship
	
	Coordinate			m_coord;				// Cell's coordinates

	int					m_histValue;			// The histogram value - number of ship this cell could be part of them
	bool				m_isHitted;				// Indicator if the cell was hitted		


public:
	/**
	 * @Details		The constructor initiates the members to status FREE, pShip to nullptr and player index to MAX_PLAYER
	 */
	Cell();

	/*
	 * @Details		Default destructors
	 */
	~Cell() = default;
	
	Cell(const Cell &) = default;
	Cell &operator=(const Cell &) = default;
	Cell(Cell &&) = default;

	/**
	* @Details		returns cell's status
	* @Return		CellStatus::ALIVE if cell is part of ship and cell wasn't hitted,
	*				CellStatus::FREE if cell isn't part of ship and cell wasn't hitted,
	*				CellStatus::DEAD if cell was hitted
	*/
	CellStatus getStatus() const { return  m_status; }

	/**
	* @Details		returns cell's sign
	* @Return		If cell is part of ship returns ship's sign otherwise SPACE
	*/
	char getSign() const {
		return nullptr != m_pShip ? m_pShip->getSign() : SPACE;
	}

	/**
	* @Details		returns cell's ship pointer
	* @Return		If cell is part of ship returns ship's pointer otherwise nullptr
	*/
	shared_ptr<Ship> getShip() const { return m_pShip; }

	/**
	* @Details		receives ship pointer and update cell's ship 
  	* @param		ship - pointer to ship
	*/
	void setShip(shared_ptr<Ship> ship);
	
	/**
	* @Details		receives cell status and update cell's status
	* @param		status - cell's new status 
	*/
	void setStatus(CellStatus status) { m_status = status; }
	
	/**
	* @Details		Check cell's status
	* @return		If cell is part of ship return true, otherwise false
	*/
	bool isEmpty() const { return  FREE == m_status; }
	
	/**
	* @Details		returns player index of the cell
	* @return		player index of cell's owner
	*/
	PlayerIndex getPlayerIndexOwner() const { return m_playerIndexOwner; }	

	/**
	* @Details		initialize the cell: status-FREE, ship's ptr - null, owner - MAX_PLAYER
	*/
	void clear() { m_status = FREE; m_pShip = nullptr; m_playerIndexOwner = MAX_PLAYER; }
	
	/**
	* @Details		getter for row index of the cell
	* @return		cell's row index
	*/
	int row() const { return m_coord.row; }
	
	/**
	* @Details		getter for col index of the cell
	* @return		cell's col index
	*/
	int col() const { return m_coord.col; }

	/**
	* @Details		getter for depth index of the cell
	* @return		cell's depth index
	*/
	int depth() const { return m_coord.depth; }
	
	/**
	* @Details		setter of histogram value in the cell
	* @param		h - given hist value
	*/
	void setHistValue(int h) { m_histValue = h; }
	
	/**
	* @Details		getter of histogram value in the cell
	* @return		cell's histogram value
	*/
	int getHistValue() const { return m_histValue; }
	
	/**
	* @Details		this function check if the cell is pending to attack (in the attack queue because one of his neighbors was discovered as part of ship)
	* @return		true of the cell in attack queue, false otherwise
	*/
	bool isPendingCell() const { return INT_MAX == m_histValue; }

	/**
	* @Details		this function calculates the squared distance of current cell from a given cell
	* @return		oc - other cell to calc distance
	* @return		the squared distance
	*/
	double squaredDistance(shared_ptr<Cell> oc) const {
		return (pow(oc->row() - this->row(), 2) + pow(oc->col() - this->col(), 2));
	}
	
	/**
	* @Details		setter for indexes of the cell
	* @param		d - depth index
	* @param		r - row index
	* @param		c - column index
	*/
	void setIndexes(int d, int r, int c) { m_coord.depth = d; m_coord.row = r; m_coord.col = c;}

	/**
	* @Details		check if this cell is part of ship
	* @return		true - if the cell is part of ship, o.w false
	*/
	bool hasShip() const { return nullptr != m_pShip; }

	/**
	* @Details		check if this cell was attacked
	* @return		true - if the cell was attacked, o.w false
	*/
	bool isHitted() const { return m_isHitted; }

	/**
	* @Details		hit cell - set indicator this cell was attacked
	*/
	void hitCell() { m_isHitted = true; }
};
