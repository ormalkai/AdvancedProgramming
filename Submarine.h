#pragma once

#include "Ship.h"

/*
* @Note		The inheritence from Ship is for future use,
*				currently there is no additional functunalities
*/
class Submarine : public Ship {
public:
	Submarine(char sign, int length, int value, _In_ WORD color);
	~Submarine() = default;
};