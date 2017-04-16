#include "Cell.h"

Cell::Cell() : m_status(CellStatus::FREE), m_pShip(nullptr), m_playerIndexOwner(MAX_PLAYER)
{}


void Cell::setShip(Ship* ship) {
	m_pShip = ship; 
	m_playerIndexOwner = Utils::getPlayerIdByShip(ship->getSign());
}