#include "Cell.h"

Cell::Cell() : m_status(ALIVE), m_pShip(nullptr)
{
}

Cell::~Cell()
{
	delete m_pShip;
}
