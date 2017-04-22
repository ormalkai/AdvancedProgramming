#pragma once

#include "Ship.h"
#include "RocketShip.h"
#include "RubberShip.h"
#include "Destroyer.h"
#include "Submarine.h"

class ShipFactory {
public:
	/**
	 * @Details		getter for instance of ship factory
	 */
	static ShipFactory& instance();

	/**
	 * @Details		create ship by char identifier
	 * @param		i - row in initBoard
	 * @param		j - column in initBoard
	 * @param		initBoard - as recievd in input
	 */
	Ship* create(int i, int j, const char ** initBoard);
private:
	/**
	 * @Details		default constructor
	 */
	ShipFactory() = default;

	/**
	 * @Details		default destructor
	 */
	~ShipFactory() = default;

	/**
	 * @Details		delete copy constructor
	 */
	ShipFactory(const ShipFactory&) = delete;

	/**
	 * @Details		ignore assignment
	 */
	ShipFactory &operator=(const ShipFactory&) { return *this; }

};