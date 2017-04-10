#pragma once

#include "Ship.h"
#include "Utils.h"

class Cell {
public:
	enum CellStatus {
		ALIVE,
		DEAD
	};

	Cell();
	~Cell();

	CellStatus getStatus() { return  m_status; }
	
	CellStatus getStatus() const { return  m_status; }
	char getSign() const
	{
		return nullptr != m_pShip ? m_pShip->getSign() : SPACE;
	}
	int row() const { return m_rIndex; }
	int col() const { return m_cIndex; }
	Ship* getShip() const { return m_pShip; }
	
	
	void clear() { m_status = DEAD; m_pShip = nullptr; }

private:
	CellStatus m_status;
	Ship* m_pShip;
	int m_rIndex, m_cIndex;
};