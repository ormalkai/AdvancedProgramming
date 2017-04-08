#pragma once

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

vector<char> g_playerIndexToChar
{
	PLAYER_A_CHAR,
	PLAYER_B_CHAR
};

vector<char> g_playerALegalSign = 
{ 
	PLAYER_A_RUBBER_SHIP, 
	PLAYER_A_ROCKET_SHIP,
	PLAYER_A_SUBMARINE, 
	PLAYER_A_DESTROYER 
};

vector<char> g_playerBLegalSign = 
{ 
	PLAYER_B_RUBBER_SHIP, 
	PLAYER_B_ROCKET_SHIP, 
	PLAYER_B_SUBMARINE, 
	PLAYER_B_DESTROYER 
};

/**
 * @Details		receives char and returns for which player the ship belongs to
 * @Param		c - query ship
 * @Return		PlayerIndex::PLAYER_A if c belongs to g_playerALegalSign,
 *				PlayerIndex::PLAYER_B if c belongs to g_playerBLegalSign,
 *				PlayerIndex::MAX_PLAYER otherwise
 */
inline PlayerIndex getPlayerIdByShip(char c)
{

	if (std::find(g_playerALegalSign.begin(), g_playerALegalSign.end(), c) != g_playerALegalSign.end())
	{
		return PlayerIndex::PLAYER_A;
	}
	else if (std::find(g_playerBLegalSign.begin(), g_playerBLegalSign.end(), c) != g_playerBLegalSign.end())
	{
		return PlayerIndex::PLAYER_B;
	}
	return PlayerIndex::MAX_PLAYER;
}

inline int getIndexByShip(char c)
{
    switch (c) {
        case 'b':
        case 'B':
            return RUBBER_SHIP;
        case 'p':
        case 'P':
            return ROCKET_SHIP;
        case 'm':
        case 'M':
            return SUBMRINE;
        case 'd':
        case 'D':
            return DESTROYER;
        default:
            return (int)ReturnCode::RC_ERROR;
    }
}

inline int getShipByIndexAndPlayer(int ship, int player)
{
	if (MAX_SHIP < ship || 0 > ship)
	{
		return (int)ReturnCode::RC_ERROR;
	}

	if (MAX_PLAYER < player || 0 > player)
	{
		return (int)ReturnCode::RC_ERROR;
	}

	switch (player) {
	case PLAYER_A:
		return g_playerALegalSign[ship];
	case PLAYER_B:
		return g_playerBLegalSign[ship];
	default:
		return (int)ReturnCode::RC_ERROR;
	}
}

inline int getPlayerCharByIndex(int player)
{
	if (MAX_PLAYER < player || 0 > player)
	{
		return (int)ReturnCode::RC_ERROR;
	}

	return g_playerIndexToChar[player];
}

