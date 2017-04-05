#pragma once

#include <map>
#include <utility>
#include <list>
#include "Cell.h"

using namespace std;

class Ship {

private:
	int m_length;
	int m_value;
	char m_sign; // Player 
	// Color
	
	list<Cell> cellList;

};
