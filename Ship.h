#pragma once

#include <vector>

using namespace std;

class Cell;

class Ship {

private:
	int m_length;
	int m_value;
	char m_sign; // Player 
				 // Color
	int m_numOfAliveCells;
	vector<Cell*> m_cellList;

public:
	Ship() = default;
	~Ship() = default;
	int getLength() { return m_length; }
	int getValue() { return m_value; }
	char getSign() { return m_sign; }

	void setLength(int length) { m_length = length; }
	void setValue(int value) { m_value = value; }
	void setSign(int sign) { m_sign = sign; }
	void addCell(Cell* cell) { m_cellList.push_back(cell); ++m_numOfAliveCells; }
	bool isShipAlive() const { return 0 == m_numOfAliveCells; }
	void executeAttack();

	Ship(char sign, int length, int value);

};