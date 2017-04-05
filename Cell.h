#pragma once

#include "Ship.h"

class Cell {

	enum cellStatus {
		ALIVE,
		DEAD
	};

	cellStatus m_status;
	Ship* m_pShip;

};