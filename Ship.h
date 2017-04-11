#pragma once

#include <vector>
#include <windows.h>

using namespace std;

class Cell;

class Ship {

private:
	int m_length;
	int m_value;
	char m_sign; // Player 
<<<<<<< HEAD
				 // Color
	int m_numOfAliveCells;
=======
	_In_ WORD m_color;		 // Color

>>>>>>> origin/master
	vector<Cell*> m_cellList;

public:
	Ship(char sign, int length, int value, _In_ WORD color);
	~Ship() = default;
	int getLength() { return m_length; }
	int getValue() { return m_value; }
	char getSign() { return m_sign; }
	_In_ WORD getColor() { return m_color; }

	void setLength(int length) { m_length = length; }
	void setValue(int value) { m_value = value; }
	void setSign(int sign) { m_sign = sign; }
	void addCell(Cell* cell) { m_cellList.push_back(cell); ++m_numOfAliveCells; }
	bool isShipAlive() const { return 0 == m_numOfAliveCells; }
	void executeAttack();


};