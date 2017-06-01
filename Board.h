#pragma once

#include "Cell.h"
#include <vector>
#include "IBattleshipGameAlgo.h"

using std::vector;

class Board : public BoardData
{
private:
	//Cell**			m_boardData;	// The board is matrix of Cells
	vector<vector<vector<Cell>>>	m_boardData;	// The board is matrix of Cells
	vector<Ship*>					m_shipsOnBoard;	// Pointer to every ship on the board
	int								m_depth;		// Number of matrixes in depth
	int								m_rows;			// Number of rows in the board
	int								m_cols;			// Number of columns in the board
	bool							m_isQuiet;		// Is quiet run, for print simulation game
	int								m_delay;		// delay between every attack in game simulation

public:


	virtual char charAt(Coordinate c) const
	{
		return m_boardData[c.depth][c.row][c.col].getSign();
	}


	/**
	 * @Details		Constructor of board
	 * @param		rows - number of rows in the board, default BOARD_ROW_SIZE (for future purposes)
	 * @param		cols - number of Columns in the board, default BOARD_COL_SIZE (for future purposes)
	 */
	Board(int depth = BOARD_DEPTH_SIZE, int rows = BOARD_ROW_SIZE, int cols = BOARD_COL_SIZE) : m_depth(depth), m_rows(rows), m_cols(cols) , m_isQuiet(false), m_delay(2000) {}

	/**
	 * @Details		Destructor of board, releases all related memory (i.e ships on board)
	 */
	~Board();

	/**
	 * @Details		Copy constructor, default.
	 */
	Board(const Board &) = delete;
	Board &operator=(const Board &) = delete;
	Board(Board &&) = delete;

	/**
	* @Details		Receive row and col indexes and returns reference for the cell in the board
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	* @return		ref of cell in row r and col c
	*/
	Cell& get(int d, int r, int c) const { return  const_cast<Cell&>(m_boardData[d][r][c]); }

	/**
	* @Details		Receive pair of row and col indexes and returns reference for the cell in the board
	* @Param		i - requested cell's indexes
	* @return		ref of cell in row r and col c
	*/
	Cell& get(tuple<int, int, int> i) const { return get(std::get<0>(i), std::get<1>(i), std::get<2>(i)); }

	/**
	* @Details		Returns num of rows in the board (int)
	* @return		num of rows in the board
	*/
	int rows() const { return m_rows; }
	
	/**
	* @Details		Returns num of cols in the board (int)
	* @return		num of cols in the board
	*/
	int cols() const { return m_cols; }
	
	/**
	* @Details		Prints the board in with sign color for each player's ship
	*/
	void printBoard() const;
	

	/**
	* @Details		Update GUI board after attack
	* @Param		i - attacked cell's row index
	* @Param		j - attacked cell's col index
	* @return		attackResult - attack restult (hit, miss, sink)
	*/
	void printAttack(int player, int d, int i, int j, AttackResult attackResult) const;

	/**
	* @Details		set true for disable game simulation
	* @Param		isQuiet - disable simulation mode
	*/
	void setIsQuiet(bool isQuiet) { m_isQuiet = isQuiet; }

	/**
	* @Details		set delay between attacks
	* @Param		delay - delay attacks value
	*/
	void setDelay(int delay) { m_delay = delay; }
	
	/**
	* @Details		revieves player index and returns player's board as matrix of chars
	* @Param		playerId - player's index
	*/
	char** Board::toCharMat(PlayerIndex playerId) const;

	void buildBoard(vector<vector<vector<char>>>& initBoard);

	void buildBoard(const BoardData& initBoard);
	
	/**
	* @Details		receive cell's coords and returns sign of requested cell
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	*/
	char getSign(int d, int r, int c) const;

	/**
	* @Details		receive cell's coords and clear the cell (status=FREE and sign=SPACE)
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	*/
	void clearCell(int d, int r, int c)
	{
		m_boardData[d][r][c].clear();
	}

	/**
	* @Details		This function checks if 2 indexes of cell is legal in current board
	* @Param		r - cell's row index
	* @Param		c - cell's col index
	*/
	bool isValidCell(int d, int i, int j) const
	{
		return (d >= 1 && d <= m_depth && i >= 1 && i <= m_rows && j >= 1 && j <= m_cols);
	}

	/**
	* @Details		This function checks if a given cell is padding cell (one of padding frame) 
	* @Param		r - cell's row index
	* @Param		c - cell's col index
	*/
	bool isPaddingCell(const Cell& cell) const { return cell.row() == 0 || cell.row() == m_rows + 1 || cell.col() == 0 || cell.col() == m_cols + 1; }

	/**
	* @Details		This function adds a dummy ship to the board by given cells
	*				The func expected that all the cells in the vector is legal ship
	* @Param		shipCells - cells of the dummy ship
	*/
	void addDummyNewShipToBoard(const vector<Cell*>& shipCells);

	/**
	* @Details		Get pointer for specific cell in the board by given row and col indexes
	* @Param		r - cell's row index
	* @Param		c - cell's col index
	* @return		cell's pointer
	*/
	Cell* getCellPointer(int d, int r, int c) const { return  const_cast<Cell*>(&(m_boardData[d][r][c])); }

	/**
	* @Details		This function prints the histogram - value of each cell, for debug and future porposes
	*/
	void printHist();
};
