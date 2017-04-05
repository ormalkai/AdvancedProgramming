#pragma once

#define BOARD_ROW_SIZE (10)
#define BOARD_COL_SIZE (10)
#define SPACE ' '

#include <list>
using namespace std;

enum class ReturnCode
{
    RC_ERROR = -1,
    RC_SUCCESS,
    RC_INVALID_ARG
};

enum class PlayerIndex
{
    PLAYER_A,
    PLAYER_B,
    MAX_PLAYER
};

enum class ShipType
{
    RUBBER_SHIP,
    ROCKET_SHIP,
    SUBMRINE,
    DESTROYER,
    MAX_SHIP
};

list<char> PLAYER_A_LEGAL_SIGN = { 'b', 'p', 'm', 'd' };

list<char> PLAYER_B_LEGAL_SIGN = { 'B', 'P', 'M', 'D' };

int getPlayerIdByShip(char c)
{

    if (std::find(PLAYER_A_LEGAL_SIGN.begin(), PLAYER_A_LEGAL_SIGN.end(), c) != PLAYER_A_LEGAL_SIGN.end())
        return PlayerIndex::PLAYER_A;
    return PlayerIndex::PLAYER_B;
}

int getIndexByShip(char c)
{
    switch (c) {
        case 'b':
        case 'B':
            return 0;
        case 'p':
        case 'P':
            return 1;
        case 'm':
        case 'M':
            return 2;
        case 'd':
        case 'D':
            return 3;
        default:
            return (int)ReturnCode::RC_ERROR;
    }
}

