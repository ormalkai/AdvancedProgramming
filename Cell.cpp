#include "Cell.h"

Cell::Cell() : m_status(EMPTY), m_pShip(nullptr)
{
}

Cell::~Cell()
{
	delete m_pShip;
}
