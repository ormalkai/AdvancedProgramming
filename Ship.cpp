#include "Ship.h"
#include "Debug.h"

Ship::Ship(char sign, int length, int value, _In_ WORD color) : m_length(length), m_value(value), m_sign(sign), m_numOfAliveCells(length), m_color(color)
{

}

void Ship::executeAttack() {
	--m_numOfAliveCells;

	if (m_numOfAliveCells < 0)
		DBG(Debug::DBG_ERROR, "Negative value in number of alive cells in ship");
}

Ship::~Ship() {}