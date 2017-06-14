#pragma once

#include <vector>
#include <windows.h>
#include <memory>

using namespace std;

class Cell;

class Ship {


private:
	int m_length;
	int m_value;
	char m_sign;				// Player 
	int m_numOfAliveCells;
	_In_ WORD m_color;			// Color
	vector<shared_ptr<Cell>> m_cellList;

public:

	/**
	* @Details		creates new ship object
	* @param		sign - ship's character
	* @param		length - ship's length
	* @param		value - ship' score 
	* @param		color - ship' color
	* @Note			The inheritence from Ship is for future use,
	*				currently there is no additional functunalities
	*/
	Ship(char sign, int length, int value, _In_ WORD color);
	
	/**
	* @Details		for define Ship as abstract class without pure function - 
	*				set pure virtual d'tor and implement
	*				credit - http://www.gotw.ca/gotw/031.htm
	*/
	virtual ~Ship() = 0;

	Ship(const Ship &) = default;
	Ship &operator=(const Ship &) = default;
	Ship(Ship &&) = default;

	/**
	* @Details		returns ship's length
	* @return		ship's length
	*/
	int getLength() const { return m_length; }

	/**
	* @Details		returns ship's score
	* @return		ship's score
	*/
	int getValue() const { return m_value; }
	
	/**
	* @Details		returns ship's character [For ex: B-rubber, d-destroyer]
	* @return		ship's character
	*/
	char getSign() const  { return m_sign; }

	/**
	* @Details		returns ship's color [for GUI]
	* @return		ship's color
	*/
	_In_ WORD getColor() const  { return m_color; }

	vector<shared_ptr<Cell>> getCellsList() const { return m_cellList; }

	/**
	* @Details		update ship's length
	* @return		ship's new length
	*/
	void setLength(int length) { m_length = length; }
	
	/**
	* @Details		update ship's value(score)
	* @return		ship's new score value
	*/
	void setValue(int value) { m_value = value; }

	/**
	* @Details		update ship's sign
	* @return		ship's new character
	*/
	void setSign(int sign) { m_sign = sign; }

	/**
	* @Details		add new cell pointer to cell list of the ship
	* @param		cell - new cell of the ship
	*/
	void addCell(shared_ptr<Cell> cell) { m_cellList.push_back(cell); }

	/**
	* @Details		check if ship is Alive 
	* @retrun		false if there is no alive cells other true
	*/
	bool isShipAlive() const { return 0 != m_numOfAliveCells; }
	
	/**
	* @Details		update alive cells counter 
	*/
	void executeAttack();

	/**
	* @Details		add cells vector to the current ship's cells, (append to the end of the vector)
	* @param		vecCell - the vector of cells we want to add 
	*/
	void addCells(vector<shared_ptr<Cell>> vecCell) { m_cellList.insert(m_cellList.end(), vecCell.begin(), vecCell.end()); }

};