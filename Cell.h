#pragma once

#include "Ship.h"

class Cell {
public:
	enum cellStatus {
		ALIVE,
		DEAD
	};
	Cell();
	~Cell();
private:
	cellStatus m_status;
	Ship* m_pShip;

};