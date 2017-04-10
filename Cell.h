#pragma once

#include "Ship.h"
#include "Utils.h"

class Cell {
public:
	enum CellStatus {
		ALIVE,
		DEAD,
		FREE
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
	void setShip(Ship* ship) { m_pShip = ship; }
	void setStatus(CellStatus status) { m_status = status; }
	void setIndex(int i, int j) { m_rIndex = i; m_cIndex = j; }
	
	void clear() { m_status = FREE; m_pShip = nullptr; }

private:
	CellStatus m_status;
	Ship* m_pShip;
	int m_rIndex, m_cIndex;
};