#pragma once

#include <map>
#include <vector>

using namespace std;

class Cell;

class Ship {

private:
	int m_length;
	int m_value;
	char m_sign; // Player 
				 // Color

	vector<Cell*> cellList;

public:
	Ship();
	~Ship();
	int getLength() { return m_length; }
	int getValue() { return m_value; }
	char getSign() { return m_sign; }

	void setLength(int length) { m_length = length; }
	void setValue(int value) { m_value = value; }
	void setSign(int sign) { m_sign = sign; }


	Ship(char sign, int length, int value);

};