#pragma once

#include "Ship.h"
#include "Utils.h"

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
	CellStatus		m_status;				// The status of the cell according to the enum above
	Ship*			m_pShip;				// pointer to the ship in this cell, NULL if no ship
	PlayerIndex		m_playerIndexOwner;		// The unique identifier of the player own the ship
	int				m_rowIndex;				// Cell's row index
	int				m_colIndex;				// Cell's col index
	int				m_histValue;			// The histogram value - number of ship this cell could be part of them
	bool			m_isHitted;				// Indicator if the cell was hitted		


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
	Ship* getShip() const { return m_pShip; }

	/**
	* @Details		receives ship pointer and update cell's ship 
  	* @param		ship - pointer to ship
	*/
	void setShip(Ship* ship);
	
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
	int row() const { return m_rowIndex; }
	
	/**
	* @Details		getter for col index of the cell
	* @return		cell's col index
	*/
	int col() const { return m_colIndex; }
	
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
	* @Details		this function check if the cell is pending - in the attack queue (because one of his neighbors was discovered as part of ship)
	* @return		cell's histogram value
	*/
	bool isPendingCell() const { return INT_MAX == m_histValue; }
	double squaredDistance(Cell* oc) const {
		return (pow(oc->row() - this->row(), 2) + pow(oc->col() - this->col(), 2));
	}
	void setIndexes(int r, int c) { m_rowIndex = r; m_colIndex = c; }

	bool hasShip() const { return nullptr != m_pShip; }
	bool isHitted() const { return m_isHitted; }

	void hitCell() { m_isHitted = true; }
};