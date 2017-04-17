#pragma once

#include "Ship.h"
#include "PlayerAlgoFactory.h"

/*
* @Note		The inheritence from Ship is for future use,
*				currently there is no additional functunalities
*/
class RocketShip : public Ship {
public:
	RocketShip(char sign, int length, int value, _In_ WORD color);
	~RocketShip() = default;
	RocketShip(const RocketShip &) = default;
	RocketShip &operator=(const RocketShip &) = default;
};