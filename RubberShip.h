#pragma once

#include "Ship.h"

/*
* @Note		The inheritence from Ship is for future use,
*				currently there is no additional functunalities
*/
class RubberShip : public Ship {
public:
	RubberShip(char sign, int length, int value, _In_ WORD color);
	~RubberShip() = default;
	RubberShip(const RubberShip &) = default;
	RubberShip &operator=(const RubberShip &) = default;
	RubberShip(RubberShip &&) = default;
};