#pragma once

#include "Ship.h"


class RubberShip : public Ship {
public:
	RubberShip(char sign, int length, int value, _In_ WORD color);
	~RubberShip();
};