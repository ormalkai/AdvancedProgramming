#pragma once

#include "Ship.h"

/*
* @Note		The inheritence from Ship is for future use,
*				currently there is no additional functunalities
*/
class DummyShip : public Ship {
public:
	DummyShip(int length);
	~DummyShip() = default;
	DummyShip(const DummyShip &) = default;
	DummyShip &operator=(const DummyShip &) = default;
	DummyShip(DummyShip &&) = default;
};

