#pragma once

#include "Ship.h"

class Cell {
public:
	enum CellStatus {
		ALIVE,
		DEAD,
		EMPTY
	};

	Cell();
	~Cell();

	CellStatus getStatus() { return  m_status; }
	void clear() { m_status = EMPTY; m_pShip = nullptr; }

private:
	CellStatus m_status;
	Ship* m_pShip;

};