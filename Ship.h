#pragma once

#include <map>
#include <list>

using namespace std;

class Cell;

class Ship { 

private:
	int m_length;
	int m_value;
	char m_sign; // Player 
	// Color
			  
	list<Cell*> cellList;

public:

	virtual char getSign() { return m_sign; }

};
