

#include "ShipFactory.h"
#include "Debug.h"
#include "Utils.h"
#include "DummyShip.h"

ShipFactory& ShipFactory::instance()
{
	static ShipFactory instance;
	return  instance;
}

Ship* ShipFactory::create(int i, int j, const char ** initBoard) const
{
	char ship = initBoard[i][j];
	int shipLen = Utils::getShipLen(ship);
	int shipValue = Utils::getShipValue(ship);
	_In_ WORD shipColor = Utils::getShipColor(ship);
	Ship* result;
	switch (ship) {
	case 'b':
	case 'B':
		result = new RubberShip(ship, shipLen, shipValue, shipColor);
		break;
	case 'p':
	case 'P':
		result = new RocketShip(ship, shipLen, shipValue, shipColor);
		break;
	case 'm':
	case 'M':
		result = new Submarine(ship, shipLen, shipValue, shipColor);
		break;
	case 'd':
	case 'D':
		result = new Destroyer(ship, shipLen, shipValue, shipColor);
		break;
	default:
		DBG(Debug::DBG_ERROR, "Invalid ship type %c", ship);
		return nullptr;
	}

	return result;

}

Ship* ShipFactory::createDummyShipByCellsVector(const vector<Cell*>& vec) const
{
	return new DummyShip(static_cast<int>(vec.size()));
}

