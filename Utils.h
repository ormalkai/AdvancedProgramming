#pragma once
#include <vector>
#include <map>
#include <windows.h>

#define NUM_OF_PLAYERS (2)
#define BOARD_ROW_SIZE (10)
#define BOARD_COL_SIZE (10)
#define BOARD_PADDING (2)
#define INIT_BOARD_ROW_SIZE (BOARD_ROW_SIZE + BOARD_PADDING)
#define INIT_BOARD_COL_SIZE (BOARD_COL_SIZE + BOARD_PADDING)
#define SPACE ' '
#define PLAYER_A_CHAR 'A'
#define PLAYER_B_CHAR 'B'
#define PLAYER_A_RUBBER_SHIP 'B'
#define PLAYER_B_RUBBER_SHIP 'b'
#define PLAYER_A_ROCKET_SHIP 'P'
#define PLAYER_B_ROCKET_SHIP 'p'
#define PLAYER_A_SUBMARINE	 'M'
#define PLAYER_B_SUBMARINE	 'm'
#define PLAYER_A_DESTROYER	 'D'
#define PLAYER_B_DESTROYER	 'd'
#define PLAYER_A_ATTACK_FILE "*.attack-a"
#define PLAYER_B_ATTACK_FILE "*.attack-b"
#define NO_ATTACK_FILE ""
#define DEFAULT_IS_QUIET_MODE (false)
#define DEFAULT_DELAY_ATTACK (2000)
#define MAX_SHIP_LEN (4)
#define NUM_OF_DIRECTIONS (4)

using namespace std;

typedef enum ReturnCode
{
	RC_ERROR = -1,
	RC_SUCCESS,
	RC_INVALID_ARG
} ReturnCode;

typedef enum PlayerIndex
{
	PLAYER_A = 0,
	PLAYER_B,
	MAX_PLAYER
} PlayerIndex;

enum ShipType
{
	RUBBER_SHIP,
	ROCKET_SHIP,
	SUBMARINE,
	DESTROYER,
	MAX_SHIP
};

typedef enum AttackRequestCode
{
	ARC_ERROR,
	ARC_SUCCESS,
	ARC_GAME_OVER,
	ARC_FINISH_REQ = -1
} AttackRequestCode;

enum class ShipDirection
{
	VERTICAL,
	HORIZONTAL,
	ALL
};

enum class Direction
{
	UP,
	DOWN,
	RIGHT,
	LEFT,
	MAX_DIRECTION
};

class Utils
{
private:
	Utils() = default;
	static map<char, int> m_shipLenBySign; // conversion map between ship type to expected length
	static map<char, int> m_shipToValue; // conversion map between ship type to expected value
	static map<char, _In_ WORD> m_shipToColor;// conversion between ship and its color
	static const vector<char> m_playerIndexToChar;
	static const map<ShipType, int> m_shipLenByType;


	static const vector<char> m_playerALegalSign;

	static const vector<char> m_playerBLegalSign;

	static const vector<string> m_expectedAttackFilePerPlayer;

public:

	/**
	 * @Details		receives char and returns for which player the ship belongs to
	 * @Param		c - query ship
	 * @Return		PlayerIndex::PLAYER_A if c belongs to m_playerALegalSign,
	 *				PlayerIndex::PLAYER_B if c belongs to m_playerBLegalSign,
	 *				PlayerIndex::MAX_PLAYER otherwise
	 */
	static PlayerIndex getPlayerIdByShip(char c);

	/*
	* @Details		receives char and returns the ship is represented by the char
	* @Param		c - query ship
	* @Return		Ship's index or ERROR [-1] for unknown char
	*/
	static int getIndexByShip(char c);

	/*
	* @Details		receives char and returns the length of the ship is represented by the char
	* @Param		c - query ship
	* @Return		Ship's length
	*/
	static int getShipLen(char c);

	/*
	* @Details		receives ship type and returns the length of the ship is represented by the char
	* @Param		s - ship type
	* @Return		Ship's length
	*/
	static int getShipLen(ShipType s);

	/*
	* @Details		receives char and returns the score of the ship is represented by the char
	* @Param		c - query ship
	* @Return		Ship's value
	*/
	static int getShipValue(char c);

	/*
	* @Details		receives char and returns the color of the ship is represented by the char
	* @Param		c - query ship
	* @Return		Ship's color
	*/
	static _In_ WORD getShipColor(char c);

	/*
	* @Details		receives ship index and plyaer index and returns the value of char reperesents the ship
	* @Param		ship - ship index
	* @Param		player - player index
	* @Return		Ship's sign value
	*/
	static int getShipByIndexAndPlayer(int ship, int player);

	/*
	* @Details		receives plyaer index and returns the sign (character) of the player [For ex A for 0, B for 1]
	* @Param		player - player index
	* @Return		player's char
	*/
	static char getPlayerCharByIndex(int player);

	/*
	* @Details		receives plyaer index and returns the attack file name regex
	* @Param		player - player index
	* @Return		attack filename regex
	*/
	static string getAttackFileByPlayer(int player);


	static std::istream& safeGetline(std::istream& is, std::string& t);

	/*
	* @Details		receives coordinates and move the cursor to the position
	* @Param		row - row index
	* @Param		col - col index
	*/
	static void gotoxy(int row, int col);

	/*
	* @Details		receives color and change text's color
	* @Param		color
	*/
	static void setTextColor(WORD color);

	/*
	* @Details		receives vector of type T and element of type T and return true if the element exists in the vector, otherwise fales
	* @Param		vec - vector of T elements
	* @Param		val - element
	* @return		true if elem exist otherwise false
	*/
	template <class T>
	static bool isExistInVec(vector<T> vec, T val)
	{
		if (find(vec.begin(), vec.end(), val) != vec.end())
		{
			return true;
		}
		return false;
	}


	Direction static getOppositeDirection(Direction d);

	static ReturnCode getListOfFilesInDirectoryBySuffix(const string& path, const string& suffix, vector<string>& files);

	static void ShowConsoleCursor(bool showFlag);
};
