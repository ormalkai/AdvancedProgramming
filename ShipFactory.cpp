

#include "ShipFactory.h"
#include "Debug.h"
#include "Utils.h"
#include "DummyShip.h"

ShipFactory& ShipFactory::instance()
{
	static ShipFactory instance;
	return  instance;
}

shared_ptr<Ship> ShipFactory::create(char ship) const
{
	int shipLen = Utils::getShipLen(ship);
	int shipValue = Utils::getShipValue(ship);
	_In_ WORD shipColor = Utils::getShipColor(ship);
	shared_ptr<Ship> result;
	switch (ship) {
	case 'b':
	case 'B':
		result = make_shared<RubberShip>(ship, shipLen, shipValue, shipColor);
		break;
	case 'p':
	case 'P':
		result = make_shared<RocketShip>(ship, shipLen, shipValue, shipColor);
		break;
	case 'm':
	case 'M':
		result = make_shared<Submarine>(ship, shipLen, shipValue, shipColor);
		break;
	case 'd':
	case 'D':
		result = make_shared<Destroyer>(ship, shipLen, shipValue, shipColor);
		break;
	default:
		DBG(Debug::DBG_ERROR, "Invalid ship type %c", ship);
		return nullptr;
	}

	return result;

}

shared_ptr<Ship> ShipFactory::createDummyShipByCellsVector(const vector<shared_ptr<Cell>>& vec)
{
	return make_shared<DummyShip>(static_cast<int>(vec.size()));
}

