#pragma once

#include "Ship.h"

class RocketShip : public Ship {
public:
	RocketShip(char sign, int length, int value, _In_ WORD color);
	~RocketShip();
};