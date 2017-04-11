#include "Cell.h"

Cell::Cell() : m_pShip(nullptr), m_playerIndexOwner(MAX_PLAYER)
{
	m_status = ALIVE;
}

Cell::~Cell()
{
	delete m_pShip;
	m_pShip = nullptr;
}

void Cell::setShip(Ship* ship) {
	m_pShip = ship; 
	m_playerIndexOwner = Utils::instance().getPlayerIdByShip(ship->getSign());
}