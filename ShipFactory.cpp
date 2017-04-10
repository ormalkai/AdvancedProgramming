

#include "ShipFactory.h"
#include "Debug.h"
#include "Utils.h"

ShipFactory* ShipFactory::instance()
{
	static ShipFactory *Instance = new ShipFactory();
	return  Instance;
}

Ship* ShipFactory::create(int i, int j, char ** initBoard)
{
	char ship = initBoard[i][j];
	int shipLen = Utils::instance().getShipLen(ship);
	int shipValue = Utils::instance().getShipValue(ship);
	Ship* result;
	switch (ship) {
	case 'b':
	case 'B':
		result = new RubberShip(ship, shipLen, shipValue);
		break;
	case 'p':
	case 'P':
		result = new RocketShip(ship, shipLen, shipValue);
		break;
	case 'm':
	case 'M':
		result = new Submarine(ship, shipLen, shipValue);
		break;
	case 'd':
	case 'D':
		result = new Destroyer(ship, shipLen, shipValue);
		break;
	default:
		DBG(Debug::DBG_ERROR, "Invalid ship type %c", ship);
		return nullptr;
	}

	return result;

}
