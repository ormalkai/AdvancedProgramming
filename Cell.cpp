#include "Cell.h"

Cell::Cell() :	m_status(CellStatus::FREE),
				m_pShip(nullptr),
				m_playerIndexOwner(MAX_PLAYER),
				m_coord(-1,-1,-1),
				m_histValue(0),
				m_isHitted(false),
				m_isOperationCell(false)
{}

void Cell::setShip(shared_ptr<Ship> ship) {
	m_pShip = ship; 
	m_playerIndexOwner = Utils::getPlayerIdByShip(ship->getSign());
}