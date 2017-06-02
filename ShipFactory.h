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
	Ship* create(char ship) const;

	/**
	* @Details		create dummy ship by given vector cells
	* @param		vec - vector of cells (Cell*) of the dummy ship
	* @return		Pointer to requested dummy ship
	*/
	static Ship* createDummyShipByCellsVector(const vector<shared_ptr<Cell>>& vec);

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
	ShipFactory &operator=(const ShipFactory&) = delete;
	
	/**
	* @Details		ignore move assignment
	*/
	ShipFactory(ShipFactory&&) = delete;

};