#pragma once

#include "Ship.h"

class Destroyer : public Ship {
public:
	Destroyer(char sign, int length, int value, _In_ WORD color);
	~Destroyer();
};