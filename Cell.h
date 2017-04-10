#pragma once

#include "Ship.h"

class Cell {
public:
	enum CellStatus {
		ALIVE,
		DEAD
	};

	Cell();
	~Cell();

	CellStatus getStatus() { return  m_status; }
	char getSign() const
	{
		return nullptr != m_pShip ? m_pShip->getSign() : SPACE;
	}

private:
	CellStatus m_status;
	Ship* m_pShip;

};