

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
	Ship* result;
	switch (ship) {
	case 'b':
	case 'B':
		result = new RubberShip();
		break;
	case 'p':
	case 'P':
		result = new RocketShip;
		break;
	case 'm':
	case 'M':
		result = new Submarine();
		break;
	case 'd':
	case 'D':
		result = new Destroyer();
		break;
	default:
		DBG(Debug::Error, "Invalid ship type %c", ship);
		return nullptr;
	}

	// create linking between Ship and Cell and vice versa
	int shipLen = Utils::instance().getShipLen(ship);

}
