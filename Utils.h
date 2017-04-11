#pragma once
#include <vector>
#include <map>
#include <windows.h>

#define NUM_OF_PLAYERS (2)
#define BOARD_ROW_SIZE (10)
#define BOARD_COL_SIZE (10)
#define INIT_BOARD_ROW_SIZE (BOARD_ROW_SIZE + 2)
#define INIT_BOARD_COL_SIZE (BOARD_COL_SIZE + 2)
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
    SUBMRINE,
    DESTROYER,
    MAX_SHIP
};

typedef enum AttackRequestCode
{
	ARC_ERROR,
	ARC_SUCCESS,
	ARC_NO_REQ
} AttackRequestCode;



class Utils
{
private:
	Utils();
	map<char, int> m_shipToLen; // conversion map between ship type to expected length
	map<char, int> m_shipToValue; // conversion map between ship type to expected value
	map<char, _In_ WORD> m_shipToColor;// conversion between ship and its color
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

	vector<string> m_expectedAttackFilePerPlayer = 
	{
		// if no attack file - empty string
		PLAYER_A_ATTACK_FILE,
		PLAYER_B_ATTACK_FILE
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
	PlayerIndex getPlayerIdByShip(char c);

	int getIndexByShip(char c);
	int getShipLen(char c);
	int getShipValue(char c);
	_In_ WORD getShipColor(char c);
	int getShipByIndexAndPlayer(int ship, int player);
	char getPlayerCharByIndex(int player);
	string getAttackFileByPlayer(int player);

	template<class T>
	bool isExistInVec(vector<T> vec, T val);

};

