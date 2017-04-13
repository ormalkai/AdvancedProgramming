#pragma once

#include "Ship.h"
#include "Utils.h"

class Cell {
	
public:
	typedef  enum CellStatus {
		ALIVE,
		DEAD,
		FREE
	}CellStatus;

private:
	CellStatus m_status;
	Ship* m_pShip;
	PlayerIndex m_playerIndexOwner;


public:
	Cell();
	~Cell();
	
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
	bool isEmpty() const { return  nullptr == m_pShip; }
	
	/**
	* @Details		returns player index of the cell
	* @return		player index of cell's owner
	*/
	PlayerIndex getPlayerIndexOwner() const { return m_playerIndexOwner; }	

	/**
	* @Details		initialize the cell: status-FREE, ship's ptr - null, owner - MAX_PLAYER
	*/
	void clear() { m_status = FREE; m_pShip = nullptr; m_playerIndexOwner = MAX_PLAYER; }
};