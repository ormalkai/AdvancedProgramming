#pragma once

#define NUM_OF_PLAYERS (2)
#define BOARD_ROW_SIZE (10)
#define BOARD_COL_SIZE (10)
#define SPACE ' '
#define PLAYER_A_CHAR 'A'
#define PLAYER_B_CHAR 'B'
#define PLAYER_A_RUBBER_SHIP 'b'
#define PLAYER_B_RUBBER_SHIP 'B'
#define PLAYER_A_ROCKET_SHIP 'p'
#define PLAYER_B_ROCKET_SHIP 'P'
#define PLAYER_A_SUBMARINE	 'm'
#define PLAYER_B_SUBMARINE	 'M'
#define PLAYER_A_DESTROYER	 'd'
#define PLAYER_B_DESTROYER	 'D'

#include <vector>
using namespace std;

enum ReturnCode
{
    RC_ERROR = -1,
    RC_SUCCESS,
    RC_INVALID_ARG
};

enum PlayerIndex
{
    PLAYER_A,
    PLAYER_B,
    MAX_PLAYER
};

enum ShipType
{
    RUBBER_SHIP,
    ROCKET_SHIP,
    SUBMRINE,
    DESTROYER,
    MAX_SHIP
};


class Utils
{
private:
	vector<char> m_playerIndexToChar
	{
		PLAYER_A_CHAR,
		PLAYER_B_CHAR
	};

	vector<char> m_playerALegalSign = 
	{ 
		PLAYER_A_RUBBER_SHIP, 
		PLAYER_A_ROCKET_SHIP,
		PLAYER_A_SUBMARINE, 
		PLAYER_A_DESTROYER 
	};

	vector<char> m_playerBLegalSign = 
	{ 
		PLAYER_B_RUBBER_SHIP, 
		PLAYER_B_ROCKET_SHIP, 
		PLAYER_B_SUBMARINE, 
		PLAYER_B_DESTROYER 
	};

public:
	static Utils& instance();

	/**
	 * @Details		receives char and returns for which player the ship belongs to
	 * @Param		c - query ship
	 * @Return		PlayerIndex::PLAYER_A if c belongs to m_playerALegalSign,
	 *				PlayerIndex::PLAYER_B if c belongs to m_playerBLegalSign,
	 *				PlayerIndex::MAX_PLAYER otherwise
	 */
	inline PlayerIndex getPlayerIdByShip(char c);

	inline int getIndexByShip(char c);

	inline int getShipByIndexAndPlayer(int ship, int player);
	inline int getPlayerCharByIndex(int player);

	template<class T>
	bool isExistInVec(vector<T> vec, T val);

};

