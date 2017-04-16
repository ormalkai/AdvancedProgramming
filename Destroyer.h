#pragma once

#include "Ship.h"

/*
 * @Note		The inheritence from Ship is for future use, 
 *				currently there is no additional functunalities
 */
class Destroyer : public Ship {
public:
	Destroyer(char sign, int length, int value, _In_ WORD color);
	~Destroyer() = default;
};