#pragma once

#include "Ship.h"

class Submarine : public Ship {
public:
	Submarine(char sign, int length, int value);
	~Submarine();
};