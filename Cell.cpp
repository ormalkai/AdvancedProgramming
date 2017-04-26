#include "Cell.h"

Cell::Cell() : m_status(CellStatus::FREE), m_pShip(nullptr), m_playerIndexOwner(MAX_PLAYER), m_isHitted(false)
{}
// TODO: Initialize all data members

void Cell::setShip(Ship* ship) {
	m_pShip = ship; 
	m_playerIndexOwner = Utils::getPlayerIdByShip(ship->getSign());
}