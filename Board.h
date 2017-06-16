#pragma once

#include <vector>
#include <memory>
#include <set>
#include "IBattleshipGameAlgo.h"
#include "Cell.h"

using std::vector;

class Board : public BoardData
{
private:
	vector<vector<vector<shared_ptr<Cell>>>>	m_boardData;	// The board is matrix of Cells
	vector<shared_ptr<Ship>>					m_shipsOnBoard;	// Pointer to every ship on the board
	int											m_depth;		// Number of matrixes in depth
	int											m_rows;			// Number of rows in the board
	int											m_cols;			// Number of columns in the board
	bool										m_isQuiet;		// Is quiet run, for print simulation game
	int											m_delay;		// delay between every attack in game simulation

	vector<int>									m_numOfShipsPerPlayer;


	class BoardBuilder
	{
	private:

		/**
		* @Details		Assistent enum for recursive funtions of analayzing input board
		*/
		enum class ShipLengthSecondDirection
		{
			FORWARD,
			BACKWORD
		};

		string					m_sboardFilePath;
		vector<vector<char>>	m_shipsPerPlayer;				// for equal ships validation
		vector<int>				m_numOfShipsPerPlayer;			// how many ships each player has
		bool					m_foundAdjacentShips;			// need to print Adjacency error per player
		vector<vector<bool>>	m_wrongSizeOrShapePerPlayer;	// 2 players X 4 ships

		vector<vector<vector<char>>>	m_initBoard;
		int						m_rows;
		int						m_cols;
		int						m_depth;


	public:
		BoardBuilder(const string sboardPath);
		BoardBuilder(const BoardBuilder&) = delete;
		BoardBuilder(BoardBuilder&&) = delete;
		BoardBuilder& operator=(const BoardBuilder &) = delete;
		~BoardBuilder();

		/**
		* @Details		Init errors dataStructures - no errors
		*/
		void initErrorDataStructures();

		/**
		* @Details		Parses the sboard file and validates it
		* @return		RC_ERROR if one of the valdiations according to Spec failed, RC_SUCCESS otherwise
		*/
		ReturnCode parseBoardFile(vector<vector<vector<char>>>& board);

		/**
		* @Details		read board from file and fill initBoard
		*/
		void readSBoardFile(ifstream& sboard);

		/**
		*
		*/
		ReturnCode getBoardDimensionsFromFile(ifstream& sboard);

		/**
		* @Details		check the board by given constaints: ship size, shape, neighbors
		*				and fill error data structure.
		* @param		initBoard - input board to verify
		*/
		void validateBoard();

		/**
		* @Details		Checks if  adjacency in specific cell is valid
		* @param		i - row ro check
		* @param		j - col to check
		* @return		true if valid, false otherwise
		*/
		bool isAdjacencyValid(int d/*depth*/, int i/*row*/, int j/*col*/) const;

		/**
		* @Details		Wrapper for recursive function for recieving current ship length in specific cell
		* @param		initBoard - the board after parsing
		* @param		expectedShip - ship to test
		* @param		row - current row to check
		* @param		col - current col to check
		* @param		direction - in which direction to go in the next recursive call
		*/
		int getShipLength(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipDirection direction);

		/**
		* @Details		recursive function for recieving current ship length in specific cell
		* @param		expectedShip - ship to test
		* @param		row - current row to check
		* @param		col - current col to check
		* @param		direction - in which direction to go in the next recursive call
		*/
		int getShipLengthHorizontal(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const;

		/**
		* @Details		Wrapper for recursive function for recieving current ship length in specific cell
		* @param		expectedShip - ship to test
		* @param		row - current row to check
		* @param		col - current col to check
		* @param		direction - in which direction to go in the next recursive call
		*/
		int getShipLengthVertical(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const;

		/**
		* @Details		Wrapper for recursive function for recieving current ship length in specific cell
		* @param		expectedShip - ship to test
		* @param		row - current row to check
		* @param		col - current col to check
		* @param		direction - in which direction to go in the next recursive call
		*/
		int getShipLengthDepth(char expectedShip, int d/*depth*/, int i/*row*/, int j/*col*/, ShipLengthSecondDirection direction) const;
		/**
		* @Details		check if there is a ship with inappropriate size
		* @return		true if at least one ship found, o/w false
		*/
		bool checkWrongSizeOrShape() const;

		/**
		* @Details		check each player has exactly number of ship as expected
		*/
		void checkIntegrityOfShips() const;

		/**
		* @Details		check if there is adjacent ships [ships w/o space cell between them]
		* @return		true if at least two ships are neighboring, o/w false
		*/
		bool checkAdjacentShips() const;

		/**
		* @Details		check if all ships in appropriate size, shape not adjacent, and the amount ships of each player is as expected
		* @return		true if all the condtions are accepted, o/w false
		*/
		bool checkErrors() const;

		void initInitBoard();
		
		int getNumOfShipByPlayer(PlayerIndex pi) const { return m_numOfShipsPerPlayer[pi]; }
	};

	
	
public:


	vector<int>& Board::shipsPerPlayer();

	virtual char charAt(Coordinate c) const
	{
		return m_boardData[c.depth][c.row][c.col]->getSign();
	}


	/**
	 * @Details		Constructor of board
	 * @param		rows - number of rows in the board, default BOARD_ROW_SIZE (for future purposes)
	 * @param		cols - number of Columns in the board, default BOARD_COL_SIZE (for future purposes)
	 */
	Board(int depth = BOARD_DEPTH_SIZE, int rows = BOARD_ROW_SIZE, int cols = BOARD_COL_SIZE) : m_depth(depth), m_rows(rows), m_cols(cols) , m_isQuiet(true), m_delay(2000) {}

	/**
	 * @Details		Destructor of board, releases all related memory (i.e ships on board)
	 */
	~Board();

	/**
	 * @Details		Copy constructor, default.
	 */
	Board(const Board &);
	Board& operator=(const Board &);
	Board(Board &&) = default;

	/**
	* @Details		Receive row and col indexes and returns reference for the cell in the board
	* @Param		r - requested cell's row index
	* @Param		c - requested cell's col index
	* @return		ref of cell in row r and col c
	*/
	shared_ptr<Cell> get(int d, int r, int c) const { return  m_boardData[d][r][c]; }

	shared_ptr<Cell> get(Coordinate coord) const { return  get(coord.depth, coord.row, coord.col); }

	/**
	* @Details		Receive pair of row and col indexes and returns reference for the cell in the board
	* @Param		i - requested cell's indexes
	* @return		ref of cell in row r and col c
	*/
	shared_ptr<Cell> get(tuple<int, int, int> i) const { return get(std::get<0>(i), std::get<1>(i), std::get<2>(i)); }
	
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
	void buildBoard(const vector<vector<vector<char>>>& initBoard);

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
		m_boardData[d][r][c]->clear();
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
	bool isPaddingCell(const shared_ptr<Cell> cell) const { return cell->row() == 0 || cell->row() == m_rows + 1 || cell->col() == 0 || cell->col() == m_cols + 1 || cell->depth() == 0 || cell->depth() == m_depth + 1; }

	/**
	* @Details		This function adds a dummy ship to the board by given cells
	*				The func expected that all the cells in the vector is legal ship
	* @Param		shipCells - cells of the dummy ship
	*/
	void addDummyNewShipToBoard(const vector<shared_ptr<Cell>>& shipCells);

	/**
	* @Details		This function prints the histogram - value of each cell, for debug and future porposes
	*/
	void printHist() const;


	void splitToPlayersBoards(Board& boardA, Board& boardB) const;
	ReturnCode loadBoardFromFile(string& boardPath);

	void getOtherPlayerShips(vector<int>& ships);

	void getShipsCoord(vector<Coordinate>& shipsCoord) const;

	void clear();

};
