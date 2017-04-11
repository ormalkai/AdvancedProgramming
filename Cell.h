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
	int m_rIndex, m_cIndex;
	PlayerIndex m_playerIndexOwner;


public:
	Cell();
	~Cell();

	CellStatus getStatus() const { return  m_status; }
	char getSign() const
	{
		return nullptr != m_pShip ? m_pShip->getSign() : SPACE;
	}
	int row() const { return m_rIndex; }
	int col() const { return m_cIndex; }
	Ship* getShip() const { return m_pShip; }
	void setShip(Ship* ship);
	void setStatus(CellStatus status) { m_status = status; }
	void setIndex(int i, int j) { m_rIndex = i; m_cIndex = j; }
	bool isEmpty() const { return  nullptr == m_pShip; }
	PlayerIndex getPlayerIndexOwner() const { return m_playerIndexOwner; }	
	void clear() { m_status = FREE; m_pShip = nullptr; m_playerIndexOwner = MAX_PLAYER; }
	void executeAttack() { setStatus(DEAD); }

};