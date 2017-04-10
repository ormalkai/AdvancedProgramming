#pragma once

#include "Ship.h"

class RocketShip : public Ship {
public:
	RocketShip(char sign, int length, int value);
	~RocketShip();
};