#pragma once


#pragma once

#include "Ship.h"
#include "RocketShip.h"
#include "RubberShip.h"
#include "Destroyer.h"
#include "Submarine.h"

class ShipFactory {
public:
	static ShipFactory* instance();

	Ship* create(int i, int j, char ** initBoard);
private:

	ShipFactory() = default;
	~ShipFactory() = default;
	ShipFactory(const ShipFactory&) = default;
	void operator=(const ShipFactory&);


};