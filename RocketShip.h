#pragma once

#include "Ship.h"

/*
* @Note		The inheritence from Ship is for future use,
*				currently there is no additional functunalities
*/
class RocketShip : public Ship {
public:
	RocketShip(char sign, int length, int value, _In_ WORD color);
	~RocketShip() = default;
};